package com.trippntechnology.tntmusicplayer.injector

import android.app.Application
import com.trippntechnology.tntmusicplayer.BuildConfig
import com.trippntechnology.tntmusicplayer.log.DebugTree
import com.trippntechnology.tntmusicplayer.log.ReleaseTree
import timber.log.Timber

class App:Application() {

    init {
        Injector.init(this)
    }

    override fun onCreate() {
        super.onCreate()
        Injector.get().inject(this)

        setupLogging()

    }

    private fun setupLogging() {
        // Always register Crashltyics (even if CrashlyticsTree is not planted)
        //        Fabric.with(this, new Crashlytics());

        if (BuildConfig.DEBUG) {
            Timber.plant(Timber.DebugTree())
        } else {
            Timber.plant(ReleaseTree())
        }
    }
}