package com.trippntechnology.tntmusicplayer.widgets

import android.app.Activity
import android.app.Dialog
import android.graphics.Color
import android.graphics.drawable.ColorDrawable
import android.os.Bundle
import com.trippntechnology.tntmusicplayer.R
import kotlinx.android.synthetic.main.dialog_directory_scanning.*

class ScanningDialog (context: Activity) : Dialog(context,false,null) {


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.dialog_directory_scanning)
        window!!.setBackgroundDrawable(ColorDrawable(Color.TRANSPARENT))
    }

    fun setBody(bodyText:String){
        alertBody.text = bodyText
    }

    fun setMaxProgress(maxProgress:Int){
        alertProgress.max = maxProgress
    }

    fun increaseCurrentProgress(){
        alertProgress.progress = alertProgress.progress++
    }
}