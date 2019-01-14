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
import javax.inject.Inject

class MainViewModel
@Inject constructor(
    cc: CoroutineContextProvider,
    val taggerLib: TaggerLib
) : BaseViewModel(cc) {

    //Scanning data
    val fullSongList = MutableLiveData<List<AudioFile>>()
    val parsingCurrentSong = MutableLiveData<ScanningDialog.CurrentProgressWrapper>()
    val numberOfSongs = SingleLiveEvent<ScanningDialog.IntegerWrapper>()

    //Updates when song is selected
    val selectedSong = SingleLiveEvent<Int>()

    //Dialog stuff
    val cancel = SingleLiveEvent<Void>()
    val saveTags = SingleLiveEvent<Int>()
    val selectNewCover = SingleLiveEvent<Void>()
    val savingInProcess = SingleLiveEvent<Void>()

    var newCover: ByteArray? = null

    init {
        performInitialScan()
    }

    private fun performInitialScan() {
        launch(Dispatchers.IO) {
            taggerLib.scanDirectory("/storage/emulated/0/Music/", parsingCurrentSong, numberOfSongs)
            getAllAudioFiles()
        }
    }

    private fun getAllAudioFiles() {
        val songs = taggerLib.getAllAudioFiles()
        fullSongList.postValue(songs!!.asList())
    }

    fun audioFileSelected(position: Int) {
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
        if (!oldAudioFile.tagsEqual(title, album, artist, year, track, newCover)) {
            savingInProcess.call()
            launch {
                val success = taggerLib.updateNewTags(
                    oldAudioFile.id,
                    title.trim(),
                    album.trim(),
                    artist.trim(),
                    year.trim(),
                    track.trim(),
                    oldAudioFile.filePath,
                    newCover
                )
                getAllAudioFiles()
                newCover = null
                saveTags.postValue(success)
            }
        }
    }

    fun cancel() {
        cancel.call()
    }


}