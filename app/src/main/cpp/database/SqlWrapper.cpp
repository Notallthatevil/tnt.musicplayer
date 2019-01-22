#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

#include "SqlWrapper.h"

inline bool file_exists(const std::string &name) {
    struct stat buffer{};
    return (stat(name.c_str(), &buffer) == 0);
}

/*
 * Creates database directory if it doesn't exist then opens the database connection.
 */
SqlWrapper::SqlWrapper() {
    //Warning setting this is deprecated but have no choice as no TMPDIR global variable available for android
    sqlite3_temp_directory = (char *) "/data/user/0/com.trippntechnology.tntmusicplayer/cache";
    if (!file_exists(DATABASE_DIRECTORY)) {
        mkdir(DATABASE_DIRECTORY.c_str(), S_IRWXU | S_IRWXG | S_IXOTH);
    }
    int rc = sqlite3_open_v2(DATABASE_NAME.c_str(), &mDb, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE,
                             nullptr);
    if (rc) {
        throw databaseCreationError();
    }
}

/*
 * Basic destructor
 * Closes the database connection
 */
SqlWrapper::~SqlWrapper() {
    sqlite3_close_v2(mDb);
}

/*
 * Creates the database
 *
 * @param tableName - The name of the database table
 */
int SqlWrapper::createTable(std::string tableName) {
    if (tableName == SONG_TABLE) {
        deleteTable(tableName);
        std::string sql = "CREATE TABLE " + SONG_TABLE + "(" +
                          SONG_ID + " INTEGER PRIMARY KEY AUTOINCREMENT, " +

                          SONG_TITLE + " TEXT, " +
                          SONG_ALBUM + " TEXT, " +
                          SONG_ARTIST + " TEXT, " +
                          SONG_YEAR + " TEXT, " +
                          SONG_TRACK + " TEXT, " +

                          SONG_FILEPATH + " TEXT NOT NULL, " +
                          SONG_DURATION + " BIGINT, " +
                          SONG_SAMPLERATE + " INT, " +
                          SONG_BITRATE + " INT, " +

                          SONG_COVER + " BLOB);";

        sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr);
        sqlite3_step(stmt);
        return sqlite3_finalize(stmt);
    }
    return -1;
}

/*
 * Deletes the specified table
 *
 * @param tableName - The name of the table to be deleted
 */
int SqlWrapper::deleteTable(std::string tableName) {
    std::string sql = "DROP TABLE IF EXISTS " + tableName;
    sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);
    return sqlite3_finalize(stmt);

}

bool SqlWrapper::tableExist(std::string tableName) {
    std::string sql = "SELECT name FROM sqlite_master WHERE type='table' AND name=\'" + tableName + "\'";

    int rc = sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr);

    if (rc == SQLITE_OK) {
        rc = sqlite3_step(stmt);

        //was found?
        if (rc == SQLITE_ROW) {
            sqlite3_clear_bindings(stmt);
            sqlite3_reset(stmt);
            sqlite3_finalize(stmt);
            return true;
        }
    }
    sqlite3_finalize(stmt);
    return false;
}

/*
 * Adds a song to the database while also making sure the contents in the file
 * are ready to be inserted.
 *
 * @param audioFile - Pointer to the audio file to be added
 */
int SqlWrapper::insertSong(AudioFile *audioFile) {

    std::string sql = "INSERT INTO " + SONG_TABLE +
                      "(TITLE,ALBUM,ARTIST,YEAR,TRACK,FILEPATH,DURATION,SAMPLERATE,BITRATE,ARTWORK) " +
                      "VALUES(?1,?2,?3,?4,?5,?6,?7,?8,?9,?10);";
    int rc = sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQLITE", "prepare failed: %s",
                            sqlite3_errmsg(mDb));
    } else {
        const char *title =
                !audioFile->getTag()->getTitle().empty() ? audioFile->getTag()->getTitle().c_str() : nullptr;
        const char *album =
                !audioFile->getTag()->getAlbum().empty() ? audioFile->getTag()->getAlbum().c_str() : nullptr;
        const char *artist =
                !audioFile->getTag()->getArtist().empty() ? audioFile->getTag()->getArtist().c_str() : nullptr;
        const char *year =
                !audioFile->getTag()->getYear().empty() ? audioFile->getTag()->getYear().c_str() : nullptr;
        const char *track =
                !audioFile->getTag()->getTrack().empty() ? audioFile->getTag()->getTrack().c_str() : nullptr;

        if (audioFile->getTag()->getTitle() == "gold") {
            int z = 0;
        }
        ///Hardcoded numbers correspond to the position in values in above sql statement starting at 1
        sqlite3_bind_text(stmt, 1, title, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, album, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, artist, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, year, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, track, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, audioFile->getFilePath().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt, 7, audioFile->getDuration());
        sqlite3_bind_int(stmt, 8, audioFile->getSampleRate());
        sqlite3_bind_int(stmt, 9, audioFile->getBitrate());
        sqlite3_bind_blob(stmt, 10, audioFile->getTag()->getCover(), audioFile->getTag()->getCoverSize(),
                          SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        if (rc!=SQLITE_DONE) {
            __android_log_print(ANDROID_LOG_ERROR, "SQLITE", "Error inserting into database %d:    %s",
                                sqlite3_extended_errcode(mDb), sqlite3_errmsg(mDb));
        }
    }
    sqlite3_clear_bindings(stmt);
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    return rc;
}

