//
// Created by Nate on 5/21/2018.
//

#ifndef TAGGER_TAG_H
#define TAGGER_TAG_H


#include <string>
#include <vector>

using namespace std;

class Tag {

protected: //NOTE:Tag data
    string mTitle;
    string mArtist;
    string mAlbum;
    string mTrack;
    string mYear;
    unsigned char *mCover = nullptr;
    int mCoverSize = 0;
    int mTagSize = 0;
	//unsigned char *mGeneratedTag = nullptr;
	Tag();

public:

    virtual ~Tag();

    virtual unsigned char* generateTags(long padding)=0;

    virtual int readTags(unsigned char *tagBuffer)=0;

    const string &getTitle() const;

    const string &getArtist() const;

    const string &getAlbum() const;

    const string &getTrack() const;

    const string &getYear() const;

    unsigned char *getCover() const;

    void setTitle(const string &Title);

    void setArtist(const string &Artist);

    void setAlbum(const string &Album);

    void setTrack(const string &Track);

    void setYear(const string &Year);

    void setCover(unsigned char *Cover, int lengthInBytes, int offset);

    int getCoverSize() const;

    int getTagSize() const;

};


#endif //TAGGER_TAG_H
