package com.trippntechnology.tntmusicplayer.binding

import android.widget.ImageView
import android.widget.TextView
import androidx.databinding.BindingAdapter
import com.bumptech.glide.Glide
import com.bumptech.glide.load.resource.bitmap.RoundedCorners
import com.bumptech.glide.request.RequestOptions
import com.bumptech.glide.load.resource.drawable.DrawableTransitionOptions.withCrossFade
import com.trippntechnology.tntmusicplayer.R
import java.util.concurrent.TimeUnit

object CustomBinders {

    @JvmStatic
    @BindingAdapter("setImage")
    fun setImage(imageView: ImageView, image: ByteArray?) {
        val requestOptions = RequestOptions()
            .placeholder(R.drawable.ic_launcher_background)
            .transforms(RoundedCorners(12))
        Glide.with(imageView)
            .load(image)
            .apply(requestOptions)
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