#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

#include "SqlWrapper.h"

/**
 * Simple inline method to determin if a file exists or not
 * @param fileName - The fully qualified file name
 * @returns if the file exists or not
 */
inline bool file_exists(const std::string &fileName) {
    struct stat buffer{};
    return (stat(fileName.c_str(), &buffer) == 0);
}

/**
 * Constructor for the @class SqlWrapper. Creates a directory and database file if they don't exist.
 * This constructor also creates a sqlite3 object and sets @param mDb to it
 */
SqlWrapper::SqlWrapper() {
    //Warning setting this is deprecated but have no choice as no TMPDIR global variable available for android
    sqlite3_temp_directory = (char *) "/data/user/0/com.trippntechnology.tntmusicplayer/cache";
    if(!file_exists(DATABASE_DIRECTORY)) {
        mkdir(DATABASE_DIRECTORY.c_str(), S_IRWXU | S_IRWXG | S_IXOTH);
    }
    if(sqlite3_open_v2(DATABASE_NAME.c_str(), &mDb, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE,
                       nullptr) != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "Database creation failed: %s",
                            sqlite3_errmsg(mDb));
    }
}

/**
 * Destructor for the @class SqlWrapper. This also destroys the sqlite3 object @param mDb
 */
SqlWrapper::~SqlWrapper() {
    sqlite3_close_v2(mDb);
}

/**
 * Creates the specified table based on what @param tableName is set too. Table name should be passed in
 * by using one of the const table names in @SqlWrapper. This method deletes the specified table if it
 * already exists thereby acting as a kind of table reset.
 * @param tableName - The name of the table to create
 * @returns a value based on the success of the database. Expecting 0 for success otherwise an error occurred
 */
int SqlWrapper::createTable(std::string tableName) {
    std::string sql;
    sqlite3_stmt *stmt = nullptr;

    deleteTable(tableName);

    if(tableName == SONG_TABLE) {
        sql = "CREATE TABLE " + SONG_TABLE + "(" +
              SONG_ID + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
              SONG_ALBUM_ID + " INT, " +

              SONG_TITLE + " TEXT, " +
              SONG_ALBUM + " TEXT, " +
              SONG_ARTIST + " TEXT, " +
              SONG_YEAR + " TEXT, " +
              SONG_TRACK + " TEXT, " +

              SONG_FILEPATH + " TEXT NOT NULL, " +
              SONG_DURATION + " BIGINT, " +
              SONG_SAMPLERATE + " INT, " +
              SONG_BITRATE + " INT, " +

              SONG_LAST_MODIFIED + " BIGINT);";

    } else if(tableName == ALBUM_TABLE) {
        sql = "CREATE TABLE " + ALBUM_TABLE + "(" +
              ALBUM_ID + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
              ALBUM_NAME + " TEXT, " +
              ALBUM_ARTWORK + " BLOB);";
    }
    if(sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "Error preparing table %s: %s",
                            tableName.c_str(), sqlite3_errmsg(mDb));
        return sqlite3_finalize(stmt);;
    }
    if(sqlite3_step(stmt) != SQLITE_DONE) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "Error creating table %s: %s",
                            tableName.c_str(), sqlite3_errmsg(mDb));
        return sqlite3_finalize(stmt);;
    }
    return sqlite3_finalize(stmt);
}

/**
 * Drops the specified table name
 * @param tableName - The name of the table to be dropped
 * @returns a value based on the success of the database. Expecting 0 for success otherwise an error occurred
 */
int SqlWrapper::deleteTable(std::string tableName) {
    sqlite3_stmt *stmt = nullptr;
    std::string sql = "DROP TABLE IF EXISTS " + tableName;
    if(sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "Error preparing to drop %s: %s",
                            tableName.c_str(), sqlite3_errmsg(mDb));
        return sqlite3_finalize(stmt);;
    }
    if(sqlite3_step(stmt) != SQLITE_DONE) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "Error dropping table %s: %s",
                            tableName.c_str(), sqlite3_errmsg(mDb));
        return sqlite3_finalize(stmt);;
    }
    return sqlite3_finalize(stmt);

}

