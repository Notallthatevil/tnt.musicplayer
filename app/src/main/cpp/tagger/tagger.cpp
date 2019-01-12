#include <jni.h>
#include <android/log.h>
#include <dirent.h>
#include <string>
#include <vector>
#include "files/AudioFile.h"
#include "files/Mp3File.h"
#include "../database/SqlWrapper.h"

struct FileAccessException : public std::exception {
    const char *what() const noexcept override {
        return "Unable to access storage. Were permissions requested?";
    }
};


/*
 * directory must look like /storage/emulated/0/Music/
 */
std::vector<std::string> scanDirectoryForAudio(const std::string &directory) {
    DIR *d = opendir(directory.c_str());
    std::vector<std::string> dirFileList;

    if (d) {

//        __android_log_print(ANDROID_LOG_INFO, "scanDirectoryForAudio", "%s", directory.c_str());

        struct dirent *dir;
        while ((dir = readdir(d)) != nullptr) {
            std::string dirName(dir->d_name);
            if (dirName != "." && dirName != "..") {
                std::string newDir = directory + dirName;

                //Check to see if opendir() returns a file or directory
                ///Directory
                if (opendir(newDir.c_str()) != nullptr) {

                    auto subDirList = scanDirectoryForAudio(newDir + "/");
                    dirFileList.insert(dirFileList.end(), subDirList.begin(), subDirList.end());

                    ///File
                } else {
                    std::string extension = dirName.substr(dirName.find_last_of('.') + 1);
                    if (extension == "mp3") {
                        dirFileList.resize(dirFileList.size() + 1);
                        dirFileList.back() = directory + dirName;
                    }
                }
            }
        }
        return dirFileList;
    } else {
        throw FileAccessException();
    }
}

std::string jstringToString(JNIEnv *env, jstring *jstr) {
    int length = env->GetStringLength(*jstr);
    char str[255];
    env->GetStringUTFRegion(*jstr, 0, length, str);
    return std::string(str, length);
}

#define javaString "java/lang/String"
extern "C"
JNIEXPORT jobjectArray
JNICALL
Java_com_trippntechnology_tntmusicplayer_nativewrappers_TaggerLib_scanDirectory(JNIEnv *env, jobject /*this*/,
                                                                                jstring directory, jobject currentSong,
                                                                                jobject numOfSongs) {


    SqlWrapper sqlWrapper;

    ///TEMP
    sqlWrapper.deleteTable(sqlWrapper.SONG_TABLE);
    ///




    std::vector<std::string> directoryList;
    try {
        directoryList = scanDirectoryForAudio(std::string(env->GetStringUTFChars(directory, nullptr)));
    } catch (FileAccessException &e) {
        __android_log_print(ANDROID_LOG_ERROR, "FileAccessException", "%s", e.what());
        return nullptr;
    }



    ///Setup Kotlin Integer wrapper so that we can return an int
    jclass jIntegerWrapper = env->FindClass(
            "com/trippntechnology/tntmusicplayer/dialogs/scanningdialog/ScanningDialog$IntegerWrapper");
    jmethodID integerWrapperConstructor = env->GetMethodID(jIntegerWrapper, "<init>", "(I)V");
    jobject jIntegerWrapperObject = env->NewObject(jIntegerWrapper, integerWrapperConstructor, directoryList.size());

    ///Setup Kotlin progress bar wrapper
    jclass jProgressWrapper = env->FindClass(
            "com/trippntechnology/tntmusicplayer/dialogs/scanningdialog/ScanningDialog$CurrentProgressWrapper");
    jmethodID progressWrapperConstructor = env->GetMethodID(jProgressWrapper, "<init>", "(ILjava/lang/String;)V");


    ///Setup MutableLiveData
    jclass jMutableCurrentSong = env->GetObjectClass(currentSong);
    jmethodID postCurrentSong = env->GetMethodID(jMutableCurrentSong, "postValue", "(Ljava/lang/Object;)V");
    ///Setup SingleLiveEvent
    jclass jMutableMaxSongs = env->GetObjectClass(numOfSongs);
    jmethodID setMaxNumberOfSongs = env->GetMethodID(jMutableMaxSongs, "postValue", "(Ljava/lang/Object;)V");
    env->CallVoidMethod(numOfSongs, setMaxNumberOfSongs, jIntegerWrapperObject);


    jobjectArray jDirList = env->NewObjectArray((jint) directoryList.size(), env->FindClass(javaString),
                                                nullptr);

    for (int i = 0; i < directoryList.size(); i++) {
        env->SetObjectArrayElement(jDirList, i, env->NewStringUTF(directoryList[i].c_str()));
    }


    auto audioFiles = new AudioFile *[directoryList.size()];


    sqlWrapper.createTable(sqlWrapper.SONG_TABLE);

    for (int i = 0; i < directoryList.size(); i++) {

        jstring directoryItem = env->NewStringUTF(directoryList[i].c_str());
        jobject jProgressWrapperObject = env->NewObject(jProgressWrapper, progressWrapperConstructor, i, directoryItem);
        env->CallVoidMethod(currentSong, postCurrentSong, jProgressWrapperObject);

        if (directoryList[i].substr(directoryList[i].find_last_of('.') + 1) == "mp3") {
            auto *mp3 = new Mp3File(&directoryList[i]);
            mp3->parse(true);
            audioFiles[i] = mp3;
        }

        __android_log_print(ANDROID_LOG_INFO, "DATABASE", "Inserting %s into database",
                            audioFiles[i]->getFilePath().c_str());

        sqlWrapper.insertSong(audioFiles[i]);
    }

    for (int i = 0; i < directoryList.size(); i++) {
        delete audioFiles[i];
    }
    delete[]audioFiles;

    __android_log_print(ANDROID_LOG_INFO, "SCANNING", "Finished");

    return jDirList;
}

