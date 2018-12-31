//
// Created by Nate on 5/21/2018.
//

#include "ID3Tag.h"


/*Returns value based on success
0 = header read successfully
-1 = @param header was null
1 = invalid id3 tag*/
int ID3Tag::readHeader(unsigned char *header) {
    if (header == nullptr) {
        return -1;
    }
    if (header[0] == 'I' && header[1] == 'D' && header[2] == '3' && header[3] >= 3) {
        int synchSafeInt = header[6] << 21 | header[7] << 14 | header[8] << 7 | header[9];
        mMajorVersion = header[3];
        mMinorVersion = header[4];
        if (header[3] == 4 && ((header[5] & (0b00010000)) != 0)) {
            mFlagFooter = true;
            mHeaderSize = 20;
        } else {
            mFlagFooter = false;
            mHeaderSize = 10;
        }
        mTagSize = mHeaderSize + synchSafeInt;
        readFlags(header[5]);

        return 0;
    } else {
        return 1;
    }
}

//NO TEST NEEDED
void ID3Tag::readFlags(char flagByte) {
    if ((flagByte & 0x80) != 0) {
        mFlagUnsynchronisation = true;
    }
    if ((flagByte & 0x40) != 0) {
        mFlagExtendedHeader = true;
    }
    if ((flagByte & 0x20) != 0) {
        mFlagExperimental = true;
    }
    if ((flagByte & 0x10) != 0) {
        mFlagFooter = true;
    }
}


/*Returns value based on the success of reading the tags
0 = tags were read successfully without any unsuspected errors
-1 = tagBuffer was null
-2 = mTagSize is 0, readHeader() must be called prior to calling readTags
1 = tags might be corrupted*/
int ID3Tag::readTags(unsigned char *tagBuffer) {
    unsigned int pos = 0;
    unsigned int frameSize = 0;
    std::string frameHeader;

    if (tagBuffer == nullptr) {
        return -1;
    }
    if (mTagSize == 0) {
        return -2;
    }
    while (pos < mTagSize - mHeaderSize) {
        frameHeader += tagBuffer[pos];
        frameHeader += tagBuffer[pos + 1];
        frameHeader += tagBuffer[pos + 2];
        frameHeader += tagBuffer[pos + 3];
        if (mFlagUnsynchronisation) {
            frameSize =
                    tagBuffer[pos + 4] << 21 | tagBuffer[pos + 5] << 14 |
                    tagBuffer[pos + 6] << 7 | tagBuffer[pos + 7];
        } else {
            frameSize =
                    tagBuffer[pos + 4] << 24 | tagBuffer[pos + 5] << 16 |
                    tagBuffer[pos + 6] << 8 | tagBuffer[pos + 7];
        }

        if (frameSize == 0) {
            pos += 4;
        } else {
            if (frameHeader == TITLE_TAG) {
                setTitle(getTextFrame(tagBuffer, pos + 10, frameSize));
            } else if (frameHeader == ARTIST_TAG) {
                setArtist(getTextFrame(tagBuffer, pos + 10, frameSize));
            } else if (frameHeader == ALBUM_TAG) {
                setAlbum(getTextFrame(tagBuffer, pos + 10, frameSize));
            } else if (frameHeader == TRACK_TAG) {
                setTrack(getTextFrame(tagBuffer, pos + 10, frameSize));
            } else if (frameHeader == YEAR_TAG) {
                setYear(getTextFrame(tagBuffer, pos + 10, frameSize));
            } else if (frameHeader == COVER_TAG) {
                int imageOffset = findCover(tagBuffer, pos + 10, frameSize);
                setCover(tagBuffer, frameSize - imageOffset, imageOffset);
            }

            pos += frameSize + 10;
            frameHeader = "";
        }

    }
    if (pos > mTagSize - mHeaderSize) {
        return 1;
    }
    return 0;
}