int SqlWrapper::deleteAudioFileByFilePath(std::string filePath) {
    std::string sql = "DELETE FROM " + SONG_TABLE + " WHERE " + SONG_FILEPATH + " = \"" + filePath + "\";";
    int rc = sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQLITE", "Error deleting from database %d:     %s",
                            sqlite3_extended_errcode(mDb),sqlite3_errmsg(mDb));
        return rc;
    }
    rc = sqlite3_step(stmt);
    if(rc!=SQLITE_DONE){
        __android_log_print(ANDROID_LOG_ERROR, "SQLITE", "Error deleting from database %d:     %s",
                            sqlite3_extended_errcode(mDb),sqlite3_errmsg(mDb));
    }
    sqlite3_step(stmt);
    return sqlite3_finalize(stmt);
}


jobjectArray SqlWrapper::retrieveAllSongs(JNIEnv *env) {
    int numberOfItems = 0;
    char *error = nullptr;

    std::string sql = "SELECT count(id) FROM " + SONG_TABLE;
    sqlite3_exec(mDb, sql.c_str(), callback, &numberOfItems, &error);

    std::string constructorParameters = "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[BLjava/lang/String;JII)V";

    jclass jAudioFile = env->FindClass("com/trippntechnology/tntmusicplayer/objects/AudioFile");
    jmethodID jConstructor = env->GetMethodID(jAudioFile, "<init>", constructorParameters.c_str());

    jobjectArray jAudioArray = env->NewObjectArray(numberOfItems, jAudioFile, nullptr);

    sql = "SELECT * FROM " + SONG_TABLE + " ORDER BY " + SONG_TITLE + " COLLATE NOCASE ASC";
    sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr);

    for (int i = 0; i < numberOfItems; i++) {
        if (sqlite3_step(stmt) != SQLITE_ROW) {
            __android_log_print(ANDROID_LOG_ERROR, "SQLITE",
                                "Reached last row before number of items was read: Error code %d",
                                sqlite3_extended_errcode(mDb));
            return nullptr;
        }

        jint jid = sqlite3_column_int(stmt, SONG_ID_COLUMN);
        //Tags
        jstring jTitle = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_TITLE_COLUMN));
        jstring jAlbum = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_ALBUM_COLUMN));
        jstring jArtist = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_ARTIST_COLUMN));
        jstring jYear = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_YEAR_COLUMN));
        jstring jTrack = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_TRACK_COLUMN));
        //Cover
        int length = sqlite3_column_bytes(stmt, SONG_COVER_COLUMN);
        char *blob = (char *) sqlite3_column_blob(stmt, SONG_COVER_COLUMN);
        jbyteArray jCover;
        if (blob != nullptr) {
            jCover = env->NewByteArray(length);
            env->SetByteArrayRegion(jCover, 0, length, (jbyte *) blob);
        } else {
            jCover = nullptr;
        }
        //Audio data
        jstring jFilepath = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_FILEPATH_COLUMN));
        jlong jDuration = sqlite3_column_int64(stmt, SONG_DURATION_COLUMN);
        jint jSampleRate = sqlite3_column_int(stmt, SONG_SAMPLERATE_COLUMN);
        jint jBitrate = sqlite3_column_int(stmt, SONG_BITRATE_COLUMN);

        jobject jSong = env->NewObject(jAudioFile, jConstructor,
                                       jid, jTitle, jAlbum, jArtist, jYear, jTrack, jCover, jFilepath, jDuration,
                                       jSampleRate, jBitrate);

        env->SetObjectArrayElement(jAudioArray, i, jSong);
    }
    sqlite3_finalize(stmt);
    return jAudioArray;
}

/*
 * Updates the selected audio file based on the id passed in
 *
 * @param audioFile - The new audio file to be updated
 * @param ID - The ID of the database entry to be updated
 */
int SqlWrapper::updateSong(Tag *tag, int ID) {
    std::string sql = "UPDATE " + SONG_TABLE + " SET " +
                      SONG_TITLE + " = :TIT, " +
                      SONG_ALBUM + " = :ALB, " +
                      SONG_ARTIST + " = :ART, " +
                      SONG_YEAR + " = :YEA, " +
                      SONG_TRACK + " = :TRA, " +
                      SONG_COVER + " = :COV WHERE " +
                      SONG_ID + " = " + std::to_string(ID) + ";";

    int rc = sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "prepare failed: %s",
                            sqlite3_errmsg(mDb));
    } else {
        sqlite3_bind_text(stmt, 1, tag->getTitle().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, tag->getAlbum().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, tag->getArtist().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, tag->getYear().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, tag->getTrack().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_blob(stmt, 6, tag->getCover(), tag->getCoverSize(), SQLITE_STATIC);
    }
    sqlite3_step(stmt);
    return sqlite3_finalize(stmt);
}

map SqlWrapper::retrieveAllFilePaths() {
    int numberOfItems = 0;
    char *error = nullptr;

    std::string sql = "SELECT count(id) FROM " + SONG_TABLE;
    sqlite3_exec(mDb, sql.c_str(), callback, &numberOfItems, &error);

    map filePaths;

    sql = "SELECT " + SONG_FILEPATH + " FROM " + SONG_TABLE;
    int rc = sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr);

    if (rc == SQLITE_OK) {
        for (int i = 0; i < numberOfItems; i++) {
            if (sqlite3_step(stmt) != SQLITE_ROW) {
                __android_log_print(ANDROID_LOG_ERROR, "SQLITE",
                                    "Reached last row before number of items was read: Error code %d",
                                    sqlite3_extended_errcode(mDb));
                filePaths.clear();
                return filePaths;
            }
            char *path = (char *) sqlite3_column_text(stmt, 0);
//            std::string path = std::string((char *) sqlite3_column_text(stmt, 1));
            filePaths[path] = i;

        }
    }
    sqlite3_finalize(stmt);
    return filePaths;
}


#pragma clang diagnostic pop