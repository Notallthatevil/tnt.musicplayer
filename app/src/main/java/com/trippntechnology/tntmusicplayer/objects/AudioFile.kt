package com.trippntechnology.tntmusicplayer.objects

data class AudioFile(
    val id: Int = -1,
    //Tags
    val title: String?,
    val album: String?,
    val artist: String?,
    val year: String?,
    val track: String?,
    val coverOffset: Int = 0,
    val coverSize: Int = 0,

    //Audio data
    val filePath: String,
    val duration: Long = 0,
    val sampleRate: Int = 0,
    val bitRate: Int = 0
) {
    fun tagsEqual(title: String?, album: String?, artist: String?, year: String?, track: String?): Boolean {
        if (title != this.title || (title.isNullOrEmpty() == this.title.isNullOrEmpty())) return false
        if (album != this.album || (album.isNullOrEmpty() == this.album.isNullOrEmpty())) return false
        if (artist != this.artist || (artist.isNullOrEmpty() == this.artist.isNullOrEmpty())) return false
        if (year != this.year || (year.isNullOrEmpty() == this.year.isNullOrEmpty())) return false
        if (track != this.track || (track.isNullOrEmpty() == this.track.isNullOrEmpty())) return false
        return true
    }
}