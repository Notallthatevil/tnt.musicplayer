package com.trippntechnology.tntmusicplayer.util.activities

import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlin.coroutines.CoroutineContext


abstract class BaseActivity : LiveDataObserverActivity(), CoroutineScope {
    private val baseActivityJob = Job() // create a job as a parent for coroutines

    override val coroutineContext: CoroutineContext
        get() = Dispatchers.Main + baseActivityJob

    fun enableActionBarBackArrow(enable: Boolean) {
        val actionBar = supportActionBar
        actionBar ?: return
        actionBar.setHomeButtonEnabled(enable)
        actionBar.setDisplayHomeAsUpEnabled(enable)
    }

    override fun onDestroy() {
        baseActivityJob.cancel()
        super.onDestroy()
    }
}