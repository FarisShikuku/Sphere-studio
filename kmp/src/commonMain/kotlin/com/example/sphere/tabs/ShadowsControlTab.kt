package com.example.sphere

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Slider
import androidx.compose.material3.SliderDefaults
import androidx.compose.material3.Surface
import androidx.compose.material3.Switch
import androidx.compose.material3.SwitchDefaults
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import kotlin.math.cos
import kotlin.math.sin

// -------------------------------------------------------------
// CONTROL TAB 2: SHADOWS & PCF (SASCHA WILLEMS VULKAN ARCHITECTURE)
// -------------------------------------------------------------
@Composable
fun ShadowsControlTab(
    enableShadows: Boolean,
    onToggleShadows: (Boolean) -> Unit,
    shadowQuality: ShadowQuality,
    onShadowQualitySelect: (ShadowQuality) -> Unit,
    shadowIntensity: Float,
    onShadowIntensityChange: (Float) -> Unit,
    shadowSoftness: Float,
    onShadowSoftnessChange: (Float) -> Unit,
    depthBias: Float,
    onDepthBiasChange: (Float) -> Unit,
    showShadowMapDebug: Boolean,
    onToggleShadowMapDebug: (Boolean) -> Unit,
    sunElevation: Float,
    sunAzimuth: Float
) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(bottom = 28.dp)
            .testTag("shadows_control_tab")
    ) {
        // 1. Master Toggles Card
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column(modifier = Modifier.weight(1f)) {
                        Row(verticalAlignment = Alignment.CenterVertically) {
                            Text(
                                text = "Directional Shadows",
                                style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold),
                                color = Color.White
                            )
                            Spacer(modifier = Modifier.width(6.dp))
                            Surface(
                                shape = RoundedCornerShape(4.dp),
                                color = Color(0xFF4F46E5).copy(alpha = 0.25f)
                            ) {
                                Text(
                                    text = "Vulkan 1.3",
                                    style = MaterialTheme.typography.labelSmall,
                                    color = Color(0xFFA5B4FC),
                                    fontSize = 10.sp,
                                    modifier = Modifier.padding(horizontal = 4.dp, vertical = 2.dp)
                                )
                            }
                        }
                        Text(
                            text = "Offscreen depth pass & light projection (SaschaWillems/Vulkan)",
                            style = MaterialTheme.typography.bodySmall,
                            color = Color(0xFF94A3B8)
                        )
                    }
                    Switch(
                        checked = enableShadows,
                        onCheckedChange = onToggleShadows,
                        colors = SwitchDefaults.colors(
                            checkedThumbColor = Color(0xFFA5B4FC),
                            checkedTrackColor = Color(0xFF4F46E5)
                        )
                    )
                }

                HorizontalDivider(color = Color(0xFF1E293B), modifier = Modifier.padding(vertical = 10.dp))

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column(modifier = Modifier.weight(1f)) {
                        Text(
                            text = "Light-Space Depth HUD",
                            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold),
                            color = Color.White
                        )
                        Text(
                            text = "Mini viewport preview of offscreen depth buffer",
                            style = MaterialTheme.typography.bodySmall,
                            color = Color(0xFF64748B)
                        )
                    }
                    Switch(
                        checked = showShadowMapDebug,
                        onCheckedChange = onToggleShadowMapDebug,
                        colors = SwitchDefaults.colors(
                            checkedThumbColor = Color(0xFF38BDF8),
                            checkedTrackColor = Color(0xFF0284C7)
                        )
                    )
                }
            }
        }

        Spacer(modifier = Modifier.height(14.dp))

        // 2. Filter Quality Selector (Hard, PCF 3x3, PCF 5x5, CSM)
        Text(
            text = "Shadow Filter Kernel (Sascha Willems)",
            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
            color = Color.White
        )
        Text(
            text = "Percentage Closer Filtering (PCF) mitigates jagged aliasing by sampling multi-tap depth grids",
            style = MaterialTheme.typography.bodySmall,
            color = Color(0xFF94A3B8)
        )

        Spacer(modifier = Modifier.height(8.dp))

        val qualities = listOf(ShadowQuality.HARD, ShadowQuality.PCF_3X3, ShadowQuality.PCF_5X5, ShadowQuality.CSM_4)
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            qualities.forEach { q ->
                val isSelected = shadowQuality == q && enableShadows
                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = if (isSelected) Color(0xFF4F46E5).copy(alpha = 0.35f) else Color(0xFF131722),
                    border = CardDefaults.outlinedCardBorder().copy(
                        brush = Brush.linearGradient(
                            if (isSelected) listOf(Color(0xFF818CF8), Color(0xFF6366F1))
                            else listOf(Color(0xFF1E293B), Color(0xFF334155))
                        )
                    ),
                    modifier = Modifier
                        .weight(1f)
                        .clickable {
                            onShadowQualitySelect(q)
                            if (!enableShadows) onToggleShadows(true)
                        }
                ) {
                    Column(
                        modifier = Modifier.padding(horizontal = 6.dp, vertical = 8.dp),
                        horizontalAlignment = Alignment.CenterHorizontally
                    ) {
                        Text(
                            text = q.shortLabel,
                            style = MaterialTheme.typography.labelSmall,
                            fontWeight = FontWeight.Bold,
                            color = if (isSelected) Color(0xFFA5B4FC) else Color(0xFFCBD5E1),
                            textAlign = TextAlign.Center
                        )
                        Text(
                            text = when (q) {
                                ShadowQuality.HARD -> "1-Tap"
                                ShadowQuality.PCF_3X3 -> "9-Tap"
                                ShadowQuality.PCF_5X5 -> "25-Tap"
                                ShadowQuality.CSM_4 -> "4-Cascade"
                                ShadowQuality.OFF -> "0-Tap"
                            },
                            style = MaterialTheme.typography.labelSmall,
                            fontSize = 9.sp,
                            color = if (isSelected) Color(0xFFC7D2FE) else Color(0xFF64748B)
                        )
                    }
                }
            }
        }

        Spacer(modifier = Modifier.height(14.dp))

        // 3. Shadow Optics & Depth Bias Controls
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                // Shadow Darkness
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text(
                        text = "Shadow Intensity (Opacity)",
                        style = MaterialTheme.typography.bodySmall,
                        color = Color(0xFF94A3B8)
                    )
                    Text(
                        text = "${(shadowIntensity * 100).toInt()}%",
                        style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.Bold),
                        color = Color(0xFFA5B4FC),
                        fontFamily = FontFamily.Monospace
                    )
                }
                Slider(
                    value = shadowIntensity,
                    onValueChange = onShadowIntensityChange,
                    valueRange = 0.1f..1.0f,
                    colors = SliderDefaults.colors(
                        thumbColor = Color(0xFFA5B4FC),
                        activeTrackColor = Color(0xFF6366F1),
                        inactiveTrackColor = Color(0xFF1E293B)
                    ),
                    modifier = Modifier.height(30.dp)
                )

                Spacer(modifier = Modifier.height(10.dp))

                // PCF Softness
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text(
                        text = "Penumbra Softness (Filter Radius)",
                        style = MaterialTheme.typography.bodySmall,
                        color = Color(0xFF94A3B8)
                    )
                    Text(
                        text = "${(shadowSoftness * 10).toInt() / 10f}x",
                        style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.Bold),
                        color = Color(0xFFA5B4FC),
                        fontFamily = FontFamily.Monospace
                    )
                }
                Slider(
                    value = shadowSoftness,
                    onValueChange = onShadowSoftnessChange,
                    valueRange = 0.5f..4.0f,
                    colors = SliderDefaults.colors(
                        thumbColor = Color(0xFFA5B4FC),
                        activeTrackColor = Color(0xFF6366F1),
                        inactiveTrackColor = Color(0xFF1E293B)
                    ),
                    modifier = Modifier.height(30.dp)
                )

                Spacer(modifier = Modifier.height(10.dp))

                // Depth Bias
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Column {
                        Text(
                            text = "Depth Bias (vkCmdSetDepthBias)",
                            style = MaterialTheme.typography.bodySmall,
                            color = Color(0xFF94A3B8)
                        )
                        Text(
                            text = "Prevents shadow acne & Peter Panning artifacts",
                            style = MaterialTheme.typography.labelSmall,
                            fontSize = 10.sp,
                            color = Color(0xFF64748B)
                        )
                    }
                    Text(
                        text = "${(depthBias * 1000).toInt() / 1000f}",
                        style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.Bold),
                        color = Color(0xFFA5B4FC),
                        fontFamily = FontFamily.Monospace
                    )
                }
                Slider(
                    value = depthBias,
                    onValueChange = onDepthBiasChange,
                    valueRange = 0.001f..0.025f,
                    colors = SliderDefaults.colors(
                        thumbColor = Color(0xFFA5B4FC),
                        activeTrackColor = Color(0xFF6366F1),
                        inactiveTrackColor = Color(0xFF1E293B)
                    ),
                    modifier = Modifier.height(30.dp)
                )
            }
        }

        Spacer(modifier = Modifier.height(14.dp))

        // 4. Live Light Camera Depth Buffer Card
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text(
                    text = "Light-Space Viewport & Depth Buffer",
                    style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold),
                    color = Color.White
                )
                Text(
                    text = "Orthographic projection from current sun position (El: ${sunElevation.toInt()}°, Az: ${sunAzimuth.toInt()}°)",
                    style = MaterialTheme.typography.bodySmall,
                    color = Color(0xFF64748B)
                )

                Spacer(modifier = Modifier.height(8.dp))

                Box(
                    modifier = Modifier
                        .fillMaxWidth()
                        .height(110.dp)
                        .background(Color(0xFF080B12), RoundedCornerShape(8.dp))
                        .border(1.dp, Color(0xFF1E293B), RoundedCornerShape(8.dp)),
                    contentAlignment = Alignment.Center
                ) {
                    Canvas(modifier = Modifier.fillMaxSize()) {
                        val cw = size.width
                        val ch = size.height
                        val ccx = cw / 2f
                        val ccy = ch / 2f

                        // Draw orthographic grid
                        for (i in -4..4) {
                            drawLine(
                                color = Color(0xFF1E293B).copy(alpha = 0.4f),
                                start = Offset(ccx + i * 28f, 0f),
                                end = Offset(ccx + i * 28f, ch),
                                strokeWidth = 1f
                            )
                            drawLine(
                                color = Color(0xFF1E293B).copy(alpha = 0.4f),
                                start = Offset(0f, ccy + i * 20f),
                                end = Offset(cw, ccy + i * 20f),
                                strokeWidth = 1f
                            )
                        }

                        // Depth projection of cube from sun angle
                        val radEl = (sunElevation * (kotlin.math.PI / 180.0)).toFloat()
                        val radAz = (sunAzimuth * (kotlin.math.PI / 180.0)).toFloat()
                        val lx = cos(radEl) * sin(radAz)
                        val lz = cos(radEl) * cos(radAz)

                        // Draw light-space occluder footprint with depth gradient
                        val rectW = 60f
                        val rectH = 45f
                        drawRoundRect(
                            brush = Brush.verticalGradient(
                                colors = listOf(Color(0xFFE2E8F0), Color(0xFF94A3B8), Color(0xFF475569))
                            ),
                            topLeft = Offset(ccx - rectW / 2f, ccy - rectH / 2f),
                            size = Size(rectW, rectH),
                            cornerRadius = androidx.compose.ui.geometry.CornerRadius(4f, 4f)
                        )
                        drawRoundRect(
                            color = Color(0xFF818CF8),
                            topLeft = Offset(ccx - rectW / 2f, ccy - rectH / 2f),
                            size = Size(rectW, rectH),
                            cornerRadius = androidx.compose.ui.geometry.CornerRadius(4f, 4f),
                            style = Stroke(width = 1.5f)
                        )

                        // Light ray directional vector arrow
                        val arrowLen = 32f
                        val endPt = Offset(ccx - lx * arrowLen, ccy + lz * arrowLen)
                        drawLine(Color(0xFFFBBF24), Offset(ccx, ccy), endPt, strokeWidth = 2f)
                        drawCircle(Color(0xFFFBBF24), radius = 3.5f, center = endPt)
                    }

                    // Depth Map Details Overlay
                    Column(
                        modifier = Modifier
                            .align(Alignment.BottomStart)
                            .padding(8.dp)
                    ) {
                        Text(
                            text = "VK_FORMAT_D32_SFLOAT • 2048×2048",
                            style = MaterialTheme.typography.labelSmall,
                            color = Color(0xFFA5B4FC),
                            fontSize = 9.sp,
                            fontFamily = FontFamily.Monospace
                        )
                        Text(
                            text = "CompareOp: VK_COMPARE_OP_LESS_OR_EQUAL",
                            style = MaterialTheme.typography.labelSmall,
                            color = Color(0xFF64748B),
                            fontSize = 8.sp,
                            fontFamily = FontFamily.Monospace
                        )
                    }
                }
            }
        }
    }
}
