package com.trippntechnology.tntmusicplayer.ux.sharedviewmodels

import android.util.Log
import android.view.View
import androidx.lifecycle.MutableLiveData
import com.trippntechnology.tntmusicplayer.dialogs.scanningdialog.ScanningDialog
import com.trippntechnology.tntmusicplayer.nativewrappers.TaggerLib
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

    val audioFileSelected = SingleLiveEvent<Int>()

    val currentProgress = MutableLiveData<ScanningDialog.CurrentProgressWrapper>()
    val maxProgress = SingleLiveEvent<ScanningDialog.IntegerWrapper>()

    private var syncing = false

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


    fun audioFileSelected(position: Int): Boolean {
        audioFileSelected.value = position
        Log.d("AudioFileListShared", position.toString())
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
}