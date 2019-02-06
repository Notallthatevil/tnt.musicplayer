//
// Created by Nate on 5/21/2018.
//

#ifndef TAGGER_TAG_H
#define TAGGER_TAG_H


#include <string>


/*
 * Abstract class for Tag. Contains basic getters and setters. Should not
 * and can not be instantiated.
 */
class Tag {

protected: //NOTE:Tag data
    std::string mTitle;
    std::string mArtist;
    std::string mAlbum;
    std::string mTrack;
    std::string mYear;
    unsigned char *mCover = nullptr;
    int mCoverSize = 0;
    int mTagSize = 0;
    int mCoverOffset = -1;
	Tag() = default;

public:

    virtual ~Tag();

    virtual unsigned char* generateTags(long padding)=0;

    virtual int readTags(unsigned char *tagBuffer)=0;

    const std::string &getTitle() const;

    const std::string &getArtist() const;

    const std::string &getAlbum() const;

    const std::string &getTrack() const;

    const std::string &getYear() const;

    unsigned char *getCover() const;

    void setTitle(const std::string &Title);

    void setArtist(const std::string &Artist);

    void setAlbum(const std::string &Album);

    void setTrack(const std::string &Track);

    void setYear(const std::string &Year);

    void setCover(unsigned char *Cover, int lengthInBytes, int offset);

    int getCoverSize() const;

    int getTagSize() const;

	int getCoverOffset() const;


};


#endif //TAGGER_TAG_H