extern "C"
JNIEXPORT jobjectArray //java audio file
JNICALL
Java_com_trippntechnology_tntmusicplayer_nativewrappers_TaggerLib_getAllAudioFiles(JNIEnv *env, jobject /*this*/) {
    SqlWrapper sqlWrapper;
    return sqlWrapper.retrieveAllSongs(env);
}




extern "C"
JNIEXPORT jint
JNICALL
Java_com_trippntechnology_tntmusicplayer_nativewrappers_TaggerLib_updateNewTags(JNIEnv *env, jobject /*this*/, jint jid,
                                                                                jstring jtitle, jstring jalbum,
                                                                                jstring jartist, jstring jyear,
                                                                                jstring jtrack, jstring jfilePath,
                                                                                jbyteArray jcover) {
    int id = jid;
    std::string title = jstringToString(env, &jtitle);
    std::string album = jstringToString(env, &jalbum);
    std::string artist = jstringToString(env, &jartist);
    std::string year = jstringToString(env, &jyear);
    std::string track = jstringToString(env, &jtrack);
    std::string filePath = jstringToString(env, &jfilePath);

    unsigned char *cover;
    int length = 0;
    if (jcover != nullptr) {
        length = env->GetArrayLength(jcover);
        cover = new unsigned char[length];
        env->GetByteArrayRegion(jcover, 0, length, (jbyte *) cover);
    } else {
        cover = nullptr;
    }

    Tag *tag = nullptr;
    AudioFile *audioFile = nullptr;

    if (filePath.substr(filePath.find_last_of('.') + 1) == "mp3") {
        tag = new ID3Tag();
        audioFile = new Mp3File(&filePath);
    }

    jint jsuccess = 0;

    if (tag != nullptr) {
        tag->setTitle(title);
        tag->setAlbum(album);
        tag->setArtist(artist);
        tag->setYear(year);
        tag->setTrack(track);
        tag->setCover(cover, length, 0);


        int rc = audioFile->saveNewTag(tag);

        if (rc == 0) {
            SqlWrapper sqlWrapper;
            sqlWrapper.updateSong(tag, id);
        } else {
            jsuccess = rc;
        }

        delete[] cover;
        cover = nullptr;
        delete tag;
        delete audioFile;
    }

    return jsuccess;
}












