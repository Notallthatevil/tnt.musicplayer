package com.trippntechnology.tntmusicplayer.injector

import android.app.Application

class App : Application() {

    init {
        Injector.init(this)
    }
    override fun onCreate() {
        super.onCreate()
        // Initialize dependency injection
        Injector.get().inject(this)
    }
}