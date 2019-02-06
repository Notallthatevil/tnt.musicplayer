package com.trippntechnology.tntmusicplayer.binding

import android.widget.ImageView
import android.widget.TextView
import androidx.databinding.BindingAdapter
import com.bumptech.glide.Glide
import com.bumptech.glide.load.resource.bitmap.RoundedCorners
import com.bumptech.glide.request.RequestOptions
import com.bumptech.glide.load.resource.drawable.DrawableTransitionOptions.withCrossFade
import com.trippntechnology.tntmusicplayer.R
import com.trippntechnology.tntmusicplayer.nativewrappers.TaggerLib
import com.trippntechnology.tntmusicplayer.objects.AudioFile
import java.util.concurrent.TimeUnit

object CustomBinders {

    /**
     * Sets the @param imageView's image to the cover that the @filePath points too. Otherwise the
     * image remains the placeholder image.
     * @param imageView - The image view the desired image should be attached to
     * @param audioFile - The audio file to look for the cover for
     */
    @JvmStatic
    @BindingAdapter("setImage")
    fun setImage(imageView: ImageView, audioFile: AudioFile) {
        var cover: ByteArray? = null
        if (audioFile.coverSize > 0 && audioFile.coverOffset >= 0) {
            cover = TaggerLib.getCover(audioFile.filePath, audioFile.coverSize, audioFile.coverOffset)
        }
        Glide.with(imageView)
            .load(cover)
            .apply(
                RequestOptions()
                    .placeholder(R.mipmap.ic_launcher)
                    .transforms(RoundedCorners(12))
            )
            .transition(withCrossFade())
            .into(imageView)
    }

    @JvmStatic
    @BindingAdapter("setDuration")
    fun setDuration(textView: TextView, duration: Long) {
        val mmss = String.format(
            "%d:%02d",
            TimeUnit.MILLISECONDS.toMinutes(duration) % TimeUnit.HOURS.toMinutes(1),
            TimeUnit.MILLISECONDS.toSeconds(duration) % TimeUnit.MINUTES.toSeconds(1)
        )
        textView.text = mmss
    }

}