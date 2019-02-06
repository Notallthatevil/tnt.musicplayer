//
// Created by Nate on 5/21/2018.
//

#include "Tag.h"

Tag::~Tag() {
    if (mCover != nullptr){
        delete [] mCover;
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

unsigned char *Tag::getCover() const {
    return mCover;
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

void Tag::setCover(unsigned char *coverBuffer, int lengthInBytes, int offset) {
	if (coverBuffer == nullptr) {
		if (mCover != nullptr) {
			delete[] mCover;
			mCover = nullptr;
			mCoverSize = 0;
		}
	}
	else {
		mCoverSize = lengthInBytes;
		mCover = new unsigned char[lengthInBytes];

		for (int i = 0; i < lengthInBytes; i++) {
			mCover[i] = coverBuffer[i+offset];
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
