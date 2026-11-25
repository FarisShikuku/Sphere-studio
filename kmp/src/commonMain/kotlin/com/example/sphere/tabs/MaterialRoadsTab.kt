package com.example.sphere.tabs

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.CheckCircle
import androidx.compose.material.icons.filled.EditRoad
import androidx.compose.material.icons.filled.Layers
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.PbrMaterial
import com.example.sphere.texture.rememberDecodedKtx2Texture

private enum class RoadTypeFilter(val label: String) {
    ALL("All Roads"),
    HIGHWAY("Highway"),
    TWO_LANE("Two-Lane"),
    URBAN("Urban & Cracked"),
    ASPHALT("Asphalt Base")
}

@Composable
fun MaterialRoadsTab(
    materials: List<PbrMaterial>,
    activeMaterial: PbrMaterial,
    onSelectRoad: (PbrMaterial) -> Unit,
    modifier: Modifier = Modifier
) {
    var selectedFilter by remember { mutableStateOf(RoadTypeFilter.ALL) }

    val allRoadMaterials = remember(materials) {
        materials.filter {
            it.category == "Roads" ||
                it.id.contains("road") ||
                it.id.contains("asphalt")
        }
    }

    val filteredRoads = remember(allRoadMaterials, selectedFilter) {
        when (selectedFilter) {
            RoadTypeFilter.ALL -> allRoadMaterials
            RoadTypeFilter.HIGHWAY -> allRoadMaterials.filter {
                it.name.contains("Highway", ignoreCase = true) ||
                    (it.roadType?.contains("Highway", ignoreCase = true) == true)
            }
            RoadTypeFilter.TWO_LANE -> allRoadMaterials.filter {
                it.name.contains("Two-Lane", ignoreCase = true) ||
                    it.name.contains("Two-Way", ignoreCase = true) ||
                    (it.laneCount?.contains("2", ignoreCase = true) == true)
            }
            RoadTypeFilter.URBAN -> allRoadMaterials.filter {
                it.name.contains("Urban", ignoreCase = true) ||
                    it.name.contains("Double Yellow", ignoreCase = true) ||
                    (it.roadType?.contains("Urban", ignoreCase = true) == true)
            }
            RoadTypeFilter.ASPHALT -> allRoadMaterials.filter {
                it.name.contains("Asphalt", ignoreCase = true) ||
                    it.name.contains("Unmarked", ignoreCase = true)
            }
        }
    }

    Column(
        modifier = modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(12.dp)
            .testTag("material_roads_tab"),
        verticalArrangement = Arrangement.spacedBy(10.dp)
    ) {
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(
                containerColor = MaterialTheme.colorScheme.surfaceContainerHighest.copy(alpha = 0.75f)
            ),
            border = BorderStroke(1.dp, MaterialTheme.colorScheme.primary.copy(alpha = 0.45f))
        ) {
            Column(
                modifier = Modifier.padding(12.dp),
                verticalArrangement = Arrangement.spacedBy(6.dp)
            ) {
                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    Icon(
                        imageVector = Icons.Default.EditRoad,
                        contentDescription = null,
                        tint = MaterialTheme.colorScheme.primary,
                        modifier = Modifier.size(20.dp)
                    )
                    Column {
                        Text(
                            "Khronos KTX 2.0 Roadway & Highway Surfaces",
                            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold)
                        )
                        Text(
                            "${allRoadMaterials.size} Road Presets • Fixed PBR size (${"4.0m–7.5m"}) • Auto-repeats across larger surfaces",
                            style = MaterialTheme.typography.labelSmall.copy(
                                color = MaterialTheme.colorScheme.primary,
                                fontSize = 10.sp
                            )
                        )
                    }
                }

                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .horizontalScroll(rememberScrollState()),
                    horizontalArrangement = Arrangement.spacedBy(6.dp)
                ) {
                    RoadTypeFilter.entries.forEach { filter ->
                        val isFilterSelected = selectedFilter == filter
                        FilterChip(
                            selected = isFilterSelected,
                            onClick = { selectedFilter = filter },
                            label = {
                                Text(
                                    filter.label,
                                    fontSize = 11.sp,
                                    fontWeight = if (isFilterSelected) FontWeight.Bold else FontWeight.Normal
                                )
                            },
                            modifier = Modifier.testTag("road_filter_${filter.name.lowercase()}"),
                            colors = FilterChipDefaults.filterChipColors(
                                selectedContainerColor = MaterialTheme.colorScheme.primaryContainer,
                                selectedLabelColor = MaterialTheme.colorScheme.onPrimaryContainer
                            )
                        )
                    }
                }
            }
        }

        filteredRoads.forEach { roadMat ->
            val isSelected = roadMat.id == activeMaterial.id
            val decodedKtx2 = rememberDecodedKtx2Texture(roadMat.assetPath)
            val kvd = decodedKtx2?.info?.kvdMetadata.orEmpty()
            val roadTypeLabel = kvd["KTXroadType"] ?: roadMat.roadType ?: "Asphalt Pavement"
            val rawLanes = kvd["KTXlaneCount"] ?: roadMat.laneCount ?: "Surface"
            val lanesLabel = when {
                rawLanes.contains("Lane", ignoreCase = true) -> rawLanes
                rawLanes == "1" -> "1 Lane"
                else -> "$rawLanes Lanes"
            }
            val markingsLabel = kvd["KTXmarkings"] ?: roadMat.roadMarkings ?: roadMat.description
            val widthMeters = kvd["KTXroadWidthM"] ?: "%.2f".format(roadMat.tileScaleMeters)
            val specLine = decodedKtx2?.let {
                "${it.info.vkFormatName} • ${it.info.pixelWidth}×${it.info.pixelHeight} • ${it.info.levelCount} Mips • Slot #${roadMat.bindlessAlbedoSlot}"
            } ?: "${roadMat.albedoFormat} • Slot #${roadMat.bindlessAlbedoSlot}"

            Card(
                modifier = Modifier
                    .fillMaxWidth()
                    .clickable { onSelectRoad(roadMat) }
                    .testTag("road_card_${roadMat.id}"),
                shape = RoundedCornerShape(12.dp),
                colors = CardDefaults.cardColors(
                    containerColor = if (isSelected) {
                        MaterialTheme.colorScheme.primaryContainer.copy(alpha = 0.35f)
                    } else {
                        MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.45f)
                    }
                ),
                border = BorderStroke(
                    if (isSelected) 1.5.dp else 1.dp,
                    if (isSelected) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.outlineVariant.copy(alpha = 0.5f)
                )
            ) {
                Row(
                    modifier = Modifier.padding(10.dp),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Box(
                        modifier = Modifier
                            .size(68.dp)
                            .clip(RoundedCornerShape(8.dp))
                            .background(roadMat.baseColor)
                    ) {
                        if (decodedKtx2 != null) {
                            Image(
                                bitmap = decodedKtx2.albedoImageBitmap,
                                contentDescription = "${roadMat.name} KTX2 road preview",
                                contentScale = ContentScale.Crop,
                                modifier = Modifier.fillMaxSize()
                            )
                        } else {
                            Icon(
                                imageVector = Icons.Default.Layers,
                                contentDescription = "${roadMat.name} preview",
                                tint = Color.White.copy(alpha = 0.65f),
                                modifier = Modifier
                                    .size(24.dp)
                                    .align(Alignment.Center)
                            )
                        }

                        if (isSelected) {
                            Icon(
                                imageVector = Icons.Default.CheckCircle,
                                contentDescription = "Active road texture",
                                tint = MaterialTheme.colorScheme.primary,
                                modifier = Modifier
                                    .size(20.dp)
                                    .align(Alignment.TopEnd)
                                    .padding(3.dp)
                            )
                        }
                    }

                    Spacer(Modifier.width(10.dp))

                    Column(
                        modifier = Modifier.weight(1f),
                        verticalArrangement = Arrangement.spacedBy(3.dp)
                    ) {
                        Text(
                            text = roadMat.name,
                            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                            maxLines = 1
                        )

                        Row(
                            horizontalArrangement = Arrangement.spacedBy(5.dp),
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Surface(
                                color = MaterialTheme.colorScheme.secondaryContainer,
                                shape = RoundedCornerShape(4.dp)
                            ) {
                                Text(
                                    text = roadTypeLabel,
                                    modifier = Modifier.padding(horizontal = 5.dp, vertical = 1.dp),
                                    style = MaterialTheme.typography.labelSmall.copy(
                                        fontSize = 9.sp,
                                        fontWeight = FontWeight.SemiBold
                                    )
                                )
                            }
                            Surface(
                                color = MaterialTheme.colorScheme.tertiaryContainer,
                                shape = RoundedCornerShape(4.dp)
                            ) {
                                Text(
                                    text = "$lanesLabel • ${widthMeters}m",
                                    modifier = Modifier.padding(horizontal = 5.dp, vertical = 1.dp),
                                    style = MaterialTheme.typography.labelSmall.copy(fontSize = 9.sp)
                                )
                            }
                        }

                        Text(
                            text = markingsLabel,
                            style = MaterialTheme.typography.bodySmall.copy(
                                color = MaterialTheme.colorScheme.onSurfaceVariant,
                                fontSize = 11.sp
                            ),
                            maxLines = 2
                        )

                        Text(
                            text = specLine,
                            style = MaterialTheme.typography.labelSmall.copy(
                                fontFamily = FontFamily.Monospace,
                                fontSize = 9.sp,
                                color = Color(0xFF38BDF8)
                            ),
                            maxLines = 1
                        )
                    }

                    Spacer(Modifier.width(8.dp))

                    FilledTonalButton(
                        onClick = { onSelectRoad(roadMat) },
                        colors = ButtonDefaults.filledTonalButtonColors(
                            containerColor = if (isSelected) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.primaryContainer,
                            contentColor = if (isSelected) MaterialTheme.colorScheme.onPrimary else MaterialTheme.colorScheme.onPrimaryContainer
                        ),
                        contentPadding = PaddingValues(horizontal = 10.dp, vertical = 6.dp),
                        modifier = Modifier
                            .heightIn(min = 48.dp)
                            .testTag("apply_road_${roadMat.id}")
                    ) {
                        Text(
                            text = if (isSelected) "Applied" else "Apply",
                            fontSize = 11.sp,
                            fontWeight = FontWeight.SemiBold
                        )
                    }
                }
            }
        }
    }
}
