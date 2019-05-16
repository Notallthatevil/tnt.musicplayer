#include <jni.h>
#include <oboe/Oboe.h>
#include "TNTAudioEngine.h"


extern "C" {
    /**
     * Initializes the TNTAudioEngine
     * @return Whether or not initialization succeeded
     */
JNIEXPORT jlong
JNICALL
Java_com_trippntechnology_tntmusicplayer_nativewrappers_PlayerLib_nativeCreateEngine(JNIEnv *env, jobject/*this*/) {
    TNTAudioEngine *engine = new(std::nothrow) TNTAudioEngine();
    return reinterpret_cast<jlong>(engine);
}

/**
 * Destroys the TNTAudioEngine and frees the memory.
 */
JNIEXPORT void
JNICALL
Java_com_google_sample_oboe_hellooboe_PlaybackEngine_native_nativeDeleteEngine(
        JNIEnv *env,
        jclass,
        jlong engineHandle) {
    delete reinterpret_cast<TNTAudioEngine *>(engineHandle);
}


void JNICALL
Java_com_trippntechnology_tntmusicplayer_nativewrappers_PlayerLib_play(JNIEnv *env, jobject/*this*/, jint jid) {

}

}