package com.trippntechnology.tntmusicplayer.injector

import androidx.lifecycle.ViewModelProvider
import dagger.Binds
import dagger.Module

@Module
abstract class ViewModelModule {

    @Binds
    internal abstract fun bindViewModelFactory(factory: ViewModelFactory): ViewModelProvider.Factory


    /**Example*/
    /*
    @Binds
    @IntoMap
    @ViewModelKey(MyViewModel::class)
    internal abstract fun bindRegistrationViewModel(viewModel: MyViewModel): ViewModel
    */
}