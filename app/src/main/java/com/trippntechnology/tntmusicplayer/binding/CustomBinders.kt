package com.trippntechnology.tntmusicplayer.binding

import android.graphics.Bitmap
import android.widget.ImageView
import android.widget.TextView
import androidx.databinding.BindingAdapter
import androidx.recyclerview.widget.ListAdapter
import androidx.recyclerview.widget.RecyclerView
import com.bumptech.glide.load.resource.bitmap.RoundedCorners
import com.trippntechnology.tntmusicplayer.R
import com.trippntechnology.tntmusicplayer.nativewrappers.TaggerLib
import com.trippntechnology.tntmusicplayer.objects.AudioFile
import java.util.concurrent.TimeUnit
import com.bumptech.glide.load.engine.DiskCacheStrategy
import com.trippntechnology.tntmusicplayer.glideutils.GlideApp

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
        CustomBinders.setImage(imageView, cover)
    }

    fun setImage(imageView: ImageView, coverBytes: ByteArray?) {
        GlideApp.with(imageView)
            .load(coverBytes)
            .placeholder(R.mipmap.ic_launcher)
            .transform(RoundedCorners(12))
            .diskCacheStrategy(DiskCacheStrategy.NONE)
            .override(imageView.width, imageView.height)
            .into(imageView)
    }

    fun setImage(imageView: ImageView, bitmap: Bitmap?) {
        GlideApp.with(imageView)
            .load(bitmap)
            .placeholder(R.mipmap.ic_launcher)
            .transform(RoundedCorners(12))
            .diskCacheStrategy(DiskCacheStrategy.NONE)
            .override(imageView.width, imageView.height)
            .into(imageView)
    }

    @JvmStatic
    @BindingAdapter("setList")
    fun <T, VH : RecyclerView.ViewHolder> setList(recyclerView: RecyclerView, list: Array<T>?) {
        list ?: return

        @Suppress("UNCHECKED_CAST")
        when {
            recyclerView.adapter is ListAdapter<*, *> -> {
                val adapter = recyclerView.adapter as ListAdapter<T, VH>
                adapter.submitList(list.toList())
            }
            else -> error("Must use a ListAdapter for app:setList")
        }
    }

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
