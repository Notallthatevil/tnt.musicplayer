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

const string &Tag::getTitle() const {
    return mTitle;
}

const string &Tag::getArtist() const {
    return mArtist;
}

const string &Tag::getAlbum() const {
    return mAlbum;
}

const string &Tag::getTrack() const {
    return mTrack;
}

const string &Tag::getYear() const {
    return mYear;
}

unsigned char *Tag::getCover() const {
    return mCover;
}

void Tag::setTitle(const string &Title) {
    mTitle = Title;
}

void Tag::setArtist(const string &Artist) {
    mArtist = Artist;
}

void Tag::setAlbum(const string &Album) {
    mAlbum = Album;
}

void Tag::setTrack(const string &Track) {
    mTrack = Track;
}

void Tag::setYear(const string &Year) {
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







