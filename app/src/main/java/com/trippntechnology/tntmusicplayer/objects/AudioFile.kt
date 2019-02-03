package com.trippntechnology.tntmusicplayer.objects

data class AudioFile(
    val id: Int,
    val albumId:Int,
    //Tags
    val title: String?,
    val album: String?,
    val artist: String?,
    val year: String?,
    val track: String?,

    //Audio data
    val filePath: String,
    val duration: Long = 0,
    val sampleRate: Int = 0,
    val bitRate: Int = 0
) {

    fun tagsEqual(title: String,album: String,artist: String,year: String,track: String):Boolean{
        if (title!= this.title)return false
        if (album!= this.album)return false
        if (artist!=this.artist) return false
        if (year != this.year) return false
        if (track!=this.track)return false
        return true
    }
}