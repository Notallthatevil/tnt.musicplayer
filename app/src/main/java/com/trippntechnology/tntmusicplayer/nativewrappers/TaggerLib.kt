package com.trippntechnology.tntmusicplayer.nativewrappers

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import com.trippntechnology.tntmusicplayer.dialogs.scanningdialog.ScanningDialog
import com.trippntechnology.tntmusicplayer.objects.AudioFile
import com.trippntechnology.tntmusicplayer.util.SingleLiveEvent

object TaggerLib {

    external fun scanDirectory(
        directory: String,
        parsingCurrentSong: MutableLiveData<ScanningDialog.CurrentProgressWrapper>,
        numberOfSongs: SingleLiveEvent<ScanningDialog.IntegerWrapper>
    ): Array<String>?

    external fun getAllAudioFiles(): LiveData<Array<AudioFile>>

    external fun backgroundScan(directory: String)

    external fun updateNewTags(id: Int, title: String, album: String, artist: String, year: String, track: String, filepath: String, cover: ByteArray?): Int

    fun updateNewTags(audioFile: AudioFile,cover:ByteArray):Int{
        val title = audioFile.title?:""
        val album = audioFile.album?:""
        val artist = audioFile.artist?:""
        val year = audioFile.year?:""
        val track = audioFile.track?:""
        return updateNewTags(audioFile.id,title,album,artist,year,track,audioFile.filePath,cover)
    }

    external fun songTableExist():Boolean

    external fun getCover(filePath:String,size:Int,offset:Int):ByteArray?

    init {
        System.loadLibrary("tagger-lib")
    }
}
