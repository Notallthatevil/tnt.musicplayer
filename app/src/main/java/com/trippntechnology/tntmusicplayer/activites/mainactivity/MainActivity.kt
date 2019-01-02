package com.trippntechnology.tntmusicplayer.activites.mainactivity

import android.Manifest
import android.content.pm.PackageManager
import android.os.Bundle
import android.widget.Toast
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.databinding.DataBindingUtil
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.ViewModelProviders
import com.trippntechnology.tntmusicplayer.R
import com.trippntechnology.tntmusicplayer.databinding.ActivityMainBinding
import com.trippntechnology.tntmusicplayer.injector.Injector
import com.trippntechnology.tntmusicplayer.util.LiveDataObserverActivity
import com.trippntechnology.tntmusicplayer.widgets.ScanningDialog
import javax.inject.Inject

class MainActivity : LiveDataObserverActivity() {

    private val PERMISSION_TO_WRITE_EXTERNAL_STORAGE = 1122

    @Inject
    lateinit var viewModelFactory: ViewModelProvider.Factory

    private lateinit var binding: ActivityMainBinding
    private lateinit var dialog :ScanningDialog

    private val viewModel by lazy {
        ViewModelProviders.of(this, viewModelFactory).get(MainViewModel::class.java)
    }


    init {
        Injector.get().inject(this)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        dialog = ScanningDialog(this)
        dialog.show()

        binding = DataBindingUtil.setContentView(this, R.layout.activity_main)
        binding.viewModel = viewModel

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED){
            ActivityCompat.requestPermissions(this,
                arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE),PERMISSION_TO_WRITE_EXTERNAL_STORAGE)
        }

        viewModel.fullSongList.observe {
            Toast.makeText(this,"Songs read",Toast.LENGTH_LONG).show()
            if(dialog.isShowing){
                dialog.dismiss()
            }
        }

        viewModel.parsingCurrentSong.observe{
            dialog.increaseCurrentProgress(it!!)
        }
        viewModel.numberOfSongs.observe{
            dialog.setMaxProgress(it!!.int)
            if (!dialog.isShowing){
                dialog.show()
            }
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        if (dialog.isShowing){
            dialog.dismiss()
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
