package com.trippntechnology.tntmusicplayer.dialogs.dialogcomponents

import androidx.fragment.app.DialogFragment
import androidx.lifecycle.LiveData
import androidx.lifecycle.Observer

abstract class LiveDataObserverDialogFragment : DialogFragment(){
    protected inline fun <T> LiveData<T>.observe(crossinline block: (T?) -> Unit) {
        observe(requireActivity(), Observer { block(it) })
    }

    protected inline fun <T> LiveData<T>.observeNotNull(crossinline block: (T) -> Unit) {
        observe(requireActivity(), Observer {
            it ?: return@Observer
            block(it)
        })
    }
}