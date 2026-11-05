package com.example.sphere.lighting.tabs

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.CenterFocusStrong
import androidx.compose.material.icons.filled.ControlCamera
import androidx.compose.material.icons.filled.Explore
import androidx.compose.material.icons.filled.WbTwilight
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import com.example.sphere.model.LightSourceParameters
import kotlin.math.atan2
import kotlin.math.cos
import kotlin.math.roundToInt
import kotlin.math.sin
import kotlin.math.sqrt

@Composable
fun LightGizmoPlacementTab(
    params: LightSourceParameters,
    onParamsChange: (LightSourceParameters) -> Unit,
    modifier: Modifier = Modifier
) {
    val scrollState = rememberScrollState()

    Column(
        modifier = modifier
            .fillMaxSize()
            .verticalScroll(scrollState)
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(16.dp)
    ) {
        // Position Card (X, Y, Z in world space)
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(10.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                        Icon(Icons.Default.ControlCamera, contentDescription = null, tint = MaterialTheme.colorScheme.primary)
                        Text("3D Light Position", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.Bold)
                    }
                    Text(
                        "(${formatFloat(params.posX)}, ${formatFloat(params.posY)}, ${formatFloat(params.posZ)}) m",
                        style = MaterialTheme.typography.bodySmall,
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.primary
                    )
                }

                // X axis
                Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                        Text("X (Lateral):", style = MaterialTheme.typography.bodySmall)
                        Text("${formatFloat(params.posX)} m", style = MaterialTheme.typography.bodySmall)
                    }
                    Slider(
                        value = params.posX,
                        onValueChange = { onParamsChange(params.copy(posX = it)) },
                        valueRange = -4f..4f,
                        modifier = Modifier.fillMaxWidth()
                    )
                }

                // Y axis (Height)
                Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                        Text("Y (Height above floor):", style = MaterialTheme.typography.bodySmall)
                        Text("${formatFloat(params.posY)} m", style = MaterialTheme.typography.bodySmall)
                    }
                    Slider(
                        value = params.posY,
                        onValueChange = { onParamsChange(params.copy(posY = it)) },
                        valueRange = 0.5f..5f,
                        modifier = Modifier.fillMaxWidth()
                    )
                }

                // Z axis (Depth)
                Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                        Text("Z (Forward / Backward):", style = MaterialTheme.typography.bodySmall)
                        Text("${formatFloat(params.posZ)} m", style = MaterialTheme.typography.bodySmall)
                    }
                    Slider(
                        value = params.posZ,
                        onValueChange = { onParamsChange(params.copy(posZ = it)) },
                        valueRange = -4f..4f,
                        modifier = Modifier.fillMaxWidth()
                    )
                }
            }
        }

        // Aim / Direction Card (Pitch and Yaw)
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(10.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                        Icon(Icons.Default.Explore, contentDescription = null, tint = MaterialTheme.colorScheme.primary)
                        Text("Beam Aim & Orientation", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.Bold)
                    }
                    Button(
                        onClick = {
                            // Point toward origin (0, 0, 0)
                            val dx = -params.posX
                            val dy = -params.posY
                            val dz = -params.posZ
                            onParamsChange(params.withDirection(dx, dy, dz))
                        },
                        contentPadding = PaddingValues(horizontal = 10.dp, vertical = 4.dp),
                        modifier = Modifier.height(32.dp)
                    ) {
                        Icon(Icons.Default.CenterFocusStrong, contentDescription = null, modifier = Modifier.size(16.dp))
                        Spacer(Modifier.width(4.dp))
                        Text("Aim at Center", style = MaterialTheme.typography.labelSmall)
                    }
                }

                // Yaw Slider
                Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                        Text("Horizontal Heading (Yaw):", style = MaterialTheme.typography.bodySmall)
                        Text("${params.yawDeg.roundToInt()}°", style = MaterialTheme.typography.bodySmall)
                    }
                    Slider(
                        value = params.yawDeg,
                        onValueChange = { newYaw ->
                            onParamsChange(params.copy(yawDeg = newYaw))
                        },
                        valueRange = 0f..360f,
                        modifier = Modifier.fillMaxWidth()
                    )
                }

                // Pitch Slider
                Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                        Text("Vertical Tilt (Pitch):", style = MaterialTheme.typography.bodySmall)
                        Text("${params.pitchDeg.roundToInt()}°", style = MaterialTheme.typography.bodySmall)
                    }
                    Slider(
                        value = params.pitchDeg,
                        onValueChange = { newPitch ->
                            onParamsChange(params.copy(pitchDeg = newPitch))
                        },
                        valueRange = -85f..85f,
                        modifier = Modifier.fillMaxWidth()
                    )
                }
            }
        }

        // Shadow & Depth Map Settings
        Card(
            shape = RoundedCornerShape(12.dp),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f))
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                        Icon(Icons.Default.WbTwilight, contentDescription = null, tint = MaterialTheme.colorScheme.primary)
                        Text("Vulkan Shadow Mapping", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.Bold)
                    }
                    Switch(
                        checked = params.castShadows,
                        onCheckedChange = { onParamsChange(params.copy(castShadows = it)) }
                    )
                }
                Text(
                    text = if (params.castShadows) "Enabled: Generating 2048x2048 PCF shadow depth map in Vulkan VkRenderPass." else "Disabled: Object cast shadows turned off.",
                    style = MaterialTheme.typography.labelSmall,
                    color = MaterialTheme.colorScheme.onSurfaceVariant
                )
            }
        }
    }
}

private fun formatFloat(v: Float): String {
    return ((v * 10f).roundToInt() / 10f).toString()
}
