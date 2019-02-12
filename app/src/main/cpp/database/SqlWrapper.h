#ifndef TNT_MUSICPLAYER_SQLWRAPPER_H
#define TNT_MUSICPLAYER_SQLWRAPPER_H

#include "sqlite3.h"
#include <android/log.h>
#include <jni.h>
#include <sys/stat.h>
#include <sstream>
#include "../tagger/files/AudioFile.h"
#include <string>
#include <map>
#include <vector>

typedef std::map<std::string, int> map;

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

    const std::string SONG_FILEPATH = "FILEPATH";
    const std::string SONG_DURATION = "DURATION";
    const std::string SONG_SAMPLERATE = "SAMPLERATE";
    const std::string SONG_BITRATE = "BITRATE";

    const std::string SONG_COVER_OFFSET = "COVER_OFFSET";
    const std::string SONG_COVER_SIZE = "COVER_SIZE";

    const std::string SONG_LAST_MODIFIED = "MODIFIED_DATE";

    enum tableColumnNumbers {
        SONG_ID_COLUMN = 0,
        SONG_TITLE_COLUMN = 1,
        SONG_ALBUM_COLUMN = 2,
        SONG_ARTIST_COLUMN = 3,
        SONG_YEAR_COLUMN = 4,
        SONG_TRACK_COLUMN = 5,
        SONG_FILEPATH_COLUMN = 6,
        SONG_DURATION_COLUMN = 7,
        SONG_SAMPLERATE_COLUMN = 8,
        SONG_BITRATE_COLUMN = 9,
        SONG_COVER_OFFSET_COLUMN = 10,
        SONG_COVER_SIZE_COLUMN = 11,
        SONG_LAST_MODIFIED_COLUMN = 12

    };

    sqlite3 *mDb;

    JNIEnv * env = nullptr;
    JavaVM *javaVM;
    jobject jLiveData = nullptr;
    jmethodID postValue = nullptr;

    bool transaction = false;

    static int callback(void *count, int argc, char **argv, char **azColName) {
        int *c = (int *) count;
        *c = atoi(argv[0]);
        return 0;
    }

    int commitChanges(sqlite3_stmt *pStmt);

    int getNumberOfEntries(const std::string &tableName);

    int updateSong(sqlite3_stmt **stmt, Tag *tag, long lastModifiedTime);

    jobjectArray retrieveAllSongs(JNIEnv *env);

    SqlWrapper();

public:
    static const std::string SONG_TABLE;

    static SqlWrapper& getInstance();

    ~SqlWrapper();

    int insertAudioFile(AudioFile *audioFile);

    int deleteTable(std::string tableName);

    int createTable(std::string tableName);

    int updateSong(Tag *tag, int ID, long lastModifiedTime);

    int updateSong(Tag *tag, std::string filePath, long lastModifiedTime);

    int deleteAudioFileByFilePath(std::string filePath);

    map retrieveAllFilePaths();

    bool tableExist(std::string tableName);

    jobject getLiveData(JNIEnv*env);

    void beginTransaction();

    void closeTransaction();

    SqlWrapper(SqlWrapper const &) = delete;

    void operator=(SqlWrapper const &) = delete;
};

#endif //TNT_MUSICPLAYER_SQLWRAPPER_H
