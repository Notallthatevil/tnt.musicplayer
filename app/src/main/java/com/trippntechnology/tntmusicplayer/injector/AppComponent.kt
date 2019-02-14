package com.trippntechnology.tntmusicplayer.injector

import android.app.Application
import com.trippntechnology.tntmusicplayer.ux.activities.mainactivity.MainActivity
import com.trippntechnology.tntmusicplayer.dialogs.edittagdialog.EditTagDialog
import com.trippntechnology.tntmusicplayer.ux.activities.mainnavactivity.MainNavActivity
import com.trippntechnology.tntmusicplayer.ux.fragments.audiofilelistfragment.AudioFileListFragment
import dagger.Component
import javax.inject.Singleton


@Singleton
@Component(modules = [AppModule::class,ViewModelModule::class])
interface AppComponent {
    fun inject(app:Application)

    //Activity
    fun inject(target:MainActivity)
    fun inject(target:MainNavActivity)
    fun inject(target:AudioFileListFragment)

    //Dialog
    fun inject(target:EditTagDialog)

    fun application():Application
}