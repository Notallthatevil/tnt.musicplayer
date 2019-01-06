package com.trippntechnology.tntmusicplayer.dialogs.edittagdialog

import android.content.res.Resources
import android.graphics.BitmapFactory
import android.graphics.drawable.BitmapDrawable
import android.widget.ImageView
import com.trippntechnology.tntmusicplayer.R
import com.trippntechnology.tntmusicplayer.nativewrappers.TaggerLib
import com.trippntechnology.tntmusicplayer.objects.AudioFile
import com.trippntechnology.tntmusicplayer.util.CoroutineContextProvider
import com.trippntechnology.tntmusicplayer.util.SingleLiveEvent
import com.trippntechnology.tntmusicplayer.viewmodelcomponents.BaseViewModel
import javax.inject.Inject

class EditTagDialogViewModel @Inject constructor(
    cc: CoroutineContextProvider,
    val taggerLib: TaggerLib
) :
    BaseViewModel(cc) {

    val cancel = SingleLiveEvent<Void>()
    val newStuff =SingleLiveEvent<Void>()

    val newCover = false

    fun saveTags(
        title: String,
        album: String,
        artist: String,
        year: String,
        track: String,
        cover: ImageView,
        oldAudioFile: AudioFile) {

        if (title != oldAudioFile.title
            || album != oldAudioFile.album
            || artist != oldAudioFile.artist
            || year != oldAudioFile.year
            || track != oldAudioFile.track
            || newCover) {


        }
    }

    fun cancel() {
        cancel.call()
    }


}