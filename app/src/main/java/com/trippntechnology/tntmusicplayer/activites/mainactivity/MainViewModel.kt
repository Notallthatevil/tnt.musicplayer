package com.trippntechnology.tntmusicplayer.activites.mainactivity

import androidx.lifecycle.MutableLiveData
import com.trippntechnology.tntmusicplayer.util.CoroutineContextProvider
import com.trippntechnology.tntmusicplayer.util.SingleLiveEvent
import com.trippntechnology.tntmusicplayer.nativewrappers.TaggerLib
import com.trippntechnology.tntmusicplayer.objects.AudioFile
import com.trippntechnology.tntmusicplayer.viewmodelcomponents.BaseViewModel
import com.trippntechnology.tntmusicplayer.dialogs.scanningdialog.ScanningDialog
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.text.FieldPosition
import javax.inject.Inject

class MainViewModel
@Inject constructor(
    cc: CoroutineContextProvider,
    val taggerLib: TaggerLib
) : BaseViewModel(cc) {

    val fullSongList = MutableLiveData<List<AudioFile>>()
    val parsingCurrentSong = MutableLiveData<ScanningDialog.CurrentProgressWrapper>()
    val numberOfSongs = SingleLiveEvent<ScanningDialog.IntegerWrapper>()

    val selectedSong = SingleLiveEvent<Int>()

    val cancel = SingleLiveEvent<Void>()
    val newStuff = SingleLiveEvent<Int>()
    val selectNewCover = SingleLiveEvent<Void>()

    var newCover: ByteArray? = null

    init {
        performInitialScan()
    }

    private fun performInitialScan() {
        launch(Dispatchers.IO) {
            taggerLib.scanDirectory("/storage/emulated/0/Music/", parsingCurrentSong, numberOfSongs)
            val songs = taggerLib.getAllAudioFiles()
            fullSongList.postValue(songs!!.asList())
        }
    }

    fun audioFileSelected(position: Int){
        selectedSong.value = position
    }

    fun selectAlbumArt() {
        selectNewCover.call()
    }


    fun saveTags(
        title: String,
        album: String,
        artist: String,
        year: String,
        track: String,
        oldAudioFile: AudioFile
    ) {
//        if (!oldAudioFile.tagsEqual(title, album, artist, year, track, newCover)) {
//            val success = taggerLib.updateNewTags(
//                oldAudioFile.id,
//                title,
//                album,
//                artist,
//                year,
//                track,
//                oldAudioFile.filePath,
//                newCover
//            )
//
//            newStuff.postValue(success)
//        }
    }

    fun cancel() {
        cancel.call()
    }



}