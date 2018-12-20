#include <jni.h>
#include <string>
#include <vector>
#include <fstream>
#include <dirent.h>
#include <algorithm>
#include <exception>
#include "Sqlite/SqlHelper.h"
#include "Files/AudioFile.h"
#include "Files/Mp3FileV2.h"
#include <errno.h>
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <cstdio>

/*  Time method execution
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
auto start = std::chrono::high_resolution_clock::now();
//Method to time
auto end = std::chrono::high_resolution_clock::now();
std::chrono::duration<double> time_span = (end - start);
__android_log_print(ANDROID_LOG_DEBUG,"TIME","Time Retrieving file paths = %f", time_span.count());
*/
using namespace std;

typedef vector<vector<string>> VectorList;


//TODO create UTF8 string for c++ to be able to write utf8 strings and retrieve them. Especially helpful for database stuff


/*
 * Returns a list of all audio files given in the @param directory
 * directory - The directory to be searched for audio files. Must be an absolute file path
 */
vector<string> get_audio_files(string directory) {
    DIR *d;
    VectorList vectorList;
    vector<string> stringList;
    d = opendir(directory.c_str());
    if (d) {
        struct dirent *dir;
        while ((dir = readdir(d)) != NULL) {
            string dirName(dir->d_name);
            if (dirName != "." && dirName != "..") {
                string newDir = directory + dirName + "/";

                //If it is a directory search it
                if (opendir(newDir.c_str()) != NULL) {
                    vectorList.push_back(get_audio_files(newDir));
                } else {
                    string sub = dirName.substr(dirName.find_last_of(".") + 1);
                    if (sub == "mp3") {
                        stringList.push_back(directory + dirName);
                    }
                }
            }
        }
    } else {
        throw fileAccessException();
    }
    closedir(d);
    unsigned long wholeSize = 0;
    for (int i = 0; i < vectorList.size(); i++) {
        wholeSize += vectorList[i].size();
    }
    vector<string> completeList;
    completeList.reserve(wholeSize + stringList.size());
    for (int i = 0; i < vectorList.size(); i++) {
        completeList.insert(completeList.end(), vectorList[i].begin(), vectorList[i].end());
    }
    completeList.insert(completeList.end(), stringList.begin(), stringList.end());
    return completeList;
}

template<typename T>
void delete_pointed_to(T *const ptr) {
    delete ptr;
}

/**
 * Returns an int based on the success of creating a database
 *
 * 0  = all operations were successful
 * -1 = unable to access files i.e. user has not granted permission
 * -2 = unable to creating database file
 */
extern "C"
JNIEXPORT jint
JNICALL
Java_com_trippntechnology_tagger_NativeWrapper_generateDatabase(JNIEnv *env, jobject /* this */) {
    string str = "/storage/emulated/0/Music/Approaching Nirvana/Notes/Approaching Nirvana - Notes - 01 August.mp3";
    Mp3FileV2 mp3FileV2(&str,true);

    //Retrieve file paths for all audio files
    vector<string> files;
    try {
        files = get_audio_files("/storage/emulated/0/Music/");
    } catch (fileAccessException) {
        __android_log_print(ANDROID_LOG_ERROR, "ERROR",
                            "Unable to access external drive. Insufficient permissions");
        return -1;
    }

    //Retrieve tags from songs
    vector<AudioFile *> audioFiles(files.size());
    for (int i = 0; i < files.size(); i++) {

        string sub = files[i].substr(files[i].find_last_of(".") + 1);
        if (sub == "mp3") {
            audioFiles[i] = new Mp3FileV2(&files[i], true);
        }
    }

    //Write tags to database
    int insertErrors = 0;
    try {
        //Biggest time hiccup
        SqlHelper sqlHelper;
        sqlHelper.dropTable(sqlHelper.SONG_TABLE);
        sqlHelper.createTable(sqlHelper.SONG_TABLE);
        for (AudioFile *f:audioFiles) {
            if (sqlHelper.insertSong(f) != 101) {
                insertErrors++;
            }
        }
    } catch (databaseCreationError()) {
        return -2;
    }
    for_each(audioFiles.begin(), audioFiles.end(), delete_pointed_to<AudioFile>);


    return insertErrors;
}


/*
 * Returns all songs from the database
 */
extern "C"
JNIEXPORT jobjectArray
JNICALL Java_com_trippntechnology_tagger_NativeWrapper_retrieveSongs(JNIEnv *env, jobject) {
    SqlHelper sqlHelper;
    return sqlHelper.retrieveAllSongs(env);
}


//TODO needs to be compatible with multiple file types not just mp3s
extern "C"
JNIEXPORT jint
JNICALL Java_com_trippntechnology_tagger_NativeWrapper_saveNewTag(JNIEnv *env, jobject,
                                                                  jbyteArray jSerialized) {
    int length = env->GetArrayLength(jSerialized);
    vector<char> buf(length);
    env->GetByteArrayRegion(jSerialized, 0, length, (jbyte *) &buf[0]);

    Mp3FileV2 song(buf);
    string filePath = song.getFilePath();
    vector<char> newTags = song.getTag()->generateTags();

    //
    auto mp3 = Mp3FileV2(&filePath, false);
    mp3.setAudio();
    vector<char> audioData = mp3.getAudio();

    newTags.insert(newTags.end(), audioData.begin(), audioData.end());

    SqlHelper sqlHelper;
    FILE *f = fopen(filePath.c_str(), "w+");
    fwrite(newTags.data(), sizeof(char), newTags.size(), f);
    fclose(f);
    sqlHelper.updateSong(&song, song.getID());

    return 1;
}