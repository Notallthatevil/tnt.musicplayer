#include <alloca.h>
//
// Created by Nate on 5/21/2018.
//

#ifndef TAGGER_ID3Tag_H
#define TAGGER_ID3Tag_H


#include "../../baseclass/Tag.h"


class ID3Tag : public Tag {
private:
    enum Constants {
        //Encodings
                ISO_8859_1 = 0x00,
        UTF_16 = 0x01,
        UTF_16BE = 0x02,
        UTF_8 = 0x03,

        MAJOR_VERSION = 0x04,
        MINOR_VERSION = 0x00,

        APIC_HEADER_SIZE = 19
    };
    const std::string TITLE_TAG = "TIT2";
    const std::string ALBUM_TAG = "TALB";
    const std::string ARTIST_TAG = "TPE1";
    const std::string TRACK_TAG = "TRCK";
    const std::string YEAR_TAG = "TYER";
    const std::string COVER_TAG = "APIC";


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
    char __unused mMajorVersion = 0xFF;
    char __unused mMinorVersion = 0xFF;
#pragma clang diagnostic pop


    char mApicBinaryHeader[19] = {0x03, 'i', 'm', 'a', 'g', 'e', '/', 'j', 'p', 'e', 'g', 0x00, 0x03, 'C', 'o',
                                  'v', 'e', 'r', 0x00};

    bool mFlagUnsynchronisation = false;
    bool mFlagExtendedHeader = false;
    bool mFlagExperimental = false;
    bool mFlagFooter = false;

protected:

    int mHeaderSize = 0;

    void readFlags(char flagByte);

    int createTextFrame(unsigned char *dest, int offset, std::string frameID, std::string data);

    int findCover(unsigned char *buffer, int offset);

    std::string getTextFrame(unsigned char *buffer, int offset, int frameSize);

    int calculateTagSize(bool footerPresent, int extendedHeaderSize);

    int createAPICFrame(unsigned char *dest, int offset);

    int createID3Header(unsigned char *dest, bool unsynch, int extendedHeaderSize, bool experimental, bool footer);

    int __unused insertExtendedHeader(int extendedHeaderSize, bool flag);

    int createID3Header(unsigned char *dest);


public:
    ID3Tag() : Tag() {}

    virtual ~ID3Tag();

    /*
     * Reads the ID3 tag header if present and identifies key information about the ID3 tag
     *
     * Returns
     * -1 if the header was null
     * 0 if header read successfully
     * 1 if header was an invalid tag header. i.e. A ID3 tag doesn't exist on this file
     */
    int readHeader(unsigned char *header);

    /*
     * Parses tagBuffer and sets the corresponding values associated with the tags
     *
     * Returns
     * -2 if mTagSize is 0, readHeader() must be called prior to calling readTags
     * -1 if tagBuffer was null
     * 0 if tags were read successfully without any unsuspected errors
     *1 if the tags might be corrupted. i.e. The tag did not meet the ID3 standards
     */
    int readTags(unsigned char *tagBuffer) override;

    /*
     * Generates a new ID3 tag and returns the tag as an unsigned char array. The length of the
     * array is set to mTagSize. Padding is set when the tag might have something else attached to
     * the end of the tag. i.e. mp3 data
     *
     * Returns the new array
     */
    unsigned char *generateTags(long padding) override;

    /*
     * Generates a new ID3 tag and returns the tag as an unsigned char array. The length of the
     * array is set to mTagSize
     *
     * Returns the new array
     */
    unsigned char __unused *generateTags();


    /*
     * Returns the size of the header
     * This value is not constant as it can change from either 10 or 20 depending on if the header
     * has an extended flag set
     */
    int getHeaderSize() const;

    enum {
        HEADER_SIZE = 10
    };
};

#endif //TAGGER_ID3Tag_H
