//
// Created by Nate on 5/21/2018.
//

#ifndef TAGGER_Mp3File_H
#define TAGGER_Mp3File_H


#include "AudioFile.h"
#include "../tags/ID3Tag.h"

class Mp3File : public AudioFile {

private:
    enum Constants {
        VERSION_1 = 0x03,
        VERSION_2 = 0x02,
        VERSION_2_5 = 0x00,
        LAYER_I = 0x03,
        LAYER_II = 0x02,
        LAYER_III = 0x01
    };

    inline int calculateFrameSize(float samplesPerFrame, float frameBitrate, float sampleRate, float padding);

    inline int calculateDuration(float fileSizeInBytes, float bitrate);

    inline int calculateDuration(float totalNumOfFrames, float samplesPerFrame, float sampleRate);

    inline int calculateBitrate(float fileSizeInBytes, float durationInMilli);


protected:
    char mMp3Version = -1;
    char mMp3Layer = -1;
    unsigned int mNumOfFrames = 0;
    ID3Tag *mId3Tag = nullptr;


    Mp3File():AudioFile() {}

    int parseMp3Data();

    int findBitrate(char bitrateChar);

    int findSampleRate(char sampleRateChar); // in Hz

    int getSamples();

    int checkForVBRHeader();

    int parseXingHeader(char *xingBuffer);

    void parseVBRIHeader(char *vbriBuffer);

    int getAverageBitrate();

    int hasHeader();

    int attachNewTag(Tag *newTag);


public:

    explicit Mp3File(string *filePath) : AudioFile(filePath){}

    virtual ~Mp3File();

    /*
     * Parses the Mp3File for relevant data.
     * If findTags is set then the file will try and parse an ID3 tag if it exists.
     * Otherwise mp3 file information is parsed and that's it.
     *
     * Returns
     * -2 if the file is not open yet. open() must be called before an operations can be done.
     * -1 if there was no file to parse
     * 0 if successful
     * 1 if the file is not a valid Mp3File
     */
    int parse(bool findTags);

    /*
     * Adds the new ID3Tag to the file and replaces the old one if it existed
     * Returns
     * -2 if an output stream couldn't be opened
     * 0 if successful
     */
    int saveNewTag(Tag *newTag) override;

    Tag *getTag() override;


};


#endif //TAGGER_Mp3File_H
