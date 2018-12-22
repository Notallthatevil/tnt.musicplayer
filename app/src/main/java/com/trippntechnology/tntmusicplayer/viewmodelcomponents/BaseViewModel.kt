package com.trippntechnology.tntmusicplayer.viewmodelcomponents

import androidx.lifecycle.ViewModel
import com.trippntechnology.tntmusicplayer.util.CoroutineContextProvider
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Job
import kotlin.coroutines.CoroutineContext

abstract class BaseViewModel(protected val cc: CoroutineContextProvider, private val defaultContext: CoroutineContext = cc.default) : ViewModel(), CoroutineScope {
    private val baseViewModelJob = Job() // create a job as a parent for coroutines

    override val coroutineContext: CoroutineContext
        get() = defaultContext + baseViewModelJob

    override fun onCleared() {
        baseViewModelJob.cancel()
        super.onCleared()
    }
}