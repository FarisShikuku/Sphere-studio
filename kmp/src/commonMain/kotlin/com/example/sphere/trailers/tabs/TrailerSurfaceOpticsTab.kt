package com.example.sphere.trailers.tabs

import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
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
import com.example.sphere.model.TrailerItem
import kotlin.math.roundToInt

@Composable
fun TrailerSurfaceOpticsTab(
    trailer: TrailerItem,
    onTrailerChange: (TrailerItem) -> Unit,
    modifier: Modifier = Modifier
) {
    val fleetSwatches = listOf(
        Color(0xFF1D4ED8), Color(0xFFDC2626), Color(0xFF15803D), Color(0xFFD97706),
        Color(0xFF334155), Color(0xFFE2E8F0), Color(0xFF111827), Color(0xFF0F766E)
    )

    Column(
        modifier = modifier
            .fillMaxWidth()
            .padding(14.dp)
            .testTag("trailer_surface_optics_tab")
    ) {
        Text(
            "Fleet Livery, PVC Translucency & PBR Optics",
            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
            color = MaterialTheme.colorScheme.primary
        )
        Spacer(Modifier.height(10.dp))

        Text("Primary Chassis / Body Livery", style = MaterialTheme.typography.labelMedium)
        Spacer(Modifier.height(8.dp))
        Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(10.dp)) {
            fleetSwatches.forEach { swatch ->
                val isSelected = trailer.baseColor == swatch
                Box(
                    modifier = Modifier
                        .size(34.dp)
                        .clip(CircleShape)
                        .background(swatch)
                        .border(if (isSelected) 3.dp else 1.dp, if (isSelected) MaterialTheme.colorScheme.primary else Color.Gray, CircleShape)
                        .clickable { onTrailerChange(trailer.copy(baseColor = swatch)) }
                )
            }
        }

        Spacer(Modifier.height(14.dp))

        Card(
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerLow),
            shape = RoundedCornerShape(12.dp)
        ) {
            Column(modifier = Modifier.padding(12.dp)) {
                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Surface Roughness (ORM.g)", style = MaterialTheme.typography.bodySmall)
                    Text("${(trailer.roughness * 100).roundToInt()}%", fontFamily = FontFamily.Monospace, fontSize = 11.sp)
                }
                Slider(value = trailer.roughness, onValueChange = { onTrailerChange(trailer.copy(roughness = it)) }, valueRange = 0.08f..0.95f)

                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Metallic Reflectance (ORM.b)", style = MaterialTheme.typography.bodySmall)
                    Text("${(trailer.metallic * 100).roundToInt()}%", fontFamily = FontFamily.Monospace, fontSize = 11.sp)
                }
                Slider(value = trailer.metallic, onValueChange = { onTrailerChange(trailer.copy(metallic = it)) }, valueRange = 0f..1f)

                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("PVC Curtain Opacity (Alpha)", style = MaterialTheme.typography.bodySmall)
                    Text("${(trailer.curtainOpacity * 100).roundToInt()}%", fontFamily = FontFamily.Monospace, fontSize = 11.sp)
                }
                Slider(value = trailer.curtainOpacity, onValueChange = { onTrailerChange(trailer.copy(curtainOpacity = it)) }, valueRange = 0.25f..1f)

                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text("Road Spray & Grime Wear", style = MaterialTheme.typography.bodySmall)
                    Text("${(trailer.wearAndTear * 100).roundToInt()}%", fontFamily = FontFamily.Monospace, fontSize = 11.sp)
                }
                Slider(value = trailer.wearAndTear, onValueChange = { onTrailerChange(trailer.copy(wearAndTear = it)) }, valueRange = 0f..1f)
            }
        }

        Spacer(Modifier.height(12.dp))

        OutlinedTextField(
            value = trailer.stencilLabel,
            onValueChange = { onTrailerChange(trailer.copy(stencilLabel = it)) },
            label = { Text("Fleet Markings / Hazmat Placard Text") },
            singleLine = true,
            modifier = Modifier.fillMaxWidth()
        )
    }
}
