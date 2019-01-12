package com.trippntechnology.tntmusicplayer.activites.mainactivity

import android.Manifest
import android.content.pm.PackageManager
import android.os.Bundle
import android.widget.Toast
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.databinding.DataBindingUtil
import androidx.fragment.app.FragmentTransaction
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.ViewModelProviders
import androidx.recyclerview.widget.DividerItemDecoration
import androidx.recyclerview.widget.LinearLayoutManager
import com.trippntechnology.tntmusicplayer.R
import com.trippntechnology.tntmusicplayer.activites.mainactivity.adapter.AudioFileAdapter
import com.trippntechnology.tntmusicplayer.databinding.ActivityMainBinding
import com.trippntechnology.tntmusicplayer.dialogs.edittagdialog.EditTagDialog
import com.trippntechnology.tntmusicplayer.injector.Injector
import com.trippntechnology.tntmusicplayer.util.LiveDataObserverActivity
import com.trippntechnology.tntmusicplayer.dialogs.scanningdialog.ScanningDialog
import com.trippntechnology.tntmusicplayer.objects.AudioFile
import kotlinx.android.synthetic.main.activity_main.*
import javax.inject.Inject

class MainActivity : LiveDataObserverActivity() {

    private val PERMISSION_TO_WRITE_EXTERNAL_STORAGE = 1122
    private val DIALOG_TAG = "dialog"

    @Inject
    lateinit var viewModelFactory: ViewModelProvider.Factory

    private lateinit var binding: ActivityMainBinding
    private lateinit var scanningDialog : ScanningDialog

    private val viewModel by lazy {
        ViewModelProviders.of(this, viewModelFactory).get(MainViewModel::class.java)
    }
    private val adapter by lazy { AudioFileAdapter(viewModel) }


    init {
        Injector.get().inject(this)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        scanningDialog = ScanningDialog(this)
        scanningDialog.show()


        binding = DataBindingUtil.setContentView(this, R.layout.activity_main)
        binding.viewModel = viewModel

        binding.mainRecyclerView.layoutManager = LinearLayoutManager(this)
        binding.mainRecyclerView.adapter = adapter
        binding.mainRecyclerView.addItemDecoration(object:DividerItemDecoration(this, VERTICAL){})

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED){
            ActivityCompat.requestPermissions(this,
                arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE),PERMISSION_TO_WRITE_EXTERNAL_STORAGE)
        }

        setupProgressBarObservers()


        viewModel.selectedSong.observe{
            showEditTagDialog(it)
        }

        //On initial scan only
        viewModel.fullSongList.observe {
            adapter.submitList(it!!)
            if(scanningDialog.isShowing){
                scanningDialog.dismiss()
            }
        }

    }

    override fun onDestroy() {
        super.onDestroy()
        if (scanningDialog.isShowing){
            scanningDialog.dismiss()
        }
    }


    private fun showEditTagDialog(listPosition: Int?){
        listPosition ?:return
        val ft = supportFragmentManager.beginTransaction()
        val prev = supportFragmentManager.findFragmentByTag(DIALOG_TAG)

        if (prev != null){
            ft.remove(prev)
        }
        ft.addToBackStack(null)
        val editTagDialog = EditTagDialog.newInstance(listPosition)
        editTagDialog.show(ft,DIALOG_TAG)
    }

    private fun setupProgressBarObservers(){
        viewModel.parsingCurrentSong.observe{
            scanningDialog.increaseCurrentProgress(it!!)
        }
        viewModel.numberOfSongs.observe{
            scanningDialog.setMaxProgress(it!!.int)
            if (!scanningDialog.isShowing){
                scanningDialog.show()
            }
        }
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<out String>, grantResults: IntArray) {
        when (requestCode){
            PERMISSION_TO_WRITE_EXTERNAL_STORAGE -> {
                if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED){
                }
            }
        }
    }
}
