package com.trippntechnology.tntmusicplayer.ux.fragments.edittagfragment

import android.content.Intent
import android.graphics.Bitmap
import android.os.Bundle
import android.provider.MediaStore
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.databinding.DataBindingUtil
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.ViewModelProviders
import com.trippntechnology.tnt.tntbaseutils.fragments.BaseFragment
import com.trippntechnology.tntmusicplayer.R
import com.trippntechnology.tntmusicplayer.binding.CustomBinders
import com.trippntechnology.tntmusicplayer.databinding.FragmentEditTagBinding
import com.trippntechnology.tntmusicplayer.injector.Injector
import com.trippntechnology.tntmusicplayer.ux.sharedviewmodels.AudioFileListSharedViewModel
import com.trippntechnology.tntmusicplayer.ux.sharedviewmodels.AudioFileListSharedViewModel.Companion.AUTO_FIND_ALBUM_ART
import com.trippntechnology.tntmusicplayer.ux.sharedviewmodels.AudioFileListSharedViewModel.Companion.AUTO_FIND_ALBUM_ART_FINISHED
import com.trippntechnology.tntmusicplayer.ux.sharedviewmodels.AudioFileListSharedViewModel.Companion.CANCEL
import com.trippntechnology.tntmusicplayer.ux.sharedviewmodels.AudioFileListSharedViewModel.Companion.SAVING_TAGS
import com.trippntechnology.tntmusicplayer.ux.sharedviewmodels.AudioFileListSharedViewModel.Companion.SELECT_ALBUM_ART
import com.trippntechnology.tntmusicplayer.ux.sharedviewmodels.AudioFileListSharedViewModel.Companion.TAGS_SAVED
import com.vikingsen.inject.viewmodel.ViewModelFactory
import kotlinx.android.synthetic.main.fragment_edit_tag.*
import java.io.ByteArrayOutputStream
import java.io.IOException
import javax.inject.Inject


class EditTagFragment : BaseFragment() {

    @Inject
    lateinit var viewModelFactory: ViewModelFactory

    private lateinit var binding: FragmentEditTagBinding


    private val viewModel by lazy {
        ViewModelProviders.of(requireActivity(), viewModelFactory).get(AudioFileListSharedViewModel::class.java)
    }

    init {
        Injector.get().inject(this)
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        binding = DataBindingUtil.inflate(inflater, R.layout.fragment_edit_tag, container, false)
        binding.audioFile =
            viewModel.audioFileList.value!![EditTagFragmentArgs.fromBundle(arguments!!).arrayPosition.toInt()]
        binding.viewModel = this.viewModel
        return binding.root
    }

    override fun setUpObservers() {
        viewModel.editTagValues.observe {
            when (it!!) {
                CANCEL -> fragmentManager?.popBackStack()
                SAVING_TAGS -> savingTagWheel.visibility = View.VISIBLE
                TAGS_SAVED -> {
                    savingTagWheel.visibility = View.GONE
                    Toast.makeText(activity, "New tags successfully saved", Toast.LENGTH_SHORT).show()
                    fragmentManager?.popBackStack()
                }
                AUTO_FIND_ALBUM_ART -> {
                    coverWheel.visibility = View.VISIBLE
                    autoFindCoverButton.isEnabled = false
                    selectCoverButton.isEnabled = false
                }
                AUTO_FIND_ALBUM_ART_FINISHED -> {
                    coverWheel.visibility = View.GONE
                    autoFindCoverButton.isEnabled = true
                    selectCoverButton.isEnabled = true
                }
                SELECT_ALBUM_ART -> {
                    val intent = Intent(Intent.ACTION_GET_CONTENT)
                    intent.type = "image/*"
                    startActivityForResult(intent, GALLERY_INTENT)
                }
                else -> {
                    savingTagWheel.visibility = View.GONE
                    Toast.makeText(activity, "An unexpected error occurred", Toast.LENGTH_SHORT).show()
                }
            }
        }
        viewModel.updateImageView.observe {
            CustomBinders.setImage(editTagCover, it)
            if (it == null) {
                Toast.makeText(activity, "No cover found.", Toast.LENGTH_SHORT).show()
            }
        }
        viewModel.selectCover.observe {
            val intent = Intent(Intent.ACTION_GET_CONTENT)
            intent.type = "image/*"
            startActivityForResult(intent, GALLERY_INTENT)
        }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)

        when (requestCode) {
            GALLERY_INTENT -> {
                data ?: return
                val contentUri = data.data
                if (!contentUri!!.toString().contains("image")) {
                    return
                }
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

    companion object {
        private const val GALLERY_INTENT = 3845
    }
}