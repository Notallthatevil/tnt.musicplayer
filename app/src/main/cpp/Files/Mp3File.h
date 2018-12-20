//
// Created by Nate on 5/21/2018.
//

#ifndef TAGGER_MP3FILEV2_H
#define TAGGER_MP3FILEV2_H


#include "AudioFile.h"
#include "../Tags/ID3TagV2.h"

class Mp3FileV2 : public AudioFile {

private:
	enum Constants {
		VERSION_1 = 0x03,
		VERSION_2 = 0x02,
		VERSION_2_5 = 0x00,
		LAYER_I = 0x03,
		LAYER_II = 0x02,
		LAYER_III = 0x01
	};

	inline int calculateFrameSize(float samplesPerFrame, float frameBitrate, float samplerate, float padding);
	inline int calculateDuration(float fileSizeInBytes, float bitrate);
	inline int calculateDuration(float totalNumOfFrames, float samplesPerFrame, float samplerate);
	inline int calculateBitrate(float fileSizeInBytes, float durationInMilli);



protected:

	char MP3_VERSION = -1;
	char MP3_LAYER = -1;

	unsigned int MP3_NUM_OF_FRAMES = -1;
	unsigned int MP3_NUM_OF_BYTES = -1;


	Mp3FileV2() {}
	int parseMp3Data();
	int findBitrate(char bitrateChar);
	int findSampleRate(char sampleRateChar); // in Hz
	int getSamples();
	int checkForVBRHeader();
	int parseXingHeader(char *xingBuffer);
	void parseVBRIHeader(char *vbriBuffer);
	int getAverageBitrate();
	int hasHeader();
	int attachNewTag(Tag *tagnewTag);

	ID3TagV2 *mId3Tag = nullptr;

public:
	Mp3FileV2(string *filePath) :AudioFile(filePath) {}

    Mp3FileV2(string *filePath, bool findTags);

    //Mp3FileV2(vector<char> deserialize);


    virtual ~Mp3FileV2();

    //void setAudio() override;

	int parse(bool findTags);

    Tag *getTag() override;

	int saveNewTag(Tag * newTag) override;

};


#endif //TAGGER_MP3FILEV2_H
