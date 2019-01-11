package com.trippntechnology.tntmusicplayer.objects

data class AudioFile(
    val id: Int,

    //Tags
    val title: String,
    val album: String,
    val artist: String,
    val year: String,
    val track: String,
    val cover: ByteArray?,

    //Audio data
    val filePath: String,
    val duration: Long = 0,
    val sampleRate: Int = 0,
    val bitRate: Int = 0
) {

    fun tagsEqual(title: String,album: String,artist: String,year: String,track: String,cover: ByteArray?):Boolean{
        if (title!= this.title)return false
        if (album!= this.album)return false
        if (artist!=this.artist) return false
        if (year != this.year) return false
        if (track!=this.track)return false
        if (cover != null) {
            if (this.cover == null) return false
            return cover.contentEquals(this.cover)
        }
        return true
    }


    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as AudioFile

        if (id != other.id) return false
        if (title != other.title) return false
        if (album != other.album) return false
        if (artist != other.artist) return false
        if (year != other.year) return false
        if (track != other.track) return false
        if (cover != null) {
            if (other.cover == null) return false
            if (!cover.contentEquals(other.cover)) return false
        } else if (other.cover != null) return false
        if (filePath != other.filePath) return false
        if (duration != other.duration) return false
        if (sampleRate != other.sampleRate) return false
        if (bitRate != other.bitRate) return false

        return true
    }

    override fun hashCode(): Int {
        var result = id
        result = 31 * result + title.hashCode()
        result = 31 * result + album.hashCode()
        result = 31 * result + artist.hashCode()
        result = 31 * result + year.hashCode()
        result = 31 * result + track.hashCode()
        result = 31 * result + (cover?.contentHashCode() ?: 0)
        result = 31 * result + filePath.hashCode()
        result = 31 * result + duration.hashCode()
        result = 31 * result + sampleRate
        result = 31 * result + bitRate
        return result
    }
}