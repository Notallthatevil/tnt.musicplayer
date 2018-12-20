//
// Created by natet on 4/7/2018.
//

#ifndef TAGGER_SQLHELPER_H
#define TAGGER_SQLHELPER_H

#include <string>
#include "sqlite3.h"
#include <android/log.h>
#include <jni.h>
#include <sys/stat.h>
#include <sstream>
#include "../Files/AudioFile.h"
#include <vector>

using namespace std;


class SqlHelper {
private:
    const string DATABASE_DIRECTORY = "/data/data/com.trippntechnology.tagger/databases";
    const string DATABASE_NAME = DATABASE_DIRECTORY+"/TNT.db";

    const string SONG_ID = "ID";
    const string SONG_TITLE = "TITLE";
    const string SONG_ARTIST = "ARTIST";
    const string SONG_ALBUM = "ALBUM";
    const string SONG_TRACK = "TRACK";
    const string SONG_YEAR = "YEAR";
    const string SONG_FILEPATH = "FILEPATH";
    const string SONG_COVER = "ARTWORK";

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
    const string SONG_TABLE = "SONGS";

    SqlHelper();

    ~SqlHelper();

    int dropTable(string tableName);

    int createTable(string tableName);

    int insertSong(AudioFile *audioFile);

    int updateSong(AudioFile *audioFile, int ID);

    string selectSong(AudioFile *audioFile);

    jobjectArray retrieveAllSongs(JNIEnv *env);

};

struct databaseCreationError : public exception {

    const char *what() const throw() {
        return "Unable to create database ";
    }
};


#endif //TAGGER_SQLHELPER_H
