//
// Created by Nate on 5/21/2018.
//

#ifndef TAGGER_TAG_H
#define TAGGER_TAG_H


#include <string>
#include <fstream>
#include <android/log.h>


/**
 * An abstract class for Tag. Contains basic getters and setters.
 */
class Tag {

protected: //NOTE:Tag data
    std::string mTitle;        /**Title of track found in the tag*/
    std::string mArtist;    /**Artist of track found in the tag*/
    std::string mAlbum;    /**Album of track found in the tag*/
    std::string mTrack;    /**Track number of track found in the tag*/
    std::string mYear;        /**Year of track found in the tag*/

    /**
     * Variable to hold the album cover. Only to be used to write a new tag
     * to the file. The getter has been deprecated.
     */
    char *mCover = nullptr;

    /**
     * The size of the cover in bytes. Used to retrieve the tag from the cover
     * from the actual file on the disk. Also used when writing a new cover to
     * the file.
     */
    int mCoverSize = 0;

    int mTagSize = 0;        /**The size of the tag in bytes*/
    int mCoverOffset = -1;    /**The offset of the cover art from the beginning of the tag*/

    Tag() = default; /**Abstract constructor*/

public:

    /**
     * Simple deconstructor. Deletes @var mCover if it is not a nullptr
     */
    virtual ~Tag();

    /**
     * Generates a new tag depending on what class what called this function. The tag is generated first
     * and then the audio data is copied into the empty bytes represented by @param padding.
     * @param padding The length of the audio data in bytes.
     * @returns the new byte array with the tag and additional padding.
     */
    virtual unsigned char *generateTags(long padding) = 0;

    /**
     * Reads the tags from the specified caller. Sets the corresponding variables if the data exists
     * within the read tag.
     * @param tagBuffer A byte buffer that contains the full tag
     * @returns A value based upon the success
     */
    virtual int readTags(unsigned char *tagBuffer) = 0;

    const std::string &getTitle() const;

    const std::string &getArtist() const;

    const std::string &getAlbum() const;

    const std::string &getTrack() const;

    const std::string &getYear() const;

    [[deprecated("Get cover was replaced by setting the offset of the cover "
    "and reading it from the actual file. Instead of moving around "
    "such a large array.")]]
    char *getCover(std::ifstream *mStream);

    void setTitle(const std::string &Title);

    void setArtist(const std::string &Artist);

    void setAlbum(const std::string &Album);

    void setTrack(const std::string &Track);

    void setYear(const std::string &Year);

    /**
     *
     * @param Cover
     * @param lengthInBytes
     * @param offset
     */
    void setCover(char *Cover, int lengthInBytes, int offset);

    int getCoverSize() const;

    int getTagSize() const;

    int getCoverOffset() const;


};


#endif //TAGGER_TAG_H
