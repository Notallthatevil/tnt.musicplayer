//
// Created by Nate on 5/21/2018.
//

#include "ID3TagV2.h"

ID3TagV2::ID3TagV2(unsigned char *header) : Tag() {
	if (header[0] == 'I' && header[1] == 'D' && header[2] == '3' && header[3] >= 3) {
		unsigned int byte0 = header[6];
		unsigned int byte1 = header[7];
		unsigned int byte2 = header[8];
		unsigned int byte3 = header[9];
		int synchSafeInt = byte0 << 21 | byte1 << 14 | byte2 << 7 | byte3;
		mMajorVersion = header[3];
		mMinorVersion = header[4];
		if (header[3] == 4 && ((header[5] & (0b00010000)) != 0)) {
			mFlagFooter = true;
			mHeaderSize = 20;
		}
		else {
			mFlagFooter = false;
			mHeaderSize = 10;
		}
		mTagSize = mHeaderSize + synchSafeInt;
		readFlags(header[5]);
	}
	else {
		throw ID3TagException();
	}
}


/*Returns value based on success
0 = header read succesfully
-1 = @param header was null
1 = invalid id3 tag*/
int ID3TagV2::readHeader(unsigned char *header) {
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
		}
		else {
			mFlagFooter = false;
			mHeaderSize = 10;
		}
		mTagSize = mHeaderSize + synchSafeInt;
		readFlags(header[5]);

		return 0;
	}
	else {
		return 1;
	}
}

