package com.trippntechnology.tntmusicplayer.nativewrappers

import androidx.lifecycle.MutableLiveData
import com.trippntechnology.tntmusicplayer.objects.AudioFile
import com.trippntechnology.tntmusicplayer.util.SingleLiveEvent
import com.trippntechnology.tntmusicplayer.widgets.ScanningDialog

object TaggerLib {

    external fun scanDirectory(
        directory: String,
        parsingCurrentSong: MutableLiveData<ScanningDialog.CurrentProgressWrapper>,
        numberOfSongs: SingleLiveEvent<ScanningDialog.IntegerWrapper>
    ): Array<String>?

    external fun getAllAudioFiles():Array<AudioFile>

    init {
        System.loadLibrary("tagger-lib")
    }
}
