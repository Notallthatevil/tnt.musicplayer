package com.trippntechnology.tntmusicplayer.binding

import android.widget.ImageView
import android.widget.TextView
import androidx.databinding.BindingAdapter
import androidx.recyclerview.widget.ListAdapter
import androidx.recyclerview.widget.RecyclerView
import com.trippntechnology.tntmusicplayer.R
import java.util.concurrent.TimeUnit

object CustomBinders{

//    @JvmStatic
//    @BindingAdapter("list")
//    fun <T, VH : RecyclerView.ViewHolder> setItems(recyclerView: RecyclerView, list: List<T>?) {
//        list ?: return
//
//        when {
//            recyclerView.adapter is ListAdapter<*, *> -> {
//                @Suppress("UNCHECKED_CAST")
//                val adapter = recyclerView.adapter as ListAdapter<T, VH>
//                adapter.submitList(list)
//            }
//            else -> error("Must use a ListAdapter app:list")
//        }
//    }

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