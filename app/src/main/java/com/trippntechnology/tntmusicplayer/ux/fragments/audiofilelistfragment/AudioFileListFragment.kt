package com.trippntechnology.tntmusicplayer.ux.fragments.audiofilelistfragment

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.databinding.DataBindingUtil
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.ViewModelProviders
import androidx.recyclerview.widget.LinearLayoutManager
import com.trippntechnology.tntmusicplayer.R
import com.trippntechnology.tntmusicplayer.databinding.FragmentAudioFileListBinding
import com.trippntechnology.tntmusicplayer.dialogs.scanningdialog.ScanningDialog
import com.trippntechnology.tntmusicplayer.injector.Injector
import com.trippntechnology.tntmusicplayer.util.fragments.BaseFragment
import com.trippntechnology.tntmusicplayer.ux.listadapters.AudioFileAdapter
import com.trippntechnology.tntmusicplayer.ux.sharedviewmodels.AudioFileListSharedViewModel
import javax.inject.Inject

class AudioFileListFragment : BaseFragment() {

    @Inject
    lateinit var viewModelFactory: ViewModelProvider.Factory

    private lateinit var binding: FragmentAudioFileListBinding

    private val viewModel by lazy {
        ViewModelProviders.of(this, viewModelFactory).get(AudioFileListSharedViewModel::class.java)
    }
    private val adapter by lazy { AudioFileAdapter(viewModel) }

    private lateinit var scanningDialog:ScanningDialog


    init {
        Injector.get().inject(this)
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        binding = DataBindingUtil.inflate(inflater, R.layout.fragment_audio_file_list, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        binding.apply {
            viewModel = this@AudioFileListFragment.viewModel
            lifecycleOwner = this@AudioFileListFragment
        }
        setupRecyclerView()
        setupScanningDialogObservers()
    }

    override fun onResume() {
        super.onResume()
        viewModel.startup()
    }

    private fun setupRecyclerView() {
        binding.audioFileListRecyclerView.layoutManager = LinearLayoutManager(activity)
        binding.audioFileListRecyclerView.adapter = adapter
    }

    private fun setupScanningDialogObservers() {
        scanningDialog = ScanningDialog(requireActivity())

        viewModel.currentProgress.observeNotNull{
            scanningDialog.increaseCurrentProgress(it)
        }
        viewModel.maxProgress.observeNotNull {
            scanningDialog.setMaxProgress(it.int)
        }
    }


//    private fun restoreState(bundle: Bundle) {
//        with(SaveStateOptions) {
//            binding.recyclerView.scrollToPosition(bundle.scrollPosition!!)
//        }
//    }
//
//    object SaveStateOptions {
//        var Bundle.scrollPosition by BundleExtra.Int()
//    }
}