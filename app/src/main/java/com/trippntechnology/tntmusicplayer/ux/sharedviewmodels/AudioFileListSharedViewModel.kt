package com.trippntechnology.tntmusicplayer.ux.sharedviewmodels

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.util.Log
import android.view.View
import androidx.lifecycle.MutableLiveData
import com.trippntechnology.tntmusicplayer.dialogs.scanningdialog.ScanningDialog
import com.trippntechnology.tntmusicplayer.nativewrappers.PlayerLib
import com.trippntechnology.tntmusicplayer.nativewrappers.TaggerLib
import com.trippntechnology.tntmusicplayer.network.coverartretriever.CoverArtRetriever
import com.trippntechnology.tntmusicplayer.objects.AudioFile
import com.trippntechnology.tntmusicplayer.util.CoroutineContextProvider
import com.trippntechnology.tntmusicplayer.util.SingleLiveEvent
import com.trippntechnology.tntmusicplayer.util.viewmodels.BaseViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch
import javax.inject.Inject

class AudioFileListSharedViewModel
@Inject constructor(
    cc: CoroutineContextProvider
) : BaseViewModel(cc) {

    val audioFileList = TaggerLib.getAllAudioFiles()

    val audioFileLongClick = SingleLiveEvent<LongClickItem>()

    val currentProgress = MutableLiveData<ScanningDialog.CurrentProgressWrapper>()
    val maxProgress = SingleLiveEvent<ScanningDialog.IntegerWrapper>()

    val editTagValues = MutableLiveData<Int>()

    val selectCover = SingleLiveEvent<Void>()
    val updateImageView = MutableLiveData<Bitmap?>()

    var autoFindJob:Job?=null

    private var syncing = false
    var newCover: ByteArray? = null

    fun startup() {
        if (TaggerLib.songTableExist()) {
            syncAudioFiles()
        } else {
            launch(Dispatchers.IO) {
                syncing = true
                TaggerLib.scanDirectory("/storage/emulated/0/", currentProgress, maxProgress)
                syncing = false
            }
        }
    }

    fun audioFileLongClick(view: View, position: Int): Boolean {
        audioFileLongClick.value = LongClickItem(view, position.toLong())
        return true
    }

    fun audioFileShortClick(id:Int){
        Log.i("PLAYER",PlayerLib.play(id).toString())
    }

    private fun syncAudioFiles() {
        if (!syncing) {
            launch(Dispatchers.IO) {
                syncing = true
                TaggerLib.backgroundScan("/storage/emulated/0/")
                syncing = false
            }
        }
    }


    //Edit Tag Fragment=================================================================================================
    fun saveTags(title: String, album: String, artist: String, year: String, track: String, oldAudioFile: AudioFile) {
        if (!oldAudioFile.tagsEqual(title, album, artist, year, track) || newCover != null) {
            editTagValues.postValue(SAVING_TAGS)
            //FIXME this should just be done in the SQL
            if (newCover == null) {
                newCover = TaggerLib.getCover(oldAudioFile.filePath, oldAudioFile.coverSize, oldAudioFile.coverOffset)
            }
            launch {
                val success = TaggerLib.updateNewTags(
                    oldAudioFile.id,
                    title.trim(),
                    album.trim(),
                    artist.trim(),
                    year.trim(),
                    track.trim(),
                    oldAudioFile.filePath,
                    newCover
                )
                newCover = null
                editTagValues.postValue(success)
            }
        }
    }

    fun selectAlbumArt(){
//        editTagValues.value = SELECT_ALBUM_ART
        selectCover.call()
    }

    fun autoFindAlbumArt(title: String,album: String,artist: String,year: String, track: String, oldAudioFile: AudioFile) {
        editTagValues.postValue(AUTO_FIND_ALBUM_ART)
        launch {
            val newAudioFile = AudioFile(id = oldAudioFile.id, title = title, album = album, artist = artist, year = year, track = track, filePath = oldAudioFile.filePath)
            newCover = CoverArtRetriever().autoFindAlbumArt(newAudioFile)
            val bitmap = if (newCover != null){
                BitmapFactory.decodeByteArray(newCover,0,newCover!!.size)
            }else{
                null
            }
            updateImageView.postValue(bitmap)
            editTagValues.postValue(AUTO_FIND_ALBUM_ART_FINISHED)
        }
    }

    fun autoFindAllAlbumArt(){
        Log.d("AUTO_FIND_COVER", "Not ready to implement")
        autoFindJob = launch {
            val list = audioFileList.value
            if (!list.isNullOrEmpty()) {
                list.forEachIndexed { index, audioFile ->
                    Log.d("AUTO_FIND_COVER", "On file $index of ${list.size}")
                    if (audioFile.coverSize < 1) {
                        Log.d("AUTO_FIND_COVER", "Adding cover to ${audioFile.title}")
                        val newCover = CoverArtRetriever().autoFindAlbumArt(audioFile)
                        if (!isActive){
                            Log.d("AUTO_FIND_COVER", "Job canceled")
                            return@launch
                        }
                        if (newCover != null) {
                            TaggerLib.updateNewTags(audioFile, newCover)
                        }
                    }

                }
            }
        }
    }

    fun cancel() {
        editTagValues.postValue(CANCEL)
    }

    data class LongClickItem(val view: View, val position: Long)

    companion object {
        const val CANCEL = 101
        const val SAVING_TAGS = 100
        const val TAGS_SAVED = 0
        const val AUTO_FIND_ALBUM_ART = 200
        const val AUTO_FIND_ALBUM_ART_FINISHED = 201
        const val SELECT_ALBUM_ART = 300
    }
}