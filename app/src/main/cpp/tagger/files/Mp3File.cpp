#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//
// Created by Nate on 5/21/2018.
//

#include "Mp3File.h"


/// PARSING

/*Returns value based on success
0 = Header exists
-1 = Header was null
1 = No header found
-2 = unable to open file
*/
int Mp3File::hasHeader() {
    if (!open()) {
        return -2;
    }
    unsigned char header[ID3Tag::HEADER_SIZE];
    mStream->read((char *) &header, ID3Tag::HEADER_SIZE);

    mId3Tag = new ID3Tag();
    return mId3Tag->readHeader(header);
}


/*	Returns value based on success
	-2 = unable to open file
	-1 = if the file is null
	0 = success
	1 = invalid header
	*/
int Mp3File::parse(bool findTags) {
    if (!open()) {
        return -2;
    }

    int rc = hasHeader();

    if (rc == -1) {
        return rc;
    }
    if (rc == 0 && findTags) {
        //get tag size
        int bufferSize = mId3Tag->getTagSize() - mId3Tag->getHeaderSize();

        //read tags into buffer
        auto *tagBuffer = new unsigned char[bufferSize];

        mStream->read((char *) tagBuffer, bufferSize);
        //parse tags
        mId3Tag->readTags(tagBuffer);

        delete[] tagBuffer;
        tagBuffer = nullptr;
    }

    rc = parseMp3Data();

    return rc;
}


/* Parses the mp3 data contained within the file
*-1 if the file is not a valid audio file
*0 if the file was parsed through successfully (doesn't guarantee a vail audio file)
*/
int Mp3File::parseMp3Data() {

    int rc;

    //Seek to audio
    if (mId3Tag != nullptr) {
        mStream->seekg(mId3Tag->getTagSize()/*Default is 0*/, std::ios::beg);
        mAudioSize = mFileSize - mId3Tag->getTagSize();
    } else {
        mStream->seekg(0);
        mAudioSize = mFileSize;
    }

    //Find audio frame 0xFFE
    char c;

    // NEED TO FIND OUT WHAT HAPPENS IF THIS IS NEVER FOUND
    int allowedAttempts = 100;
    // Could this cause a false negative?
    int attempts = 0;
    while (mStream->get(c)) {
        if (c == (char) 0xff && ((mStream->peek() >> 5) == (char) 0x07)) {
            //header found
            mStream->get(c);
            mMp3Version = ((c >> 3) & (char) 0x03);
            mMp3Layer = ((c >> 1) & (char) 0x03);

            mStream->get(c);
            mBitrate = findBitrate(c);
            mSampleRate = findSampleRate(c);
            break;
        }
        attempts++;
        if (attempts >= allowedAttempts) {
            return -1;
        }
    }

    //Discover if file is CBR or VBR
    rc = checkForVBRHeader();

    switch (rc) {
        case 1:
            char vbriHeader[28];
            mStream->read(vbriHeader, 28);
            parseVBRIHeader(vbriHeader);
            mDuration = calculateDuration(mNumOfFrames, getSamples(), mSampleRate);
            mBitrate = calculateBitrate(mAudioSize, mDuration);
            break;
        case 0:
            char xingBuffer[120];
            mStream->read(xingBuffer, 120);
            rc = parseXingHeader(xingBuffer);
            if (rc == 0) {
                mDuration = calculateDuration(mNumOfFrames, getSamples(), mSampleRate);
                mBitrate = calculateBitrate(mAudioSize, mDuration);
                break;
            }
        case 2:
            mBitrate = getAverageBitrate();

            if (mBitrate != -1) {
                mDuration = calculateDuration(mAudioSize, mBitrate);
                break;
            } else {
                return -1;
            }
        default:
            return rc;
    }
    return 0;
}

