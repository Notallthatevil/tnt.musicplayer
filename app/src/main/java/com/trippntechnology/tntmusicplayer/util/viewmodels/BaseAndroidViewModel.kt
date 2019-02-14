package com.trippntechnology.tntmusicplayer.util.viewmodels

import android.app.Application
import androidx.lifecycle.AndroidViewModel
import com.trippntechnology.tntmusicplayer.util.CoroutineContextProvider
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Job
import kotlin.coroutines.CoroutineContext

abstract class BaseAndroidViewModel(
    application: Application,
    protected val cc: CoroutineContextProvider,
    private val defaultContext: CoroutineContext = cc.default
) : AndroidViewModel(application), CoroutineScope {
    protected val baseViewModelJob = Job() // create a job as a parent for coroutines

    override val coroutineContext: CoroutineContext
        get() = defaultContext + baseViewModelJob

    override fun onCleared() {
        baseViewModelJob.cancel()
        super.onCleared()
    }
}