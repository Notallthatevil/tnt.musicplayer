package com.trippntechnology.tntmusicplayer.binding

import android.widget.ImageView
import android.widget.TextView
import androidx.databinding.BindingAdapter
import com.trippntechnology.tntmusicplayer.R
import java.util.concurrent.TimeUnit

object CustomBinders{

    @JvmStatic
    @BindingAdapter("setImage")
    fun setImage(imageView: ImageView,image:ByteArray?){
        if (image==null){
            imageView.setImageResource(R.mipmap.ic_launcher)
        }
        //Temp
        imageView.setImageResource(R.mipmap.ic_launcher)
    }

    @JvmStatic
    @BindingAdapter("setDuration")
    fun setDuration(textView: TextView,duration:Long){
        val mmss = String.format("%02d:%02d",
            TimeUnit.MILLISECONDS.toMinutes(duration) % TimeUnit.HOURS.toMinutes(1),
            TimeUnit.MILLISECONDS.toSeconds(duration) % TimeUnit.MINUTES.toSeconds(1))
        textView.text = mmss
    }

}