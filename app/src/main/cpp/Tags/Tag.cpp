//
// Created by Nate on 5/21/2018.
//

#include "Tag.h"

Tag::Tag() {

}

Tag::~Tag() {
    if (mCover != nullptr){
        delete [] mCover;
        mCover = nullptr;
    }
}

const string &Tag::getTitle() const {
    return Tag::mTitle;
}

const string &Tag::getArtist() const {
    return Tag::mArtist;
}

const string &Tag::getAlbum() const {
    return Tag::mAlbum;
}

const string &Tag::getTrack() const {
    return Tag::mTrack;
}

const string &Tag::getYear() const {
    return Tag::mYear;
}

unsigned char *Tag::getCover() const {
    return Tag::mCover;
}

void Tag::setTitle(const string &Title) {
    Tag::mTitle = Title;
}

void Tag::setArtist(const string &Artist) {
    Tag::mArtist = Artist;
}

void Tag::setAlbum(const string &Album) {
    Tag::mAlbum = Album;
}

void Tag::setTrack(const string &Track) {
    Tag::mTrack = Track;
}

void Tag::setYear(const string &Year) {
    Tag::mYear = Year;
}

void Tag::setCover(unsigned char *coverBuffer, int lengthInBytes, int offset) {
	if (coverBuffer == nullptr) {
		if (Tag::mCover != nullptr) {
			delete[] Tag::mCover;
			Tag::mCover = nullptr;
			Tag::mCoverSize = 0;
		}
	}
	else {
		Tag::mCoverSize = lengthInBytes;
		Tag::mCover = new unsigned char[lengthInBytes];

		for (int i = 0; i < lengthInBytes; i++) {
			Tag::mCover[i] = coverBuffer[i+offset];
		}
	}
}

int Tag::getCoverSize() const {
    return Tag::mCoverSize;
}

int Tag::getTagSize() const {
    return Tag::mTagSize;
}







