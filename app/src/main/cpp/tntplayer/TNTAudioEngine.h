//
// Created by Nate on 3/27/2019.
//

#ifndef TNT_MUSICPLAYER_TNTAUDIOENGINE_H
#define TNT_MUSICPLAYER_TNTAUDIOENGINE_H


#include <stdint.h>
#include <oboe/Oboe.h>
#include <trace.h>
#include <logging_macros.h>
#include <array>
#include "SineGenerator.h"

constexpr int32_t kBufferSizeAutomatic = 0;
constexpr int32_t kMaximumChannelCount = 8;

class TNTAudioEngine : oboe::AudioStreamCallback {
public:
    TNTAudioEngine();
    ~TNTAudioEngine();

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames);

private:

    oboe::AudioApi mAudioApi = oboe::AudioApi::Unspecified;/**Try to use AAudio. If not available then use OpenSL ES.*/
    int32_t mDeviceId = oboe::kUnspecified;
    int32_t mSampleRate;
    int32_t mFramesPerBurst;
    int32_t mChannelCount;
    int32_t mBufferSizeSelection = kBufferSizeAutomatic;
    double mCurrentOutputLatencyMillis = 0;
    oboe::AudioStream *mAudioStream;
    bool mIsToneOn = true;
    bool mIsLatencyDetectionSupported = false;

    std::unique_ptr<oboe::LatencyTuner> mLatencyTuner;

    std::array<SineGenerator, kMaximumChannelCount> mOscillators;

    void createPlaybackStream();
    void setupPlaybackStreamParameters(oboe::AudioStreamBuilder *builder);
    void prepareOscillators();
    void closeOutputStream();
    oboe::Result calculateCurrentOutputLatencyMillis(oboe::AudioStream *stream, double *latencyMillis);

};

#endif //TNT_MUSICPLAYER_TNTAUDIOENGINE_H
