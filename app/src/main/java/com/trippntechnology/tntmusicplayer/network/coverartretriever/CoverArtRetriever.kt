package com.trippntechnology.tntmusicplayer.network.coverartretriever

import com.trippntechnology.tntmusicplayer.network.jsonreader.CoverArtJsonReader
import com.trippntechnology.tntmusicplayer.network.xmlparser.CoverArtParser
import com.trippntechnology.tntmusicplayer.objects.AudioFile
import com.trippntechnology.tntmusicplayer.objects.XMLAlbum
import org.xmlpull.v1.XmlPullParserException
import timber.log.Timber
import java.io.ByteArrayOutputStream
import java.io.FileNotFoundException
import java.io.IOException
import java.net.URL


class CoverArtRetriever {


    fun autoFindAlbumArt(audioFile: AudioFile): ByteArray? {
        var albumIds: List<XMLAlbum>? = null
        try {
            try {
                albumIds = retrieveAlbumIDs(audioFile)
            } catch (e: XmlPullParserException) {
                e.printStackTrace()
            } catch (e: IOException) {
                e.printStackTrace()
            }
            albumIds ?: return null
            val coverArtURL = URL("$BASE_COVER_URL${albumIds[0].xmlID}")

            Timber.d("Cover art URL: $BASE_COVER_URL${albumIds[0].xmlID}")
            val imageURL = CoverArtJsonReader().getCoverArtUrl(coverArtURL.openStream())


            val imageStream = URL(imageURL).openStream()
            val boas = ByteArrayOutputStream()
            val buffer = ByteArray(1024)
            var count = imageStream.read(buffer)
            while (count != -1) {
                boas.write(buffer, 0, count)
                count = imageStream.read(buffer)
            }
            return boas.toByteArray()
        } catch (e: FileNotFoundException) {
            return null
        }
    }


    @Throws(XmlPullParserException::class, IOException::class)
    private fun retrieveAlbumIDs(audioFile: AudioFile): List<XMLAlbum>? {
        val urlStream = URL(createSearchQuery(audioFile)).openStream()
        val list = CoverArtParser().parse(urlStream, audioFile)
        Timber.d("list has returned")
        if (list != null) {
            return list
        }
        val albumSearch = albumSearch(audioFile)
        if (albumSearch != null) {
            Timber.d("album has returned")
            return albumSearch
        }
        val songTitleSearch = songTitleSearch(audioFile)
        if (songTitleSearch != null) {
            Timber.d( "title has returned")
            return songTitleSearch
        }
        val artistSearch = artistSearch(audioFile)
        if (artistSearch != null) {
            Timber.d( "artist has returned")
            return artistSearch
        }
        Timber.d("returning null")
        return null
    }

    private fun albumSearch(audioFile: AudioFile): List<XMLAlbum>? {
        val urlStream = URL(createAlbumSearchQuery(audioFile.album)).openStream()
        return CoverArtParser().parse(urlStream, audioFile)
    }

    private fun songTitleSearch(audioFile: AudioFile): List<XMLAlbum>? {
        val urlStream = URL(createSongTitleSearchQuery(audioFile.title)).openStream()
        return CoverArtParser().parse(urlStream, audioFile)
    }

    private fun artistSearch(audioFile: AudioFile): List<XMLAlbum>? {
        val artistIdStream = URL(createArtistSearchQuery(audioFile.artist)).openStream()
        val parser = CoverArtParser()
        val artistIds = parser.parse(artistIdStream, audioFile)
        return if (artistIds.isNullOrEmpty()) {
            null
        } else {
            val urlStream = URL(createArtistIdLookup(artistIds[0].xmlID)).openStream()
            parser.parse(urlStream, audioFile)
        }
    }

    private fun createSearchQuery(audioFile: AudioFile): String? {
        var query: String
        when {
            audioFile.album != null -> {
                query = ALBUM_QUERY + "${audioFile.album}"
                if (audioFile.artist != null) {
                    query += " AND artist:${audioFile.artist}"
                }
            }
            audioFile.title != null -> {
                query = SONG_TITLE_QUERY + "${audioFile.title}"
                if (audioFile.artist != null) {
                    query += " AND artist:${audioFile.artist}"
                }
            }
            audioFile.artist != null -> query = ARTIST_QUERY + "${audioFile.artist}"
            else -> {
                return null
            }
        }
        query += "&limit=7"
        return query
    }

    private fun createAlbumSearchQuery(album: String?): String {
        return if (album != null) {
            "$ALBUM_QUERY$album&limit=7"
        } else {
            return ""
        }
    }

    private fun createSongTitleSearchQuery(title: String?): String {
        return if (title != null) {
            "$SONG_TITLE_QUERY$title&limit=7"
        } else {
            return ""
        }
    }

    private fun createArtistSearchQuery(artist: String?): String {
        return if (artist != null) {
            "$ARTIST_QUERY$artist&limit=7"
        } else {
            return ""
        }
    }

    private fun createArtistIdLookup(artistId: String?): String {
        return if (artistId != null) {
            "$ARTIST_LOOKUP$artistId$INCLUDE_ALBUM_SEARCH"
        } else {
            return ""
        }
    }

    companion object {
        private const val BASE_MUSIC_URL = "https://musicbrainz.org/ws/2/"
        const val ARTIST_QUERY = BASE_MUSIC_URL + "artist/?query="
        const val ARTIST_LOOKUP = BASE_MUSIC_URL + "artist/"
        const val ALBUM_QUERY = BASE_MUSIC_URL + "release-group/?query="
        const val SONG_TITLE_QUERY = BASE_MUSIC_URL + "recording/?query="
        const val INCLUDE_ALBUM_SEARCH = "?inc=release-groups"


        private const val BASE_COVER_URL = "https://coverartarchive.org/release-group/"
    }
}
