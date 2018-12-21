//
// Created by Nate on 5/21/2018.
//

#include "AudioFile.h"

AudioFile::AudioFile(string *filePath) {
    mFilePath = *filePath;
}

bool AudioFile::open() {
	if(mIsOpen) {
		return true;
	}
	mStream = new ifstream(getFilePath().c_str(), ios::ate|ios::binary);
	mIsOpen = mStream->is_open();
	if (!mIsOpen) {
		return false;
	}
	mFileSize = (unsigned long) mStream->tellg();
	mStream->seekg(0,ios::beg);
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

string AudioFile::getFilePath() const {
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

long AudioFile::getDuration() const{
	return mDuration;
}

