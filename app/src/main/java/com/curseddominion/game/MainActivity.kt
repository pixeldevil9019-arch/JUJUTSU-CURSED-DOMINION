package com.curseddominion.game

import android.app.Activity
import android.opengl.GLSurfaceView
import android.os.Bundle
import android.view.Gravity
import android.widget.FrameLayout
import android.widget.TextView
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class MainActivity : Activity() {
    companion object {
        init { System.loadLibrary("cursed_dominion") }
    }

    private external fun nativeBuildLabel(): String
    private external fun nativeSurfaceCreated()
    private external fun nativeDrawFrame()
    private external fun nativeSurfaceChanged(width: Int, height: Int)

    private lateinit var glView: GLSurfaceView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        glView = GLSurfaceView(this).apply {
            setEGLContextClientVersion(3)
            preserveEGLContextOnPause = true
            setRenderer(object : GLSurfaceView.Renderer {
                override fun onSurfaceCreated(unused: GL10?, config: EGLConfig?) {
                    nativeSurfaceCreated()
                }
                override fun onSurfaceChanged(unused: GL10?, width: Int, height: Int) {
                    nativeSurfaceChanged(width, height)
                }
                override fun onDrawFrame(unused: GL10?) {
                    nativeDrawFrame()
                }
            })
            renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY
        }

        val title = TextView(this).apply {
            text = "JUJUTSU: CURSED DOMINION\n${nativeBuildLabel()}\nPHASE 1 • RENDERER BOOTSTRAP"
            textSize = 18f
            gravity = Gravity.CENTER
            setTextColor(0xFFECE8F4.toInt())
            setBackgroundColor(0x990B0912.toInt())
            setPadding(24, 16, 24, 16)
        }

        setContentView(FrameLayout(this).apply {
            addView(glView, FrameLayout.LayoutParams(-1, -1))
            addView(title, FrameLayout.LayoutParams(-2, -2, Gravity.TOP or Gravity.CENTER_HORIZONTAL))
        })
    }

    override fun onPause() {
        glView.onPause()
        super.onPause()
    }

    override fun onResume() {
        super.onResume()
        if (::glView.isInitialized) glView.onResume()
    }
}
