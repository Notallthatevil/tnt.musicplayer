package com.trippntechnology.tntmusicplayer.injector

import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import com.trippntechnology.tntmusicplayer.ux.activities.mainactivity.MainViewModel
import com.trippntechnology.tntmusicplayer.ux.sharedviewmodels.AudioFileListSharedViewModel
import dagger.Binds
import dagger.Module
import dagger.multibindings.IntoMap

@Module
abstract class ViewModelModule {

    @Binds
    internal abstract fun bindViewModelFactory(factory: ViewModelFactory): ViewModelProvider.Factory


    @Binds
    @IntoMap
    @ViewModelKey(MainViewModel::class)
    internal abstract fun bindMainViewModel(viewModel: MainViewModel):ViewModel

    @Binds
    @IntoMap
    @ViewModelKey(AudioFileListSharedViewModel::class)
    internal abstract fun bindAudioFileListSharedViewModel(viewModel: AudioFileListSharedViewModel):ViewModel

    /**Example*/
    /*
    @Binds
    @IntoMap
    @ViewModelKey(MyViewModel::class)
    internal abstract fun bindRegistrationViewModel(viewModel: MyViewModel): ViewModel
    */
}