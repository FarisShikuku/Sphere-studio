package com.example

import android.media.AudioManager
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.animation.Crossfade
import androidx.compose.animation.core.tween
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.tooling.preview.Preview
import com.example.sphere.SphereStudioApp
import com.example.sphere.SphereStudioWelcomeScreen
import com.example.ui.theme.MyApplicationTheme

class MainActivity : ComponentActivity() {
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    volumeControlStream = AudioManager.STREAM_MUSIC
    enableEdgeToEdge()
    setContent {
      MyApplicationTheme {
        SphereStudioEntry()
      }
    }
  }
}

@Composable
fun SphereStudioEntry() {
  var isSplashActive by remember { mutableStateOf(true) }

  Crossfade(
    targetState = isSplashActive,
    animationSpec = tween(durationMillis = 500),
    label = "splash_crossfade"
  ) { showSplash ->
    if (showSplash) {
      SphereStudioWelcomeScreen(
        onDismiss = { isSplashActive = false }
      )
    } else {
      SphereStudioApp()
    }
  }
}

@Preview(showBackground = true)
@Composable
fun SphereStudioPreview() {
  MyApplicationTheme {
    SphereStudioApp()
  }
}
