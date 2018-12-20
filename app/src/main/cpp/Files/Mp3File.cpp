//
// Created by Nate on 5/21/2018.
//

#include "Mp3FileV2.h"


Mp3FileV2::Mp3FileV2(string *filePath, bool findTags) : AudioFile(filePath) {
    //open();
    unsigned char header[ID3TagV2::HEADER_SIZE];
    mStream->read((char *) &header, ID3TagV2::HEADER_SIZE);
    if (header[0] == 'I' && header[1] == 'D' && header[2] == '3') {
        mId3Tag = new ID3TagV2(header);
        if (findTags) {
            //Get tag size

            //Read tag into a buffer
            long bufferSize = mId3Tag->getTagSize() - mId3Tag->getHeaderSize();
            unsigned char *tagBuffer = new unsigned char[bufferSize];
            mStream->read((char *) tagBuffer, bufferSize);

            //Read data from buffer into id3tag
            mId3Tag->readTags(tagBuffer);

            delete[] tagBuffer;
            tagBuffer = nullptr;
        }
    }
}






/// PARSING

/*Returns value based on success
0 = Header exists
-1 = Header was null
1 = No header found
*/
int Mp3FileV2::hasHeader() {
	unsigned char header[ID3TagV2::HEADER_SIZE];
	mStream->read((char *)&header, ID3TagV2::HEADER_SIZE);

	mId3Tag = new ID3TagV2();
	return mId3Tag->readHeader(header);
}





/*	Returns value based on success
	-2 = unable to open file
	-1 = header was null
	0 = success
	1 = invalid header
	*/
int Mp3FileV2::parse(bool findTags) {
	if (!open()) {
		return -2;
	}

	int rc = hasHeader();
	
	if (rc == -1) {
		return rc;
	}
	if (rc == 0 && findTags) {
		//get tag size
		int bufferSize = mId3Tag->getTagSize() - mId3Tag->getHeaderSize();

		//read tags into buffer
		unsigned char *tagBuffer = new unsigned char[bufferSize];

		mStream->read((char*)tagBuffer, bufferSize);
		//parse tags
		rc = mId3Tag->readTags(tagBuffer);

		delete[] tagBuffer;
		tagBuffer = nullptr;
	}

	rc = parseMp3Data();

	return rc;
}


/* Parses the mp3 data contained within the file
*-1 if the file is not a valid audio file
*0 if the file was parsed through successfully (doesn't gaurentee a vaild audio file)
*/
int Mp3FileV2::parseMp3Data() {

	int rc;

	//Seek to audio
	if (mId3Tag != nullptr) {
		mStream->seekg(mId3Tag->getTagSize()/*Default is 0*/, ios::beg);
		AudioFile::mAudioSize = AudioFile::mFileSize - Mp3FileV2::mId3Tag->getTagSize();
	}
	else {
		mStream->seekg(0);
		AudioFile::mAudioSize = AudioFile::mFileSize;
	}
	
	//Find audio frame 0xFFE
	char c;

	// NEED TO FIND OUT WHAT HAPPENS IF THIS IS NEVER FOUND
	int allowedAttempts = 100;
	// Could this cause a false negative?
	int attempts = 0;
	while (mStream->get(c)) {
		if (c == (char)0xff && ((mStream->peek() >> 5) == (char)0x07)) {
			//header found
			mStream->get(c);
			Mp3FileV2::MP3_VERSION = ((c >> 3) & (char)0x03);
			Mp3FileV2::MP3_LAYER = ((c >> 1) & (char)0x03);

			mStream->get(c);
			AudioFile::mBitrate = findBitrate(c);
			AudioFile::mSamplerate = findSampleRate(c);
			break;
		}
		attempts++;
		if (attempts >= allowedAttempts){
			return -1;
		}
	}

	//Discover if file is CBR or VBR
	rc = checkForVBRHeader();

	switch (rc){
	case 1:
		char vbriHeader[28];
		mStream->read(vbriHeader, 28);
		parseVBRIHeader(vbriHeader);
		AudioFile::mDuration = calculateDuration(Mp3FileV2::MP3_NUM_OF_FRAMES, getSamples(), AudioFile::mSamplerate);
		AudioFile::mBitrate = calculateBitrate(AudioFile::mAudioSize, AudioFile::mDuration);
		break;
	case 0:
		char xingBuffer[120];
		mStream->read(xingBuffer, 120);
		rc = parseXingHeader(xingBuffer);
		if (rc == 0) {
			AudioFile::mDuration = calculateDuration(Mp3FileV2::MP3_NUM_OF_FRAMES, getSamples(), AudioFile::mSamplerate);
			AudioFile::mBitrate = calculateBitrate(AudioFile::mAudioSize, AudioFile::mDuration);
			break;
		}
	case 2:
		AudioFile::mBitrate = getAverageBitrate();

		if (AudioFile::mBitrate != -1) {
			AudioFile::mDuration = calculateDuration(mAudioSize, AudioFile::mBitrate);
			break;
		}
		else {
			return -1;
		}
	default:
		return rc;
	}
	return 0;
}

