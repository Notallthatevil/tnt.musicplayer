#ifndef TAGGER_SQLHELPER_H
#define TAGGER_SQLHELPER_H

#include "sqlite3.h"
#include <android/log.h>
#include <jni.h>
#include <sys/stat.h>
#include <sstream>
#include "../tagger/files/AudioFile.h"
#include <string>
#include <map>

typedef std::map<std::string, int> map;

class SqlWrapper {
private:
    const std::string DATABASE_DIRECTORY = "/data/data/com.trippntechnology.tntmusicplayer/databases";
    const std::string DATABASE_NAME = DATABASE_DIRECTORY + "/TNT.db";

    const std::string SONG_ID = "ID";
    const std::string SONG_ALBUM_ID = "ALBUM_ID";

    const std::string SONG_TITLE = "TITLE";
    const std::string SONG_ALBUM = "ALBUM";
    const std::string SONG_ARTIST = "ARTIST";
    const std::string SONG_YEAR = "YEAR";
    const std::string SONG_TRACK = "TRACK";

    const std::string SONG_FILEPATH = "FILEPATH";
    const std::string SONG_DURATION = "DURATION";
    const std::string SONG_SAMPLERATE = "SAMPLERATE";
    const std::string SONG_BITRATE = "BITRATE";

    const std::string SONG_LAST_MODIFIED = "MODIFIED_DATE";

    const std::string ALBUM_ID = "ID";
    const std::string ALBUM_NAME = "ALBUM_NAME";
    const std::string ALBUM_ARTWORK = "ARTWORK";

    enum tableColumnNumbers {
        SONG_ID_COLUMN = 0,
        SONG_ALBUM_ID_COLUMN = 1,
        SONG_TITLE_COLUMN = 2,
        SONG_ALBUM_COLUMN = 3,
        SONG_ARTIST_COLUMN = 4,
        SONG_YEAR_COLUMN = 5,
        SONG_TRACK_COLUMN = 6,
        SONG_FILEPATH_COLUMN = 7,
        SONG_DURATION_COLUMN = 8,
        SONG_SAMPLERATE_COLUMN = 9,
        SONG_BITRATE_COLUMN = 10,
        SONG_LAST_MODIFIED_COLUMN = 11,

        ALBUM_ID_COLUMN = 0,
        ALBUM_NAME_COLUMN = 1,
        ALBUM_ARTWORK_COLUMN = 2
    };

    sqlite3 *mDb;

    static int callback(void *count, int argc, char **argv, char **azColName) {
        int *c = (int *) count;
        *c = atoi(argv[0]);
        return 0;
    }

    int insertAlbum(std::string albumName, unsigned char *cover, long coverSize);

    int updateAlbumCover(std::string albumName, unsigned char *cover, long coverSize);

    int getAlbumId(std::string albumName, unsigned char *cover, long coverSize);

    int getNumberOfEntries(const std::string &tableName);

    int updateSong(sqlite3_stmt **stmt, Tag *tag, long lastModifiedTime);

public:
    const std::string SONG_TABLE = "SONGS";
    const std::string ALBUM_TABLE = "ALBUMS";

    SqlWrapper();

    ~SqlWrapper();

    int deleteTable(std::string tableName);

    int createTable(std::string tableName);

    int insertSong(AudioFile *audioFile);

    int deleteAudioFileByFilePath(std::string filePath);

    jobjectArray retrieveAllSongs(JNIEnv *env);

    int updateSong(Tag *tag, int ID, long lastModifiedTime);

    int updateSong(Tag *tag, std::string filePath, long lastModifiedTime);


    bool tableExist(std::string tableName);

    map retrieveAllFilePaths();

};


#endif //TAGGER_SQLHELPER_H
