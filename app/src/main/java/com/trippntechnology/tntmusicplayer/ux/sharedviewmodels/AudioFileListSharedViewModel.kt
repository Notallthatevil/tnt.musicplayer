package com.trippntechnology.tntmusicplayer.ux.sharedviewmodels

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.view.View
import androidx.lifecycle.MutableLiveData
import com.trippntechnology.tntmusicplayer.dialogs.scanningdialog.ScanningDialog
import com.trippntechnology.tntmusicplayer.nativewrappers.TaggerLib
import com.trippntechnology.tntmusicplayer.network.coverartretriever.CoverArtRetriever
import com.trippntechnology.tntmusicplayer.objects.AudioFile
import com.trippntechnology.tntmusicplayer.util.CoroutineContextProvider
import com.trippntechnology.tntmusicplayer.util.SingleLiveEvent
import com.trippntechnology.tntmusicplayer.util.viewmodels.BaseViewModel
import kotlinx.coroutines.Dispatchers
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

    val showProgressWheel = MutableLiveData<Int>()

    val updateImageView = MutableLiveData<Bitmap?>()

    private var syncing = false
    private var newCover: ByteArray? = null

    fun startup() {
        if (TaggerLib.songTableExist()) {
            syncAudioFiles()
        } else {
            launch(Dispatchers.IO) {
                syncing = true
                TaggerLib.scanDirectory("/storage/emulated/0/Music/", currentProgress, maxProgress)
                syncing = false
            }
        }
    }

    fun audioFileLongClick(view: View, position: Int): Boolean {
        audioFileLongClick.value = LongClickItem(view, position.toLong())
        return true
    }

    private fun syncAudioFiles() {
        if (!syncing) {
            launch(Dispatchers.IO) {
                syncing = true
                TaggerLib.backgroundScan("/storage/emulated/0/Music/")
                syncing = false
            }
        }
    }


    //Edit Tag Fragment=================================================================================================
    fun saveTags(title: String, album: String, artist: String, year: String, track: String, oldAudioFile: AudioFile) {
        if (!oldAudioFile.tagsEqual(title, album, artist, year, track) || newCover != null) {
            showProgressWheel.postValue(SAVING_TAGS)
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
                showProgressWheel.postValue(success)
            }
        }
    }

    fun autoFindAlbumArt(
        title: String,
        album: String,
        artist: String,
        year: String,
        track: String,
        oldAudioFile: AudioFile) {
        showProgressWheel.postValue(AUTO_FIND_COVER)
        launch {
            val newAudioFile = AudioFile(id = oldAudioFile.id, title = title, album = album, artist = artist, year = year, track = track, filePath = oldAudioFile.filePath)
            newCover = CoverArtRetriever().autoFindAlbumArt(newAudioFile)
            val bitmap = if (newCover != null){
                BitmapFactory.decodeByteArray(newCover,0,newCover!!.size)
            }else{
                null
            }
            updateImageView.postValue(bitmap)
            showProgressWheel.postValue(AUTO_FIND_COVER_FINISHED)
        }
    }

    fun cancel() {
        showProgressWheel.postValue(CANCEL)
    }

    data class LongClickItem(val view: View, val position: Long)

    companion object {
        const val CANCEL = 101
        const val SAVING_TAGS = 100
        const val TAGS_SAVED = 0
        const val AUTO_FIND_COVER = 102
        const val AUTO_FIND_COVER_FINISHED = 103
    }
}