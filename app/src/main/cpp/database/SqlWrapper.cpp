#include "SqlWrapper.h"

inline bool file_exists(const std::string &name) {
    struct stat buffer{};
    return (stat(name.c_str(), &buffer) == 0);
}

/*
 * Creates database directory if it doesn't exist then opens the database connection.
 */
SqlWrapper::SqlWrapper() {
    if (!file_exists(DATABASE_DIRECTORY)) {
        mkdir(DATABASE_DIRECTORY.c_str(), S_IRWXU | S_IRWXG | S_IXOTH);
    }
    int rc = sqlite3_open_v2(DATABASE_NAME.c_str(), &mDb, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE,
                             NULL);
    if (rc) {
        throw databaseCreationError();
    }
}

/*
 * Basic destructor
 * Closes the database connection
 */
SqlWrapper::~SqlWrapper() {
    sqlite3_finalize(stmt);
    sqlite3_close_v2(mDb);
}

/*
 * Creates the database
 *
 * @param tableName - The name of the database table
 */
int SqlWrapper::createTable(std::string tableName) {
    if (tableName == SONG_TABLE) {
        std::string sql = "CREATE TABLE " + SONG_TABLE + "(" +
                     SONG_ID + " INTEGER PRIMARY KEY AUTOINCREMENT, " +
                     SONG_TITLE + " TEXT, " +
                     SONG_ARTIST + " TEXT, " +
                     SONG_ALBUM + " TEXT, " +
                     SONG_TRACK + " TEXT, " +
                     SONG_YEAR + " TEXT, " +
                     SONG_FILEPATH + " TEXT NOT NULL, " +
                     SONG_COVER + " BLOB);";
        sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, NULL);
        int rc = sqlite3_step(stmt);
        return rc;
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
    sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, NULL);
    int rc = sqlite3_step(stmt);
    return rc;
}


/*
 * Adds a song to the database while also making sure the contents in the file
 * are ready to be inserted.
 *
 * @param audioFile - Pointer to the audio file to be added
 */
int SqlWrapper::insertSong(AudioFile *audioFile) {
    std::string sql = "INSERT INTO " + SONG_TABLE +
                 "(TITLE,ARTIST,ALBUM,TRACK,YEAR,FILEPATH,ARTWORK) " +
                 "VALUES(:TIT,:ART,:ALB,:TRA,:YEA,:FIL,?);";
    int rc = sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "prepare failed: %s",
                            sqlite3_errmsg(mDb));
    } else {
        sqlite3_bind_text(stmt, SONG_TITLE_COLUMN, audioFile->getTag()->getTitle().c_str(), -1,
                          SQLITE_STATIC);
        sqlite3_bind_text(stmt, SONG_ARTIST_COLUMN, audioFile->getTag()->getArtist().c_str(), -1,
                          SQLITE_STATIC);
        sqlite3_bind_text(stmt, SONG_ALBUM_COLUMN, audioFile->getTag()->getAlbum().c_str(), -1,
                          SQLITE_STATIC);
        sqlite3_bind_text(stmt, SONG_TRACK_COLUMN, audioFile->getTag()->getTrack().c_str(), -1,
                          SQLITE_STATIC);
        sqlite3_bind_text(stmt, SONG_YEAR_COLUMN, audioFile->getTag()->getYear().c_str(), -1,
                          SQLITE_STATIC);
        sqlite3_bind_text(stmt, SONG_FILEPATH_COLUMN, audioFile->getFilePath().c_str(), -1,
                          SQLITE_STATIC);
        sqlite3_bind_blob(stmt, SONG_COVER_COLUMN, audioFile->getTag()->getCover(),
                          audioFile->getTag()->getCoverSize(), SQLITE_STATIC);
        rc = sqlite3_step(stmt);
    }
    return rc;
}


jobjectArray SqlWrapper::retrieveAllSongs(JNIEnv *env) {
    std::string sql = "SELECT * FROM " + SONG_TABLE;
    sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, NULL);
    jclass jSongClass = env->FindClass("com/trippntechnology/tagger/Song");
    jmethodID jSongConstructor = env->GetMethodID(jSongClass, "<init>",
                                                  "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[B)V");
    std::vector<jobject> songList;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        jint jID = sqlite3_column_int(stmt, SONG_ID_COLUMN);
        jstring jTitle = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_TITLE_COLUMN));
        jstring jArtist = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_ARTIST_COLUMN));
        jstring jAlbum = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_ALBUM_COLUMN));
        jstring jTrack = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_TRACK_COLUMN));
        jstring jYear = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_YEAR_COLUMN));
        jstring jFilepath = env->NewStringUTF(
                (char *) sqlite3_column_text(stmt, SONG_FILEPATH_COLUMN));

        int length = sqlite3_column_bytes(stmt, SONG_COVER_COLUMN);
        char *blob = (char *) sqlite3_column_blob(stmt, SONG_COVER_COLUMN);
        jbyteArray jCover = env->NewByteArray(length);
        env->SetByteArrayRegion(jCover, 0, length, (jbyte *) blob);
        jobject jSong = env->NewObject(jSongClass, jSongConstructor, jID, jTitle, jArtist, jAlbum,
                                       jTrack, jYear, jFilepath, jCover);
        songList.push_back(jSong);
    }
    jobjectArray jSongList = env->NewObjectArray((jint) songList.size(), jSongClass, NULL);
    for (int i = 0; i < songList.size(); i++) {
        env->SetObjectArrayElement(jSongList, i, songList[i]);
    }
    return jSongList;
}

//RETURNS the filepath of the song with the corresponding ID

std::string SqlWrapper::selectSong(AudioFile *audioFile) {
    std::stringstream ss;
    ss << audioFile->getID();
    std::string sql =
            "SELECT " + SONG_FILEPATH + " FROM " + SONG_TABLE + " WHERE " + SONG_ID +
            " = " + ss.str();
    sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);
    return std::string((char *) sqlite3_column_text(stmt, 0));
}

/*
 * Updates the selected audio file based on the id passed in
 *
 * @param audioFile - The new audio file to be updated
 * @param ID - The ID of the database entry to be updated
 */
int SqlWrapper::updateSong(AudioFile *audioFile, int ID) {
    Tag *tag = audioFile->getTag();
    std::string sql = "UPDATE " + SONG_TABLE + " SET " +
                 SONG_TITLE + " = :TIT, " +
                 SONG_ARTIST + " = :ART, " +
                 SONG_ALBUM + " = :ALB, " +
                 SONG_TRACK + " = :TRA, " +
                 SONG_YEAR + " = :YEA, " +
                 SONG_COVER + " = ? WHERE " +
                 SONG_ID + " = " + std::to_string(ID) + ";";

    int rc = sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "prepare failed: %s",
                            sqlite3_errmsg(mDb));
    } else {
        sqlite3_bind_text(stmt, 1, tag->getTitle().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, tag->getArtist().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, tag->getAlbum().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, tag->getTrack().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, tag->getYear().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_blob(stmt, 6, tag->getCover(), tag->getCoverSize(), SQLITE_STATIC);
    }
    rc = sqlite3_step(stmt);
    return rc;
}