/*Essentially look up tables*/
int Mp3File::findBitrate(char c) {
    char bitrateChar = (c >> 4) & (char) 0x0F;
    switch (bitrateChar) {
        case (char) 0x00:
            return 0;
        case (char) 0x01:
            switch (mMp3Version) {

                case Mp3File::VERSION_1:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                        case Mp3File::LAYER_II:
                        case Mp3File::LAYER_III:
                            return 32;
                        default:
                            break;
                    }

                case Mp3File::VERSION_2:
                case Mp3File::VERSION_2_5:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 32;
                        case Mp3File::LAYER_II:
                        case Mp3File::LAYER_III:
                            return 8;
                        default:
                            break;
                    }

                default:
                    break;
            }
        case (char) 0x02:
            switch (mMp3Version) {

                case Mp3File::VERSION_1:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 64;
                        case Mp3File::LAYER_II:
                            return 48;
                        case Mp3File::LAYER_III:
                            return 40;

                        default:
                            break;
                    }

                case Mp3File::VERSION_2:
                case Mp3File::VERSION_2_5:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 48;

                        case Mp3File::LAYER_II:
                        case Mp3File::LAYER_III:
                            return 16;
                        default:
                            break;
                    }

                default:
                    break;
            }

        case (char) 0x03:
            switch (mMp3Version) {

                case Mp3File::VERSION_1:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 96;
                        case Mp3File::LAYER_II:
                            return 56;
                        case Mp3File::LAYER_III:
                            return 48;

                        default:
                            break;
                    }

                case Mp3File::VERSION_2:
                case Mp3File::VERSION_2_5:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 56;

                        case Mp3File::LAYER_II:
                        case Mp3File::LAYER_III:
                            return 24;
                        default:
                            break;
                    }

                default:
                    break;
            }

        case (char) 0x04:
            switch (mMp3Version) {

                case Mp3File::VERSION_1:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 128;
                        case Mp3File::LAYER_II:
                            return 64;
                        case Mp3File::LAYER_III:
                            return 56;

                        default:
                            break;
                    }

                case Mp3File::VERSION_2:
                case Mp3File::VERSION_2_5:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 64;

                        case Mp3File::LAYER_II:
                        case Mp3File::LAYER_III:
                            return 32;
                        default:
                            break;
                    }

                default:
                    break;
            }

        case (char) 0x05:
            switch (mMp3Version) {

                case Mp3File::VERSION_1:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 160;
                        case Mp3File::LAYER_II:
                            return 80;
                        case Mp3File::LAYER_III:
                            return 64;

                        default:
                            break;
                    }

                case Mp3File::VERSION_2:
                case Mp3File::VERSION_2_5:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 80;

                        case Mp3File::LAYER_II:
                        case Mp3File::LAYER_III:
                            return 40;
                        default:
                            break;
                    }

                default:
                    break;
            }

        case (char) 0x06:
            switch (mMp3Version) {

                case Mp3File::VERSION_1:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 192;
                        case Mp3File::LAYER_II:
                            return 96;
                        case Mp3File::LAYER_III:
                            return 80;

                        default:
                            break;
                    }

                case Mp3File::VERSION_2:
                case Mp3File::VERSION_2_5:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 96;

                        case Mp3File::LAYER_II:
                        case Mp3File::LAYER_III:
                            return 48;
                        default:
                            break;
                    }

                default:
                    break;
            }

        case (char) 0x07:
            switch (mMp3Version) {

                case Mp3File::VERSION_1:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 224;
                        case Mp3File::LAYER_II:
                            return 112;
                        case Mp3File::LAYER_III:
                            return 96;

                        default:
                            break;
                    }

                case Mp3File::VERSION_2:
                case Mp3File::VERSION_2_5:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 112;

                        case Mp3File::LAYER_II:
                        case Mp3File::LAYER_III:
                            return 56;
                        default:
                            break;
                    }

                default:
                    break;
            }

        case (char) 0x08:
            switch (mMp3Version) {

                case Mp3File::VERSION_1:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 256;
                        case Mp3File::LAYER_II:
                            return 128;
                        case Mp3File::LAYER_III:
                            return 112;

                        default:
                            break;
                    }

                case Mp3File::VERSION_2:
                case Mp3File::VERSION_2_5:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 128;

                        case Mp3File::LAYER_II:
                        case Mp3File::LAYER_III:
                            return 64;
                        default:
                            break;
                    }

                default:
                    break;
            }

        case (char) 0x09:
            switch (mMp3Version) {

                case Mp3File::VERSION_1:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 288;
                        case Mp3File::LAYER_II:
                            return 160;
                        case Mp3File::LAYER_III:
                            return 128;

                        default:
                            break;
                    }

                case Mp3File::VERSION_2:
                case Mp3File::VERSION_2_5:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 144;

                        case Mp3File::LAYER_II:
                        case Mp3File::LAYER_III:
                            return 80;
                        default:
                            break;
                    }

                default:
                    break;
            }

        case (char) 0x0A:
            switch (mMp3Version) {

                case Mp3File::VERSION_1:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 320;
                        case Mp3File::LAYER_II:
                            return 192;
                        case Mp3File::LAYER_III:
                            return 160;

                        default:
                            break;
                    }

                case Mp3File::VERSION_2:
                case Mp3File::VERSION_2_5:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 160;

                        case Mp3File::LAYER_II:
                        case Mp3File::LAYER_III:
                            return 96;
                        default:
                            break;
                    }

                default:
                    break;
            }

        case (char) 0x0B:
            switch (mMp3Version) {

                case Mp3File::VERSION_1:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 352;
                        case Mp3File::LAYER_II:
                            return 224;
                        case Mp3File::LAYER_III:
                            return 192;

                        default:
                            break;
                    }

                case Mp3File::VERSION_2:
                case Mp3File::VERSION_2_5:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 176;

                        case Mp3File::LAYER_II:
                        case Mp3File::LAYER_III:
                            return 112;
                        default:
                            break;
                    }

                default:
                    break;
            }

        case (char) 0x0C:
            switch (mMp3Version) {

                case Mp3File::VERSION_1:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 384;
                        case Mp3File::LAYER_II:
                            return 256;
                        case Mp3File::LAYER_III:
                            return 224;

                        default:
                            break;
                    }

                case Mp3File::VERSION_2:
                case Mp3File::VERSION_2_5:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 192;

                        case Mp3File::LAYER_II:
                        case Mp3File::LAYER_III:
                            return 128;
                        default:
                            break;
                    }

                default:
                    break;
            }

        case (char) 0x0D:
            switch (mMp3Version) {

                case Mp3File::VERSION_1:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 416;
                        case Mp3File::LAYER_II:
                            return 320;
                        case Mp3File::LAYER_III:
                            return 256;

                        default:
                            break;
                    }

                case Mp3File::VERSION_2:
                case Mp3File::VERSION_2_5:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 224;

                        case Mp3File::LAYER_II:
                        case Mp3File::LAYER_III:
                            return 144;
                        default:
                            break;
                    }

                default:
                    break;
            }

        case (char) 0x0E:
            switch (mMp3Version) {

                case Mp3File::VERSION_1:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 448;
                        case Mp3File::LAYER_II:
                            return 384;
                        case Mp3File::LAYER_III:
                            return 320;

                        default:
                            break;
                    }

                case Mp3File::VERSION_2:
                case Mp3File::VERSION_2_5:

                    switch (mMp3Layer) {

                        case Mp3File::LAYER_I:
                            return 256;

                        case Mp3File::LAYER_II:
                        case Mp3File::LAYER_III:
                            return 160;
                        default:
                            break;
                    }

                default:
                    break;
            }
        case (char) 0x0F:
            return 0;
        default:
            return -3;
    }

}