//NO TEST NEEDED
void ID3TagV2::readFlags(char flagByte) {
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
int ID3TagV2::readTags(unsigned char *tagBuffer) {
	unsigned int pos = 0;
	unsigned int frameSize = 0;
	string frameHeader;

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
		}
		else {
			frameSize =
				tagBuffer[pos + 4] << 24 | tagBuffer[pos + 5] << 16 |
				tagBuffer[pos + 6] << 8 | tagBuffer[pos + 7];
		}

		if (frameSize == 0) {
			pos += 4;
		}
		else {
			if (frameHeader == TITLE_TAG) {
				setTitle(getTextFrame(tagBuffer, pos + 10, frameSize));
			}
			else if (frameHeader == ARTIST_TAG) {
				setArtist(getTextFrame(tagBuffer, pos + 10, frameSize));
			}
			else if (frameHeader == ALBUM_TAG) {
				setAlbum(getTextFrame(tagBuffer, pos + 10, frameSize));
			}
			else if (frameHeader == TRACK_TAG) {
				setTrack(getTextFrame(tagBuffer, pos + 10, frameSize));
			}
			else if (frameHeader == YEAR_TAG) {
				setYear(getTextFrame(tagBuffer, pos + 10, frameSize));
			}
			else if (frameHeader == COVER_TAG) {
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


/*Returns string from the passed in frame. Will always return some type of string
within the given frame size
*/
string ID3TagV2::getTextFrame(unsigned char *buffer, int offset, int frameSize) {
	string frameData = "";
	int i;

	switch (buffer[offset]) {

	case 0x01: //UTF-16LE

		//BOM present
		if (buffer[1 + offset] == 0xff || buffer[2 + offset] == 0xff) {
			i = 3;
		}
		else {
			i = 1;
		}
			if (buffer[1 + offset] == 0x0ff) {

			//start at three to skip encoding and endianess
				for (i; i < frameSize; i += 2) {
					//FIXME Fix to work with utf-16 characters
				
					unsigned int charSize = buffer[i + 1] << 8 | buffer[i];
					if (charSize > UCHAR_MAX) {
						//Inserts placeholder currently
						frameData += (char) 0x1F;
					}
					else if(charSize == 0){
						break;
					}
					else {
						frameData += buffer[i];
					}
				}
			break;
			}
		
	case 0x02: //UTF-16BE

		//BOM present
		if (buffer[1 + offset] == 0xff || buffer[2 + offset] == 0xff) {
			i = 3;
		}
		else {
			i = 1;
		}

		for (i; i < frameSize; i += 2) {

			//FIXME Fix to work with utf-16 characters

			unsigned int charSize = buffer[i] << 8 | buffer[i+1];
			if (charSize > UCHAR_MAX) {
				//Inserts placeholder currently
				frameData += (char)0x1F;
			}
			else if (charSize == 0) {
				break;
			}
			else {
				frameData += buffer[i+1];
			}

		}
		break;

	case 0x03: //UTF-8
		//Needs to be implimented
	case 0x00: //ISO-8859-1
	default:
		for (int i = 1; i < frameSize; i++) {
			frameData += buffer[i + offset];
		}
		break;
	}
	return frameData;
}



//Needs to be redone to improve reliability 

/* Returns an int that indicates where the binary image data acually starts in the frame
*/
int ID3TagV2::findCover(unsigned char *buffer, int offset, int frameSize) {
	if (buffer == nullptr) {
		return -1;
	}
	int apicFrameOffset = offset;
	int frameSizeOffset = frameSize;
	unsigned char encoding = buffer[apicFrameOffset++];
	string mimeType = "";
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
	}
	else {
		while (buffer[apicFrameOffset] != 0x00) {
			apicFrameOffset++;
		}
		apicFrameOffset++;
	}
	return apicFrameOffset;
}




ID3TagV2::~ID3TagV2() {
	/*if (mGeneratedTag != nullptr){
		delete[] mGeneratedTag;
		mGeneratedTag = nullptr;
	}*/
}




//TAG GENERATION



/*Generates binary data in the form of an ID3 tag and then returns that data.

Padding - Adds extra space to the end of the tag array. Used to attach new tags to mp3


*/

///CALL DELETE ON THIS OBJECT AFTER USE
unsigned char * ID3TagV2::generateTags(long padding) {
	mTagSize = calculateTagSize(false, 0);
	unsigned char *mGeneratedTag = new unsigned char[mTagSize + padding];
	if (mTagSize > 0) {

		int offset = createID3Header(mGeneratedTag);
		if (mTitle != "") {
			offset = createTextFrame(mGeneratedTag, offset, TITLE_TAG, mTitle);
		}
		if (mArtist != "") {
			offset = createTextFrame(mGeneratedTag, offset, ARTIST_TAG, mArtist);
		}
		if (mAlbum != "") {
			offset = createTextFrame(mGeneratedTag, offset, ALBUM_TAG, mAlbum);
		}
		if (mTrack != "") {
			offset = createTextFrame(mGeneratedTag, offset, TRACK_TAG, mTrack);
		}
		if (mYear != "") {
			offset = createTextFrame(mGeneratedTag, offset, YEAR_TAG, mYear);
		}
		if (mCover != nullptr) {
			offset = createAPICFrame(mGeneratedTag, offset);
		}
	}
	return mGeneratedTag;
}

unsigned char * ID3TagV2::generateTags() {
	return generateTags(0);
}


int ID3TagV2::createID3Header(unsigned char * dest) {
	return createID3Header(dest, false, 0, false, false);
}

int ID3TagV2::createID3Header(unsigned char * dest, bool unsynch, int extendedHeaderSize, bool experimental, bool footer) {

	//Set flags for tag
	mFlagUnsynchronisation = unsynch;
	mFlagExtendedHeader = extendedHeaderSize > 0 ? true : false;
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
		dest[offset] = dest[offset] | 0x80;
	}
	if (mFlagExtendedHeader) {
		dest[offset] = dest[offset] | 0x40;
	}
	if (mFlagExperimental) {
		dest[offset] = dest[offset] | 0x20;
	}
	if (mFlagFooter) {
		dest[offset] = dest[offset] | 0x10;
		footerPresent = 10;
	}
	//Insert tag size (Whole tag - tag header - footer (if present))
	int dataSize = mTagSize - frameStartingPosition - footerPresent;
	for (int i = 0; i < 4; i++) {
		dest[++offset] = (unsigned char)((dataSize >> (21 - (7 * i))) & 0x7f);
	}

	return frameStartingPosition + extendedHeaderSize;
}



//Adding extended header needs to be done post tag creation.
//TODO create method
int ID3TagV2::insertExtendedheader(int extendedHeaderSize, bool flag) {
	return -1;
}


int ID3TagV2::createTextFrame(unsigned char * dest, int offset, const string frameID, string data) {

	//FrameID
	for (int i = 0; i < 4; i++) {
		dest[offset++] = frameID[i];
	}

	//Frame size
	//Add 1 for encoding byte
	unsigned int dataSize = data.size() + 1;
	for (int i = 0; i < 4; i++) {
		if (mFlagUnsynchronisation){
			dest[offset++] = (unsigned char)((dataSize >> (21 - (7 * i))) & 0x7f);

		}
		else {
			dest[offset++] = (unsigned char)(dataSize >> (24 - (8 * i)));
		}
	}

	//flags
	dest[offset++] = 0x00;
	dest[offset++] = 0x00;

	//encoding
	//Going to always encode in UTF-8 as it has the most options as well
	//as following the ASCII standard
	dest[offset++] = 0x03;

	//insert data
	for (int i = 0; i < data.size(); i++) {
		dest[offset++] = data[i];
	}

	return offset;
}

unsigned char *ID3TagV2::createFrameFlags() {
	return nullptr;
}


int ID3TagV2::createAPICFrame(unsigned char * dest, int offset) {
	for (int i = 0; i < 4; i++) {
		dest[offset++] = COVER_TAG[i];
	}

	unsigned int dataSize = mCoverSize + mApicBinaryHeaderSize;
	for (int i = 0; i < 4; i++) {
		if (mFlagUnsynchronisation) {
			dest[offset++] = (unsigned char)((dataSize >> (21 - (7 * i))) & 0x7f);

		}
		else {
			dest[offset++] = (unsigned char)(dataSize >> (24 - (8 * i)));
		}
	}

	dest[offset++] = 0x00;
	dest[offset++] = 0x00;

	for (int i = 0; i < mApicBinaryHeaderSize; i++) {
		dest[offset++] = mApicBinaryHeader[i];
	}
	for (int i = 0; i < mCoverSize; i++) {
		dest[offset++] = mCover[i];
	}

	return offset;
}


/* Returns an unsigned char array of 4 that indicate the size of the frame. If the size
is greater then 268435455 then it returns null as the size is invalid.
@param synchSafe - Only use for the tag header. Frame headers should always be false.
*/
unsigned char * ID3TagV2::getTagSizeBytes(unsigned int dataSize, bool synchSafe) {
	if (dataSize > 0xfffffff) {
		return nullptr;
	}
	unsigned char frameSize[4];
	if (synchSafe) {
		for (int i = 0; i < 4; i++) {
			frameSize[i] = (unsigned char)((dataSize >> (21 - (7 * i))) & 0x7f);
		}
	}
	else {
		for (int i = 0; i < 4; i++)
			frameSize[i] = (unsigned char)(dataSize >>(24- (8 * i)));
	}

	//FIXME returns address of local variable
	return frameSize;
}


//TODO Add bools to indicated extended header and footer

//To be used with tag generation only
/*Returns int indicating the size of the tag*/
int ID3TagV2::calculateTagSize(bool footerPresent, int extendedHeaderSize) {
	unsigned int tagSize = extendedHeaderSize;
	if (footerPresent) {
		tagSize += 10;
	}

	//Adds extra byte to include the encoding byte
	if (mTitle != "") {
		tagSize += mTitle.size() + 1 + 10;
	}
	if (mArtist != "") {
		tagSize += mArtist.size() + 1 + 10;
	}
	if (mAlbum != "") {
		tagSize += mAlbum.size() + 1 + 10;
	}
	if (mTrack != "") {
		tagSize += mTrack.size() + 1 + 10;
	}
	if (mYear != "") {
		tagSize += mYear.size() + 1 + 10;
	}
	if (mCover != nullptr) {
		tagSize += mApicBinaryHeader.size() + mCoverSize+10;
	}
	if (tagSize == 0) {
		return 0;
	}
	else if(tagSize>0xFFFFFFF){
		return -2;
	}
	else {
		return tagSize += 10;
	}
}



