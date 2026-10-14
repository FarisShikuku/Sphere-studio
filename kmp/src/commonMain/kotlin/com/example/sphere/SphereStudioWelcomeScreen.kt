package com.example.sphere

import androidx.compose.animation.core.FastOutSlowInEasing
import androidx.compose.animation.core.RepeatMode
import androidx.compose.animation.core.animateFloat
import androidx.compose.animation.core.infiniteRepeatable
import androidx.compose.animation.core.rememberInfiniteTransition
import androidx.compose.animation.core.tween
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeDrawing
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.layout.windowInsetsPadding
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import kotlinx.coroutines.delay

@Composable
fun SphereStudioWelcomeScreen(
    onDismiss: () -> Unit = {}
) {
    // Automatically transition to the main Test Screen after 1.8 seconds
    LaunchedEffect(Unit) {
        delay(1800)
        onDismiss()
    }

    val infiniteTransition = rememberInfiniteTransition(label = "sphere_glow")
    val pulse by infiniteTransition.animateFloat(
        initialValue = 0.85f,
        targetValue = 1.05f,
        animationSpec = infiniteRepeatable(
            animation = tween(durationMillis = 1800, easing = FastOutSlowInEasing),
            repeatMode = RepeatMode.Reverse
        ),
        label = "pulse_anim"
    )

    val primaryColor = MaterialTheme.colorScheme.primary
    val secondaryColor = MaterialTheme.colorScheme.tertiary
    val surfaceColor = Color(0xFF090B10)

    Box(
        modifier = Modifier
            .fillMaxSize()
            .background(
                Brush.radialGradient(
                    colors = listOf(
                        primaryColor.copy(alpha = 0.15f),
                        surfaceColor
                    ),
                    radius = 1200f
                )
            )
            .windowInsetsPadding(WindowInsets.safeDrawing)
            .clickable { onDismiss() } // Tap to skip splash immediately
            .testTag("welcome_splash_screen"),
        contentAlignment = Alignment.Center
    ) {
        Column(
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.Center,
            modifier = Modifier.testTag("welcome_container")
        ) {
            // Elegant Sphere Emblem
            Canvas(modifier = Modifier.size(160.dp).testTag("sphere_emblem")) {
                val center = Offset(size.width / 2f, size.height / 2f)
                val baseRadius = size.minDimension / 2.8f * pulse

                // Outer ambient glow ring
                drawCircle(
                    brush = Brush.radialGradient(
                        colors = listOf(
                            primaryColor.copy(alpha = 0.35f),
                            Color.Transparent
                        ),
                        center = center,
                        radius = baseRadius * 1.5f
                    ),
                    radius = baseRadius * 1.5f,
                    center = center
                )

                // Concentric orbital rings
                drawCircle(
                    color = primaryColor.copy(alpha = 0.5f),
                    radius = baseRadius * 1.15f,
                    center = center,
                    style = Stroke(width = 2.dp.toPx())
                )

                drawCircle(
                    color = secondaryColor.copy(alpha = 0.35f),
                    radius = baseRadius * 0.85f,
                    center = center,
                    style = Stroke(width = 1.5.dp.toPx())
                )

                // Core Sphere with radial depth gradient
                drawCircle(
                    brush = Brush.radialGradient(
                        colors = listOf(
                            primaryColor.copy(alpha = 0.95f),
                            secondaryColor.copy(alpha = 0.85f),
                            primaryColor.copy(alpha = 0.4f)
                        ),
                        center = Offset(center.x - baseRadius * 0.25f, center.y - baseRadius * 0.25f),
                        radius = baseRadius
                    ),
                    radius = baseRadius * 0.65f,
                    center = center
                )
            }

            Spacer(modifier = Modifier.height(28.dp))

            // Title: Welcome to Sphere Studio
            Text(
                text = "Welcome to Sphere Studio",
                style = MaterialTheme.typography.headlineMedium.copy(
                    fontWeight = FontWeight.Bold,
                    letterSpacing = (-0.5).sp
                ),
                color = Color.White,
                textAlign = TextAlign.Center,
                modifier = Modifier.testTag("welcome_title")
            )

            Spacer(modifier = Modifier.height(10.dp))

            // Subtitle / Splash status
            Surface(
                shape = RoundedCornerShape(20.dp),
                color = Color(0xFF131722).copy(alpha = 0.8f)
            ) {
                Row(
                    modifier = Modifier.padding(horizontal = 14.dp, vertical = 6.dp),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Box(
                        modifier = Modifier
                            .size(7.dp)
                            .clip(CircleShape)
                            .background(Color(0xFF38BDF8))
                    )
                    Spacer(modifier = Modifier.width(8.dp))
                    Text(
                        text = "Loading Test Scene Viewport...",
                        style = MaterialTheme.typography.labelSmall,
                        color = Color(0xFF94A3B8),
                        fontFamily = FontFamily.Monospace
                    )
                }
            }

            Spacer(modifier = Modifier.height(36.dp))

            Text(
                text = "Tap anywhere to skip",
                style = MaterialTheme.typography.labelSmall,
                color = Color(0xFF475569)
            )
        }
    }
}
