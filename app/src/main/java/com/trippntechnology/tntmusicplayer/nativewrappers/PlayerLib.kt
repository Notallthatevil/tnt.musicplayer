package com.trippntechnology.tntmusicplayer.nativewrappers

import android.content.Context
import android.media.AudioManager

object PlayerLib {
    private var mEngineHandle: Long = 0


    fun createEngine(/*context:Context*/):Boolean{
//        if (mEngineHandle == 0L){
////            setDefaultStreamValues(context)
//            mEngineHandle = nativeCreateEngine()
//        }
//        return mEngineHandle != 0L
        return false
    }

    fun deleteEngine(){
//        if (mEngineHandle != 0L) {
//            nativeDeleteEngine(mEngineHandle)
//        }
//        mEngineHandle = 0
    }

    /**
     * Initializes the TNTAudioEngine
     * @return Whether or not initialization succeeded
     */
    external fun nativeCreateEngine():Long

    /**
     * Destroys the TNTAudioEngine and frees the memory.
     */
//    external fun nativeDeleteEngine(mEngineHandle:Long)

    external fun play(audioFileId: Int)

    init {
        System.loadLibrary("player-lib")
    }
}