/**
 * Inserts the passed in audio file into the database.
 * @param audioFile - The audio file to be written
 * @returns a value based on the success of the database. Expecting 0 for success otherwise an error occurred
 */
int SqlWrapper::insertSong(AudioFile *audioFile) {
    sqlite3_stmt *stmt = nullptr;
    int albumID = getAlbumId(audioFile->getTag()->getAlbum(), audioFile->getTag()->getCover(),
                             audioFile->getTag()->getCoverSize());

    std::string sql = "INSERT INTO " + SONG_TABLE +
                      "(TITLE,ALBUM_ID,ALBUM,ARTIST,YEAR,TRACK,FILEPATH,DURATION,SAMPLERATE,BITRATE,MODIFIED_DATE) " +
                      "VALUES(?1,?2,?3,?4,?5,?6,?7,?8,?9,?10,?11);";


    if(sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "Song insert prepare failed on %s: %s",
                            audioFile->getTag()->getTitle().c_str(), sqlite3_errmsg(mDb));
        return sqlite3_finalize(stmt);
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

        ///Hardcoded numbers correspond to the position in values in above sql statement starting at 1
        sqlite3_bind_text(stmt, 1, title, -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, albumID);
        sqlite3_bind_text(stmt, 3, album, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, artist, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, year, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, track, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 7, audioFile->getFilePath().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt, 8, audioFile->getDuration());
        sqlite3_bind_int(stmt, 9, audioFile->getSampleRate());
        sqlite3_bind_int(stmt, 10, audioFile->getBitrate());
        sqlite3_bind_int64(stmt, 11, audioFile->getLastModified());

        if(sqlite3_step(stmt) != SQLITE_DONE) {
            __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "Error inserting into database %d: %s",
                                sqlite3_extended_errcode(mDb), sqlite3_errmsg(mDb));
        }
    }
    sqlite3_clear_bindings(stmt);
    sqlite3_reset(stmt);
    return sqlite3_finalize(stmt);
}

/**
 * Updates the song at the given @param ID.
 * @param tag - The new tag associated with the audio file
 * @param ID - The ID of the audio file to be updated
 * @param lastModifiedTime - The new last modified time returned form writting the new @param tag
 * @returns a value based on the success of the database. Expecting 0 for success otherwise an error occurred
 */
int SqlWrapper::updateSong(Tag *tag, int ID, long lastModifiedTime) {
    sqlite3_stmt *stmt;
    std::string sql = "UPDATE " + SONG_TABLE + " SET " +
                      SONG_TITLE + " = ?1, " +
                      SONG_ALBUM_ID = " = ?2, " +
                                      SONG_ALBUM + " = ?3, " +
                                      SONG_ARTIST + " = ?4, " +
                                      SONG_YEAR + " = ?5, " +
                                      SONG_TRACK + " = ?6, " +
                                      SONG_LAST_MODIFIED + " = ?7, WHERE " +
                                      SONG_ID + " = " + std::to_string(ID) + ";";


    if(sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "Song update prepare failed: %s",
                            sqlite3_errmsg(mDb));
        return sqlite3_finalize(stmt);

    }
    return updateSong(&stmt,tag,lastModifiedTime);
}


/**
 * Updates the song at the given @param filePath.
 * @param tag - The new tag associated with the audio file
 * @param filePath - The file path of the audio file to be updated
 * @param lastModifiedTime - The new last modified time returned form writting the new @param tag
 * @returns a value based on the success of the database. Expecting 0 for success otherwise an error occurred
 */
