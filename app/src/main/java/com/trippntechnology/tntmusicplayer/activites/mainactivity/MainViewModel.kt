package com.trippntechnology.tntmusicplayer.activites.mainactivity

import android.util.Log
import android.view.View
import androidx.lifecycle.MutableLiveData
import com.trippntechnology.tntmusicplayer.util.CoroutineContextProvider
import com.trippntechnology.tntmusicplayer.util.SingleLiveEvent
import com.trippntechnology.tntmusicplayer.nativewrappers.TaggerLib
import com.trippntechnology.tntmusicplayer.objects.AudioFile
import com.trippntechnology.tntmusicplayer.viewmodelcomponents.BaseViewModel
import com.trippntechnology.tntmusicplayer.dialogs.scanningdialog.ScanningDialog
import com.trippntechnology.tntmusicplayer.network.coverartretriever.CoverArtRetriever
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
    val updateDialogCover = SingleLiveEvent<ByteArray>()


    var newCover: ByteArray? = null

    private var syncing = false

    fun startUp() {
        if (taggerLib.songTableExist()) {
            fullSongList.value = taggerLib.getAllAudioFiles().asList()
            syncAudioFiles()
        } else {
            launch(Dispatchers.IO) {
                syncing = true
                taggerLib.scanDirectory("/storage/emulated/0/Music/", parsingCurrentSong, numberOfSongs)
                updateFullSongList(taggerLib.getAllAudioFiles().asList())
                syncing = false
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
        if (!syncing) {
            launch(Dispatchers.IO) {
                syncing = true
                updateFullSongList(taggerLib.backgroundScan("/storage/emulated/0/Music/")?.asList())
                syncing = false
            }
        }
    }


    //DIALOG============================================================================================================
    fun selectAlbumArt() {
        selectNewCover.call()
    }

    fun autoFindAlbumArt(
        title: String,
        album: String,
        artist: String,
        year: String,
        track: String,
        oldAudioFile: AudioFile
    ) {
        launch {

            val newAudioFile = AudioFile(
                id = oldAudioFile.id,
                title = title,
                album = album,
                artist = artist,
                year = year,
                track = track,
                filePath = oldAudioFile.filePath
            )
            newCover = CoverArtRetriever().autoFindAlbumArt(newAudioFile)
            updateDialogCover.postValue(newCover)
        }
    }

    fun autoFindAllAlbumArt() {
        Log.d("AUTO_FIND_COVER", "Not ready to implement")
        launch {
            val list = fullSongList.value as MutableList
            if (!list.isNullOrEmpty()) {
                list.forEachIndexed { index, audioFile ->
                    Log.d("AUTO_FIND_COVER", "On file $index of ${list.size}")
                    if (audioFile.coverSize < 1) {
                        Log.d("AUTO_FIND_COVER", "Adding cover to ${audioFile.title}")
                        val newCover = CoverArtRetriever().autoFindAlbumArt(audioFile)
                        if (newCover != null) {
                            taggerLib.updateNewTags(audioFile, newCover)
                        }
                    }
                }
            }
            updateFullSongList(taggerLib.getAllAudioFiles().asList())
        }
    }

    fun saveTags(title: String, album: String, artist: String, year: String, track: String, oldAudioFile: AudioFile) {
        if (!oldAudioFile.tagsEqual(title, album, artist, year, track) || newCover != null) {
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