package com.trippntechnology.tntmusicplayer.network.coverartretriever

import android.util.Log
import com.trippntechnology.tntmusicplayer.network.xmlparser.CoverArtParer
import com.trippntechnology.tntmusicplayer.objects.AudioFile
import com.trippntechnology.tntmusicplayer.objects.XMLAlbum
import java.net.URL


class CoverArtRetriever {


    fun retrieveAlbumIDs(audioFile: AudioFile):List<XMLAlbum>?{
//        val urlStream = URL(createSearchQuery(audioFile)).openStream()
//        val list = CoverArtParer().parse(urlStream,audioFile)
//        Log.d("XML","list has returned")
//        if (list!= null){
//           return list
//        }
//        val albumSearch = albumSearch(audioFile)
//        if (albumSearch!=null){
//            Log.d("XML","album has returned")
//            return albumSearch
//        }
//        val songTitleSearch = songTitleSearch(audioFile)
//        if (songTitleSearch!=null){
//            Log.d("XML","title has returned")
//            return songTitleSearch
//        }
        val artistSearch = artistSearch(audioFile)
        if (artistSearch!=null){
            Log.d("XML","artist has returned")
            return artistSearch
        }
        Log.d("XML","returning null")
        return null
    }

    private fun albumSearch(audioFile: AudioFile): List<XMLAlbum>? {
        val urlStream = URL(createAlbumSearchQuery(audioFile.album)).openStream()
        return CoverArtParer().parse(urlStream,audioFile)
    }

    private fun songTitleSearch(audioFile: AudioFile): List<XMLAlbum>? {
        val urlStream = URL(createSongTitleSearchQuery(audioFile.title)).openStream()
        return CoverArtParer().parse(urlStream,audioFile)
    }

    private fun artistSearch(audioFile: AudioFile): List<XMLAlbum>? {
        val artistIdStream = URL(createArtistSearchQuery(audioFile.artist)).openStream()
        val parser = CoverArtParer()
        val artistIds = parser.parse(artistIdStream,audioFile)
        return if (artistIds.isNullOrEmpty()){
            null
        }else{
            val urlStream = URL(createArtistIdLookup(artistIds[0].xmlID)).openStream()
            parser.parse(urlStream,audioFile)
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

    private fun createArtistIdLookup(artistId:String?):String{
        return if (artistId != null) {
            "$ARTIST_LOOKUP$artistId$INCLUDE_ALBUM_SEARCH"
        } else {
            return ""
        }
    }


//METHOD REQUIRES EXTENSIVE LOGIC
//
//    fun retrieveCoverArt(audioFile: AudioFile): Bitmap {
//        if (audioFile.artist != null && audioFile.album != null) {
//            val albumID = getAlbumID(createQuery(ALBUM_QUERY, audioFile.album, ARTIST_QUERY, audioFile.artist))[0].xmlID
//        } else if (audioFile.title != null && audioFile.artist != null) {
//            val artistID = getArtistID(createQuery(ARTIST_QUERY, audioFile.artist, SONG_TITLE_QUERY, audioFile.title))
//        } else if (audioFile.title != null && audioFile.album != null) {
//            val artistID = getArtistID(createQuery(ALBUM_QUERY, audioFile.album, SONG_TITLE_QUERY, audioFile.title))
//        }
//    }
//
//
//    fun createQuery(type: String, search: String, extraType: String? = null, extraSearch: String? = null): String {
//        var query = "$type\"$search\""
//
//        if (extraType != null && extraSearch != null) {
//            query += " AND $extraType:\"$extraSearch\""
//        }
//        return query
//    }
//
//    fun getArtistID(artistQuery: String): ArrayList<XMLArtist> {
//        val url = URL(artistQuery)
//        val parserFactory = SAXParserFactory.newInstance()
//        val parser = parserFactory.newSAXParser()
//        val handler = XMLContentHandler()
//        parser.parse(url.openStream(), handler)
//        return handler.xmlArtists
//    }
//
//    fun getAlbumID(albumQuery: String): ArrayList<XMLAlbum> {
//        val url = URL(albumQuery)
//        val parserFactory = SAXParserFactory.newInstance()
//        val parser = parserFactory.newSAXParser()
//        val handler = XMLContentHandler()
//        parser.parse(url.openStream(), handler)
//        return handler.xmlAlbums
//    }
//
//    fun getSongTitleID(songTitleQuery: String): ArrayList<XMLSongTitle> {
//        val url = URL(songTitleQuery)
//        val parserFactory = SAXParserFactory.newInstance()
//        val parser = parserFactory.newSAXParser()
//        val handler = XMLContentHandler()
//        parser.parse(url.openStream(), handler)
//        return handler.xmlSongTitle
//    }

    companion object {
        private const val BASE_MUSIC_URL = "https://musicbrainz.org/ws/2/"
        const val ARTIST_QUERY = BASE_MUSIC_URL + "artist/?query="
        const val ARTIST_LOOKUP = BASE_MUSIC_URL + "artist/"
        const val ALBUM_QUERY = BASE_MUSIC_URL + "release-group/?query="
        const val SONG_TITLE_QUERY = BASE_MUSIC_URL + "recording/?query="
        const val INCLUDE_ALBUM_SEARCH = "?inc=release-groups"


        private val BASE_COVER_URL = "https://coverartarchive.org/release/"
    }
}
