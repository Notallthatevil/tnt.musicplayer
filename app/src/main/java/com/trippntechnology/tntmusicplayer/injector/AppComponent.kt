package com.trippntechnology.tntmusicplayer.injector

import android.app.Application
import com.trippntechnology.tntmusicplayer.ux.activities.mainnavactivity.MainNavActivity
import com.trippntechnology.tntmusicplayer.ux.fragments.audiofilelistfragment.AudioFileListFragment
import com.trippntechnology.tntmusicplayer.ux.fragments.edittagfragment.EditTagFragment
import dagger.Component
import javax.inject.Singleton


@Singleton
@Component(modules = [AppModule::class])
interface AppComponent {
    fun inject(app:Application)

    fun inject(target:MainNavActivity)
    fun inject(target:AudioFileListFragment)
    fun inject(target:EditTagFragment)

    fun application():Application
}