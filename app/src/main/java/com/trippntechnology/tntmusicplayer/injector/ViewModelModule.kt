package com.trippntechnology.tntmusicplayer.injector

import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import com.trippntechnology.tntmusicplayer.activites.mainactivity.MainViewModel
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

    /**Example*/
    /*
    @Binds
    @IntoMap
    @ViewModelKey(MyViewModel::class)
    internal abstract fun bindRegistrationViewModel(viewModel: MyViewModel): ViewModel
    */
}