int Mp3File::findSampleRate(char c) {
    char sampleRateChar = (c >> 2) & (char) 0x03;
    switch (sampleRateChar) {
        case (char) 0x00:
            switch (mMp3Version) {
                case VERSION_1:
                    return 44100;
                case VERSION_2:
                    return 22050;
                case VERSION_2_5:
                    return 11025;
                default:
                    break;
            }
        case (char) 0x01:
            switch (mMp3Version) {
                case VERSION_1:
                    return 48000;
                case VERSION_2:
                    return 24000;
                case VERSION_2_5:
                    return 12000;
                default:
                    break;
            }
        case (char) 0x02:
            switch (mMp3Version) {
                case VERSION_1:
                    return 32000;
                case VERSION_2:
                    return 16000;
                case VERSION_2_5:
                    return 8000;
                default:
                    break;
            }
        default:
            return -2;
    }
}

int Mp3File::getSamples() {
    switch (mMp3Version) {
        case Mp3File::VERSION_1:
            switch (mMp3Layer) {
                case Mp3File::LAYER_I:
                    return 384;
                case Mp3File::LAYER_II:
                case Mp3File::LAYER_III:
                    return 1152;
                default:
                    break;
            }
        case Mp3File::VERSION_2:
            switch (mMp3Layer) {
                case Mp3File::LAYER_I:
                    return 384;
                case Mp3File::LAYER_II:
                    return 1152;
                case Mp3File::LAYER_III:
                    return 576;
                default:
                    break;
            }
        case Mp3File::VERSION_2_5:
            switch (mMp3Layer) {
                case Mp3File::LAYER_I:
                    return 384;
                case Mp3File::LAYER_II:
                    return 1152;
                case Mp3File::LAYER_III:
                    return 576;
                default:
                    break;
            }

        default:
            return -2;
    }
}