int SqlWrapper::updateSong(Tag *tag, std::string filePath, long lastModifiedTime) {
    sqlite3_stmt *stmt;
    std::string sql = "UPDATE " + SONG_TABLE + " SET " +
                      SONG_TITLE + " = ?1, " +
                      SONG_ALBUM_ID = " = ?2, " +
                                      SONG_ALBUM + " = ?3, " +
                                      SONG_ARTIST + " = ?4, " +
                                      SONG_YEAR + " = ?5, " +
                                      SONG_TRACK + " = ?6, " +
                                      SONG_LAST_MODIFIED + " = ?7, WHERE " +
                                      SONG_ID + " = \'" + filePath + "\';";


    if(sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "Song update prepare failed: %s",
                            sqlite3_errmsg(mDb));
        return sqlite3_finalize(stmt);
    }
    return updateSong(&stmt,tag,lastModifiedTime);
}

/**
 * The actual update process. This method updates the audio file found in the @param stmt
 * @param stmt - The @typedef sqlite3_stmt created with a prepare statment
 * @param tag - The new tag associated with the audio file
 * @param lastModifiedTime - The new last modified time returned form writting the new @param tag
 * @returns a value based on the success of the database. Expecting 0 for success otherwise an error occurred
 */
int SqlWrapper::updateSong(sqlite3_stmt **stmt,Tag *tag, long lastModifiedTime) {
    sqlite3_bind_text(*stmt, 1, tag->getTitle().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(*stmt, 2, getAlbumId(tag->getAlbum(), tag->getCover(), tag->getCoverSize()));
    sqlite3_bind_text(*stmt, 2, tag->getAlbum().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(*stmt, 3, tag->getArtist().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(*stmt, 4, tag->getYear().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(*stmt, 5, tag->getTrack().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(*stmt, 6, lastModifiedTime);
    if(sqlite3_step(*stmt) != SQLITE_DONE) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "Error updating song %d: %s",
                            sqlite3_extended_errcode(mDb), sqlite3_errmsg(mDb));
    }
    sqlite3_clear_bindings(*stmt);
    return sqlite3_finalize(*stmt);
}

/**
 * Inserts the desired album into the database if the album doesn't exist yet.
 * @param albumName - The album name to be inserted into the database
 * @param cover - The bytes representing the cover
 * @param coverSize - The size of @param cover
 * @returns a value based on the success of the database. Expecting 0 for success otherwise an error occurred
 */
int SqlWrapper::insertAlbum(std::string albumName, unsigned char *cover, long coverSize) {
    sqlite3_stmt *stmt = nullptr;
    std::string sql = "INSERT INTO " + ALBUM_TABLE + "(" + ALBUM_NAME + ")" +
                      "SELECT \'" + albumName + "\' WHERE NOT EXISTS (SELECT 1 FROM " + ALBUM_TABLE + " WHERE " +
                      ALBUM_NAME + " =\'" + albumName + "\');";

    if(sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "Album insert prepare failed on %s: %s",
                            albumName.c_str(), sqlite3_errmsg(mDb));
        return sqlite3_finalize(stmt);
    }
    if(sqlite3_step(stmt) != SQLITE_DONE) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "Error inserting album into database %d: %s",
                            sqlite3_extended_errcode(mDb), sqlite3_errmsg(mDb));
    } else {
        updateAlbumCover(albumName, cover, coverSize);
    }

    return sqlite3_finalize(stmt);
}


/**
 * Adds the cover byte array to the database only if the value currently in the database is null, otherwise it is left
 * as is and skipped.
 * @param albumName - The album name to be updated
 * @param cover - The bytes representing the cover
 * @param coverSize - The size of @param cover
 * @returns a value based on the success of the database. Expecting 0 for success otherwise an error occurred
 */
int SqlWrapper::updateAlbumCover(std::string albumName, unsigned char *cover, long coverSize) {
    sqlite3_stmt *stmt = nullptr;

    std::string sql = "UPDATE " + SONG_TABLE + " SET " +
                      ALBUM_ARTWORK + "= ?1 WHERE " +
                      ALBUM_NAME + " = \'" + albumName + "\' AND " + ALBUM_ARTWORK + " IS NULL;";

    if(sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "Setting cover prepare failed on %s: %s",
                            albumName.c_str(), sqlite3_errmsg(mDb));
        return sqlite3_finalize(stmt);
    }
    sqlite3_bind_blob(stmt, 1, cover, coverSize, SQLITE_TRANSIENT);

    if(sqlite3_step(stmt) != SQLITE_DONE) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "Setting cover failed on %s: %s",
                            albumName.c_str(), sqlite3_errmsg(mDb));
    }
    return sqlite3_finalize(stmt);
}

