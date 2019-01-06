package com.trippntechnology.tntmusicplayer.injector

import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import com.trippntechnology.tntmusicplayer.activites.mainactivity.MainViewModel
import com.trippntechnology.tntmusicplayer.dialogs.edittagdialog.EditTagDialogViewModel
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
    @ViewModelKey(EditTagDialogViewModel::class)
    internal abstract fun bindEditTagViewModel(viewModel: EditTagDialogViewModel):ViewModel

    /**Example*/
    /*
    @Binds
    @IntoMap
    @ViewModelKey(MyViewModel::class)
    internal abstract fun bindRegistrationViewModel(viewModel: MyViewModel): ViewModel
    */
}