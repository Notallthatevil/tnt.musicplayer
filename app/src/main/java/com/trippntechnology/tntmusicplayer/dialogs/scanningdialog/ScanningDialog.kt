package com.trippntechnology.tntmusicplayer.dialogs.scanningdialog

import android.annotation.SuppressLint
import android.app.Activity
import android.app.Dialog
import android.graphics.Color
import android.graphics.drawable.ColorDrawable
import android.os.Bundle
import com.trippntechnology.tntmusicplayer.R
import kotlinx.android.synthetic.main.dialog_progress_indicator.*

@SuppressLint("SetTextI18n")
class ScanningDialog(context: Activity) : Dialog(context, false, null) {


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.dialog_progress_indicator)
        window!!.setBackgroundDrawable(ColorDrawable(Color.TRANSPARENT))
    }

    private fun setBody(bodyText: String) {
        if(!isShowing){
            show()
        }
        alertBody.text = bodyText
    }

    fun setMaxProgress(maxProgress: Int) {
        if(!isShowing){
            show()
        }
        alertProgress.max = maxProgress
        alertProgress.progress = 0
        alertProgressCounter.text = "${alertProgress.progress}/${alertProgress.max}"
    }

    fun increaseCurrentProgress(wrapper: CurrentProgressWrapper) {
        if(!isShowing){
            show()
        }
        alertProgress.progress = wrapper.currentItemPosition
        alertProgressCounter.text = "${alertProgress.progress}/${alertProgress.max}"
        setBody(wrapper.currentItem)
    }

    data class CurrentProgressWrapper(val currentItemPosition: Int, val currentItem: String)
    data class IntegerWrapper(val int: Int)
}