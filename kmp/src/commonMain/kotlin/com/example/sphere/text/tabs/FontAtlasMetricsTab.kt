package com.example.sphere.text.tabs

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.GridOn
import androidx.compose.material.icons.filled.SquareFoot
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.FontTypeface

@Composable
fun FontAtlasMetricsTab(
    font: FontTypeface,
    showAtlasView: Boolean,
    onToggleAtlasView: (Boolean) -> Unit,
    showMetrics: Boolean,
    onToggleMetrics: (Boolean) -> Unit,
    modifier: Modifier = Modifier
) {
    Column(
        modifier = modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(14.dp)
    ) {
        // VIEWPORT OVERLAY CONTROLS
        Text(
            "FONT ATLAS & METRICS INSPECTION",
            style = MaterialTheme.typography.labelSmall.copy(
                fontWeight = FontWeight.Bold,
                color = MaterialTheme.colorScheme.primary,
                letterSpacing = 1.sp
            )
        )
        Spacer(Modifier.height(8.dp))

        Card(
            colors = CardDefaults.cardColors(
                containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f)
            ),
            modifier = Modifier.fillMaxWidth()
        ) {
            Column(modifier = Modifier.padding(12.dp)) {
                // Switch 1: Atlas View
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Row(verticalAlignment = Alignment.CenterVertically) {
                        Icon(
                            Icons.Default.GridOn,
                            contentDescription = null,
                            tint = MaterialTheme.colorScheme.primary,
                            modifier = Modifier.size(20.dp)
                        )
                        Spacer(Modifier.width(10.dp))
                        Column {
                            Text(
                                "Inspect 1024x1024 SDF Atlas",
                                style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.SemiBold)
                            )
                            Text(
                                "View 16x16 glyph distance field map",
                                style = MaterialTheme.typography.labelSmall.copy(color = MaterialTheme.colorScheme.onSurfaceVariant)
                            )
                        }
                    }
                    Switch(
                        checked = showAtlasView,
                        onCheckedChange = onToggleAtlasView,
                        modifier = Modifier.testTag("toggle_atlas_view_switch")
                    )
                }

                Spacer(Modifier.height(10.dp))
                HorizontalDivider()
                Spacer(Modifier.height(10.dp))

                // Switch 2: Typographic Metrics
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Row(verticalAlignment = Alignment.CenterVertically) {
                        Icon(
                            Icons.Default.SquareFoot,
                            contentDescription = null,
                            tint = Color(0xFF40C4FF),
                            modifier = Modifier.size(20.dp)
                        )
                        Spacer(Modifier.width(10.dp))
                        Column {
                            Text(
                                "Typographic Metric Guides",
                                style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.SemiBold)
                            )
                            Text(
                                "Show baseline, ascender, descender & bounds",
                                style = MaterialTheme.typography.labelSmall.copy(color = MaterialTheme.colorScheme.onSurfaceVariant)
                            )
                        }
                    }
                    Switch(
                        checked = showMetrics,
                        onCheckedChange = onToggleMetrics,
                        modifier = Modifier.testTag("toggle_metrics_switch")
                    )
                }
            }
        }

        Spacer(Modifier.height(16.dp))

        // FONT METRICS TABLE
        Text(
            "FONT SPECIFICATIONS // ${font.label.uppercase()}",
            style = MaterialTheme.typography.labelSmall.copy(
                fontWeight = FontWeight.Bold,
                color = MaterialTheme.colorScheme.primary,
                letterSpacing = 1.sp
            )
        )
        Spacer(Modifier.height(8.dp))

        Column(
            modifier = Modifier
                .fillMaxWidth()
                .background(Color(0xFF0C1018), RoundedCornerShape(8.dp))
                .padding(12.dp)
        ) {
            MetricRow("Atlas Dimensions", "1024 x 1024 pixels (16x16 cells)")
            MetricRow("Atlas Format", "VK_FORMAT_R8_UNORM (SDF)")
            MetricRow("Glyphs Baked", "95 characters (ASCII 32..126)")
            MetricRow("Native Point Size", "48.0 pt")
            MetricRow("Ascender Height", "+40.0 px (above baseline)")
            MetricRow("Descender Depth", "-12.0 px (below baseline)")
            MetricRow("Line Height", "56.0 px (1.16x leading)")
            MetricRow("SDF Pixel Spread", "8.0 px distance range")
            MetricRow("Sampler Filter", "VK_FILTER_LINEAR (smoothstep AA)")
            MetricRow("Texture VRAM", "1,048,576 Bytes (1.00 MB)")
        }

        Spacer(Modifier.height(16.dp))

        // SASCHA WILLEMS DISTANCE FIELD ADVANTAGE EXPLANATION
        Text(
            "WHY SASCHA WILLEMS SDF FONTS?",
            style = MaterialTheme.typography.labelSmall.copy(
                fontWeight = FontWeight.Bold,
                color = MaterialTheme.colorScheme.primary,
                letterSpacing = 1.sp
            )
        )
        Spacer(Modifier.height(6.dp))
        Text(
            "Traditional raster bitmap fonts blur heavily when scaled up and pixelate when viewed close in 3D. Distance Field fonts store signed distances to glyph contours rather than binary pixels.\n\nUsing a single hardware linear sampler and a GLSL smoothstep() instruction, the GPU evaluates infinite-resolution vector glyph edges in constant time, allowing free dynamic outlines, dropshadows, and glow effects without extra geometry.",
            style = MaterialTheme.typography.bodySmall.copy(
                color = MaterialTheme.colorScheme.onSurfaceVariant,
                lineHeight = 18.sp
            )
        )
    }
}

@Composable
private fun MetricRow(label: String, value: String) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(vertical = 3.dp),
        horizontalArrangement = Arrangement.SpaceBetween
    ) {
        Text(
            label,
            style = MaterialTheme.typography.bodySmall.copy(
                color = Color(0xFF8C9BAE),
                fontSize = 11.sp
            )
        )
        Text(
            value,
            style = MaterialTheme.typography.bodySmall.copy(
                color = Color(0xFFE2EAF4),
                fontWeight = FontWeight.Medium,
                fontFamily = FontFamily.Monospace,
                fontSize = 11.sp
            )
        )
    }
}
