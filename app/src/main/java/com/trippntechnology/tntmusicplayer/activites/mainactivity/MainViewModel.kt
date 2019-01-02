package com.trippntechnology.tntmusicplayer.activites.mainactivity

import androidx.lifecycle.MutableLiveData
import com.trippntechnology.tntmusicplayer.util.CoroutineContextProvider
import com.trippntechnology.tntmusicplayer.util.SingleLiveEvent
import com.trippntechnology.tntmusicplayer.viewmodelcomponents.BaseViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import javax.inject.Inject

class MainViewModel
@Inject constructor(cc: CoroutineContextProvider) : BaseViewModel(cc) {

    val fullSongList = MutableLiveData<Array<String>>()
    val parsingCurrentSong = MutableLiveData<String>()
    val numberOfSongs = SingleLiveEvent<Int>()

    init {
        scanDirectory()
    }

    fun scanDirectory() {
        launch(Dispatchers.IO) {
            val songs = scanDirectory("/storage/emulated/0/Music/", parsingCurrentSong, numberOfSongs)
            fullSongList.postValue(songs)
        }
    }

    private external fun scanDirectory(
        directory: String,
        parsingCurrentSong: MutableLiveData<String>,
        numberOfSongs: SingleLiveEvent<Int>
    ): Array<String>?

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("tagger-lib")
        }
    }
}