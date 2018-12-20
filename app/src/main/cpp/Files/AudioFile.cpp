//
// Created by Nate on 5/21/2018.
//

#include "AudioFile.h"

AudioFile::AudioFile(string *filePath) {
    AudioFile::mFilePath = *filePath;
   /* mStream = new ifstream(filePath->c_str(), ios::ate);
    mFileSize = (unsigned long) mStream->tellg();
    mStream->seekg(0, ios::beg);*/
}

bool AudioFile::open() {
	if(AudioFile::mIsOpen) {
		return true;
	}
	AudioFile::mStream = new ifstream(getFilePath().c_str(), ios::ate|ios::binary);
	AudioFile::mIsOpen = AudioFile::mStream->is_open();
	if (!AudioFile::mIsOpen) {
		return false;
	}
	AudioFile::mFileSize = (unsigned long) AudioFile::mStream->tellg();
	AudioFile::mStream->seekg(0,ios::beg);
	AudioFile::mIsOpen = true;
	return AudioFile::mIsOpen;
}

AudioFile::~AudioFile() {
	AudioFile::mIsOpen = false;
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

int AudioFile::getID() const {
    return mSqlID;
}

void AudioFile::setID(int ID) {
    AudioFile::mSqlID = ID;
}

void AudioFile::setFilePath(const string &filePath) {
    AudioFile::mFilePath = filePath;
}

//unsigned char* AudioFile::getAudio() {
    //return mAudioData;
//}

unsigned long AudioFile::getDuration() const{
	return AudioFile::mDuration;
}

//void AudioFile::setAudio() {
//    if(mAudioData.size()<1){
//        throw invalidAudioDataSize();
//    }
//    mStream->read(&mAudioData[0], mAudioData.size());
//}

