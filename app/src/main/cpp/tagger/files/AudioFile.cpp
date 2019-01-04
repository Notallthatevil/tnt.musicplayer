//
// Created by Nate on 5/21/2018.
//

#include "AudioFile.h"

AudioFile::AudioFile(std::string *filePath) {
    mFilePath = *filePath;
}

bool AudioFile::open() {
    if (mIsOpen) {
        return true;
    }
    mStream = new std::ifstream(getFilePath().c_str(), std::ios::ate | std::ios::binary);
    mIsOpen = mStream->is_open();
    if (!mIsOpen) {
        return false;
    }
    mFileSize = (unsigned long) mStream->tellg();
    mStream->seekg(0, std::ios::beg);
    mIsOpen = true;
    return mIsOpen;
}

AudioFile::~AudioFile() {
    mIsOpen = false;
    if (mStream != nullptr) {
        delete mStream;
        mStream = nullptr;
    }
    if (mFileData != nullptr) {
        delete[] mFileData;
        mFileData = nullptr;
    }
}

std::string AudioFile::getFilePath() const {
    return mFilePath;
}

unsigned long AudioFile::getFileSize() const {
    return mFileSize;
}

long AudioFile::getID() const {
    return mSqlID;
}

void AudioFile::setID(long ID) {
    mSqlID = ID;
}

long AudioFile::getDuration() const {
    return mDuration;
}

int AudioFile::getBitrate() {
    return mBitrate;
}

int AudioFile::getSampleRate() {
    return mSampleRate;
}

