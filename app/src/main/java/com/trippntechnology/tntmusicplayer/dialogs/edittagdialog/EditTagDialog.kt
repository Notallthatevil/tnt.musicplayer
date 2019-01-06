package com.trippntechnology.tntmusicplayer.dialogs.edittagdialog

import android.graphics.Color
import android.graphics.drawable.ColorDrawable
import android.os.Bundle
import android.widget.Toast
import androidx.databinding.DataBindingUtil
import androidx.fragment.app.FragmentActivity
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.ViewModelProviders
import com.trippntechnology.tntmusicplayer.R
import com.trippntechnology.tntmusicplayer.databinding.DialogEditTagBinding
import com.trippntechnology.tntmusicplayer.dialogs.LiveDataObserverDialog
import com.trippntechnology.tntmusicplayer.injector.Injector
import com.trippntechnology.tntmusicplayer.objects.AudioFile
import javax.inject.Inject

class EditTagDialog(val context: FragmentActivity, private val audioFile: AudioFile) : LiveDataObserverDialog(context) {


    @Inject
    lateinit var viewModelFactory: ViewModelProvider.Factory

    private lateinit var binding: DialogEditTagBinding

    private val viewModel by lazy {
        ViewModelProviders.of(context, viewModelFactory).get(EditTagDialogViewModel::class.java)
    }

    init {
        Injector.get().inject(this)
    }


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = DataBindingUtil.inflate(layoutInflater,R.layout.dialog_edit_tag,null,false)
        setContentView(binding.root)
        binding.viewModel = viewModel
        binding.audioFile = audioFile


        window!!.setBackgroundDrawable(ColorDrawable(Color.TRANSPARENT))

        setupObservers()

    }



    private fun setupObservers(){
        viewModel.cancel.observe{
            dismiss()
        }
        viewModel.newStuff.observe{
            Toast.makeText(context,"Tags discovered new data",Toast.LENGTH_SHORT).show()
        }
    }
}