//
// Created by Nate on 5/21/2018.
//

#ifndef TAGGER_FILE_H
#define TAGGER_FILE_H

#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include "Tag.h"
#ifndef WIN32
#include <unistd.h>
#endif
#ifdef WIN32
#define stat _stat
#endif


class AudioFile {
private:
protected:
    AudioFile() = default;

    struct stat result;
    int mBitrate = 0;
    int mSampleRate = 0;
    long mDuration = 0;  // milliseconds
    long mLastModified = 0; // Time in seconds since Jan 1 1970
    long mSqlID = -1;
    bool mIsOpen = false;
    unsigned long mFileSize = 0;
    unsigned long mAudioSize = 0;
    unsigned char *mFileData = nullptr;
    std::string mFilePath;
    std::ifstream *mStream = nullptr;


public:
    /*
     * Basic constructor. Identifies the file to be opened.
     */
    explicit AudioFile(std::string *filePath){
            mFilePath = *filePath;
    }

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
    virtual long saveNewTag(Tag *newTag) = 0;

    unsigned long __unused getFileSize() const;

    long getDuration() const;

    long __unused getID() const;

    void __unused setID(long ID);

    int getBitrate() const;
    int getSampleRate()const;
    long getLastModified() const;
    std::string getFilePath() const;
};

#endif  // TAGGER_FILE_H