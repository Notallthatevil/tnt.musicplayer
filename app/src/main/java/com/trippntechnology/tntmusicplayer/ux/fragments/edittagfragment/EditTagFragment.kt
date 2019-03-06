package com.trippntechnology.tntmusicplayer.ux.fragments.edittagfragment

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.databinding.DataBindingUtil
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.ViewModelProviders
import com.trippntechnology.tntmusicplayer.R
import com.trippntechnology.tntmusicplayer.binding.CustomBinders
import com.trippntechnology.tntmusicplayer.databinding.FragmentEditTagBinding
import com.trippntechnology.tntmusicplayer.injector.Injector
import com.trippntechnology.tntmusicplayer.util.fragments.BaseFragment
import com.trippntechnology.tntmusicplayer.ux.sharedviewmodels.AudioFileListSharedViewModel
import com.trippntechnology.tntmusicplayer.ux.sharedviewmodels.AudioFileListSharedViewModel.Companion.AUTO_FIND_COVER
import com.trippntechnology.tntmusicplayer.ux.sharedviewmodels.AudioFileListSharedViewModel.Companion.AUTO_FIND_COVER_FINISHED
import com.trippntechnology.tntmusicplayer.ux.sharedviewmodels.AudioFileListSharedViewModel.Companion.CANCEL
import com.trippntechnology.tntmusicplayer.ux.sharedviewmodels.AudioFileListSharedViewModel.Companion.SAVING_TAGS
import com.trippntechnology.tntmusicplayer.ux.sharedviewmodels.AudioFileListSharedViewModel.Companion.TAGS_SAVED
import kotlinx.android.synthetic.main.fragment_edit_tag.*
import javax.inject.Inject

class EditTagFragment : BaseFragment() {

    @Inject
    lateinit var viewModelFactory: ViewModelProvider.Factory

    private lateinit var binding: FragmentEditTagBinding


    private val viewModel by lazy {
        ViewModelProviders.of(this, viewModelFactory).get(AudioFileListSharedViewModel::class.java)
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

    override fun onResume() {
        super.onResume()
        setUpObservers()
    }

    private fun setUpObservers() {
        viewModel.showProgressWheel.observe {
            when (it!!) {
                CANCEL -> fragmentManager?.popBackStack()
                SAVING_TAGS -> savingTagWheel.visibility = View.VISIBLE
                TAGS_SAVED -> {
                    savingTagWheel.visibility = View.GONE
                    Toast.makeText(activity, "New tags successfully saved", Toast.LENGTH_SHORT).show()
                    fragmentManager?.popBackStack()
                }
                AUTO_FIND_COVER -> {
                    coverWheel.visibility = View.VISIBLE
                    autoFindCoverButton.isEnabled = false
                    selectCoverButton.isEnabled = false
                }
                AUTO_FIND_COVER_FINISHED -> {
                    coverWheel.visibility = View.GONE
                    autoFindCoverButton.isEnabled = true
                    selectCoverButton.isEnabled = true
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
    }
}