/*Essentially look up tables*/
int Mp3FileV2::findBitrate(char c) {
	char bitrateChar = (c >> 4) & (char)0x0F;
	switch (bitrateChar) {
	case (char)0x00:
		return 0;
	case (char)0x01:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 32;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 32;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 8;
			}
		
		}
	case (char)0x02:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 64;
			case Mp3FileV2::LAYER_II:
				return 48;
			case Mp3FileV2::LAYER_III:
				return 40;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 48;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 16;
			}

		}
		
	case (char)0x03:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 96;
			case Mp3FileV2::LAYER_II:
				return 56;
			case Mp3FileV2::LAYER_III:
				return 48;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 56;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 24;
			}

		}

	case (char)0x04:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 128;
			case Mp3FileV2::LAYER_II:
				return 64;
			case Mp3FileV2::LAYER_III:
				return 56;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 64;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 32;
			}

		}

	case (char)0x05:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 160;
			case Mp3FileV2::LAYER_II:
				return 80;
			case Mp3FileV2::LAYER_III:
				return 64;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 80;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 40;
			}

		}

	case (char)0x06:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 192;
			case Mp3FileV2::LAYER_II:
				return 96;
			case Mp3FileV2::LAYER_III:
				return 80;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 96;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 48;
			}

		}

	case (char)0x07:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 224;
			case Mp3FileV2::LAYER_II:
				return 112;
			case Mp3FileV2::LAYER_III:
				return 96;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 112;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 56;
			}

		}

	case (char)0x08:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 256;
			case Mp3FileV2::LAYER_II:
				return 128;
			case Mp3FileV2::LAYER_III:
				return 112;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 128;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 64;
			}

		}

	case (char)0x09:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 288;
			case Mp3FileV2::LAYER_II:
				return 160;
			case Mp3FileV2::LAYER_III:
				return 128;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 144;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 80;
			}

		}

	case (char)0x0A:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 320;
			case Mp3FileV2::LAYER_II:
				return 192;
			case Mp3FileV2::LAYER_III:
				return 160;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 160;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 96;
			}

		}

	case (char)0x0B:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 352;
			case Mp3FileV2::LAYER_II:
				return 224;
			case Mp3FileV2::LAYER_III:
				return 192;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 176;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 112;
			}

		}

	case (char)0x0C:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 384;
			case Mp3FileV2::LAYER_II:
				return 256;
			case Mp3FileV2::LAYER_III:
				return 224;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 192;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 128;
			}

		}

	case(char)0x0D:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 416;
			case Mp3FileV2::LAYER_II:
				return 320;
			case Mp3FileV2::LAYER_III:
				return 256;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 224;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 144;
			}

		}

	case (char)0x0E:
		switch (Mp3FileV2::MP3_VERSION) {

		case Mp3FileV2::VERSION_1:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 448;
			case Mp3FileV2::LAYER_II:
				return 384;
			case Mp3FileV2::LAYER_III:
				return 320;

			}

		case Mp3FileV2::VERSION_2:
		case Mp3FileV2::VERSION_2_5:

			switch (Mp3FileV2::MP3_LAYER) {

			case Mp3FileV2::LAYER_I:
				return 256;

			case Mp3FileV2::LAYER_II:
			case Mp3FileV2::LAYER_III:
				return 160;
			}

		}
	case (char) 0x0F:
		return 0;
	default:
		return -3;
	}
	
}
int Mp3FileV2::findSampleRate(char c) {
	char sampleRateChar = (c >> 2) & (char)0x03;
	switch (sampleRateChar){
	case (char)0x00:
		switch (Mp3FileV2::MP3_VERSION){
		case VERSION_1:
			return 44100;
		case VERSION_2:
			return 22050;
		case VERSION_2_5:
			return 11025;
		}
	case (char)0x01:
		switch (Mp3FileV2::MP3_VERSION) {
		case VERSION_1:
			return 48000;
		case VERSION_2:
			return 24000;
		case VERSION_2_5:
			return 12000;
		}
	case(char)0x02:
		switch (Mp3FileV2::MP3_VERSION) {
		case VERSION_1:
			return 32000;
		case VERSION_2:
			return 16000;
		case VERSION_2_5:
			return 8000;
		}
	default:
		return -2;
	}
}
int Mp3FileV2::getSamples() {
	switch (Mp3FileV2::MP3_VERSION){
	case Mp3FileV2::VERSION_1:
		switch (Mp3FileV2::MP3_LAYER){
		case Mp3FileV2::LAYER_I:
			return 384;
		case Mp3FileV2::LAYER_II:
		case Mp3FileV2::LAYER_III:
			return 1152;

		}
	case Mp3FileV2::VERSION_2:
		switch (Mp3FileV2::MP3_LAYER) {
		case Mp3FileV2::LAYER_I:
			return 384;
		case Mp3FileV2::LAYER_II:
			return 1152;
		case Mp3FileV2::LAYER_III:
			return 576;
		}
	case Mp3FileV2::VERSION_2_5:
		switch (Mp3FileV2::MP3_LAYER) {
		case Mp3FileV2::LAYER_I:
			return 384;
		case Mp3FileV2::LAYER_II:
			return 1152;
		case Mp3FileV2::LAYER_III:
			return 576;
		}

	default:
		return -2;
	}
}