/**
 * Inserts the desired album if it doesn't exist along with the cover, and then returns the ID for the given @param
 * albumName.
 * @note - The reason an ID is used and not just the album name itself is to allow the user to use different images in
 * the event the album names overlap. Or if the user wants to have a different cover on one audio file in the album.
 * @param albumName - The album name to query for an ID
 * @param cover - The bytes representing the cover
 * @param coverSize - The size of @param cover
 * @returns the ID associated with @param albumName
 */
int SqlWrapper::getAlbumId(std::string albumName, unsigned char *cover, long coverSize) {
    int albumID = -1;

    if(insertAlbum(albumName, cover, coverSize) != SQLITE_OK) {
        return albumID;
    }

    sqlite3_stmt *stmt = nullptr;
    std::string sql =
            "SELECT " + ALBUM_ID + " FROM " + ALBUM_TABLE + " WHERE " + ALBUM_NAME + " =\'" + albumName + "\';";

    if(sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "Retrieving album id prepare failed on %s: %s",
                            albumName.c_str(), sqlite3_errmsg(mDb));
        sqlite3_finalize(stmt);
        return albumID;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "Error inserting album into database %d: %s",
                            sqlite3_extended_errcode(mDb), sqlite3_errmsg(mDb));
    } else {
        albumID = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return albumID;

}

/**
 * Deletes the specified audio file from the database
 * @param filePath - The absolute file path to the audio file
 * @returns a value based on the success of the database. Expecting 0 for success otherwise an error occurred
 */
int SqlWrapper::deleteAudioFileByFilePath(std::string filePath) {
    sqlite3_stmt *stmt = nullptr;
    std::string sql = "DELETE FROM " + SONG_TABLE + " WHERE " + SONG_FILEPATH + " = \"" + filePath + "\";";

    if(sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "Error preparing to delete from database %d: %s",
                            sqlite3_extended_errcode(mDb), sqlite3_errmsg(mDb));
        return sqlite3_finalize(stmt);
    }
    if(sqlite3_step(stmt) != SQLITE_DONE) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "Error deleting from database %d: %s",
                            sqlite3_extended_errcode(mDb), sqlite3_errmsg(mDb));
    }
    return sqlite3_finalize(stmt);
}

/**
 * Queries the database for all of the audio files contained within it.
 * @param env - A pointer to the JNI environment
 * @returns a jobjectArray that represents an array of @class AudioFile
 */
