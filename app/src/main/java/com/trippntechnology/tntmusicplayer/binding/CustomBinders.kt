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
import java.util.concurrent.TimeUnit

object CustomBinders {


    /**
     * Sets the @param imageView's image to the cover that the @filePath points too. Otherwise the
     * image remains the placeholder image.
     * @param imageView - The image view the desired image should be attached to
     * @param filePath - The absolute file path to the image
     * @param size - The size of the image
     * @param offset - Where to begin looking for the image in the file
     */
    @JvmStatic
    @BindingAdapter("setImage")
    fun setImage(imageView: ImageView, filePath: String, size: Int, offset: Int) {
        var cover: ByteArray? = null
        if (size > 0 && offset >= 0) {
            cover = TaggerLib.getCover(filePath, size, offset)
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

    /**
     * Sets the given @param textView's text to the duration in mm:ss format.
     * @param textView - The text view to set the text for
     * @param duration - The duration of the audio file in milliseconds
     */
    @JvmStatic
    @BindingAdapter("setDuration")
    fun setDuration(textView: TextView, duration: Long) {
        textView.text = String.format(
            "%d:%02d",
            TimeUnit.MILLISECONDS.toMinutes(duration) % TimeUnit.HOURS.toMinutes(1),
            TimeUnit.MILLISECONDS.toSeconds(duration) % TimeUnit.MINUTES.toSeconds(1)
        )
    }

}