package com.trippntechnology.tntmusicplayer.activites.mainactivity

import androidx.lifecycle.MutableLiveData
import com.trippntechnology.tntmusicplayer.util.CoroutineContextProvider
import com.trippntechnology.tntmusicplayer.util.SingleLiveEvent
import com.trippntechnology.tntmusicplayer.nativewrappers.TaggerLib
import com.trippntechnology.tntmusicplayer.objects.AudioFile
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

    val fullSongList = MutableLiveData<List<AudioFile>>()
    val parsingCurrentSong = MutableLiveData<ScanningDialog.CurrentProgressWrapper>()
    val numberOfSongs = SingleLiveEvent<ScanningDialog.IntegerWrapper>()

    init {
        scanDirectory()
    }

    fun scanDirectory() {
        launch(Dispatchers.IO) {
            taggerLib.scanDirectory("/storage/emulated/0/Music/", parsingCurrentSong, numberOfSongs)
            val songs = taggerLib.getAllAudioFiles()
            fullSongList.postValue(songs!!.asList())
        }
    }

}