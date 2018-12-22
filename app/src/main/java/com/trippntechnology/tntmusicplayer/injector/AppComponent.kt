package com.trippntechnology.tntmusicplayer.injector

import android.app.Application
import com.trippntechnology.tntmusicplayer.activites.mainactivity.MainActivity
import dagger.Component
import javax.inject.Singleton


@Singleton
@Component(modules = [AppModule::class,ViewModelModule::class])
interface AppComponent {
    fun inject(app:Application)

    //UI
    fun inject(target:MainActivity)


    fun application():Application
}