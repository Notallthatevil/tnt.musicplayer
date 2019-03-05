package com.trippntechnology.tntmusicplayer.network.jsonreader

import android.util.JsonReader
import android.util.Log
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

        while (jsonReader.nextName()!="images"){
            jsonReader.skipValue()
        }
        jsonReader.beginArray()
        jsonReader.beginObject()
        while (jsonReader.hasNext()) {
            val name = jsonReader.nextName()
            when (name) {
                "image" -> {
                    var imageURL = jsonReader.nextString()
                    if (imageURL[4] != 's') {
                        imageURL = imageURL.substring(0, 4) + 's' + imageURL.substring(4, imageURL.length)
                    }
                    Log.d("AUTO_FIND_COVER", imageURL)
                    return imageURL
                }
                else -> jsonReader.skipValue()
            }

        }
        return null
    }
}