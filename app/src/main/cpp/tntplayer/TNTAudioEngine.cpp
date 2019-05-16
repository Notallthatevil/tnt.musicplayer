//
// Created by Nate on 3/27/2019.
//

#include "TNTAudioEngine.h"

constexpr int64_t kNanosPerMillisecond = 1000000; // Use int64_t to avoid overflows in calculations
constexpr int32_t kDefaultChannelCount = 2; // Stereo

TNTAudioEngine::TNTAudioEngine() {
    // Initialize the trace functions, this enables you to output trace statements without
    // blocking. See https://developer.android.com/studio/profile/systrace-commandline.html
    Trace::initialize();

    mChannelCount = kDefaultChannelCount;
    createPlaybackStream();

}


TNTAudioEngine::~TNTAudioEngine() {
    closeOutputStream();
}


void TNTAudioEngine::createPlaybackStream() {

    oboe::AudioStreamBuilder builder;
    setupPlaybackStreamParameters(&builder);

    oboe::Result result = builder.openStream(&mAudioStream);

    if(result == oboe::Result::OK && mAudioStream != nullptr) {

        mSampleRate = mAudioStream->getSampleRate();
        mFramesPerBurst = mAudioStream->getFramesPerBurst();

        int channelCount = mAudioStream->getChannelCount();
        if(channelCount != mChannelCount) {
            LOGW("Requested %d channels but received %d", mChannelCount, channelCount);
        }

        // Set the buffer size to the burst size - this will give us the minimum possible latency
        mAudioStream->setBufferSizeInFrames(mFramesPerBurst);

        // TODO: Implement Oboe_convertStreamToText
        // PrintAudioStreamInfo(mPlayStream);
        prepareOscillators();

        // Create a latency tuner which will automatically tune our buffer size.
        mLatencyTuner = std::unique_ptr<oboe::LatencyTuner>(new oboe::LatencyTuner(*mAudioStream));
        // Start the stream - the dataCallback function will start being called
        result = mAudioStream->requestStart();
        if(result != oboe::Result::OK) {
            LOGE("Error starting stream. %s", oboe::convertToText(result));
        }

        mIsLatencyDetectionSupported = (mAudioStream->getTimestamp(CLOCK_MONOTONIC, 0, 0) !=
                                        oboe::Result::ErrorUnimplemented);

    } else {
        LOGE("Failed to create stream. Error: %s", oboe::convertToText(result));
    }

}

void TNTAudioEngine::prepareOscillators() {

    double frequency = 440.0;
    constexpr double interval = 110.0;
    constexpr float amplitude = 0.25;

    for(SineGenerator &osc : mOscillators) {
        osc.setup(frequency, mSampleRate, amplitude);
        frequency += interval;
    }
}

void TNTAudioEngine::setupPlaybackStreamParameters(oboe::AudioStreamBuilder *builder) {
    builder->setAudioApi(mAudioApi);
    builder->setDeviceId(mDeviceId);
    builder->setChannelCount(mChannelCount);

    // We request EXCLUSIVE mode since this will give us the lowest possible latency.
    // If EXCLUSIVE mode isn't available the builder will fall back to SHARED mode.
    builder->setSharingMode(oboe::SharingMode::Exclusive);
    builder->setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder->setCallback(this);
}

void TNTAudioEngine::closeOutputStream() {
    if(mAudioStream != nullptr) {
        oboe::Result result = mAudioStream->requestStop();
        if(result != oboe::Result::OK) {
            LOGE("Error stopping output stream. %s", oboe::convertToText(result));
        }

        result = mAudioStream->close();
        if(result != oboe::Result::OK) {
            LOGE("Error closing output stream. %s", oboe::convertToText(result));
        }
    }
}

oboe::DataCallbackResult
TNTAudioEngine::onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) {
    int32_t bufferSize = audioStream->getBufferSizeInFrames();

    if(mBufferSizeSelection == kBufferSizeAutomatic) {
        mLatencyTuner->tune();
    } else if(bufferSize != (mBufferSizeSelection * mFramesPerBurst)) {
        auto setBufferResult = audioStream->setBufferSizeInFrames(mBufferSizeSelection * mFramesPerBurst);
        if(setBufferResult == oboe::Result::OK) bufferSize = setBufferResult.value();
    }

    /**
     * The following output can be seen by running a systrace. Tracing is preferable to logging
     * inside the callback since tracing does not block.
     *
     * See https://developer.android.com/studio/profile/systrace-commandline.html
     */
    auto underrunCountResult = audioStream->getXRunCount();

    Trace::beginSection("numFrames %d, Underruns %d, buffer size %d",
                        numFrames, underrunCountResult.value(), bufferSize);

    int32_t channelCount = audioStream->getChannelCount();

    // If the tone is on we need to use our synthesizer to render the audio data for the sine waves
    if(audioStream->getFormat() == oboe::AudioFormat::Float) {
        if(mIsToneOn) {
            for(int i = 0; i < channelCount; ++i) {
                mOscillators[i].render(static_cast<float *>(audioData) + i, channelCount, numFrames);
            }
        } else {
            memset(static_cast<uint8_t *>(audioData), 0,
                   sizeof(float) * channelCount * numFrames);
        }
    } else {
        if(mIsToneOn) {
            for(int i = 0; i < channelCount; ++i) {
                mOscillators[i].render(static_cast<int16_t *>(audioData) + i, channelCount, numFrames);
            }
        } else {
            memset(static_cast<uint8_t *>(audioData), 0,
                   sizeof(int16_t) * channelCount * numFrames);
        }
    }

    if(mIsLatencyDetectionSupported) {
        calculateCurrentOutputLatencyMillis(audioStream, &mCurrentOutputLatencyMillis);
    }

    Trace::endSection();
    return oboe::DataCallbackResult::Continue;
}


oboe::Result TNTAudioEngine::calculateCurrentOutputLatencyMillis(oboe::AudioStream *stream,
                                                                 double *latencyMillis) {

    // Get the time that a known audio frame was presented for playing
    auto result = stream->getTimestamp(CLOCK_MONOTONIC);

    if(result == oboe::Result::OK) {

        oboe::FrameTimestamp playedFrame = result.value();

    // Get the write index for the next audio frame
        int64_t writeIndex = stream->getFramesWritten();

    // Calculate the number of frames between our known frame and the write index
        int64_t frameIndexDelta = writeIndex - playedFrame.position;

    // Calculate the time which the next frame will be presented
        int64_t frameTimeDelta = (frameIndexDelta * oboe::kNanosPerSecond) / mSampleRate;
        int64_t nextFramePresentationTime = playedFrame.timestamp + frameTimeDelta;

    // Assume that the next frame will be written at the current time
        using namespace std::chrono;
        int64_t nextFrameWriteTime =
                duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();

    // Calculate the latency
        *latencyMillis = static_cast<double>(nextFramePresentationTime - nextFrameWriteTime)
                         / kNanosPerMillisecond;
    } else {
        LOGE("Error calculating latency: %s", oboe::convertToText(result.error()));
    }

    return result;
}