package com.example.sphere

import androidx.compose.animation.AnimatedVisibility
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.CenterFocusStrong
import androidx.compose.material.icons.filled.Check
import androidx.compose.material.icons.filled.Radar
import androidx.compose.material.icons.filled.Visibility
import androidx.compose.material.icons.filled.VisibilityOff
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.FilterChip
import androidx.compose.material3.FilterChipDefaults
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Switch
import androidx.compose.material3.SwitchDefaults
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.CloudDetectionRepository
import com.example.sphere.model.CloudGenus
import com.example.sphere.model.DetectedCloudFormation

@Composable
fun CloudDetectionTab(
    isDetectionEnabled: Boolean,
    onToggleDetection: (Boolean) -> Unit,
    selectedFilterGenus: CloudGenus?,
    onFilterGenusChange: (CloudGenus?) -> Unit,
    selectedFormationId: String?,
    onSelectFormation: (DetectedCloudFormation) -> Unit,
    onFocusCamera: (azimuth: Float, elevation: Float) -> Unit,
    cloudCoverage: Float,
    onCloudCoverageChange: (Float) -> Unit,
    detectedFormations: List<DetectedCloudFormation> = CloudDetectionRepository.defaultFormations
) {
    Column(
        modifier = Modifier
            .fillMaxWidth()
            .padding(bottom = 24.dp)
            .testTag("cloud_detection_tab")
    ) {
        // Card 1: Master Scanner Controls & Quick Genus Filter
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.padding(bottom = 12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column {
                        Row(verticalAlignment = Alignment.CenterVertically) {
                            Icon(
                                imageVector = Icons.Default.Radar,
                                contentDescription = null,
                                tint = if (isDetectionEnabled) Color(0xFF38BDF8) else Color(0xFF64748B),
                                modifier = Modifier.size(18.dp)
                            )
                            Spacer(modifier = Modifier.width(6.dp))
                            Text(
                                text = "Live Sky Cloud Scanner",
                                style = MaterialTheme.typography.titleMedium.copy(fontWeight = FontWeight.Bold),
                                color = Color.White
                            )
                        }
                        Text(
                            text = if (isDetectionEnabled) "LiDAR & CV Cloud Genera Tracker Active" else "Scanner Paused",
                            style = MaterialTheme.typography.bodySmall,
                            color = if (isDetectionEnabled) Color(0xFF38BDF8) else Color(0xFF64748B)
                        )
                    }

                    Switch(
                        checked = isDetectionEnabled,
                        onCheckedChange = onToggleDetection,
                        colors = SwitchDefaults.colors(
                            checkedThumbColor = Color(0xFF38BDF8),
                            checkedTrackColor = Color(0xFF0284C7)
                        ),
                        modifier = Modifier.testTag("toggle_cloud_detection_switch")
                    )
                }

                Spacer(modifier = Modifier.height(12.dp))

                // Cloud Genera Quick Selector Chips
                Text(
                    text = "Cloud Types & Genera Filter:",
                    fontSize = 11.sp,
                    fontWeight = FontWeight.SemiBold,
                    color = Color(0xFF94A3B8)
                )
                Spacer(modifier = Modifier.height(6.dp))

                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .horizontalScroll(rememberScrollState()),
                    horizontalArrangement = Arrangement.spacedBy(6.dp)
                ) {
                    // "All Types" Chip
                    FilterChip(
                        selected = selectedFilterGenus == null,
                        onClick = { onFilterGenusChange(null) },
                        label = { Text("All (${detectedFormations.size})", fontSize = 11.sp) },
                        colors = FilterChipDefaults.filterChipColors(
                            selectedContainerColor = Color(0xFF0284C7),
                            selectedLabelColor = Color.White
                        ),
                        modifier = Modifier.testTag("filter_genus_all")
                    )

                    CloudGenus.entries.forEach { genus ->
                        val count = detectedFormations.count { it.genus == genus }
                        val isSelected = selectedFilterGenus == genus
                        FilterChip(
                            selected = isSelected,
                            onClick = { onFilterGenusChange(if (isSelected) null else genus) },
                            label = {
                                Text(
                                    "${genus.iconEmoji} ${genus.symbolCode} ($count)",
                                    fontSize = 11.sp,
                                    fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal
                                )
                            },
                            colors = FilterChipDefaults.filterChipColors(
                                selectedContainerColor = genus.badgeColor.copy(alpha = 0.35f),
                                selectedLabelColor = Color.White,
                                selectedLeadingIconColor = genus.badgeColor
                            ),
                            border = FilterChipDefaults.filterChipBorder(
                                enabled = true,
                                selected = isSelected,
                                borderColor = if (isSelected) genus.badgeColor else Color(0xFF334155)
                            ),
                            modifier = Modifier.testTag("filter_genus_${genus.name.lowercase()}")
                        )
                    }
                }
            }
        }

        // Card 2: Selected Cloud Detail Inspector (if one is selected)
        val selectedFormation = detectedFormations.find { it.id == selectedFormationId }
        AnimatedVisibility(visible = selectedFormation != null) {
            if (selectedFormation != null) {
                Card(
                    colors = CardDefaults.cardColors(containerColor = Color(0xFF161E2E)),
                    shape = RoundedCornerShape(12.dp),
                    border = CardDefaults.outlinedCardBorder().copy(
                        brush = Brush.linearGradient(
                            listOf(selectedFormation.genus.badgeColor, Color(0xFF0F172A))
                        )
                    ),
                    modifier = Modifier
                        .padding(bottom = 12.dp)
                        .testTag("selected_cloud_inspector_card")
                ) {
                    Column(modifier = Modifier.padding(14.dp)) {
                        Row(
                            modifier = Modifier.fillMaxWidth(),
                            horizontalArrangement = Arrangement.SpaceBetween,
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Row(verticalAlignment = Alignment.CenterVertically) {
                                Surface(
                                    color = selectedFormation.genus.badgeColor,
                                    shape = RoundedCornerShape(6.dp)
                                ) {
                                    Text(
                                        text = selectedFormation.genus.symbolCode,
                                        color = Color.White,
                                        fontWeight = FontWeight.Bold,
                                        fontSize = 11.sp,
                                        fontFamily = FontFamily.Monospace,
                                        modifier = Modifier.padding(horizontal = 6.dp, vertical = 2.dp)
                                    )
                                }
                                Spacer(modifier = Modifier.width(8.dp))
                                Column {
                                    Text(
                                        text = "${selectedFormation.genus.displayName} (${selectedFormation.speciesVariant})",
                                        style = MaterialTheme.typography.titleMedium.copy(fontWeight = FontWeight.Bold),
                                        color = Color.White
                                    )
                                    Text(
                                        text = selectedFormation.genus.commonName,
                                        style = MaterialTheme.typography.bodySmall,
                                        color = Color(0xFF94A3B8)
                                    )
                                }
                            }

                            Button(
                                onClick = {
                                    onFocusCamera(selectedFormation.azimuthDegrees, selectedFormation.elevationDegrees)
                                },
                                colors = ButtonDefaults.buttonColors(containerColor = selectedFormation.genus.badgeColor),
                                shape = RoundedCornerShape(8.dp),
                                modifier = Modifier.testTag("focus_camera_selected_cloud_button")
                            ) {
                                Icon(Icons.Default.CenterFocusStrong, contentDescription = null, modifier = Modifier.size(14.dp))
                                Spacer(modifier = Modifier.width(4.dp))
                                Text("Focus View", fontSize = 11.sp, fontWeight = FontWeight.Bold)
                            }
                        }

                        Spacer(modifier = Modifier.height(10.dp))
                        Text(
                            text = selectedFormation.genus.description,
                            style = MaterialTheme.typography.bodySmall,
                            color = Color(0xFFCBD5E1)
                        )

                        Spacer(modifier = Modifier.height(10.dp))

                        // Grid of physical telemetry
                        val metrics = listOf(
                            "Base Altitude" to "${selectedFormation.baseAltitudeMeters.toInt()} m",
                            "Top Ceiling" to "${selectedFormation.topAltitudeMeters.toInt()} m",
                            "Optical Depth (τ)" to "%.1f".format(selectedFormation.opticalDepthTau),
                            "Radar Reflectivity" to "%.1f dBZ".format(selectedFormation.radarReflectivityDbz),
                            "Updraft Velocity" to "%.1f m/s".format(selectedFormation.convectiveUpdraftMs),
                            "Ice Fraction" to "${(selectedFormation.iceCrystalFraction * 100).toInt()}%",
                            "Water Content" to "%.2f g/m³".format(selectedFormation.dropletWaterGramsPerM3),
                            "Confidence" to "%.1f%%".format(selectedFormation.confidenceScore * 100)
                        )

                        Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                            Column(modifier = Modifier.weight(1f)) {
                                metrics.take(4).forEach { (label, value) ->
                                    Row(
                                        modifier = Modifier.fillMaxWidth().padding(vertical = 2.dp),
                                        horizontalArrangement = Arrangement.SpaceBetween
                                    ) {
                                        Text(label, fontSize = 10.sp, color = Color(0xFF94A3B8))
                                        Text(value, fontSize = 10.sp, fontWeight = FontWeight.Bold, color = Color.White, fontFamily = FontFamily.Monospace)
                                    }
                                }
                            }
                            Column(modifier = Modifier.weight(1f)) {
                                metrics.drop(4).forEach { (label, value) ->
                                    Row(
                                        modifier = Modifier.fillMaxWidth().padding(vertical = 2.dp),
                                        horizontalArrangement = Arrangement.SpaceBetween
                                    ) {
                                        Text(label, fontSize = 10.sp, color = Color(0xFF94A3B8))
                                        Text(value, fontSize = 10.sp, fontWeight = FontWeight.Bold, color = Color.White, fontFamily = FontFamily.Monospace)
                                    }
                                }
                            }
                        }

                        Spacer(modifier = Modifier.height(8.dp))
                        Surface(
                            color = Color(0xFF0F172A),
                            shape = RoundedCornerShape(6.dp),
                            modifier = Modifier.fillMaxWidth()
                        ) {
                            Row(modifier = Modifier.padding(8.dp), verticalAlignment = Alignment.CenterVertically) {
                                Text("Precipitation Threat: ", fontSize = 10.sp, fontWeight = FontWeight.Bold, color = Color(0xFF94A3B8))
                                Text(selectedFormation.genus.typicalPrecipitation, fontSize = 10.sp, color = Color(0xFFFBBF24))
                            }
                        }
                    }
                }
            }
        }

        // Card 3: Detected Formations List (Click to focus / inspect)
        Card(
            colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.padding(bottom = 12.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text(
                    text = "Detected Formations in 360° Sky Dome",
                    style = MaterialTheme.typography.titleMedium.copy(fontWeight = FontWeight.Bold),
                    color = Color.White
                )
                Text(
                    text = "Tap any detected formation to highlight and center the celestial camera",
                    style = MaterialTheme.typography.bodySmall,
                    color = Color(0xFF94A3B8)
                )
                Spacer(modifier = Modifier.height(10.dp))

                val filteredFormations = if (selectedFilterGenus != null) {
                    detectedFormations.filter { it.genus == selectedFilterGenus }
                } else {
                    detectedFormations
                }

                filteredFormations.forEach { formation ->
                    val isSelected = formation.id == selectedFormationId

                    Surface(
                        shape = RoundedCornerShape(8.dp),
                        color = if (isSelected) Color(0xFF1E293B) else Color(0xFF0F172A),
                        border = if (isSelected) {
                            CardDefaults.outlinedCardBorder().copy(
                                brush = Brush.linearGradient(listOf(formation.genus.badgeColor, Color(0xFF38BDF8)))
                            )
                        } else null,
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(vertical = 4.dp)
                            .clickable {
                                onSelectFormation(formation)
                                onFocusCamera(formation.azimuthDegrees, formation.elevationDegrees)
                            }
                            .testTag("formation_item_${formation.id}")
                    ) {
                        Row(
                            modifier = Modifier
                                .fillMaxWidth()
                                .padding(10.dp),
                            horizontalArrangement = Arrangement.SpaceBetween,
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Row(verticalAlignment = Alignment.CenterVertically, modifier = Modifier.weight(1f)) {
                                Box(
                                    modifier = Modifier
                                        .size(36.dp)
                                        .background(formation.genus.badgeColor.copy(alpha = 0.20f), CircleShape)
                                        .border(1.dp, formation.genus.badgeColor, CircleShape),
                                    contentAlignment = Alignment.Center
                                ) {
                                    Text(
                                        text = formation.genus.symbolCode,
                                        fontSize = 11.sp,
                                        fontWeight = FontWeight.Bold,
                                        color = formation.genus.badgeColor,
                                        fontFamily = FontFamily.Monospace
                                    )
                                }

                                Spacer(modifier = Modifier.width(10.dp))

                                Column {
                                    Row(verticalAlignment = Alignment.CenterVertically) {
                                        Text(
                                            text = formation.genus.displayName,
                                            fontSize = 13.sp,
                                            fontWeight = FontWeight.Bold,
                                            color = Color.White
                                        )
                                        Spacer(modifier = Modifier.width(6.dp))
                                        Text(
                                            text = "${(formation.confidenceScore * 100).toInt()}% match",
                                            fontSize = 10.sp,
                                            color = Color(0xFF4ADE80),
                                            fontFamily = FontFamily.Monospace
                                        )
                                    }
                                    Text(
                                        text = "Az: ${formation.azimuthDegrees.toInt()}° | El: ${formation.elevationDegrees.toInt()}° • Alt: ${(formation.baseAltitudeMeters / 1000).toInt()}-${(formation.topAltitudeMeters / 1000).toInt()}km",
                                        fontSize = 10.sp,
                                        color = Color(0xFF94A3B8),
                                        fontFamily = FontFamily.Monospace
                                    )
                                }
                            }

                            IconButton(
                                onClick = {
                                    onSelectFormation(formation)
                                    onFocusCamera(formation.azimuthDegrees, formation.elevationDegrees)
                                },
                                modifier = Modifier.size(32.dp)
                            ) {
                                Icon(
                                    imageVector = Icons.Default.CenterFocusStrong,
                                    contentDescription = "Center on cloud",
                                    tint = if (isSelected) Color(0xFFFBBF24) else Color(0xFF64748B),
                                    modifier = Modifier.size(18.dp)
                                )
                            }
                        }
                    }
                }
            }
        }
    }
}
