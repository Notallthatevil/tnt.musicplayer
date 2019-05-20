# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library(
        audiofiles-lib
        SHARED
        audioclasses/base/AudioFile.cpp
        audioclasses/base/Tag.cpp

        #mp3
        audioclasses/mp3/files/Mp3File.cpp
        audioclasses/mp3/tags/ID3Tag.cpp
)

add_library(
        database-lib
        SHARED
        database/sqlite3.c
        database/SqlWrapper.cpp
)
add_library( # Sets the name of the library.
        tagger-lib
        SHARED
        tagger/tagger.cpp
        )

add_library(
        player-lib
        SHARED
        tntplayer/TNTAudioEngine.cpp
        tntplayer/SineGenerator.cpp
        tntplayer/player.cpp)

target_link_libraries(
        audiofiles-lib
        log
)

target_link_libraries(
        database-lib
        audiofiles-lib
)

target_link_libraries(
        tagger-lib
        database-lib
)

target_link_libraries(
        player-lib
        database-lib
        oboe
)

set(OBOE_DIR ../../../../oboe ABSOLUTE)
add_subdirectory(${OBOE_DIR})
include_directories(${OBOE_DIR}/include)
include_directories(utils)