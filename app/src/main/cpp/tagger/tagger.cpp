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

extern "C"
JNIEXPORT jobjectArray
JNICALL
Java_com_trippntechnology_tntmusicplayer_nativewrappers_TaggerLib_scanDirectory(JNIEnv *env, jobject /*this*/,
                                                                                jstring directory, jobject currentSong,
                                                                                jobject numOfSongs) {


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


    jobjectArray jDirList = env->NewObjectArray((jint) directoryList.size(), env->FindClass("java/lang/String"),
                                                nullptr);

    for (int i = 0; i < directoryList.size(); i++) {
        env->SetObjectArrayElement(jDirList, i, env->NewStringUTF(directoryList[i].c_str()));
    }

    SqlWrapper sqlWrapper;
    sqlWrapper.createTable(sqlWrapper.SONG_TABLE);

    for (int i = 0; i < directoryList.size(); i++) {

        jstring directoryItem = env->NewStringUTF(directoryList[i].c_str());
        jobject jProgressWrapperObject = env->NewObject(jProgressWrapper, progressWrapperConstructor, i, directoryItem);
        env->CallVoidMethod(currentSong, postCurrentSong, jProgressWrapperObject);

        if (directoryList[i].substr(directoryList[i].find_last_of('.') + 1) == "mp3") {
            Mp3File mp3(&directoryList[i]);
            mp3.parse(true);
            sqlWrapper.insertSong(&mp3);

        }
    }
    __android_log_print(ANDROID_LOG_INFO, "SCANNING", "Finished");
    return jDirList;
}

extern "C"
JNIEXPORT jobjectArray //java audio file
JNICALL
Java_com_trippntechnology_tntmusicplayer_nativewrappers_TaggerLib_getAllAudioFiles(JNIEnv *env,
                                                                                   jobject /*this*/) {
    SqlWrapper sqlWrapper;
    return sqlWrapper.retrieveAllSongs(env);
}

extern "C"
JNIEXPORT jboolean
JNICALL
Java_com_trippntechnology_tntmusicplayer_nativewrappers_TaggerLib_songTableExist(JNIEnv *env, jobject /*this*/) {
    SqlWrapper sqlWrapper;
    jboolean jbool = sqlWrapper.tableExist(sqlWrapper.SONG_TABLE) ? JNI_TRUE : JNI_FALSE;
    return jbool;
}


extern "C"
JNIEXPORT jint
JNICALL
Java_com_trippntechnology_tntmusicplayer_nativewrappers_TaggerLib_updateNewTags(JNIEnv *env, jobject /*this*/,
                                                                                jint jid,
                                                                                jstring jtitle, jstring jalbum,
                                                                                jstring jartist, jstring jyear,
                                                                                jstring jtrack,
                                                                                jstring jfilePath,
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