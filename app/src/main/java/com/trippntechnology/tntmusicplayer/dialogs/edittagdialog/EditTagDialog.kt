package com.trippntechnology.tntmusicplayer.dialogs.edittagdialog

import android.content.Intent
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.Color
import android.graphics.drawable.ColorDrawable
import android.os.Bundle
import android.provider.MediaStore
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.coordinatorlayout.widget.CoordinatorLayout
import androidx.databinding.DataBindingUtil
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.ViewModelProviders
import com.google.android.material.snackbar.Snackbar
import com.trippntechnology.tntmusicplayer.R
import com.trippntechnology.tntmusicplayer.activites.mainactivity.MainViewModel
import com.trippntechnology.tntmusicplayer.binding.CustomBinders
import com.trippntechnology.tntmusicplayer.databinding.DialogEditTagBinding
import com.trippntechnology.tntmusicplayer.dialogs.dialogcomponents.LiveDataObserverDialogFragment
import com.trippntechnology.tntmusicplayer.injector.Injector
import kotlinx.android.synthetic.main.dialog_edit_tag.*
import java.io.ByteArrayOutputStream
import java.io.IOException
import javax.inject.Inject

class EditTagDialog : LiveDataObserverDialogFragment() {

    companion object {
        const val AUDIO_FILE = "edit_tag_audio_file"
        const val GALLERY = 42

        @JvmStatic
        fun newInstance(listPosition: Int) = EditTagDialog().apply {
            arguments = Bundle().apply {
                putInt(AUDIO_FILE, listPosition)
            }
        }
    }


    @Inject
    lateinit var viewModelFactory: ViewModelProvider.Factory

    private lateinit var binding: DialogEditTagBinding

    private val viewModel by lazy {
        ViewModelProviders.of(requireActivity(), viewModelFactory).get(MainViewModel::class.java)
    }

    init {
        Injector.get().inject(this)

    }


    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        binding = DataBindingUtil.inflate(inflater, R.layout.dialog_edit_tag, container, false)
        binding.viewModel = viewModel
        arguments?.getInt(AUDIO_FILE).let {
            binding.audioFile = viewModel.fullSongList.value!![it!!]
        }
        dialog!!.window!!.setBackgroundDrawable(ColorDrawable(Color.TRANSPARENT))
        return binding.root
    }

    override fun onResume() {
        super.onResume()
        setupObservers()
    }


    private fun setupObservers() {
        viewModel.cancel.observe {
            dismiss()
        }

        viewModel.saveTags.observe {
            if (it == 0) {
                dismiss()
                Toast.makeText(context, "Tags saved", Toast.LENGTH_SHORT).show()
            } else {
                Toast.makeText(context, "Error saving tags", Toast.LENGTH_LONG).show()
            }
        }

        viewModel.selectNewCover.observe {
            val galleryIntent = Intent(Intent.ACTION_PICK, MediaStore.Images.Media.EXTERNAL_CONTENT_URI)
            startActivityForResult(galleryIntent, GALLERY)
        }

        viewModel.savingInProcess.observe {
            Toast.makeText(context, "Saving new tags . . .", Toast.LENGTH_SHORT).show()
        }

        viewModel.updateDialogCover.observe {
            CustomBinders.setImage(editTagCover, it)
        }
    }

    override fun onPause() {
        super.onPause()
        viewModel.cancel.removeObservers(requireActivity())
        viewModel.saveTags.removeObservers(requireActivity())
        viewModel.selectNewCover.removeObservers(requireActivity())
        viewModel.savingInProcess.removeObservers(requireActivity())
        viewModel.updateDialogCover.removeObservers(requireActivity())
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)

        when (requestCode) {
            GALLERY -> {
                data ?: return

                val contentUri = data.data

                try {
                    val bitmap = MediaStore.Images.Media.getBitmap(requireActivity().contentResolver, contentUri)
                    val stream = ByteArrayOutputStream()
                    bitmap.compress(Bitmap.CompressFormat.JPEG, 100, stream)
                    CustomBinders.setImage(editTagCover, bitmap)
                    viewModel.newCover = stream.toByteArray()
                } catch (e: IOException) {
                    e.printStackTrace()
                }
            }
        }


    }
}