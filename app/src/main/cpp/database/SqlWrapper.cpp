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
    sqlite3_temp_directory = (char*)"/data/user/0/com.trippntechnology.tntmusicplayer/cache";
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
                          SONG_ALBUM + " TEXT, " +
                          SONG_ARTIST + " TEXT, " +
                          SONG_YEAR + " TEXT, " +
                          SONG_TRACK + " TEXT, " +
                          SONG_COVER + " BLOB, " +

                          SONG_FILEPATH + " TEXT NOT NULL, " +
                          SONG_DURATION + " BIGINT, " +
                          SONG_SAMPLERATE + " INT, " +
                          SONG_BITRATE + " INT);";
        sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr);
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
    sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr);
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
                      "(TITLE,ALBUM,ARTIST,YEAR,TRACK,ARTWORK,FILEPATH,DURATION,SAMPLERATE,BITRATE) " +
                      "VALUES(:TIT,:ALB,:ART,:YEA,:TRA,:COV,:FIL,:DUR,:SAM,:BIT);";
    int rc = sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "SQL_ERROR", "prepare failed: %s",
                            sqlite3_errmsg(mDb));
    } else {
        ///Hardcoded numbers correspond to the position in values in above sql statement starting at 1
        sqlite3_bind_text(stmt, 1, audioFile->getTag()->getTitle().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, audioFile->getTag()->getAlbum().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, audioFile->getTag()->getArtist().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, audioFile->getTag()->getYear().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, audioFile->getTag()->getTrack().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_blob(stmt, 6, audioFile->getTag()->getCover(), audioFile->getTag()->getCoverSize(), SQLITE_STATIC);
        sqlite3_bind_text(stmt, 7, audioFile->getFilePath().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int64(stmt, 8, audioFile->getDuration());
        sqlite3_bind_int(stmt, 9, audioFile->getSampleRate());
        sqlite3_bind_int(stmt, 10, audioFile->getBitrate());

        rc = sqlite3_step(stmt);
    }
    return rc;
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

    sql = "SELECT * FROM " + SONG_TABLE + " ORDER BY "+SONG_TITLE+" ASC";
    sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr);

    for (int i = 0; i < numberOfItems; i++) {
        int rc = sqlite3_step(stmt);
        if (rc != SQLITE_ROW) {
            int err = sqlite3_extended_errcode(mDb);
            __android_log_print(ANDROID_LOG_ERROR, "SQLITE", "Reached last row before number of items was read");
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
        jbyteArray jCover = env->NewByteArray(length);
        env->SetByteArrayRegion(jCover, 0, length, (jbyte *) blob);
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


//    std::string sql = "SELECT * FROM " + SONG_TABLE;
//    jclass jSongClass = env->FindClass("com/trippntechnology/tagger/Song");
//    jmethodID jSongConstructor = env->GetMethodID(jSongClass, "<init>",
//    );
//    std::vector<jobject> songList;
//    while (sqlite3_step(stmt) == SQLITE_ROW) {
//        jint jID = sqlite3_column_int(stmt, SONG_ID_COLUMN);
//        jstring jTitle = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_TITLE_COLUMN));
//        jstring jArtist = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_ARTIST_COLUMN));
//        jstring jAlbum = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_ALBUM_COLUMN));
//        jstring jTrack = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_TRACK_COLUMN));
//        jstring jYear = env->NewStringUTF((char *) sqlite3_column_text(stmt, SONG_YEAR_COLUMN));
//        jstring jFilepath = env->NewStringUTF(
//                (char *) sqlite3_column_text(stmt, SONG_FILEPATH_COLUMN));
//
//        int length = sqlite3_column_bytes(stmt, SONG_COVER_COLUMN);
//        char *blob = (char *) sqlite3_column_blob(stmt, SONG_COVER_COLUMN);
//        jbyteArray jCover = env->NewByteArray(length);
//        env->SetByteArrayRegion(jCover, 0, length, (jbyte *) blob);
//        jobject jSong = env->NewObject(jSongClass, jSongConstructor, jID, jTitle, jArtist, jAlbum,
//                                       jTrack, jYear, jFilepath, jCover);
//        songList.push_back(jSong);
//    }
//    jobjectArray jSongList = env->NewObjectArray((jint) songList.size(), jSongClass, nullptr);
//    for (int i = 0; i < songList.size(); i++) {
//        env->SetObjectArrayElement(jSongList, i, songList[i]);
//    }
    return jAudioArray;
}

//RETURNS the filepath of the song with the corresponding ID

std::string SqlWrapper::selectSong(AudioFile *audioFile) {
    std::stringstream ss;
    ss << audioFile->getID();
    std::string sql =
            "SELECT " + SONG_FILEPATH
            + " FROM " + SONG_TABLE + " WHERE " + SONG_ID
            +
            " = " + ss.str();
    sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr);
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
                      SONG_TITLE
                      + " = :TIT, " +
                      SONG_ARTIST
                      + " = :ART, " +
                      SONG_ALBUM
                      + " = :ALB, " +
                      SONG_TRACK
                      + " = :TRA, " +
                      SONG_YEAR
                      + " = :YEA, " +
                      SONG_COVER
                      + " = ? WHERE " +
                      SONG_ID
                      + " = " + std::to_string(ID) + ";";

    int rc = sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr);

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

#pragma clang diagnostic pop