//
//
//
//
//
//
//
//
//
//
//
//
//
///*  Time method execution
//#include <iostream>
//#include <ctime>
//#include <ratio>
//#include <chrono>
//auto start = std::chrono::high_resolution_clock::now();
////Method to time
//auto end = std::chrono::high_resolution_clock::now();
//std::chrono::duration<double> time_span = (end - start);
//__android_log_print(ANDROID_LOG_DEBUG,"TIME","Time Retrieving file paths = %f", time_span.count());
//*/
//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "InfiniteRecursion"
///*
//using namespace std;
//
//typedef vector<vector<string>> VectorList;
//
//
////TODO create UTF8 string for c++ to be able to write utf8 strings and retrieve them. Especially helpful for database stuff
//
//
//*
// * Returns a list of all audio files given in the @param directory
// * directory - The directory to be searched for audio files. Must be an absolute file path
// *
//vector<string> get_audio_files(const string directory) {
//    DIR *d;
//    VectorList vectorList;
//    vector<string> stringList;
//    d = opendir(directory.c_str());
//    if (d) {
//        struct dirent *dir;
//        while ((dir = readdir(d)) != nullptr) {
//            string dirName(dir->d_name);
//            if (dirName != "." && dirName != "..") {
//                string newDir = directory + dirName + "/";
//
//                //If it is a directory search it
//                if (opendir(newDir.c_str()) != nullptr) {
//                    vectorList.push_back(get_audio_files(newDir));
//                } else {
//                    string sub = dirName.substr(dirName.find_last_of('.') + 1);
//                    if (sub == "mp3") {
//                        stringList.push_back(directory + dirName);
//                    }
//                }
//            }
//        }
//    } else {
////        throw fileAccessException();
//    }
//    closedir(d);
//    unsigned long wholeSize = 0;
//    for (int i = 0; i < vectorList.size(); i++) {
//        wholeSize += vectorList[i].size();
//    }
//    vector<string> completeList;
//    completeList.reserve(wholeSize + stringList.size());
//    for (int i = 0; i < vectorList.size(); i++) {
//        completeList.insert(completeList.end(), vectorList[i].begin(), vectorList[i].end());
//    }
//    completeList.insert(completeList.end(), stringList.begin(), stringList.end());
//    return completeList;
//}
//
//
//
//**
// * Returns an int based on the success of creating a database
// *
// * 0  = all operations were successful
// * -1 = unable to access files i.e. user has not granted permission
// * -2 = unable to creating database file
// *
//extern "C"
//JNIEXPORT jint
//JNICALL
//Java_com_trippntechnology_tagger_NativeWrapper_generateDatabase(JNIEnv *env, jobject) {
//    string str = "/storage/emulated/0/Music/Approaching Nirvana/Notes/Approaching Nirvana - Notes - 01 August.mp3";
//    Mp3FileV2 mp3FileV2(&str,true);
//
//    //Retrieve file paths for all audio files
//    vector<string> files;
//    try {
//        files = get_audio_files("/storage/emulated/0/Music/");
//    } catch (fileAccessException) {
//        __android_log_print(ANDROID_LOG_ERROR, "ERROR",
//                            "Unable to access external drive. Insufficient permissions");
//        return -1;
//    }
//
//    //Retrieve tags from songs
//    vector<AudioFile *> audioFiles(files.size());
//    for (int i = 0; i < files.size(); i++) {
//
//        string sub = files[i].substr(files[i].find_last_of(".") + 1);
//        if (sub == "mp3") {
//            audioFiles[i] = new Mp3FileV2(&files[i], true);
//        }
//    }
//
//    //Write tags to database
//    int insertErrors = 0;
//    try {
//        //Biggest time hiccup
//        SqlWrapper sqlHelper;
//        sqlHelper.deleteTable(sqlHelper.SONG_TABLE);
//        sqlHelper.createTable(sqlHelper.SONG_TABLE);
//        for (AudioFile *f:audioFiles) {
//            if (sqlHelper.insertSong(f) != 101) {
//                insertErrors++;
//            }
//        }
//    } catch (databaseCreationError()) {
//        return -2;
//    }
//    for_each(audioFiles.begin(), audioFiles.end(), delete_pointed_to<AudioFile>);
//
//
//    return insertErrors;
//}
//
//
//*
// * Returns all songs from the database
// *
//extern "C"
//JNIEXPORT jobjectArray
//JNICALL Java_com_trippntechnology_tagger_NativeWrapper_retrieveSongs(JNIEnv *env, jobject) {
//    SqlWrapper sqlHelper;
//    return sqlHelper.retrieveAllSongs(env);
//}
//
//
////TODO needs to be compatible with multiple file types not just mp3s
//extern "C"
//JNIEXPORT jint
//JNICALL Java_com_trippntechnology_tagger_NativeWrapper_saveNewTag(JNIEnv *env, jobject,
//                                                                  jbyteArray jSerialized) {
//    int length = env->GetArrayLength(jSerialized);
//    vector<char> buf(length);
//    env->GetByteArrayRegion(jSerialized, 0, length, (jbyte *) &buf[0]);
//
//    Mp3FileV2 song(buf);
//    string filePath = song.getFilePath();
//    vector<char> newTags = song.getTag()->generateTags();
//
//    //
//    auto mp3 = Mp3FileV2(&filePath, false);
//    mp3.setAudio();
//    vector<char> audioData = mp3.getAudio();
//
//    newTags.insert(newTags.end(), audioData.begin(), audioData.end());
//
//    SqlWrapper sqlHelper;
//    FILE *f = fopen(filePath.c_str(), "w+");
//    fwrite(newTags.data(), sizeof(char), newTags.size(), f);
//    fclose(f);
//    sqlHelper.updateSong(&song, song.getID());
//
//    return 1;
//}
//
//*/
