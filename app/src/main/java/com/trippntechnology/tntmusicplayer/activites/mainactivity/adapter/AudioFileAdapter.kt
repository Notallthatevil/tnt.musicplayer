package com.trippntechnology.tntmusicplayer.activites.mainactivity.adapter

import android.provider.MediaStore
import android.view.LayoutInflater
import android.view.ViewGroup
import androidx.databinding.DataBindingUtil
import androidx.recyclerview.widget.DiffUtil
import androidx.recyclerview.widget.ListAdapter
import androidx.recyclerview.widget.RecyclerView
import com.bumptech.glide.ListPreloader
import com.bumptech.glide.RequestBuilder
import com.bumptech.glide.load.engine.DiskCacheStrategy
import com.bumptech.glide.load.resource.bitmap.RoundedCorners
import com.bumptech.glide.load.resource.drawable.DrawableTransitionOptions
import com.trippntechnology.tntmusicplayer.R
import com.trippntechnology.tntmusicplayer.activites.mainactivity.MainViewModel
import com.trippntechnology.tntmusicplayer.databinding.ListItemAudioFileBinding
import com.trippntechnology.tntmusicplayer.glideutils.GlideApp
import com.trippntechnology.tntmusicplayer.nativewrappers.TaggerLib
import com.trippntechnology.tntmusicplayer.objects.AudioFile


class AudioFileAdapter(private val viewModel: MainViewModel) :
    ListAdapter<AudioFile, AudioFileAdapter.ViewHolder>(DIFF_CALLBACK) {

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): AudioFileAdapter.ViewHolder {
        return ViewHolder(parent).apply {
            binding.viewModel = viewModel
        }
    }

    override fun onBindViewHolder(holder: AudioFileAdapter.ViewHolder, position: Int) {
        holder.binding.audioFile = getItem(position)
        holder.binding.position = position
    }


    companion object {
        private val DIFF_CALLBACK =
            object : DiffUtil.ItemCallback<AudioFile>() {
                override fun areItemsTheSame(oldItem: AudioFile, newItem: AudioFile): Boolean {
                    return oldItem == newItem
                }

                override fun areContentsTheSame(oldItem: AudioFile, newItem: AudioFile): Boolean {
                    return oldItem.hashCode() == newItem.hashCode()
                }
            }
    }

    class ViewHolder(
        parent: ViewGroup, val binding: ListItemAudioFileBinding =
            DataBindingUtil.inflate(
                LayoutInflater.from(parent.context), R.layout.list_item_audio_file, parent, false
            )
    ) : RecyclerView.ViewHolder(binding.root)

}