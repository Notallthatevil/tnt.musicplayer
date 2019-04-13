package com.trippntechnology.tntmusicplayer.network.xmlparser

import android.util.Xml
import com.trippntechnology.tntmusicplayer.objects.AudioFile
import com.trippntechnology.tntmusicplayer.objects.XMLAlbum
import org.xmlpull.v1.XmlPullParser
import org.xmlpull.v1.XmlPullParserException
import timber.log.Timber
import java.io.IOException
import java.io.InputStream


class CoverArtParser {


    @Throws(XmlPullParserException::class, IOException::class)
    fun parse(inputStream: InputStream, audioFile: AudioFile): List<XMLAlbum>? {
        inputStream.use {
            val parser: XmlPullParser = Xml.newPullParser()
            parser.setFeature(XmlPullParser.FEATURE_PROCESS_NAMESPACES, false)
            parser.setInput(it, null)
            parser.nextTag()
            val list = readFeed(parser, audioFile)
            return if (!list.isEmpty()) {
                list
            } else {
                return null
            }
        }
    }

    @Throws(XmlPullParserException::class, IOException::class)
    private fun readFeed(parser: XmlPullParser, audioFile: AudioFile): List<XMLAlbum> {
        val albumsList = mutableListOf<XMLAlbum>()
        val artistList = mutableListOf<XMLAlbum>()
        parser.require(XmlPullParser.START_TAG, null, "metadata")
        while (parser.next() != XmlPullParser.END_DOCUMENT) {
            if (parser.eventType != XmlPullParser.START_TAG) {
                continue
            }
            Timber.d(parser.name)
            when (parser.name) {
                "release-group" -> if (audioFile.album != null) {
                    val xmlAlbum = readAlbum(parser)
                    if (xmlAlbum != null) {
                        albumsList.add(xmlAlbum)
                    }
                }
                "artist" -> {
                    if (audioFile.artist != null) {
                        val xmlArtist = readArtist(parser)
                        if (xmlArtist != null) {
                            artistList.add(xmlArtist)
                        }
                    }
                }
            }
        }
        return if (albumsList.isNotEmpty()) {
            albumsList
        } else {
            artistList
        }
    }

    @Throws(IOException::class, XmlPullParserException::class)
    private fun readAlbum(parser: XmlPullParser): XMLAlbum? {
        parser.require(XmlPullParser.START_TAG, null, "release-group")
        val albumID = parser.getAttributeValue(null, "id")
        Timber.d(albumID)
        val type = parser.getAttributeValue(null, "type")
        skip(parser)
        if (type.equals("single", true) || type.equals("album", true)) {
            return XMLAlbum(albumID)
        }
        return null
    }

    @Throws(IOException::class, XmlPullParserException::class)
    private fun readArtist(parser: XmlPullParser): XMLAlbum? {
        parser.require(XmlPullParser.START_TAG, null, "artist")
        val artistID = parser.getAttributeValue(null, "id")
        Timber.d(artistID)
        return XMLAlbum(artistID)
    }


    @Throws(XmlPullParserException::class, IOException::class)
    private fun skip(parser: XmlPullParser) {
        if (parser.eventType != XmlPullParser.START_TAG) {
            throw IllegalStateException()
        }
        var depth = 1
        while (depth != 0) {
            when (parser.next()) {
                XmlPullParser.END_TAG -> depth--
                XmlPullParser.START_TAG -> depth++
            }
        }
    }
}