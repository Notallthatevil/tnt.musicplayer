package com.trippntechnology.tntmusicplayer.ux.sharedviewmodels

import android.view.View
import androidx.lifecycle.MutableLiveData
import com.trippntechnology.tntmusicplayer.dialogs.scanningdialog.ScanningDialog
import com.trippntechnology.tntmusicplayer.nativewrappers.TaggerLib
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

    private var syncing = false
    private var newCover :ByteArray? = null

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
        audioFileLongClick.value = LongClickItem(view,position.toLong())
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
            showProgressWheel.postValue(-1)
            //FIXME this should just be done in the SQL
            if (newCover == null){
                newCover = TaggerLib.getCover(oldAudioFile.filePath,oldAudioFile.coverSize,oldAudioFile.coverOffset);
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

    fun cancel(){
        showProgressWheel.postValue(-2)
    }

    data class LongClickItem(val view: View,val position: Long)
}