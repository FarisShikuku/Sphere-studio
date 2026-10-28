package com.example.sphere

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ViewInAr
import androidx.compose.material.icons.outlined.Cloud
import androidx.compose.material.icons.outlined.Texture
import androidx.compose.material.icons.outlined.TextFields
import androidx.compose.material.icons.outlined.WbSunny
import androidx.compose.material3.Badge
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Tab
import androidx.compose.material3.TabRow
import androidx.compose.material3.TabRowDefaults
import androidx.compose.material3.TabRowDefaults.tabIndicatorOffset
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.PathEffect
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

// -------------------------------------------------------------
// EMPTY / PLACEHOLDER ASSET DIRECTORY SCREEN
// -------------------------------------------------------------
@Composable
fun EmptyAssetScreen(
    category: AssetCategory,
    onBackToTest: () -> Unit,
    onNavigateToSky: () -> Unit,
    onNavigateToWeather: () -> Unit,
    onNavigateToSurfaces: () -> Unit = {},
    onNavigateToText: () -> Unit = {},
    modifier: Modifier = Modifier
) {
    var selectedEmptyTab by remember { mutableIntStateOf(0) }

    Column(
        modifier = modifier
            .fillMaxSize()
            .background(Color(0xFF0B0D13))
            .testTag("empty_asset_screen_${category.id}")
    ) {
        // 1. TOP VIEWPORT: ISOMETRIC BLUEPRINT LATTICE & EMPTY ASSET CONTAINER (weight 1.08f)
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .weight(1.08f)
                .background(Color(0xFF07090E))
                .testTag("empty_asset_viewport")
        ) {
            Canvas(modifier = Modifier.fillMaxSize()) {
                val w = size.width
                val h = size.height
                val center = Offset(w / 2f, h / 2f)

                // Background gradient
                drawRect(
                    brush = Brush.radialGradient(
                        colors = listOf(Color(0xFF0F172A), Color(0xFF04060A)),
                        center = center,
                        radius = w * 0.75f
                    )
                )

                // Isometric coordinate lattice grid
                val step = 32f
                val gridColor = Color(0xFF1E293B).copy(alpha = 0.45f)
                val accentGrid = Color(0xFF334155).copy(alpha = 0.7f)

                var x = 0f
                while (x < w) {
                    drawLine(if ((x / step).toInt() % 4 == 0) accentGrid else gridColor, Offset(x, 0f), Offset(x, h), strokeWidth = 1f)
                    x += step
                }
                var y = 0f
                while (y < h) {
                    drawLine(if ((y / step).toInt() % 4 == 0) accentGrid else gridColor, Offset(0f, y), Offset(w, y), strokeWidth = 1f)
                    y += step
                }

                // Empty bounding box wireframe
                val boxRadius = minOf(w, h) * 0.28f
                val dashEffect = PathEffect.dashPathEffect(floatArrayOf(12f, 8f), 0f)

                // Top diamond
                val topPath = Path().apply {
                    moveTo(center.x, center.y - boxRadius * 0.8f)
                    lineTo(center.x + boxRadius, center.y - boxRadius * 0.2f)
                    lineTo(center.x, center.y + boxRadius * 0.4f)
                    lineTo(center.x - boxRadius, center.y - boxRadius * 0.2f)
                    close()
                }
                drawPath(topPath, color = Color(0xFF38BDF8).copy(alpha = 0.35f), style = Stroke(width = 1.5f, pathEffect = dashEffect))

                // Vertical posts
                val postColor = Color(0xFF64748B).copy(alpha = 0.4f)
                drawLine(postColor, Offset(center.x, center.y - boxRadius * 0.8f), Offset(center.x, center.y - boxRadius * 0.1f), strokeWidth = 1.5f)
                drawLine(postColor, Offset(center.x + boxRadius, center.y - boxRadius * 0.2f), Offset(center.x + boxRadius, center.y + boxRadius * 0.5f), strokeWidth = 1.5f)
                drawLine(postColor, Offset(center.x - boxRadius, center.y - boxRadius * 0.2f), Offset(center.x - boxRadius, center.y + boxRadius * 0.5f), strokeWidth = 1.5f)
                drawLine(postColor, Offset(center.x, center.y + boxRadius * 0.4f), Offset(center.x, center.y + boxRadius * 1.1f), strokeWidth = 1.5f)

                // Bottom diamond
                val botPath = Path().apply {
                    moveTo(center.x, center.y - boxRadius * 0.1f)
                    lineTo(center.x + boxRadius, center.y + boxRadius * 0.5f)
                    lineTo(center.x, center.y + boxRadius * 1.1f)
                    lineTo(center.x - boxRadius, center.y + boxRadius * 0.5f)
                    close()
                }
                drawPath(botPath, color = Color(0xFF475569).copy(alpha = 0.3f), style = Stroke(width = 1.5f, pathEffect = dashEffect))

                // Center crosshair
                drawLine(Color(0xFFEF4444).copy(alpha = 0.6f), Offset(center.x - 24f, center.y), Offset(center.x + 24f, center.y), strokeWidth = 2f)
                drawLine(Color(0xFF10B981).copy(alpha = 0.6f), Offset(center.x, center.y - 24f), Offset(center.x, center.y + 24f), strokeWidth = 2f)
            }

            // Floating Top HUD
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(12.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = Color(0xCC0F172A),
                    border = CardDefaults.outlinedCardBorder()
                ) {
                    Row(
                        modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Icon(
                            imageVector = category.icon,
                            contentDescription = null,
                            tint = Color(0xFF94A3B8),
                            modifier = Modifier.size(14.dp)
                        )
                        Spacer(modifier = Modifier.width(6.dp))
                        Text(
                            text = "native_assets/src/${category.id}/",
                            style = MaterialTheme.typography.labelSmall,
                            color = Color(0xFF94A3B8),
                            fontFamily = FontFamily.Monospace
                        )
                    }
                }

                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = Color(0xCC1E293B)
                ) {
                    Text(
                        text = "EMPTY DIRECTORY",
                        style = MaterialTheme.typography.labelSmall.copy(fontWeight = FontWeight.Bold),
                        color = Color(0xFFF59E0B),
                        modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp),
                        fontFamily = FontFamily.Monospace
                    )
                }
            }

            // Center Blueprint Label Overlay
            Card(
                colors = CardDefaults.cardColors(containerColor = Color(0xDD0D111A)),
                shape = RoundedCornerShape(12.dp),
                border = CardDefaults.outlinedCardBorder().copy(
                    brush = Brush.linearGradient(listOf(Color(0xFF1E293B), Color(0xFF334155)))
                ),
                modifier = Modifier
                    .align(Alignment.Center)
                    .padding(24.dp)
            ) {
                Column(
                    modifier = Modifier.padding(16.dp),
                    horizontalAlignment = Alignment.CenterHorizontally
                ) {
                    Icon(
                        imageVector = category.icon,
                        contentDescription = null,
                        tint = Color(0xFF64748B),
                        modifier = Modifier.size(36.dp)
                    )
                    Spacer(modifier = Modifier.height(8.dp))
                    Text(
                        text = "${category.title} Generator",
                        style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                        color = Color.White
                    )
                    Spacer(modifier = Modifier.height(4.dp))
                    Text(
                        text = "Directory strictly empty — awaiting resource definitions",
                        style = MaterialTheme.typography.bodySmall,
                        color = Color(0xFF94A3B8),
                        textAlign = TextAlign.Center
                    )
                }
            }
        }

        HorizontalDivider(color = Color(0xFF1E293B), thickness = 1.dp)

        // 2. BOTTOM CONTROLS PANEL: SAME PROPORTIONS AS TEST SCREEN (weight 0.92f)
        Surface(
            modifier = Modifier
                .fillMaxWidth()
                .weight(0.92f),
            color = Color(0xFF0F121A)
        ) {
            Column(modifier = Modifier.fillMaxSize()) {
                TabRow(
                    selectedTabIndex = selectedEmptyTab,
                    containerColor = Color(0xFF131722),
                    contentColor = Color.White,
                    indicator = { tabPositions ->
                        TabRowDefaults.SecondaryIndicator(
                            Modifier.tabIndicatorOffset(tabPositions[selectedEmptyTab]),
                            color = Color(0xFF94A3B8),
                            height = 3.dp
                        )
                    }
                ) {
                    Tab(
                        selected = selectedEmptyTab == 0,
                        onClick = { selectedEmptyTab = 0 },
                        text = { Text("Directory Status", fontSize = 12.sp, fontWeight = FontWeight.SemiBold) }
                    )
                    Tab(
                        selected = selectedEmptyTab == 1,
                        onClick = { selectedEmptyTab = 1 },
                        text = { Text("README Specs", fontSize = 12.sp, fontWeight = FontWeight.SemiBold) }
                    )
                    Tab(
                        selected = selectedEmptyTab == 2,
                        onClick = { selectedEmptyTab = 2 },
                        text = { Text("Active Modules", fontSize = 12.sp, fontWeight = FontWeight.SemiBold) }
                    )
                }

                // Tab Body
                Column(
                    modifier = Modifier
                        .fillMaxSize()
                        .verticalScroll(rememberScrollState())
                        .padding(16.dp)
                ) {
                    when (selectedEmptyTab) {
                        0 -> {
                            Card(
                                colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
                                shape = RoundedCornerShape(12.dp)
                            ) {
                                Column(modifier = Modifier.padding(16.dp)) {
                                    Row(
                                        modifier = Modifier.fillMaxWidth(),
                                        horizontalArrangement = Arrangement.SpaceBetween,
                                        verticalAlignment = Alignment.CenterVertically
                                    ) {
                                        Text(
                                            text = "Empty Directory Status",
                                            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                                            color = Color.White
                                        )
                                        Badge(containerColor = Color(0xFF451A03), contentColor = Color(0xFFFBBF24)) {
                                            Text("Awaiting Assets", fontSize = 10.sp, fontWeight = FontWeight.Bold)
                                        }
                                    }
                                    Spacer(modifier = Modifier.height(8.dp))
                                    Text(
                                        text = "As strictly directed, 'native_assets/src/${category.id}/' is kept empty in .gitignore until complete resource definitions and mesh/shader pipelines are provisioned.",
                                        style = MaterialTheme.typography.bodySmall,
                                        color = Color(0xFF94A3B8)
                                    )
                                    Spacer(modifier = Modifier.height(14.dp))
                                    Surface(
                                        shape = RoundedCornerShape(8.dp),
                                        color = Color(0xFF090B10),
                                        border = CardDefaults.outlinedCardBorder()
                                    ) {
                                        Column(modifier = Modifier.padding(12.dp)) {
                                            Text(
                                                text = "• Location: native_assets/src/${category.id}/\n• Status: Excluded in .gitignore (Empty)\n• Active Implemented Subsystem: Atmosphere & Sky (sky/)\n• Calibration Scene: Test Screen (3D Cube & PBR)",
                                                style = MaterialTheme.typography.labelSmall,
                                                color = Color(0xFFCBD5E1),
                                                fontFamily = FontFamily.Monospace,
                                                lineHeight = 18.sp
                                            )
                                        }
                                    }
                                }
                            }
                        }
                        1 -> {
                            Card(
                                colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
                                shape = RoundedCornerShape(12.dp)
                            ) {
                                Column(modifier = Modifier.padding(16.dp)) {
                                    Text(
                                        text = "Subsystem Architectural Role",
                                        style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                                        color = Color.White
                                    )
                                    Spacer(modifier = Modifier.height(8.dp))
                                    Text(
                                        text = category.description,
                                        style = MaterialTheme.typography.bodyMedium,
                                        color = Color(0xFFE2E8F0)
                                    )
                                    Spacer(modifier = Modifier.height(12.dp))
                                    Text(
                                        text = "Reference: Full Procedural Asset Tree in README.md",
                                        style = MaterialTheme.typography.labelSmall,
                                        color = Color(0xFF64748B)
                                    )
                                }
                            }
                        }
                        2 -> {
                            Card(
                                colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
                                shape = RoundedCornerShape(12.dp)
                            ) {
                                Column(modifier = Modifier.padding(16.dp)) {
                                    Text(
                                        text = "Implemented & Active Modules",
                                        style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                                        color = Color.White
                                    )
                                    Spacer(modifier = Modifier.height(8.dp))
                                    Text(
                                        text = "You can navigate to the fully implemented modules via the drawer or buttons below:",
                                        style = MaterialTheme.typography.bodySmall,
                                        color = Color(0xFF94A3B8)
                                    )
                                    Spacer(modifier = Modifier.height(14.dp))
                                    Row(
                                        modifier = Modifier.fillMaxWidth(),
                                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                                    ) {
                                        Surface(
                                            shape = RoundedCornerShape(8.dp),
                                            color = Color(0xFF0369A1).copy(alpha = 0.35f),
                                            border = CardDefaults.outlinedCardBorder().copy(brush = Brush.linearGradient(listOf(Color(0xFF0284C7), Color(0xFF38BDF8)))),
                                            modifier = Modifier.weight(1f).clickable { onNavigateToSky() }
                                        ) {
                                            Row(
                                                modifier = Modifier.padding(10.dp),
                                                verticalAlignment = Alignment.CenterVertically,
                                                horizontalArrangement = Arrangement.Center
                                            ) {
                                                Icon(Icons.Outlined.WbSunny, contentDescription = null, tint = Color(0xFF38BDF8), modifier = Modifier.size(16.dp))
                                                Spacer(modifier = Modifier.width(6.dp))
                                                Text("Sky", color = Color.White, style = MaterialTheme.typography.labelMedium, fontWeight = FontWeight.Bold)
                                            }
                                        }

                                        Surface(
                                            shape = RoundedCornerShape(8.dp),
                                            color = Color(0xFF0891B2).copy(alpha = 0.35f),
                                            border = CardDefaults.outlinedCardBorder().copy(brush = Brush.linearGradient(listOf(Color(0xFF06B6D4), Color(0xFF38BDF8)))),
                                            modifier = Modifier.weight(1f).clickable { onNavigateToWeather() }
                                        ) {
                                            Row(
                                                modifier = Modifier.padding(10.dp),
                                                verticalAlignment = Alignment.CenterVertically,
                                                horizontalArrangement = Arrangement.Center
                                            ) {
                                                Icon(Icons.Outlined.Cloud, contentDescription = null, tint = Color(0xFF38BDF8), modifier = Modifier.size(16.dp))
                                                Spacer(modifier = Modifier.width(6.dp))
                                                Text("Weather", color = Color.White, style = MaterialTheme.typography.labelMedium, fontWeight = FontWeight.Bold)
                                            }
                                        }

                                        Surface(
                                            shape = RoundedCornerShape(8.dp),
                                            color = Color(0xFF0F766E).copy(alpha = 0.35f),
                                            border = CardDefaults.outlinedCardBorder().copy(brush = Brush.linearGradient(listOf(Color(0xFF14B8A6), Color(0xFF2DD4BF)))),
                                            modifier = Modifier.weight(1f).clickable { onNavigateToSurfaces() }
                                        ) {
                                            Row(
                                                modifier = Modifier.padding(10.dp),
                                                verticalAlignment = Alignment.CenterVertically,
                                                horizontalArrangement = Arrangement.Center
                                            ) {
                                                Icon(Icons.Outlined.Texture, contentDescription = null, tint = Color(0xFF2DD4BF), modifier = Modifier.size(16.dp))
                                                Spacer(modifier = Modifier.width(6.dp))
                                                Text("Surfaces", color = Color.White, style = MaterialTheme.typography.labelMedium, fontWeight = FontWeight.Bold)
                                             }
                                        }

                                        Surface(
                                            shape = RoundedCornerShape(8.dp),
                                            color = Color(0xFF4338CA).copy(alpha = 0.35f),
                                            border = CardDefaults.outlinedCardBorder().copy(brush = Brush.linearGradient(listOf(Color(0xFF6366F1), Color(0xFF818CF8)))),
                                            modifier = Modifier.weight(1f).clickable { onNavigateToText() }
                                        ) {
                                            Row(
                                                modifier = Modifier.padding(10.dp),
                                                verticalAlignment = Alignment.CenterVertically,
                                                horizontalArrangement = Arrangement.Center
                                            ) {
                                                Icon(Icons.Outlined.TextFields, contentDescription = null, tint = Color(0xFF818CF8), modifier = Modifier.size(16.dp))
                                                Spacer(modifier = Modifier.width(6.dp))
                                                Text("Text", color = Color.White, style = MaterialTheme.typography.labelMedium, fontWeight = FontWeight.Bold)
                                            }
                                        }

                                        Surface(
                                            shape = RoundedCornerShape(8.dp),
                                            color = Color(0xFF1E2638),
                                            border = CardDefaults.outlinedCardBorder(),
                                            modifier = Modifier.weight(1f).clickable { onBackToTest() }
                                        ) {
                                            Row(
                                                modifier = Modifier.padding(10.dp),
                                                verticalAlignment = Alignment.CenterVertically,
                                                horizontalArrangement = Arrangement.Center
                                            ) {
                                                Icon(Icons.Default.ViewInAr, contentDescription = null, tint = MaterialTheme.colorScheme.primary, modifier = Modifier.size(16.dp))
                                                Spacer(modifier = Modifier.width(6.dp))
                                                Text("Test", color = Color.White, style = MaterialTheme.typography.labelMedium, fontWeight = FontWeight.Bold)
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
