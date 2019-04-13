package com.trippntechnology.tntmusicplayer.util.fragments

import androidx.fragment.app.Fragment
import androidx.lifecycle.LiveData
import androidx.lifecycle.Observer

abstract class LiveDataObserverFragment : Fragment() {
    protected inline fun <T> LiveData<T>.observe(crossinline block: (T?) -> Unit) {
        observe(this@LiveDataObserverFragment, Observer { block(it) })
    }

    protected inline fun <T> LiveData<T>.observeNotNull(crossinline block: (T) -> Unit) {
        observe(this@LiveDataObserverFragment, Observer {
            it ?: return@Observer
            block(it)
        })
    }
}