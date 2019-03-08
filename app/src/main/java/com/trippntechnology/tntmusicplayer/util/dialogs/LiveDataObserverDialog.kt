package com.trippntechnology.tntmusicplayer.util.dialogs

import android.app.Dialog
import androidx.fragment.app.FragmentActivity
import androidx.lifecycle.LiveData
import androidx.lifecycle.Observer


abstract class LiveDataObserverDialog(val fragmentActivity: FragmentActivity) : Dialog(fragmentActivity) {
    protected inline fun <T> LiveData<T>.observe(crossinline block: (T?) -> Unit) {
        observe(fragmentActivity, Observer { block(it) })
    }

    protected inline fun <T> LiveData<T>.observeNotNull(crossinline block: (T) -> Unit) {
        observe(fragmentActivity, Observer {
            it ?: return@Observer
            block(it)
        })
    }
}