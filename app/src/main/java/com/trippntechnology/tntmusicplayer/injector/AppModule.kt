package com.trippntechnology.tntmusicplayer.injector

import android.app.Application
import com.trippntechnology.tntmusicplayer.nativewrappers.TaggerLib
import com.trippntechnology.tntmusicplayer.util.CoroutineContextProvider
import dagger.Module
import dagger.Provides
import javax.inject.Singleton

@Module
class AppModule(private val application: Application) {

    @Provides
    @Singleton
    internal fun provideApplication(): Application {
        return application
    }

    @Provides
    @Singleton
    fun provideCoroutineContextProvider(): CoroutineContextProvider {
        return CoroutineContextProvider.MainCoroutineContextProvider
    }

    @Provides
    @Singleton
    fun provideTaggerLib():TaggerLib{
        return TaggerLib
    }

}