/* Parses first Mp3 frame looking for a VBR header returns an int based on success
*-1 = Reached end of stream
* 0 = Found a Xing/Info header
* 1 = Found a VBRI header
* 2 = Found another Mp3 header
*
* If 2 is returned the file is either CBR or the average bitrate must be found. To be safe we look at a couple other frames
* and compare the bitrates to check for consistency. 
*
*
*/
int Mp3FileV2::checkForVBRHeader(){
	char VBRHeader[4];
	int tempPos = 0;
	while (mStream->get(VBRHeader[0])) {
		if (VBRHeader[0] == 'X' || VBRHeader[0] == 'I') {
			tempPos = mStream->tellg();
			mStream->read(&VBRHeader[1], 3);
			if ((VBRHeader[1] == 'i' && VBRHeader[2] == 'n' & VBRHeader[3] == 'g') ||
				(VBRHeader[1] == 'n' && VBRHeader[2] == 'f' & VBRHeader[3] == 'o')) {
					mStream->seekg(tempPos - 1);
					return 0;
			}
			else {
				mStream->seekg(tempPos);
			}
		}
		else if (VBRHeader[0] == 'V') {
			tempPos = mStream->tellg();
			mStream->read(&VBRHeader[1], 3);
			if (VBRHeader[1] == 'B' && VBRHeader[2] == 'R' & VBRHeader[3] == 'I') {
				mStream->seekg(tempPos - 1);
				return 1;
			}

		}
		else if (VBRHeader[0] == (char)0xff && ((mStream->peek() >> 5) == (char)0x07)) {
			//Seek to the char before the Mp3 header so that the getAverageBitrate reads the right char
			mStream->unget();
			return 2;
		}
	}
	return -1;
}


