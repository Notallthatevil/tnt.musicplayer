//
// Created by Nate on 5/21/2018.
//

#ifndef TAGGER_ID3TAGV2_H
#define TAGGER_ID3TAGV2_H


#include "Tag.h"
#include <string>

using namespace std;

class ID3TagV2 : public Tag {
private: //NOTE: CONSTANTS
    const string TITLE_TAG = "TIT2";
    const string ALBUM_TAG = "TALB";
    const string ARTIST_TAG = "TPE1";
    const string TRACK_TAG = "TRCK";
    const string YEAR_TAG = "TYER";
    const string COVER_TAG = "APIC";
    const unsigned char MAJOR_VERSION = 0x04;
    const unsigned char MINOR_VERSION = 0x00;
    const char ISO_8859_1 = 0x00;
    const char UTF_16 = 0x01;
    const char UTF_16BE = 0x02;
    const char UTF_8 = 0x03;
	const vector<char> mApicBinaryHeader{ 0x03, 'i', 'm', 'a', 'g', 'e', '/', 'j', 'p', 'e', 'g', 0x00, 0x03, 'C','o', 'v', 'e', 'r', 0x00 };
	const int mApicBinaryHeaderSize = 19;


    //Header flags
    char mMajorVersion;
    char mMinorVersion;
	
	//Indicates that all frames use unsynchronisation i.e. 0x0FFFFFFF
    bool mFlagUnsynchronisation = false;
    bool mFlagExtendedHeader = false;
    bool mFlagExperimental = false;
    bool mFlagFooter = false;



     //NOTE:Methods


protected:

	int mHeaderSize = 0;

	void readFlags(char flagByte);

	int createTextFrame(unsigned char * dest, int offset, const string frameID, string data);

	unsigned char *createFrameFlags();

	int findCover(unsigned char * buffer, int offset, int frameSize);

	string getTextFrame(unsigned char *buffer, int offset, int frameSize);

	[[deprecated("Function is now down within respected methods. This is useless")]]
	unsigned char *getTagSizeBytes(unsigned int dataSize, bool synchSafe);

	int calculateTagSize(bool footerPresent, int extendedHeaderSize);

	int createAPICFrame(unsigned char * dest,int offset);

	int createID3Header(unsigned char * dest, bool unsynch, int extendedHeaderSize, bool experimental, bool footer);

	int insertExtendedheader(int extendedHeaderSize, bool flag);

	int createID3Header(unsigned char * dest);


public:
    ID3TagV2() : Tag() {}

	[[deprecated("Should construct default object then call readHeader(unsigned char *header)")]]
	ID3TagV2(unsigned char *header);

	virtual ~ID3TagV2();

	int readHeader(unsigned char * header);

    unsigned char * generateTags(long padding);

	unsigned char * generateTags();

    int readTags(unsigned char *tagBuffer);

	int getHeaderSize() const {
		return mHeaderSize;
	}

    static const int HEADER_SIZE = 10;

};

struct ID3TagException : public exception {

    const char *what() const throw() {
        return "ID3 Tag is not valid. May have errors parsing the file";
    }
};


#endif //TAGGER_ID3TAGV2_H
