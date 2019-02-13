package com.trippntechnology.tntmusicplayer.ux.activities.mainactivity

import android.Manifest
import android.content.pm.PackageManager
import android.os.Bundle
import android.view.Menu
import android.view.MenuItem
import android.widget.Toast
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.databinding.DataBindingUtil
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.ViewModelProviders
import androidx.recyclerview.widget.DividerItemDecoration
import androidx.recyclerview.widget.LinearLayoutManager
import com.trippntechnology.tntmusicplayer.R
import com.trippntechnology.tntmusicplayer.ux.activities.mainactivity.adapter.AudioFileAdapter
import com.trippntechnology.tntmusicplayer.databinding.ActivityMainBinding
import com.trippntechnology.tntmusicplayer.dialogs.edittagdialog.EditTagDialog
import com.trippntechnology.tntmusicplayer.injector.Injector
import com.trippntechnology.tntmusicplayer.util.LiveDataObserverActivity
import com.trippntechnology.tntmusicplayer.dialogs.scanningdialog.ScanningDialog
import javax.inject.Inject

class MainActivity : LiveDataObserverActivity() {

    private val PERMISSION_TO_WRITE_EXTERNAL_STORAGE = 1122
    private val DIALOG_TAG = "dialog"

    @Inject
    lateinit var viewModelFactory: ViewModelProvider.Factory

    private lateinit var binding: ActivityMainBinding
    private lateinit var scanningDialog: ScanningDialog

    private val viewModel by lazy {
        ViewModelProviders.of(this, viewModelFactory).get(MainViewModel::class.java)
    }
    private val adapter by lazy { AudioFileAdapter(viewModel) }


    init {
        Injector.get().inject(this)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        //Setup binding
        binding = DataBindingUtil.setContentView(this, R.layout.activity_main)
        binding.viewModel = viewModel
        //Setup recycle view
        binding.mainRecyclerView.layoutManager = LinearLayoutManager(this)
        binding.mainRecyclerView.adapter = adapter
        binding.mainRecyclerView.addItemDecoration(object : DividerItemDecoration(this, VERTICAL) {})

        //Setup observers
        setupProgressBarObservers()
        setUpListObservers()

        //Check for permission and start app
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE), PERMISSION_TO_WRITE_EXTERNAL_STORAGE)
        } else {
            viewModel.startUp()
        }
    }

    override fun onResume() {
        super.onResume()
        viewModel.syncAudioFiles()
    }

    override fun onDestroy() {
        super.onDestroy()
        if (scanningDialog.isShowing) {
            scanningDialog.dismiss()
        }
    }


    private fun showEditTagDialog(listPosition: Int?) {
        listPosition ?: return
        val ft = supportFragmentManager.beginTransaction()
        val prev = supportFragmentManager.findFragmentByTag(DIALOG_TAG)

        if (prev != null) {
            ft.remove(prev)
        }
        ft.addToBackStack(null)
        val editTagDialog = EditTagDialog.newInstance(listPosition)
        editTagDialog.show(ft, DIALOG_TAG)
    }

    private fun setupProgressBarObservers() {
        scanningDialog = ScanningDialog(this)

        viewModel.parsingCurrentSong.observe {
            scanningDialog.increaseCurrentProgress(it!!)
        }
        viewModel.numberOfSongs.observe {
            scanningDialog.setMaxProgress(it!!.int)
        }
    }

    private fun setUpListObservers() {
        viewModel.selectedSong.observe {
            showEditTagDialog(it)
        }
        viewModel.fullSongList.observe {
            adapter.submitList(it?.toList())
            if (scanningDialog.isShowing) {
                scanningDialog.dismiss()
            }
        }
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<out String>, grantResults: IntArray) {
        when (requestCode) {
            PERMISSION_TO_WRITE_EXTERNAL_STORAGE -> {
                if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    viewModel.startUp()
                } else {
                    Toast.makeText(this, "Storage permission required to use app", Toast.LENGTH_LONG).show()
                    finish()
                }
            }
        }
    }

    override fun onCreateOptionsMenu(menu: Menu?): Boolean {
        menuInflater.inflate(R.menu.menu_main,menu)
        return super.onCreateOptionsMenu(menu)
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        when(item.itemId){
            R.id.autoTag->viewModel.autoFindAllAlbumArt()
        }
        return super.onOptionsItemSelected(item)
    }
}