/*Sets the number of frames that the Xing frame identifies.
Returns 0 if header contains number of frames field.
Returns 2 if header doesn't contain number of frames field.
	returns two so that the average bitrate will be found

*/
int Mp3FileV2::parseXingHeader(char * xingBuffer){
	///Flags that aren't necessary at this time

	//bool framesField = xingBuffer[7] & (char)0x01;
	//bool bytesField = xingBuffer[7] & (char)0x02;
	//bool tocfield = xingBuffer[7] & (char)0x04;
	//bool qualityfield = xingBuffer[7] & (char)0x08;
	
	if (xingBuffer[7] & (char)0x01) {
		Mp3FileV2::MP3_NUM_OF_FRAMES = xingBuffer[8] << 24 | xingBuffer[9] << 16 | xingBuffer[10] << 8 | xingBuffer[11];
		return 0;
	}
	return 2;
	//Other two flags not necessary at this time
}

/*
Sets the number of frames field
*/
void Mp3FileV2::parseVBRIHeader(char * vbriBuffer){
	Mp3FileV2::MP3_NUM_OF_FRAMES = vbriBuffer[14] << 24 | vbriBuffer[15] << 16 | vbriBuffer[16] << 8 | vbriBuffer[17];
}


/*
Determines the bitrate of file that doesn't contain a bitrate related frame. Returns bitrate of file 
or -1 if file is invalid
*/
int Mp3FileV2::getAverageBitrate(){
	int tempPos = mStream->tellg();
	mStream->seekg(AudioFile::mFileSize / 3);
	int bitrate1 = -1;
	int bitrate2 = -1;
	char c;
	while (mStream->get(c)) {
		if (c == (char)0xff && ((mStream->peek() >> 5) == (char)0x07)) {

			mStream->get(c);
			mStream->get(c);
			if (bitrate1 == -1) {
				bitrate1 = findBitrate(c);
				mStream->seekg(mStream->tellg()*2);
			}
			else {
				bitrate2 = findBitrate(c);
				break;
			}
		}
	}
	if (AudioFile::mBitrate == bitrate1 && bitrate1 == bitrate2) {
		return AudioFile::mBitrate;
	}
	else {
		int frames = 1;
		int totalBitrate = AudioFile::mBitrate;
		mStream->seekg(tempPos);
		int allowedAttempts = AudioFile::mFileSize / 50;
		int attempts = 0;
		while (mStream->get(c)) {
			if (c == (char)0xff && ((mStream->peek() >> 5) == (char)0x07)) {

				mStream->get(c);
				mStream->get(c);
				
				int frameBitrate = findBitrate(c);
				if (frameBitrate > 0) {
						totalBitrate += frameBitrate;
						frames++;
						int frameSize = calculateFrameSize(getSamples(), frameBitrate, findSampleRate(c), (c & 0x02));
						int currentOffset = mStream->tellg();
						if (frameSize < 0) {
							int pos = mStream->tellg();
							return-1;
						}

						//Subtract 3 from offset for already being 3 bytes in
						mStream->seekg(currentOffset-3 + frameSize);

						attempts = 0;
				}
			}
			attempts++;
			if (attempts>allowedAttempts){
				return -1;
			}
		}
		return totalBitrate / frames;
	}
}


inline int Mp3FileV2::calculateFrameSize(float samplesPerFrame, float frameBitrate, float samplerate, float padding){
	return ((samplesPerFrame / 8 * frameBitrate * 1000) / samplerate) + padding;
}

inline int Mp3FileV2::calculateDuration(float fileSizeInBytes, float bitrate) {
	return ((fileSizeInBytes * 8) / (bitrate * 1000)) * 1000;
}

inline int Mp3FileV2::calculateDuration(float totalNumOfFrames, float samplesPerFrame, float samplerate){
	return (totalNumOfFrames * samplesPerFrame / samplerate) * 1000;
}

inline int Mp3FileV2::calculateBitrate(float fileSizeInBytes, float durationInMilli) {
	return ((fileSizeInBytes * 8) / (durationInMilli * 1000))*1000;
}


