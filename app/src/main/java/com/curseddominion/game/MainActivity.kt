package com.curseddominion.game

import android.app.Activity
import android.opengl.GLSurfaceView
import android.os.Bundle
import android.view.Gravity
import android.view.MotionEvent
import android.widget.FrameLayout
import android.widget.TextView
import android.os.Handler
import android.os.Looper
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
    private external fun nativeTouch(x: Float, y: Float, action: Int)
    private external fun nativeStatus(): String

    private lateinit var glView: GLSurfaceView
    private lateinit var status: TextView
    private val handler = Handler(Looper.getMainLooper())
    private val statusRefresh = object : Runnable {
        override fun run() {
            if (::status.isInitialized) status.text = nativeStatus()
            handler.postDelayed(this, 250L)
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        glView = GLSurfaceView(this).apply {
            setEGLContextClientVersion(3)
            preserveEGLContextOnPause = true
            setRenderer(object : GLSurfaceView.Renderer {
                override fun onSurfaceCreated(unused: GL10?, config: EGLConfig?) = nativeSurfaceCreated()
                override fun onSurfaceChanged(unused: GL10?, width: Int, height: Int) =
                    nativeSurfaceChanged(width, height)
                override fun onDrawFrame(unused: GL10?) = nativeDrawFrame()
            })
            renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY
            setOnTouchListener { view, event ->
                val width = view.width.coerceAtLeast(1)
                val height = view.height.coerceAtLeast(1)
                when (event.actionMasked) {
                    MotionEvent.ACTION_DOWN, MotionEvent.ACTION_MOVE,
                    MotionEvent.ACTION_UP, MotionEvent.ACTION_CANCEL -> {
                        nativeTouch(
                            (event.x / width).coerceIn(0f, 1f),
                            (event.y / height).coerceIn(0f, 1f),
                            if (event.actionMasked == MotionEvent.ACTION_CANCEL)
                                MotionEvent.ACTION_UP else event.actionMasked
                        )
                        true
                    }
                    else -> true
                }
            }
        }

        status = TextView(this).apply {
            text = "Loading arena..."
            textSize = 14f
            setTextColor(0xFFFFFFFF.toInt())
            setBackgroundColor(0xAA100D1B.toInt())
            setPadding(18, 10, 18, 10)
        }
        val title = TextView(this).apply {
            text = "JUJUTSU: CURSED DOMINION  •  ARENA PROTOTYPE"
            textSize = 15f
            gravity = Gravity.CENTER
            setTextColor(0xFFF2EAFB.toInt())
            setBackgroundColor(0xCC100D1B.toInt())
            setPadding(18, 10, 18, 10)
        }
        val help = TextView(this).apply {
            text = "LEFT: MOVE     RIGHT: CURSED STRIKE"
            textSize = 13f
            gravity = Gravity.CENTER
            setTextColor(0xFFE5D9F2.toInt())
            setBackgroundColor(0xAA100D1B.toInt())
            setPadding(16, 8, 16, 8)
        }

        setContentView(FrameLayout(this).apply {
            addView(glView, FrameLayout.LayoutParams(-1, -1))
            addView(title, FrameLayout.LayoutParams(-2, -2, Gravity.TOP or Gravity.CENTER_HORIZONTAL))
            addView(status, FrameLayout.LayoutParams(-2, -2, Gravity.TOP or Gravity.START))
            addView(help, FrameLayout.LayoutParams(-2, -2, Gravity.BOTTOM or Gravity.CENTER_HORIZONTAL))
        })
    }

    override fun onPause() {
        handler.removeCallbacks(statusRefresh)
        if (::glView.isInitialized) glView.onPause()
        super.onPause()
    }

    override fun onResume() {
        super.onResume()
        if (::glView.isInitialized) glView.onResume()
        handler.post(statusRefresh)
    }

    override fun onDestroy() {
        handler.removeCallbacks(statusRefresh)
        super.onDestroy()
    }
}