/* Parses first Mp3 frame looking for a VBR header returns an int based on success
*-1 = Reached end of stream
* 0 = Found a Xing/Info header
* 1 = Found a VBRI header
* 2 = Found another Mp3 header
*
* If 2 is returned the file is either CBR or the average bitrate must be found. To be safe we look at a couple other frames
* and compare the bitrate to check for consistency.
*/
int Mp3File::checkForVBRHeader() {
    char VBRHeader[4];
    int tempPos = 0;
    while (mStream->get(VBRHeader[0])) {
        if (VBRHeader[0] == 'X' || VBRHeader[0] == 'I') {
            tempPos = (int) mStream->tellg();
            mStream->read(&VBRHeader[1], 3);
            if ((VBRHeader[1] == 'i' && VBRHeader[2] == 'n' & VBRHeader[3] == 'g') ||
                (VBRHeader[1] == 'n' && VBRHeader[2] == 'f' & VBRHeader[3] == 'o')) {
                mStream->seekg(tempPos - 1);
                return 0;
            } else {
                mStream->seekg(tempPos);
            }
        } else if (VBRHeader[0] == 'V') {
            tempPos = (int) mStream->tellg();
            mStream->read(&VBRHeader[1], 3);
            if (VBRHeader[1] == 'B' && VBRHeader[2] == 'R' & VBRHeader[3] == 'I') {
                mStream->seekg(tempPos - 1);
                return 1;
            }

        } else if (VBRHeader[0] == (char) 0xff && ((mStream->peek() >> 5) == (char) 0x07)) {
            //Seek to the char before the Mp3 header so that the getAverageBitrate reads the right char
            mStream->unget();
            return 2;
        }
    }
    return -1;
}


/*Sets the number of frames that the Xing frame identifies.
Returns 0 if header contains number of frames field.
Returns 2 if header doesn't contain number of frames field.
	returns two so that the average bitrate will be found

*/
int Mp3File::parseXingHeader(char *xingBuffer) {
    ///Flags that aren't necessary at this time

    //bool framesField = xingBuffer[7] & (char)0x01;
    //bool bytesField = xingBuffer[7] & (char)0x02;
    //bool tocfield = xingBuffer[7] & (char)0x04;
    //bool qualityfield = xingBuffer[7] & (char)0x08;

    if (xingBuffer[7] & (char) 0x01) {
        mNumOfFrames = (unsigned int) (xingBuffer[8] << 24 | xingBuffer[9] << 16 | xingBuffer[10] << 8 |
                                       xingBuffer[11]);
        return 0;
    }
    return 2;
    //Other two flags not necessary at this time
}

/*
Sets the number of frames field
*/
void Mp3File::parseVBRIHeader(char *vbriBuffer) {
    mNumOfFrames = (unsigned int) (vbriBuffer[14] << 24 | vbriBuffer[15] << 16 | vbriBuffer[16] << 8 |
                                   vbriBuffer[17]);
}


