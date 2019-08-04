//
// Created by Nate on 5/21/2018.
//

#include "Tag.h"

Tag::~Tag() {
    if(mCover != nullptr) {
        delete[] mCover;
        mCover = nullptr;
    }
}

const std::string &Tag::getTitle() const {
    return mTitle;
}

const std::string &Tag::getArtist() const {
    return mArtist;
}

const std::string &Tag::getAlbum() const {
    return mAlbum;
}

const std::string &Tag::getTrack() const {
    return mTrack;
}

const std::string &Tag::getYear() const {
    return mYear;
}

char *Tag::getCover(std::ifstream *stream) {
    if(mCover != nullptr) {
        return mCover;
    } else if(stream != nullptr) {
        if(!stream->is_open()) {
            __android_log_print(ANDROID_LOG_DEBUG, "Tag.cpp", "mStream was not open so unable to return cover");
            return nullptr;
        } else {
            mCover = new char[mCoverSize];
            stream->seekg(mCoverOffset, std::ios::beg);
            stream->read((char *) mCover, mCoverSize);
            return mCover;
        }
    }
    //Unreachable
    return nullptr;
}

void Tag::setTitle(const std::string &Title) {
    mTitle = Title;
}

void Tag::setArtist(const std::string &Artist) {
    mArtist = Artist;
}

void Tag::setAlbum(const std::string &Album) {
    mAlbum = Album;
}

void Tag::setTrack(const std::string &Track) {
    mTrack = Track;
}

void Tag::setYear(const std::string &Year) {
    mYear = Year;
}

void Tag::setCover(char *coverBuffer, int lengthInBytes, int offset) {
    if(mCover != nullptr) {
        delete[] mCover;
        mCover = nullptr;
        mCoverSize = 0;
    }
    if(coverBuffer != nullptr) {
        mCoverSize = lengthInBytes;
        mCover = new char[lengthInBytes];

        for(int i = 0; i < lengthInBytes; i++) {
            mCover[i] = coverBuffer[i + offset];
        }
    }
}

int Tag::getCoverSize() const {
    return mCoverSize;
}

int Tag::getTagSize() const {
    return mTagSize;
}

int Tag::getCoverOffset() const {
    return mCoverOffset;
}
