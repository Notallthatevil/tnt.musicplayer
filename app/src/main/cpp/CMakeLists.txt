# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library(
        baseclass-lib
        SHARED
        baseclass/AudioFile.cpp
        baseclass/Tag.cpp
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
        tagger/files/Mp3File.cpp
        tagger/tags/ID3Tag.cpp
        tagger/tagger.cpp
        )

add_library(
        player-lib
        SHARED
        tntplayer/TNTAudioPlayer.cpp
        tntplayer/player.cpp)

target_link_libraries(
        baseclass-lib
        log
)

target_link_libraries(
        database-lib
        baseclass-lib
)

target_link_libraries(
        tagger-lib
        database-lib
)

target_link_libraries(
        player-lib
        database-lib
)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

#find_library( # Sets the name of the path variable.
#        log-lib
#
#        # Specifies the name of the NDK library that
#        # you want CMake to locate.
#        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

#target_link_libraries(
#        tagger-lib
#        #        sqlite-lib
#        #        base-file-lib
#        log
#)

#target_link_libraries(
#        sqlite-lib
#        base-file-lib
##        log
#)
#
#target_link_libraries(
#        base-file-lib
#        log
#)
