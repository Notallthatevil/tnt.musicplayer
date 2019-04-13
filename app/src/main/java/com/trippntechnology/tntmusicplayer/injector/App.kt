package com.trippntechnology.tntmusicplayer.injector

import android.app.Application
import com.trippntechnology.tntmusicplayer.BuildConfig
import com.trippntechnology.tntmusicplayer.log.DebugTree
import com.trippntechnology.tntmusicplayer.log.ReleaseTree
import timber.log.Timber

class App : Application() {

    init {
        Injector.init(this)
    }
    override fun onCreate() {
        super.onCreate()
        // Initialize dependency injection
        Injector.get().inject(this)
        if (BuildConfig.DEBUG) {
            Timber.plant(DebugTree())
        } else {
            Timber.plant(ReleaseTree())
        }

    }
}