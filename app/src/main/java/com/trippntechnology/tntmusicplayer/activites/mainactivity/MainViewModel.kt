package com.trippntechnology.tntmusicplayer.activites.mainactivity

import androidx.lifecycle.MutableLiveData
import com.trippntechnology.tntmusicplayer.util.CoroutineContextProvider
import com.trippntechnology.tntmusicplayer.util.SingleLiveEvent
import com.trippntechnology.tntmusicplayer.nativewrappers.TaggerLib
import com.trippntechnology.tntmusicplayer.viewmodelcomponents.BaseViewModel
import com.trippntechnology.tntmusicplayer.widgets.ScanningDialog
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import javax.inject.Inject

class MainViewModel
@Inject constructor(
    cc: CoroutineContextProvider,
    val taggerLib: TaggerLib
) : BaseViewModel(cc) {

    val fullSongList = MutableLiveData<Array<String>>()
    val parsingCurrentSong = MutableLiveData<ScanningDialog.CurrentProgressWrapper>()
    val numberOfSongs = SingleLiveEvent<ScanningDialog.IntegerWrapper>()

    init {
        scanDirectory()
    }

    fun scanDirectory() {
        launch(Dispatchers.IO) {
            val songs = taggerLib.scanDirectory("/storage/emulated/0/Music/", parsingCurrentSong, numberOfSongs)
            fullSongList.postValue(songs)
        }
    }

}