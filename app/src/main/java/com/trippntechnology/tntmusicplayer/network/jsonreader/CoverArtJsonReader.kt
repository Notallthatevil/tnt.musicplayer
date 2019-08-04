package com.trippntechnology.tntmusicplayer.network.jsonreader

import android.util.JsonReader
import timber.log.Timber
import java.io.InputStream
import java.io.InputStreamReader

class CoverArtJsonReader {

    fun getCoverArtUrl(inputStream: InputStream): String? {
        JsonReader(InputStreamReader(inputStream)).use {
            return findImageUrl(it)
        }
    }


    private fun findImageUrl(jsonReader: JsonReader): String? {
        jsonReader.beginObject()

        while (jsonReader.nextName() != "images") {
            jsonReader.skipValue()
        }
        var size1200: String? = null
        var size500: String? = null
        var size250: String? = null
        var sizeLarge: String? = null
        var sizeSmall: String? = null

        jsonReader.beginArray()
        jsonReader.beginObject()
        while (jsonReader.hasNext()) {
            val name = jsonReader.nextName()
            if (name == "thumbnails") {
                jsonReader.beginObject()
                while (jsonReader.hasNext()) {
                    when (jsonReader.nextName()) {
                        "1200" -> size1200 = getURL(jsonReader)
                        "500" -> size500 = getURL(jsonReader)
                        "250" -> size250 = getURL(jsonReader)
                        "large" -> sizeLarge = getURL(jsonReader)
                        "small" -> sizeSmall = getURL(jsonReader)
                        else -> jsonReader.skipValue()
                    }
                }
                return size1200 ?: (size500 ?: (sizeLarge ?: (size250 ?: sizeSmall)))
            }
            jsonReader.skipValue()
        }
        return null
    }

    private fun getURL(jsonReader: JsonReader): String {
        var imageURL = jsonReader.nextString()
        if (imageURL[4] != 's') {
            imageURL = imageURL.substring(0, 4) + 's' + imageURL.substring(4, imageURL.length)
        }
        Timber.d(imageURL)
        return imageURL
    }
}