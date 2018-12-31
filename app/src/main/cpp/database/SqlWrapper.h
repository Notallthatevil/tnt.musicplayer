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


class SqlWrapper {
private:
    const std::string DATABASE_DIRECTORY = "/data/data/com.trippntechnology.tagger/databases";
    const std::string DATABASE_NAME = DATABASE_DIRECTORY+"/TNT.db";

    const std::string SONG_ID = "ID";
    const std::string SONG_TITLE = "TITLE";
    const std::string SONG_ARTIST = "ARTIST";
    const std::string SONG_ALBUM = "ALBUM";
    const std::string SONG_TRACK = "TRACK";
    const std::string SONG_YEAR = "YEAR";
    const std::string SONG_FILEPATH = "FILEPATH";
    const std::string SONG_COVER = "ARTWORK";

    const int SONG_ID_COLUMN = 0;
    const int SONG_TITLE_COLUMN = 1;
    const int SONG_ARTIST_COLUMN = 2;
    const int SONG_ALBUM_COLUMN = 3;
    const int SONG_TRACK_COLUMN = 4;
    const int SONG_YEAR_COLUMN = 5;
    const int SONG_FILEPATH_COLUMN = 6;
    const int SONG_COVER_COLUMN = 7;

    sqlite3 *mDb;
    sqlite3_stmt *stmt;

public:
    const std::string SONG_TABLE = "SONGS";

    SqlWrapper();

    ~SqlWrapper();

    int deleteTable(std::string tableName);

    int createTable(std::string tableName);

    int insertSong(AudioFile *audioFile);

    int updateSong(AudioFile *audioFile, int ID);

    std::string selectSong(AudioFile *audioFile);

    jobjectArray retrieveAllSongs(JNIEnv *env);

};

struct databaseCreationError : public std::exception {

    const char *what() const throw() override {
        return "Unable to create database ";
    }
};


#endif //TAGGER_SQLHELPER_H
