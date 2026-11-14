package com.example.sphere.trailers.tabs

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.AxleConfigType
import com.example.sphere.model.BrakeSystemType
import com.example.sphere.model.SuspensionType
import com.example.sphere.model.TrailerItem
import kotlin.math.roundToInt

@Composable
fun TrailerRunningGearTab(
    trailer: TrailerItem,
    onTrailerChange: (TrailerItem) -> Unit,
    modifier: Modifier = Modifier
) {
    Column(
        modifier = modifier
            .fillMaxWidth()
            .padding(14.dp)
            .testTag("trailer_running_gear_tab")
    ) {
        Text(
            "Shared Running Gear (gen/trailers/running_gear)",
            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
            color = MaterialTheme.colorScheme.primary
        )
        Spacer(Modifier.height(10.dp))

        // Axle Configuration
        Text("Axle Bogie Configuration", style = MaterialTheme.typography.labelMedium, fontWeight = FontWeight.Bold)
        Spacer(Modifier.height(6.dp))
        Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(6.dp)) {
            AxleConfigType.entries.forEach { cfg ->
                val isSelected = trailer.axleConfig == cfg
                FilledTonalButton(
                    onClick = { onTrailerChange(trailer.copy(axleConfig = cfg)) },
                    modifier = Modifier.weight(1f),
                    colors = ButtonDefaults.filledTonalButtonColors(
                        containerColor = if (isSelected) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.surfaceVariant,
                        contentColor = if (isSelected) MaterialTheme.colorScheme.onPrimary else MaterialTheme.colorScheme.onSurfaceVariant
                    ),
                    contentPadding = PaddingValues(horizontal = 2.dp, vertical = 6.dp)
                ) {
                    Text(cfg.displayName.substringBefore(" "), fontSize = 10.sp)
                }
            }
        }

        Spacer(Modifier.height(14.dp))

        // Suspension & Brakes
        Card(
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerLow),
            shape = RoundedCornerShape(12.dp)
        ) {
            Column(modifier = Modifier.padding(12.dp)) {
                Text("Suspension Type", style = MaterialTheme.typography.labelMedium, fontWeight = FontWeight.Bold)
                Spacer(Modifier.height(6.dp))
                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                    SuspensionType.entries.forEach { susp ->
                        FilterChip(
                            selected = trailer.suspensionType == susp,
                            onClick = { onTrailerChange(trailer.copy(suspensionType = susp)) },
                            label = { Text(susp.displayName, fontSize = 10.sp) }
                        )
                    }
                }

                Spacer(Modifier.height(10.dp))
                Text("Braking & ABS Hardware", style = MaterialTheme.typography.labelMedium, fontWeight = FontWeight.Bold)
                Spacer(Modifier.height(6.dp))
                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                    BrakeSystemType.entries.forEach { brk ->
                        FilterChip(
                            selected = trailer.brakeType == brk,
                            onClick = { onTrailerChange(trailer.copy(brakeType = brk)) },
                            label = { Text(brk.displayName, fontSize = 10.sp) }
                        )
                    }
                }
            }
        }

        Spacer(Modifier.height(14.dp))

        // Lift & Steerable Axles
        Card(
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerLow),
            shape = RoundedCornerShape(12.dp)
        ) {
            Column(modifier = Modifier.padding(12.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column(modifier = Modifier.weight(1f)) {
                        Text("Retractable Front Lift Axle", fontWeight = FontWeight.SemiBold, fontSize = 13.sp)
                        Text("Lifts front axle clear of pavement on empty return runs", style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.onSurfaceVariant)
                    }
                    Switch(
                        checked = trailer.hasLiftAxle && trailer.liftAxleRaised,
                        onCheckedChange = { raised ->
                            onTrailerChange(trailer.copy(hasLiftAxle = true, liftAxleRaised = raised))
                        }
                    )
                }

                Spacer(Modifier.height(8.dp))
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Column(modifier = Modifier.weight(1f)) {
                        Text("Self-Steering Rear Axle", fontWeight = FontWeight.SemiBold, fontSize = 13.sp)
                        Text("Turns rear bogie wheels to track tight cornering", style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.onSurfaceVariant)
                    }
                    Switch(
                        checked = trailer.hasSteerableAxle,
                        onCheckedChange = { enabled ->
                            onTrailerChange(trailer.copy(hasSteerableAxle = enabled))
                        }
                    )
                }

                if (trailer.hasSteerableAxle) {
                    Spacer(Modifier.height(8.dp))
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                        Text("Rear Axle Steer Angle", style = MaterialTheme.typography.labelSmall)
                        Text("${trailer.steerAngleDeg.roundToInt()}°", fontFamily = FontFamily.Monospace, fontSize = 11.sp, color = MaterialTheme.colorScheme.primary)
                    }
                    Slider(
                        value = trailer.steerAngleDeg,
                        onValueChange = { onTrailerChange(trailer.copy(steerAngleDeg = it)) },
                        valueRange = -28f..28f
                    )
                }
            }
        }
    }
}
