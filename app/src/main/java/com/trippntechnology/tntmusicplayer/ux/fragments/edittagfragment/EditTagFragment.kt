package com.trippntechnology.tntmusicplayer.ux.fragments.edittagfragment

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.databinding.DataBindingUtil
import androidx.fragment.app.Fragment
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.ViewModelProviders
import androidx.transition.*
import com.trippntechnology.tntmusicplayer.R
import com.trippntechnology.tntmusicplayer.databinding.FragmentEditTagBinding
import com.trippntechnology.tntmusicplayer.injector.Injector
import com.trippntechnology.tntmusicplayer.ux.sharedviewmodels.AudioFileListSharedViewModel
import javax.inject.Inject

class EditTagFragment : Fragment() {

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
//        sharedElementEnterTransition = TransitionSet().apply {
//            addTransition(ChangeBounds())
//            addTransition(ChangeTransform())
//            addTransition(ChangeClipBounds())
//            duration = 300
//        }
//        sharedElementReturnTransition = TransitionSet().apply {
//            addTransition(ChangeBounds())
//            addTransition(ChangeTransform())
//            addTransition(ChangeClipBounds())
//            duration = 300
//        }
        enterTransition = Fade()
        exitTransition = Fade()
        binding = DataBindingUtil.inflate(inflater,R.layout.fragment_edit_tag,container,false)
        binding.audioFile = viewModel.audioFileList.value!![EditTagFragmentArgs.fromBundle(arguments!!).arrayPosition.toInt()]
        binding.viewModel = this.viewModel
        return binding.root
    }
}