/*Returns std::string from the passed in frame. Will always return some type of std::string
within the given frame size
*/
std::string ID3Tag::getTextFrame(unsigned char *buffer, int offset, int frameSize) {
    std::string frameData;
    int i;

    switch (buffer[offset]) {

        case UTF_16: //UTF-16LE

            //BOM present
            if (buffer[1 + offset] == 0xff || buffer[2 + offset] == 0xff) {
                i = 3;
            } else {
                i = 1;
            }
            if (buffer[1 + offset] == 0x0ff) {

                //start at three to skip encoding and endianness
                for (i; i < frameSize; i += 2) {
                    //FIXME Fix to work with utf-16 characters

                    unsigned int charSize = buffer[i + 1] << 8u | buffer[i];
                    if (charSize > UCHAR_MAX) {
                        //Inserts placeholder currently
                        frameData += (char) 0x1F;
                    } else if (charSize == 0) {
                        break;
                    } else {
                        frameData += buffer[i];
                    }
                }
                break;
            }

        case UTF_16BE: //UTF-16BE

            //BOM present
            if (buffer[1 + offset] == 0xff || buffer[2 + offset] == 0xff) {
                i = 3;
            } else {
                i = 1;
            }

            for (i; i < frameSize; i += 2) {

                //FIXME Fix to work with utf-16 characters

                unsigned int charSize = buffer[i] << 8u | buffer[i + 1];
                if (charSize > UCHAR_MAX) {
                    //Inserts placeholder currently
                    frameData += (char) 0x1F;
                } else if (charSize == 0) {
                    break;
                } else {
                    frameData += buffer[i + 1];
                }

            }
            break;

        case UTF_8:
            //Needs to be implemented
        case ISO_8859_1: //ISO-8859-1
        default:
            for (int j = 1; j < frameSize; j++) {
                frameData += buffer[j + offset];
            }
            break;
    }
    return frameData;
}



//Needs to be redone to improve reliability 

/* Returns an int that indicates where the binary image data actually starts in the frame
*/
int ID3Tag::findCover(unsigned char *buffer, int offset, int frameSize) {
    if (buffer == nullptr) {
        return -1;
    }
    int apicFrameOffset = offset;
    int frameSizeOffset = frameSize;
    unsigned char encoding = buffer[apicFrameOffset++];
    std::string mimeType;
    while (buffer[apicFrameOffset] != 0x00) {
        mimeType += buffer[apicFrameOffset++];
    }
    char pictureType = buffer[++apicFrameOffset];
    apicFrameOffset++;
    if (encoding == 0x01 || encoding == 0x02 /*UTF-16*/) {
        while (buffer[apicFrameOffset] != 0x00 ||
               buffer[apicFrameOffset + 1] != 0x00) {
            apicFrameOffset += 2;
        }
        apicFrameOffset += 2;
    } else {
        while (buffer[apicFrameOffset] != 0x00) {
            apicFrameOffset++;
        }
        apicFrameOffset++;
    }
    return apicFrameOffset;
}


ID3Tag::~ID3Tag() = default;




//TAG GENERATION



/*Generates binary data in the form of an ID3 tag and then returns that data.

Padding - Adds extra space to the end of the tag array. Used to attach new tags to mp3


*/

///CALL DELETE ON THIS OBJECT AFTER USE
unsigned char *ID3Tag::generateTags(long padding) {
    mTagSize = calculateTagSize(false, 0);
    auto *mGeneratedTag = new unsigned char[mTagSize + padding];
    if (mTagSize > 0) {

        int offset = createID3Header(mGeneratedTag);
        if (!mTitle.empty()) {
            offset = createTextFrame(mGeneratedTag, offset, TITLE_TAG, mTitle);
        }
        if (!mArtist.empty()) {
            offset = createTextFrame(mGeneratedTag, offset, ARTIST_TAG, mArtist);
        }
        if (!mAlbum.empty()) {
            offset = createTextFrame(mGeneratedTag, offset, ALBUM_TAG, mAlbum);
        }
        if (!mTrack.empty()) {
            offset = createTextFrame(mGeneratedTag, offset, TRACK_TAG, mTrack);
        }
        if (!mYear.empty()) {
            offset = createTextFrame(mGeneratedTag, offset, YEAR_TAG, mYear);
        }
        if (mCover != nullptr) {
            offset = createAPICFrame(mGeneratedTag, offset);
        }
    }
    return mGeneratedTag;
}

unsigned char *ID3Tag::generateTags() {
    return generateTags(0);
}


int ID3Tag::createID3Header(unsigned char *dest) {
    return createID3Header(dest, false, 0, false, false);
}