/*


DURATION
 
DURATION IN MS = ((filesize in bits)/(bitrate * 1000)) * 1000

DURATION IN MS = (Total frame count * Samples per frame / Samplerate)*1000 

FRAMESIZE

FRAMESIZE IN BYTES = (((Samples Per Frame / 8 * Bitrate) / Sampling Rate) + Padding Size) * 1000


*/





















/*
Need to take the new tag and copy it to a new stream that also include 
*/

int Mp3FileV2::attachNewTag(Tag * newTag){
	if (hasHeader() == 0) {
		AudioFile::mAudioSize = AudioFile::mFileSize - Mp3FileV2::mId3Tag->getTagSize();
		mStream->seekg(Mp3FileV2::mId3Tag->getTagSize());
	}
	else {
		AudioFile::mAudioSize = AudioFile::mFileSize;
		mStream->seekg(0);
	}

	AudioFile::mFileData = newTag->generateTags(AudioFile::mAudioSize);
		mStream->read((char*) &AudioFile::mFileData[newTag->getTagSize()], AudioFile::mAudioSize);
		AudioFile::mFileSize = newTag->getTagSize() + AudioFile::mAudioSize;
		mStream->read((char*)&AudioFile::mFileData[newTag->getTagSize()], AudioFile::mAudioSize);

	return 0;
}


//Abstraction layer for creating the tag
int Mp3FileV2::saveNewTag(Tag * newTag) {
	if (attachNewTag(newTag) == 0) {
		mStream->close();
		ofstream outStream(getFilePath(), ios::trunc | ios::beg | ios::binary);
		if (outStream.is_open()) {
			outStream.write((char*)AudioFile::mFileData, AudioFile::mFileSize);
		}
		else {
			return -2;
		}
	}
	else {
		return -1;
	}
}






//NEED TO PREPARE A TAG AND ATTACH INSTEAD OF CREATE A NEW MP3

//Mp3FileV2::Mp3FileV2(vector<char> deserialize) : AudioFile(deserialize) {
//    mId3Tag = new ID3TagV2;
//    setID(int((unsigned char) deserialize[1] << 24 |
//              (unsigned char) deserialize[2] << 16 |
//              (unsigned char) deserialize[3] << 8 |
//              (unsigned char) deserialize[4]));
//    int assign = 0;
//    int offset = 5;
//    while (offset < deserialize.size()) {
//        unsigned long length = (unsigned long) ((unsigned char) deserialize[offset] << 24 |
//                                                (unsigned char) deserialize[offset + 1] << 16 |
//                                                (unsigned char) deserialize[offset + 2] << 8 |
//                                                (unsigned char) deserialize[offset + 3]);
//        offset += 4;
//        vector<char> subBuffer(deserialize.begin() + offset, deserialize.begin() + offset + length);
//        if (length > 0) {
//            switch (assign) {
//                case 0:
//                    getTag()->setTitle(string(subBuffer.data(), length));
//                    break;
//                case 1:
//                    getTag()->setArtist(string(subBuffer.data(), length));
//                    break;
//                case 2:
//                    getTag()->setAlbum(string(subBuffer.data(), length));
//                    break;
//                case 3:
//                    getTag()->setTrack(string(subBuffer.data(), length));
//                    break;
//                case 4:
//                    getTag()->setYear(string(subBuffer.data(), length));
//                    break;
//                case 5:
//                    setFilePath(string(subBuffer.data(), length));
//                    break;
//                case 6:
//                    //getTag()->setCover((unsigned char *) subBuffer.data());
//                    //getTag()->setCoverSize(length);
//                default:
//                    break;
//            }
//        }
//        offset += length;
//        assign++;
//    }
//}

Mp3FileV2::~Mp3FileV2() {
    delete mId3Tag;
    mId3Tag = nullptr;
}

Tag *Mp3FileV2::getTag() {
    return mId3Tag;
}

//FIXME trims the first little bit of audio
//void Mp3FileV2::setAudio() {
//    mAudioData.resize(getFileSize() - mId3Tag->getTagSize());
//    mStream->seekg(mId3Tag->getTagSize(),ios_base::beg);
//    AudioFile::setAudio();
//}
