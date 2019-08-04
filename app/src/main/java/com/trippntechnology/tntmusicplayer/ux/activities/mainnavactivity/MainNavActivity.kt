package com.trippntechnology.tntmusicplayer.ux.activities.mainnavactivity

import android.Manifest
import android.content.pm.PackageManager
import android.os.Bundle
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.databinding.DataBindingUtil
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.ViewModelProviders
import com.trippntechnology.tnt.tntbaseutils.activities.BaseActivity
import com.trippntechnology.tntmusicplayer.R
import com.trippntechnology.tntmusicplayer.databinding.ActivityMainNavBinding
import com.trippntechnology.tntmusicplayer.injector.Injector
import com.trippntechnology.tntmusicplayer.nativewrappers.PlayerLib
import com.trippntechnology.tntmusicplayer.ux.sharedviewmodels.AudioFileListSharedViewModel
import com.vikingsen.inject.viewmodel.ViewModelFactory
import javax.inject.Inject

class MainNavActivity : BaseActivity() {

    private var binding: ActivityMainNavBinding? = null

    @Inject
    lateinit var viewModelFactory: ViewModelFactory

    private val viewModel by lazy {
        ViewModelProviders.of(this, viewModelFactory).get(AudioFileListSharedViewModel::class.java)
    }

    init {
        Injector.get().inject(this)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        if (ContextCompat.checkSelfPermission(
                this,
                Manifest.permission.WRITE_EXTERNAL_STORAGE
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            ActivityCompat.requestPermissions(
                this,
                arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE),
                WRITE_EXTERNAL_STORAGE
            )
        } else {
            showView()
        }
    }

    override fun onDestroy() {
        PlayerLib.deleteEngine()
        super.onDestroy()
    }

    private fun showView(){
        PlayerLib.createEngine()
        binding = DataBindingUtil.setContentView(this, R.layout.activity_main_nav)
    }

    override fun onStop() {
        super.onStop()
        viewModel.autoFindJob?.cancel()
    }



    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<out String>, grantResults: IntArray) {
        when (requestCode) {
            WRITE_EXTERNAL_STORAGE -> {
                if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    showView()
                } else {
                    ActivityCompat.requestPermissions(
                        this,
                        arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE),
                        WRITE_EXTERNAL_STORAGE
                    )
                }
            }
        }
    }

    companion object {
        private const val WRITE_EXTERNAL_STORAGE = 1
    }
}