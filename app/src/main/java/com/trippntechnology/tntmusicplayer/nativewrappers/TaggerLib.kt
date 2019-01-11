package com.trippntechnology.tntmusicplayer.nativewrappers

import androidx.lifecycle.MutableLiveData
import com.trippntechnology.tntmusicplayer.objects.AudioFile
import com.trippntechnology.tntmusicplayer.util.SingleLiveEvent
import com.trippntechnology.tntmusicplayer.dialogs.scanningdialog.ScanningDialog

object TaggerLib {

    external fun scanDirectory(
        directory: String,
        parsingCurrentSong: MutableLiveData<ScanningDialog.CurrentProgressWrapper>,
        numberOfSongs: SingleLiveEvent<ScanningDialog.IntegerWrapper>
    ): Array<String>?

    external fun getAllAudioFiles(): Array<AudioFile>?

    external fun updateNewTags(
        id: Int,
        title: String,
        album: String,
        artist: String,
        year: String,
        track: String,
        filepath: String,
        cover: ByteArray?
    ): Int?

    init {
        System.loadLibrary("tagger-lib")
    }
}
