package com.trippntechnology.tntmusicplayer.nativewrappers

object PlayerLib {
    external fun play(audioFileId: Int): Int

    init {
        System.loadLibrary("player-lib")
    }
}