package com.example.sphere.text.tabs

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.*

@Composable
fun WorldAndBillboardTab(
    mountTarget: TextMountTarget,
    onMountTargetChange: (TextMountTarget) -> Unit,
    wallMaterial: WallMaterialType,
    onWallMaterialChange: (WallMaterialType) -> Unit,
    postStyle: PostStyleType,
    onPostStyleChange: (PostStyleType) -> Unit,
    textApplicationStyle: TextApplicationStyle,
    onTextApplicationStyleChange: (TextApplicationStyle) -> Unit,
    mountHeightOffset: Float,
    onMountHeightOffsetChange: (Float) -> Unit,
    wallWidthMeters: Float,
    onWallWidthMetersChange: (Float) -> Unit,
    wallHeightMeters: Float,
    onWallHeightMetersChange: (Float) -> Unit,
    postHeightMeters: Float,
    onPostHeightMetersChange: (Float) -> Unit,
    renderMode: TextRenderMode,
    onRenderModeChange: (TextRenderMode) -> Unit,
    zoomFactor: Float,
    onZoomFactorChange: (Float) -> Unit,
    rotX: Float,
    onRotXChange: (Float) -> Unit,
    rotY: Float,
    onRotYChange: (Float) -> Unit,
    autoRotate: Boolean,
    onToggleAutoRotate: (Boolean) -> Unit,
    badgeStyle: TextBadgeStyle,
    onBadgeStyleChange: (TextBadgeStyle) -> Unit,
    onResetCamera: () -> Unit,
    modifier: Modifier = Modifier
) {
    Column(
        modifier = modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(14.dp)
    ) {
        // 1. TEXT MOUNT TARGET SELECTION (WALL, POST, FLOATING, HUD)
        Text(
            "TEXT MOUNT TARGET",
            style = MaterialTheme.typography.labelSmall.copy(
                fontWeight = FontWeight.Bold,
                color = MaterialTheme.colorScheme.primary,
                letterSpacing = 1.sp
            )
        )
        Spacer(Modifier.height(8.dp))
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            TextMountTarget.entries.forEach { target ->
                val isSelected = mountTarget == target
                Card(
                    onClick = { onMountTargetChange(target) },
                    colors = CardDefaults.cardColors(
                        containerColor = if (isSelected) MaterialTheme.colorScheme.primaryContainer else MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.45f)
                    ),
                    modifier = Modifier
                        .weight(1f)
                        .testTag("mount_target_${target.name.lowercase()}")
                ) {
                    Column(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(vertical = 10.dp, horizontal = 6.dp),
                        horizontalAlignment = Alignment.CenterHorizontally
                    ) {
                        Icon(
                            imageVector = when (target) {
                                TextMountTarget.WALL -> Icons.Default.Layers
                                TextMountTarget.POST -> Icons.Default.VerticalAlignTop
                                TextMountTarget.FLOATING -> Icons.Default.ViewInAr
                                TextMountTarget.HUD -> Icons.Default.Screenshot
                            },
                            contentDescription = null,
                            tint = if (isSelected) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.onSurfaceVariant,
                            modifier = Modifier.size(20.dp)
                        )
                        Spacer(Modifier.height(4.dp))
                        Text(
                            target.shortName,
                            style = MaterialTheme.typography.labelSmall.copy(
                                fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal,
                                color = if (isSelected) MaterialTheme.colorScheme.onPrimaryContainer else MaterialTheme.colorScheme.onSurface
                            ),
                            maxLines = 1
                        )
                    }
                }
            }
        }

        Spacer(Modifier.height(14.dp))

        // 2. CONTEXTUAL SETTINGS BASED ON MOUNT TARGET
        when (mountTarget) {
            TextMountTarget.WALL -> {
                // WALL MATERIAL
                Card(
                    colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerHigh),
                    modifier = Modifier.fillMaxWidth()
                ) {
                    Column(modifier = Modifier.padding(12.dp)) {
                        Row(verticalAlignment = Alignment.CenterVertically) {
                            Icon(Icons.Default.Architecture, contentDescription = null, tint = MaterialTheme.colorScheme.primary, modifier = Modifier.size(16.dp))
                            Spacer(Modifier.width(6.dp))
                            Text("Architectural Wall Surface", style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold))
                        }
                        Spacer(Modifier.height(8.dp))

                        // Material options
                        Row(
                            modifier = Modifier.fillMaxWidth(),
                            horizontalArrangement = Arrangement.spacedBy(6.dp)
                        ) {
                            WallMaterialType.entries.forEach { mat ->
                                val isSelected = wallMaterial == mat
                                FilterChip(
                                    selected = isSelected,
                                    onClick = { onWallMaterialChange(mat) },
                                    label = { Text(mat.label, fontSize = 10.sp, fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal) },
                                    leadingIcon = {
                                        Box(
                                            modifier = Modifier
                                                .size(10.dp)
                                                .clip(CircleShape)
                                                .background(mat.primaryColor)
                                        )
                                    },
                                    modifier = Modifier.testTag("wall_mat_${mat.name.lowercase()}")
                                )
                            }
                        }

                        Spacer(Modifier.height(10.dp))
                        // Wall Dimensions
                        Row(
                            modifier = Modifier.fillMaxWidth(),
                            horizontalArrangement = Arrangement.SpaceBetween
                        ) {
                            Text("Wall Width: ${"%.1f".format(wallWidthMeters)}m", style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.Medium))
                            Text("Height: ${"%.1f".format(wallHeightMeters)}m", style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.Medium))
                        }
                        Slider(
                            value = wallWidthMeters,
                            onValueChange = onWallWidthMetersChange,
                            valueRange = 1.6f..6.0f,
                            modifier = Modifier.testTag("wall_width_slider")
                        )
                    }
                }
            }
            TextMountTarget.POST -> {
                // POST STYLE
                Card(
                    colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerHigh),
                    modifier = Modifier.fillMaxWidth()
                ) {
                    Column(modifier = Modifier.padding(12.dp)) {
                        Row(verticalAlignment = Alignment.CenterVertically) {
                            Icon(Icons.Default.Signpost, contentDescription = null, tint = MaterialTheme.colorScheme.primary, modifier = Modifier.size(16.dp))
                            Spacer(Modifier.width(6.dp))
                            Text("Street & Sign Post Configuration", style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold))
                        }
                        Spacer(Modifier.height(8.dp))

                        // Post options
                        Row(
                            modifier = Modifier.fillMaxWidth(),
                            horizontalArrangement = Arrangement.spacedBy(6.dp)
                        ) {
                            PostStyleType.entries.forEach { style ->
                                val isSelected = postStyle == style
                                FilterChip(
                                    selected = isSelected,
                                    onClick = { onPostStyleChange(style) },
                                    label = { Text(style.label, fontSize = 10.sp, fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal) },
                                    leadingIcon = {
                                        Box(
                                            modifier = Modifier
                                                .size(10.dp)
                                                .clip(CircleShape)
                                                .background(style.poleColor)
                                        )
                                    },
                                    modifier = Modifier.testTag("post_style_${style.name.lowercase()}")
                                )
                            }
                        }

                        Spacer(Modifier.height(10.dp))
                        Row(
                            modifier = Modifier.fillMaxWidth(),
                            horizontalArrangement = Arrangement.SpaceBetween
                        ) {
                            Text("Post Height: ${"%.1f".format(postHeightMeters)}m", style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.Medium))
                            Text("Ground Socket: Anchor Bolts", style = MaterialTheme.typography.bodySmall.copy(color = MaterialTheme.colorScheme.onSurfaceVariant))
                        }
                        Slider(
                            value = postHeightMeters,
                            onValueChange = onPostHeightMetersChange,
                            valueRange = 1.8f..4.5f,
                            modifier = Modifier.testTag("post_height_slider")
                        )
                    }
                }
            }
            else -> {}
        }

        // 3. TEXT APPLICATION STYLE (PLAQUE, DECAL, 3D CHANNEL LETTERS, NEON)
        if (mountTarget == TextMountTarget.WALL || mountTarget == TextMountTarget.POST) {
            Spacer(Modifier.height(14.dp))
            Text(
                "TEXT APPLICATION STYLE",
                style = MaterialTheme.typography.labelSmall.copy(
                    fontWeight = FontWeight.Bold,
                    color = MaterialTheme.colorScheme.primary,
                    letterSpacing = 1.sp
                )
            )
            Spacer(Modifier.height(8.dp))
            Column(verticalArrangement = Arrangement.spacedBy(6.dp)) {
                TextApplicationStyle.entries.forEach { appStyle ->
                    val isSelected = textApplicationStyle == appStyle
                    Card(
                        onClick = { onTextApplicationStyleChange(appStyle) },
                        colors = CardDefaults.cardColors(
                            containerColor = if (isSelected) MaterialTheme.colorScheme.secondaryContainer else MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.4f)
                        ),
                        modifier = Modifier.fillMaxWidth().testTag("app_style_${appStyle.name.lowercase()}")
                    ) {
                        Row(
                            modifier = Modifier.padding(horizontal = 12.dp, vertical = 8.dp),
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            RadioButton(selected = isSelected, onClick = { onTextApplicationStyleChange(appStyle) })
                            Spacer(Modifier.width(8.dp))
                            Column {
                                Text(
                                    appStyle.label,
                                    style = MaterialTheme.typography.bodyMedium.copy(
                                        fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal
                                    )
                                )
                                Text(
                                    appStyle.description,
                                    style = MaterialTheme.typography.labelSmall.copy(
                                        color = MaterialTheme.colorScheme.onSurfaceVariant,
                                        fontSize = 10.sp
                                    )
                                )
                            }
                        }
                    }
                }
            }

            // Vertical Offset Slider on Wall/Post
            Spacer(Modifier.height(10.dp))
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                Text(
                    "Vertical Mount Offset: ${"%.2f".format(mountHeightOffset)}m",
                    style = MaterialTheme.typography.bodySmall.copy(fontWeight = FontWeight.Medium)
                )
                Text(
                    if (mountHeightOffset == 0f) "Centered" else if (mountHeightOffset > 0) "Higher" else "Lower",
                    style = MaterialTheme.typography.bodySmall.copy(color = MaterialTheme.colorScheme.primary)
                )
            }
            Slider(
                value = mountHeightOffset,
                onValueChange = onMountHeightOffsetChange,
                valueRange = -0.50f..0.50f,
                modifier = Modifier.testTag("mount_height_offset_slider")
            )
        }

        Spacer(Modifier.height(14.dp))
        HorizontalDivider(color = MaterialTheme.colorScheme.surfaceVariant)
        Spacer(Modifier.height(14.dp))

        // 4. BACKGROUND PLAQUE / FRAME STYLE
        Text(
            "BACKGROUND PLAQUE / FRAME STYLE",
            style = MaterialTheme.typography.labelSmall.copy(
                fontWeight = FontWeight.Bold,
                color = MaterialTheme.colorScheme.primary,
                letterSpacing = 1.sp
            )
        )
        Spacer(Modifier.height(8.dp))
        Column(verticalArrangement = Arrangement.spacedBy(6.dp)) {
            TextBadgeStyle.entries.forEach { style ->
                val isSelected = badgeStyle == style
                Card(
                    onClick = { onBadgeStyleChange(style) },
                    colors = CardDefaults.cardColors(
                        containerColor = if (isSelected) MaterialTheme.colorScheme.secondaryContainer else MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.4f)
                    ),
                    modifier = Modifier.fillMaxWidth().testTag("badge_style_${style.name.lowercase()}")
                ) {
                    Row(
                        modifier = Modifier.padding(horizontal = 12.dp, vertical = 8.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        RadioButton(selected = isSelected, onClick = { onBadgeStyleChange(style) })
                        Spacer(Modifier.width(8.dp))
                        Text(
                            style.label,
                            style = MaterialTheme.typography.bodyMedium.copy(
                                fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal
                            )
                        )
                    }
                }
            }
        }

        Spacer(Modifier.height(16.dp))
        HorizontalDivider(color = MaterialTheme.colorScheme.surfaceVariant)
        Spacer(Modifier.height(16.dp))

        // 5. SDF ZOOM MAGNIFICATION TEST
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Column {
                Text(
                    "SDF VECTOR ZOOM TEST",
                    style = MaterialTheme.typography.labelSmall.copy(
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.primary,
                        letterSpacing = 1.sp
                    )
                )
                Text(
                    "Observe zero pixelation even at 5.0x magnification",
                    style = MaterialTheme.typography.bodySmall.copy(color = MaterialTheme.colorScheme.onSurfaceVariant)
                )
            }
            Text(
                "${"%.1f".format(zoomFactor)}x",
                style = MaterialTheme.typography.titleMedium.copy(
                    fontWeight = FontWeight.Bold,
                    color = MaterialTheme.colorScheme.primary
                )
            )
        }
        Slider(
            value = zoomFactor,
            onValueChange = onZoomFactorChange,
            valueRange = 0.6f..5.0f,
            modifier = Modifier.testTag("zoom_factor_slider")
        )

        Spacer(Modifier.height(14.dp))
        HorizontalDivider(color = MaterialTheme.colorScheme.surfaceVariant)
        Spacer(Modifier.height(14.dp))

        // 6. 3D CAMERA & ORBIT ROTATION
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Text(
                "3D CAMERA ORBIT CONTROLS",
                style = MaterialTheme.typography.labelSmall.copy(
                    fontWeight = FontWeight.Bold,
                    color = MaterialTheme.colorScheme.primary,
                    letterSpacing = 1.sp
                )
            )
            Row {
                IconButton(
                    onClick = { onToggleAutoRotate(!autoRotate) },
                    modifier = Modifier.testTag("auto_orbit_button")
                ) {
                    Icon(
                        if (autoRotate) Icons.Default.Pause else Icons.Default.PlayArrow,
                        contentDescription = "Toggle Orbit"
                    )
                }
                IconButton(
                    onClick = onResetCamera,
                    modifier = Modifier.testTag("reset_camera_button")
                ) {
                    Icon(Icons.Default.RestartAlt, contentDescription = "Reset Camera")
                }
            }
        }

        Spacer(Modifier.height(6.dp))
        Text(
            "Pitch (Elevation): ${rotX.toInt()}°",
            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Medium)
        )
        Slider(
            value = rotX,
            onValueChange = onRotXChange,
            valueRange = -45f..75f,
            modifier = Modifier.testTag("camera_pitch_slider")
        )

        Spacer(Modifier.height(6.dp))
        Text(
            "Yaw (Azimuth): ${rotY.toInt()}°",
            style = MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Medium)
        )
        Slider(
            value = rotY,
            onValueChange = onRotYChange,
            valueRange = -180f..180f,
            modifier = Modifier.testTag("camera_yaw_slider")
        )
    }
}