jobjectArray SqlWrapper::retrieveAllSongs(JNIEnv *env) {
    sqlite3_stmt *stmt;
    int numberOfItems = getNumberOfEntries(SONG_TABLE);

    //Set up java audio file
    std::string constructorParameters = "(IILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;JII)V";
    jclass jAudioFile = env->FindClass("com/trippntechnology/tntmusicplayer/objects/AudioFile");
    jmethodID jConstructor = env->GetMethodID(jAudioFile, "<init>", constructorParameters.c_str());
    jobjectArray jAudioArray = env->NewObjectArray(numberOfItems, jAudioFile, nullptr);

    std::string sql = "SELECT * FROM " + SONG_TABLE + " ORDER BY " + SONG_TITLE + " COLLATE NOCASE ASC";
    if(sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR",
                            "Retrieving all songs prepare statement failed %d",
                            sqlite3_extended_errcode(mDb));
        sqlite3_finalize(stmt);
        return nullptr;
    }
    for(int i = 0; i < numberOfItems; i++) {
        if(sqlite3_step(stmt) != SQLITE_ROW) {
            __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR",
                                "Reached last row before number of items was read: Error code %d",
                                sqlite3_extended_errcode(mDb));
            sqlite3_finalize(stmt);
            return nullptr;
        }

        jint jid = sqlite3_column_int(stmt, SONG_ID_COLUMN);
        jint jAlbumId = sqlite3_column_int(stmt, SONG_ALBUM_ID_COLUMN);
        //Tags
        jstring jTitle = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_TITLE_COLUMN));
        jstring jAlbum = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_ALBUM_COLUMN));
        jstring jArtist = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_ARTIST_COLUMN));
        jstring jYear = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_YEAR_COLUMN));
        jstring jTrack = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_TRACK_COLUMN));
        //Audio data
        jstring jFilepath = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_FILEPATH_COLUMN));
        jlong jDuration = sqlite3_column_int64(stmt, SONG_DURATION_COLUMN);
        jint jSampleRate = sqlite3_column_int(stmt, SONG_SAMPLERATE_COLUMN);
        jint jBitrate = sqlite3_column_int(stmt, SONG_BITRATE_COLUMN);

        jobject jSong = env->NewObject(jAudioFile, jConstructor,
                                       jid, jAlbumId, jTitle, jAlbum, jArtist, jYear, jTrack, jFilepath, jDuration,
                                       jSampleRate, jBitrate);

        env->SetObjectArrayElement(jAudioArray, i, jSong);
    }
    if(sqlite3_finalize(stmt) != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR",
                            "An unexpected error occurred while retrieving all audio files: %s",
                            sqlite3_errmsg(mDb));
    }
    return jAudioArray;
}

/**
 * Retrieves a list of the audio file file paths.
 * @returns a map of the audio files. The key is the file path and the value is the last edit date
 */
map SqlWrapper::retrieveAllFilePaths() {
    int numberOfItems = getNumberOfEntries(SONG_TABLE);
    sqlite3_stmt *stmt;
    map filePaths;

    std::string sql = "SELECT " + SONG_FILEPATH + ", " + SONG_LAST_MODIFIED + " FROM " + SONG_TABLE;
    if(sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR",
                            "Retrieving all file paths prepare statement failed %s",
                            sqlite3_errmsg(mDb));
        sqlite3_finalize(stmt);
        filePaths.clear();
        return filePaths;
    }

    for(int i = 0; i < numberOfItems; i++) {

        if(sqlite3_step(stmt) != SQLITE_ROW) {
            __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR",
                                "Reached last row before number of items was read: Error code %d",
                                sqlite3_extended_errcode(mDb));
            filePaths.clear();
            return filePaths;
        }
        char *path = (char *) sqlite3_column_text(stmt, 0);
        filePaths[path] = sqlite3_column_int64(stmt, 1);

    }
    sqlite3_finalize(stmt);
    return filePaths;
}

/**
 * Checks to see if the passed in table name exists.
 * @param tableName - The name of the table to find
 * @returns the tables existence
 */
bool SqlWrapper::tableExist(std::string tableName) {
    sqlite3_stmt *stmt;
    std::string sql = "SELECT name FROM sqlite_master WHERE type='table' AND name=\'" + tableName + "\'";

    if(sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR",
                            "Prepare to check table existence failed. %s",
                            sqlite3_errmsg(mDb));
    }

    if(sqlite3_step(stmt) == SQLITE_ROW) {
        //was found?
        sqlite3_finalize(stmt);
        return true;
    }
    sqlite3_finalize(stmt);
    return false;
}


/**
 * Determines the number of rows in a given table
 * @param tableName - The name of table to find the number of rows
 * @returns the number of rows in the table
 */
int SqlWrapper::getNumberOfEntries(const std::string &tableName) {
    int numberOfItems = 0;
    char *error = nullptr;

    std::string sql = "SELECT count(id) FROM " + tableName;
    sqlite3_exec(mDb, sql.c_str(), callback, &numberOfItems, &error);
    return numberOfItems;
}


#pragma clang diagnostic pop