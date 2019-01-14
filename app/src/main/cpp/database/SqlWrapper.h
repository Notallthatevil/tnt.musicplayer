#ifndef TAGGER_SQLHELPER_H
#define TAGGER_SQLHELPER_H

#include "sqlite3.h"
#include <android/log.h>
#include <jni.h>
#include <sys/stat.h>
#include <sstream>
#include "../tagger/files/AudioFile.h"
#include <vector>
#include <string>

#pragma TNT.auto_vacuum = 1
class SqlWrapper {
private:
    const std::string DATABASE_DIRECTORY = "/data/data/com.trippntechnology.tntmusicplayer/databases";
    const std::string DATABASE_NAME = DATABASE_DIRECTORY + "/TNT.db";

    const std::string SONG_ID = "ID";

    const std::string SONG_TITLE = "TITLE";
    const std::string SONG_ALBUM = "ALBUM";
    const std::string SONG_ARTIST = "ARTIST";
    const std::string SONG_YEAR = "YEAR";
    const std::string SONG_TRACK = "TRACK";
    const std::string SONG_COVER = "ARTWORK";

    const std::string SONG_FILEPATH = "FILEPATH";
    const std::string SONG_DURATION = "DURATION";
    const std::string SONG_SAMPLERATE = "SAMPLERATE";
    const std::string SONG_BITRATE = "BITRATE";
    enum tableColumnNumbers {
        SONG_ID_COLUMN = 0,
        SONG_TITLE_COLUMN = 1,
        SONG_ALBUM_COLUMN = 2,
        SONG_ARTIST_COLUMN = 3,
        SONG_YEAR_COLUMN = 4,
        SONG_TRACK_COLUMN = 5,
        SONG_COVER_COLUMN = 6,
        SONG_FILEPATH_COLUMN = 7,
        SONG_DURATION_COLUMN = 8,
        SONG_SAMPLERATE_COLUMN = 9,
        SONG_BITRATE_COLUMN = 10
    };

    sqlite3 *mDb;
    sqlite3_stmt *stmt;

    static int callback(void *count, int argc, char **argv, char **azColName) {
        int *c = (int *) count;
        *c = atoi(argv[0]);
        return 0;
    }

public:
    const std::string SONG_TABLE = "SONGS";

    SqlWrapper();

    ~SqlWrapper();

    int deleteTable(std::string tableName);

    int createTable(std::string tableName);

    int insertSong(AudioFile *audioFile);

    jobjectArray retrieveAllSongs(JNIEnv *env);

    int updateSong(Tag *tag, int ID);

    std::string selectSong(AudioFile *audioFile);

};

struct databaseCreationError : public std::exception {

    const char *what() const noexcept override {
        return "Unable to create database ";
    }
};


#endif //TAGGER_SQLHELPER_H