int ID3Tag::createID3Header(unsigned char *dest, bool unsynch, int extendedHeaderSize, bool experimental, bool footer) {

    //Set flags for tag
    mFlagUnsynchronisation = unsynch;
    mFlagExtendedHeader = extendedHeaderSize > 0;
    mFlagExperimental = experimental;
    mFlagFooter = footer;

    int footerPresent = 0;
    int frameStartingPosition = 10;
    int offset = 0;


    //Assign header values
    dest[offset++] = 'I';
    dest[offset++] = 'D';
    dest[offset++] = '3';
    dest[offset++] = MAJOR_VERSION;
    dest[offset++] = MINOR_VERSION;
    dest[offset] = 0x00;

    //set flags
    if (mFlagUnsynchronisation) {
        dest[offset] = dest[offset] | 0x80u;
    }
    if (mFlagExtendedHeader) {
        dest[offset] = dest[offset] | 0x40u;
    }
    if (mFlagExperimental) {
        dest[offset] = dest[offset] | 0x20u;
    }
    if (mFlagFooter) {
        dest[offset] = dest[offset] | 0x10u;
        footerPresent = 10;
    }
    //Insert tag size (Whole tag - tag header - footer (if present))
    int dataSize = mTagSize - frameStartingPosition - footerPresent;
    for (int i = 0; i < 4; i++) {
        dest[++offset] = (unsigned char) ((dataSize >> (21 - (7 * i))) & 0x7f);
    }

    return frameStartingPosition + extendedHeaderSize;
}


//Adding extended header needs to be done post tag creation.
//TODO create method
int ID3Tag::insertExtendedHeader(int extendedHeaderSize, bool flag) {
    return -1;
}


int ID3Tag::createTextFrame(unsigned char *dest, int offset, std::string frameID, std::string data) {

    //FrameID
    for (int i = 0; i < 4; i++) {
        dest[offset++] = (unsigned char) frameID[i];
    }

    //Frame size
    //Add 1 for encoding byte
    unsigned int dataSize = data.size() + 1;
    for (int i = 0; i < 4; i++) {
        if (mFlagUnsynchronisation) {
            dest[offset++] = (unsigned char) ((dataSize >> (21 - (7 * i))) & 0x7f);

        } else {
            dest[offset++] = (unsigned char) (dataSize >> (24 - (8 * i)));
        }
    }

    //flags
    dest[offset++] = 0x00;
    dest[offset++] = 0x00;

    //encoding
    //Going to always encode in UTF-8 as it has the most options as well
    //as following the ASCII standard
    dest[offset++] = UTF_8;

    //insert data
    for (char i : data) {
        dest[offset++] = (unsigned char) i;
    }

    return offset;
}


int ID3Tag::createAPICFrame(unsigned char *dest, int offset) {
    for (int i = 0; i < 4; i++) {
        dest[offset++] = (unsigned char) COVER_TAG[i];
    }

    unsigned int dataSize = (unsigned int)(mCoverSize + APIC_HEADER_SIZE);
    for (int i = 0; i < 4; i++) {
        if (mFlagUnsynchronisation) {
            dest[offset++] = (unsigned char) ((dataSize >> (21 - (7 * i))) & 0x7f);

        } else {
            dest[offset++] = (unsigned char) (dataSize >> (24 - (8 * i)));
        }
    }

    dest[offset++] = 0x00;
    dest[offset++] = 0x00;

    for (char i : mApicBinaryHeader) {
        dest[offset++] = (unsigned char) i;
    }
    for (int i = 0; i < mCoverSize; i++) {
        dest[offset++] = mCover[i];
    }

    return offset;
}



//TODO Add bool to indicated extended header and footer

//To be used with tag generation only
/*Returns int indicating the size of the tag*/
int ID3Tag::calculateTagSize(bool footerPresent, int extendedHeaderSize) {
    int tagSize = extendedHeaderSize;
    if (footerPresent) {
        tagSize += 10;
    }
    //Adds extra byte to include the encoding byte
    if (!mTitle.empty()) {
        tagSize += mTitle.size() + 1 + 10;
    }
    if (!mArtist.empty()) {
        tagSize += mArtist.size() + 1 + 10;
    }
    if (!mAlbum.empty()) {
        tagSize += mAlbum.size() + 1 + 10;
    }
    if (!mTrack.empty()) {
        tagSize += mTrack.size() + 1 + 10;
    }
    if (!mYear.empty()) {
        tagSize += mYear.size() + 1 + 10;
    }
    if (mCover != nullptr) {
        tagSize += APIC_HEADER_SIZE + mCoverSize + 10;
    }
    if (tagSize == 0) {
        return 0;
    } else if (tagSize > 0xFFFFFFF) {
        return -2;
    } else {
        return tagSize += 10;
    }
}

int ID3Tag::getHeaderSize() const {
    return mHeaderSize;
}