/*
Determines the bitrate of file that doesn't contain a bitrate related frame. Returns bitrate of file 
or -1 if file is invalid
*/
int Mp3File::getAverageBitrate() {
    int tempPos = (int) mStream->tellg();
    mStream->seekg(mFileSize / 3);
    int bitrate1 = -1;
    int bitrate2 = -1;
    char c;
    while (mStream->get(c)) {
        if (c == (char) 0xff && ((mStream->peek() >> 5) == (char) 0x07)) {

            mStream->get(c);
            mStream->get(c);
            if (bitrate1 == -1) {
                bitrate1 = findBitrate(c);
                mStream->seekg(mStream->tellg() * 2);
            } else {
                bitrate2 = findBitrate(c);
                break;
            }
        }
    }
    if (mBitrate == bitrate1 && bitrate1 == bitrate2) {
        return mBitrate;
    } else {
        int frames = 1;
        int totalBitrate = mBitrate;
        mStream->seekg(tempPos);
        unsigned long allowedAttempts = mFileSize / 50;
        int attempts = 0;
        while (mStream->get(c)) {
            if (c == (char) 0xff && ((mStream->peek() >> 5) == (char) 0x07)) {

                mStream->get(c);
                mStream->get(c);

                int frameBitrate = findBitrate(c);
                if (frameBitrate > 0) {
                    totalBitrate += frameBitrate;
                    frames++;
                    int frameSize = calculateFrameSize(getSamples(), frameBitrate, findSampleRate(c), (c & 0x02));
                    int currentOffset = (int) mStream->tellg();
                    if (frameSize < 0) {
                        return -1;
                    }

                    //Subtract 3 from offset for already being 3 bytes in
                    mStream->seekg(currentOffset - 3 + frameSize);

                    attempts = 0;
                }
            }
            attempts++;
            if (attempts > allowedAttempts) {
                return -1;
            }
        }
        return totalBitrate / frames;
    }
}


/*
DURATION IN MS = ((file size in bits)/(bitrate * 1000)) * 1000
DURATION IN MS = (Total frame count * Samples per frame / Sample Rate)*1000
FRAME SIZE IN BYTES = (((Samples Per Frame / 8 * Bitrate) / Sampling Rate) + Padding Size) * 1000
*/
inline int Mp3File::calculateFrameSize(float samplesPerFrame, float frameBitrate, float sampleRate, float padding) {
    return (int) (samplesPerFrame / 8 * frameBitrate * 1000 / sampleRate + padding);
}

inline int Mp3File::calculateDuration(float fileSizeInBytes, float bitrate) {
    return (int) (((fileSizeInBytes * 8) / (bitrate * 1000)) * 1000);
}

inline int Mp3File::calculateDuration(float totalNumOfFrames, float samplesPerFrame, float sampleRate) {
    return (int) (totalNumOfFrames * samplesPerFrame / sampleRate * 1000);
}

inline int Mp3File::calculateBitrate(float fileSizeInBytes, float durationInMilli) {
    return (int) ((fileSizeInBytes * 8 / (durationInMilli * 1000)) * 1000);
}


/*
Need to take the new tag and copy it to a new stream that also include 
*/

int Mp3File::attachNewTag(Tag *newTag) {
    if (hasHeader() == 0) {
        mAudioSize = mFileSize - mId3Tag->getTagSize();
        mStream->seekg(mId3Tag->getTagSize());
    } else {
        mAudioSize = mFileSize;
        mStream->seekg(0);
    }

    mFileData = newTag->generateTags(mAudioSize);
    mStream->read((char *) &mFileData[newTag->getTagSize()], mAudioSize);
    mFileSize = newTag->getTagSize() + mAudioSize;
    mStream->read((char *) &mFileData[newTag->getTagSize()], mAudioSize);

    return 0;
}


//Abstraction layer for creating the tag
int Mp3File::saveNewTag(Tag *newTag) {

    if (attachNewTag(newTag) == 0) {
        mStream->close();
        std::ofstream outStream(getFilePath(), std::ios::trunc | std::ios::beg | std::ios::binary);
        if (outStream.is_open()) {
            outStream.write((char *) mFileData, mFileSize);
            outStream.close();
        } else {
            return -2;
        }
    } else {
        return -1;
    }
    return 0;
}

Mp3File::~Mp3File() {
    if (mId3Tag != nullptr) {
        delete mId3Tag;
        mId3Tag = nullptr;
    }
}

Tag *Mp3File::getTag() {
    return mId3Tag;
}


#pragma clang diagnostic pop