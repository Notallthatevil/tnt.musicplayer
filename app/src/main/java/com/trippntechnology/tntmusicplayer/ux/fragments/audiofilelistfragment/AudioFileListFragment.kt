package com.trippntechnology.tntmusicplayer.ux.fragments.audiofilelistfragment

import android.os.Bundle
import android.view.*
import androidx.databinding.DataBindingUtil
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.ViewModelProviders
import androidx.navigation.fragment.NavHostFragment
import androidx.recyclerview.widget.LinearLayoutManager
import com.trippntechnology.tnt.tntbaseutils.fragments.BaseFragment
import com.trippntechnology.tntmusicplayer.R
import com.trippntechnology.tntmusicplayer.databinding.FragmentAudioFileListBinding
import com.trippntechnology.tntmusicplayer.dialogs.scanningdialog.ScanningDialog
import com.trippntechnology.tntmusicplayer.injector.Injector
import com.trippntechnology.tntmusicplayer.ux.adapters.AudioFileAdapter
import com.trippntechnology.tntmusicplayer.ux.sharedviewmodels.AudioFileListSharedViewModel
import com.vikingsen.inject.viewmodel.ViewModelFactory
import javax.inject.Inject

class AudioFileListFragment : BaseFragment() {


    override fun setUpObservers() {
        TODO("not implemented") //To change body of created functions use File | Settings | File Templates.
    }

    @Inject
    lateinit var viewModelFactory: ViewModelFactory

    private lateinit var binding: FragmentAudioFileListBinding

    private val viewModel by lazy {
        ViewModelProviders.of(this, viewModelFactory).get(AudioFileListSharedViewModel::class.java)
    }
    private val adapter by lazy { AudioFileAdapter(viewModel) }

    private lateinit var scanningDialog: ScanningDialog


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
        setupAudioFileObservers()
    }

    override fun onResume() {
        super.onResume()
        viewModel.startup()
    }

    private fun setupRecyclerView() {
        binding.audioFileListRecyclerView.layoutManager = LinearLayoutManager(activity)
        binding.audioFileListRecyclerView.adapter = adapter
    }

    private fun setupAudioFileObservers() {
        viewModel.audioFileLongClick.observe {
            showEditTag(it!!)
        }
    }

    private fun setupScanningDialogObservers() {
        scanningDialog = ScanningDialog(requireActivity())

        viewModel.currentProgress.observeNotNull {
            if (it.currentItemPosition == -1) {
                scanningDialog.dismiss()
            } else {
                scanningDialog.increaseCurrentProgress(it)
            }
        }
        viewModel.maxProgress.observeNotNull {
            scanningDialog.setMaxProgress(it.int)
        }
    }

    override fun onCreateOptionsMenu(menu: Menu, inflater: MenuInflater) {
        inflater.inflate(R.menu.menu_main,menu)
        return super.onCreateOptionsMenu(menu,inflater)
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        when(item.itemId){
            R.id.autoTag->viewModel.autoFindAllAlbumArt()
        }
        return super.onOptionsItemSelected(item)
    }

    private fun showEditTag(longClickItem: AudioFileListSharedViewModel.LongClickItem) {
        val directions = AudioFileListFragmentDirections.editTagAction().setArrayPosition(longClickItem.position)
        NavHostFragment.findNavController(this).navigate(directions)
    }

}