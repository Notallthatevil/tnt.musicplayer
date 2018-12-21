//
// Created by Nate on 5/21/2018.
//

#ifndef TAGGER_FILE_H
#define TAGGER_FILE_H

#include <string>
#include <fstream>
#include <vector>
#include "../Tags/Tag.h"

using namespace std;

class AudioFile {
private:
protected:
    AudioFile() = default;

    int mBitrate = 0;
    int mSampleRate = 0;
    long mDuration = 0;  // milliseconds
    long mSqlID = -1;
    bool mIsOpen = false;
    unsigned long mFileSize = 0;
    unsigned long mAudioSize = 0;
    unsigned char *mFileData = nullptr;
    string mFilePath;
    ifstream *mStream = nullptr;


public:
    /*
     * Basic constructor. Identifies the file to be opened.
     */
    explicit AudioFile(string *filePath);

    virtual ~AudioFile();

    /*
     * Attempts to open the file.
     *
     * Returns true if successful or if the file is already opened.
     * Otherwise returns false.
     */
    bool open();

    /*
     * Abstract method.
     * Returns the associated tag
     */
    virtual Tag *getTag() = 0;

    /*
     * Abstract method.
     * Creates and attaches the new tag to the AudioFile.
     * Open must be called prior
     */
    virtual int saveNewTag(Tag *newTag) = 0;

    unsigned long getFileSize() const;

    long getDuration() const;

    long getID() const;

    void setID(long ID);

    string getFilePath() const;
};

#endif  // TAGGER_FILE_H