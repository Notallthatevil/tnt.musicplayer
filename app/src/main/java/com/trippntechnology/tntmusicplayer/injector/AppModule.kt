package com.trippntechnology.tntmusicplayer.injector

import android.app.Application
import com.trippntechnology.tntmusicplayer.nativewrappers.TaggerLib
import dagger.Module
import dagger.Provides
import javax.inject.Singleton

@Module(includes = [AssistedModule::class])
class AppModule(private val application: Application) {

    @Provides
    @Singleton
    internal fun provideApplication(): Application {
        return application
    }
}