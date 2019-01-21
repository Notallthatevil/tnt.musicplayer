package com.trippntechnology.tntmusicplayer.activites.mainactivity

import android.view.View
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
    private val taggerLib: TaggerLib
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
        if (taggerLib.songTableExist()) {
            fullSongList.value = taggerLib.getAllAudioFiles().asList()
            syncAudioFiles()
        } else {
            launch(Dispatchers.IO) {
                taggerLib.scanDirectory("/storage/emulated/0/Music/", parsingCurrentSong, numberOfSongs)
                updateFullSongList(taggerLib.getAllAudioFiles().asList())
            }
        }
    }

    //Background thread
    private fun updateFullSongList(audioFileList: List<AudioFile>?) {
        audioFileList ?: return
        fullSongList.postValue(audioFileList)
    }

    fun audioFileSelected(view: View, position: Int): Boolean {
        selectedSong.value = position
        return true
    }

    fun syncAudioFiles() {
        launch (Dispatchers.IO) {
            updateFullSongList(taggerLib.backgroundScan("/storage/emulated/0/Music/")?.asList())
        }
    }


    //DIALOG============================================================================================================
    fun selectAlbumArt() {
        selectNewCover.call()
    }

    fun saveTags(title: String, album: String, artist: String, year: String, track: String, oldAudioFile: AudioFile) {
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
                updateFullSongList(taggerLib.getAllAudioFiles().asList())
                newCover = null
                saveTags.postValue(success)
            }
        }
    }

    fun cancel() {
        cancel.call()
    }


}