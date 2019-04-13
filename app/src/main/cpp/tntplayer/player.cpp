#include <jni.h>

extern "C"
JNIEXPORT jint
JNICALL Java_com_trippntechnology_tntmusicplayer_nativewrappers_PlayerLib_play(JNIEnv *env,jobject/*this*/,jint jid){
    return jid;
}