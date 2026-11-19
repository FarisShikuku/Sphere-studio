package com.example.sphere.vehicles

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.gestures.awaitEachGesture
import androidx.compose.foundation.gestures.awaitFirstDown
import androidx.compose.foundation.gestures.waitForUpOrCancellation
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.outlined.VolumeOff
import androidx.compose.material.icons.automirrored.outlined.VolumeUp
import androidx.compose.material.icons.outlined.*
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.rememberUpdatedState
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.VehicleCategoryType
import com.example.sphere.model.VehicleItem
import kotlin.math.roundToInt

@Composable
fun VehicleDriveConsole(
    vehicle: VehicleItem,
    audioController: VehicleAudioController,
    streamingStats: VehicleStreamingStats,
    modifier: Modifier = Modifier
) {
    val isMinibus = vehicle.category == VehicleCategoryType.BUS_MINIBUS
    val ignitionStatusLabel = when (audioController.ignitionState) {
        1 -> "CRANKING"
        2 -> "RUNNING"
        else -> "OFF"
    }
    val gearLabel = if (audioController.gear <= 0) "N" else "D${audioController.gear}"

    Card(
        colors = CardDefaults.cardColors(
            containerColor = MaterialTheme.colorScheme.surfaceContainerLow
        ),
        border = BorderStroke(1.dp, MaterialTheme.colorScheme.outlineVariant),
        shape = RoundedCornerShape(16.dp),
        modifier = modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp)
            .testTag("vehicle_drive_console_card")
    ) {
        Column(
            modifier = Modifier.padding(12.dp),
            verticalArrangement = Arrangement.spacedBy(10.dp)
        ) {
            // Header + Digital Instrument Telemetry + Mute
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Column(modifier = Modifier.weight(1f)) {
                    Row(
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        Text(
                            text = "Powertrain & Audio Console",
                            style = MaterialTheme.typography.titleSmall,
                            fontWeight = FontWeight.Bold,
                            maxLines = 1,
                            overflow = TextOverflow.Ellipsis
                        )
                        Surface(
                            shape = RoundedCornerShape(8.dp),
                            color = when (audioController.ignitionState) {
                                2 -> MaterialTheme.colorScheme.primaryContainer
                                1 -> MaterialTheme.colorScheme.tertiaryContainer
                                else -> MaterialTheme.colorScheme.surfaceContainerHighest
                            }
                        ) {
                            Text(
                                text = ignitionStatusLabel,
                                style = MaterialTheme.typography.labelSmall,
                                fontFamily = FontFamily.Monospace,
                                fontWeight = FontWeight.Bold,
                                color = when (audioController.ignitionState) {
                                    2 -> MaterialTheme.colorScheme.onPrimaryContainer
                                    1 -> MaterialTheme.colorScheme.onTertiaryContainer
                                    else -> MaterialTheme.colorScheme.onSurfaceVariant
                                },
                                modifier = Modifier.padding(horizontal = 6.dp, vertical = 2.dp)
                            )
                        }
                    }
                    Text(
                        text = "44.1kHz PCM • Grid (${streamingStats.gridCellX}, ${streamingStats.gridCellZ}) • ${streamingStats.heroMeshletCount} meshlets @ ≤${streamingStats.maxTrisPerMeshlet}Δ",
                        style = MaterialTheme.typography.labelSmall,
                        fontFamily = FontFamily.Monospace,
                        color = MaterialTheme.colorScheme.onSurfaceVariant,
                        maxLines = 1,
                        overflow = TextOverflow.Ellipsis
                    )
                }

                IconButton(
                    onClick = { audioController.toggleMute() },
                    modifier = Modifier
                        .minimumInteractiveComponentSize()
                        .testTag("vehicle_audio_mute_button")
                ) {
                    Icon(
                        imageVector = if (audioController.isMuted) {
                            Icons.AutoMirrored.Outlined.VolumeOff
                        } else {
                            Icons.AutoMirrored.Outlined.VolumeUp
                        },
                        contentDescription = if (audioController.isMuted) "Unmute Vehicle Audio" else "Mute Vehicle Audio",
                        tint = if (audioController.isMuted) {
                            MaterialTheme.colorScheme.error
                        } else {
                            MaterialTheme.colorScheme.primary
                        }
                    )
                }
            }

            // Live Tachometer / Speedometer / Gear / Boost Strip
            Surface(
                shape = RoundedCornerShape(12.dp),
                color = MaterialTheme.colorScheme.surfaceContainer,
                modifier = Modifier.fillMaxWidth()
            ) {
                Column(
                    modifier = Modifier.padding(horizontal = 12.dp, vertical = 8.dp),
                    verticalArrangement = Arrangement.spacedBy(6.dp)
                ) {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        TelemetryReadoutCell(
                            label = "RPM",
                            value = "${audioController.rpm.roundToInt()}",
                            modifier = Modifier.testTag("telemetry_rpm_text")
                        )
                        TelemetryReadoutCell(
                            label = "SPEED",
                            value = "${audioController.speedKmh.roundToInt()} km/h",
                            modifier = Modifier.testTag("telemetry_speed_text")
                        )
                        TelemetryReadoutCell(
                            label = "GEAR",
                            value = gearLabel,
                            modifier = Modifier.testTag("telemetry_gear_text")
                        )
                        TelemetryReadoutCell(
                            label = "BOOST",
                            value = "${String.format("%.1f", audioController.turboBoostBar)}b",
                            modifier = Modifier.testTag("telemetry_boost_text")
                        )
                        TelemetryReadoutCell(
                            label = "DIST",
                            value = "${audioController.distanceM.roundToInt()}m",
                            modifier = Modifier.testTag("telemetry_distance_text")
                        )
                    }

                    LinearProgressIndicator(
                        progress = { audioController.normalizedRpm },
                        modifier = Modifier
                            .fillMaxWidth()
                            .height(6.dp),
                        color = if (audioController.normalizedRpm > 0.82f) {
                            MaterialTheme.colorScheme.error
                        } else {
                            MaterialTheme.colorScheme.primary
                        },
                        trackColor = MaterialTheme.colorScheme.surfaceContainerHighest
                    )
                }
            }

            // Row 1: Ignition, Horn, Auto-Cruise
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                Button(
                    onClick = { audioController.toggleIgnition() },
                    colors = ButtonDefaults.buttonColors(
                        containerColor = if (audioController.isEngineOn) {
                            MaterialTheme.colorScheme.errorContainer
                        } else {
                            MaterialTheme.colorScheme.primary
                        },
                        contentColor = if (audioController.isEngineOn) {
                            MaterialTheme.colorScheme.onErrorContainer
                        } else {
                            MaterialTheme.colorScheme.onPrimary
                        }
                    ),
                    shape = RoundedCornerShape(12.dp),
                    contentPadding = PaddingValues(horizontal = 10.dp, vertical = 10.dp),
                    modifier = Modifier
                        .weight(1.25f)
                        .defaultMinSize(minHeight = 48.dp)
                        .testTag("vehicle_ignition_button")
                ) {
                    Icon(
                        imageVector = Icons.Outlined.PowerSettingsNew,
                        contentDescription = if (audioController.isEngineOn) "Stop Engine" else "Start Engine",
                        modifier = Modifier.size(18.dp)
                    )
                    Spacer(Modifier.width(6.dp))
                    Text(
                        text = if (audioController.isEngineOn) "Stop Engine" else "Start Engine",
                        fontWeight = FontWeight.SemiBold,
                        maxLines = 1,
                        overflow = TextOverflow.Ellipsis
                    )
                }

                FilledTonalButton(
                    onClick = { audioController.triggerHorn(isMinibus) },
                    shape = RoundedCornerShape(12.dp),
                    contentPadding = PaddingValues(horizontal = 10.dp, vertical = 10.dp),
                    modifier = Modifier
                        .weight(0.9f)
                        .defaultMinSize(minHeight = 48.dp)
                        .testTag("vehicle_horn_button")
                ) {
                    Icon(
                        imageVector = Icons.Outlined.Campaign,
                        contentDescription = "Sound Vehicle Horn",
                        modifier = Modifier.size(18.dp)
                    )
                    Spacer(Modifier.width(5.dp))
                    Text("Horn", fontWeight = FontWeight.SemiBold, maxLines = 1)
                }

                FilledTonalButton(
                    onClick = { audioController.toggleCruise() },
                    colors = ButtonDefaults.filledTonalButtonColors(
                        containerColor = if (audioController.cruiseDrive) {
                            MaterialTheme.colorScheme.secondaryContainer
                        } else {
                            MaterialTheme.colorScheme.surfaceContainerHighest
                        }
                    ),
                    shape = RoundedCornerShape(12.dp),
                    contentPadding = PaddingValues(horizontal = 10.dp, vertical = 10.dp),
                    modifier = Modifier
                        .weight(0.95f)
                        .defaultMinSize(minHeight = 48.dp)
                        .testTag("vehicle_cruise_button")
                ) {
                    Icon(
                        imageVector = Icons.Outlined.Speed,
                        contentDescription = "Toggle Cruise Drive",
                        modifier = Modifier.size(18.dp)
                    )
                    Spacer(Modifier.width(5.dp))
                    Text(
                        text = if (audioController.cruiseDrive) "Cruising" else "Cruise",
                        fontWeight = FontWeight.SemiBold,
                        maxLines = 1
                    )
                }
            }

            // Row 2: Holdable Foot Pedals (Brake & Gas)
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.spacedBy(10.dp)
            ) {
                HoldablePedalButton(
                    label = if (audioController.brakePressed) "Braking (Air)" else "Hold Brake",
                    icon = Icons.Outlined.PauseCircle,
                    isPressed = audioController.brakePressed,
                    isPrimaryPedal = false,
                    onPressedChange = { audioController.setBrake(it) },
                    modifier = Modifier
                        .weight(1f)
                        .testTag("vehicle_brake_pedal")
                )

                HoldablePedalButton(
                    label = if (audioController.throttlePressed) "Accelerating" else "Hold Gas",
                    icon = Icons.Outlined.LocalGasStation,
                    isPressed = audioController.throttlePressed,
                    isPrimaryPedal = true,
                    onPressedChange = { down ->
                        if (down && !audioController.isEngineOn) {
                            audioController.setIgnition(true)
                        }
                        audioController.setThrottle(down)
                    },
                    modifier = Modifier
                        .weight(1.2f)
                        .testTag("vehicle_gas_pedal")
                )
            }
        }
    }
}

@Composable
private fun TelemetryReadoutCell(
    label: String,
    value: String,
    modifier: Modifier = Modifier
) {
    Column(
        horizontalAlignment = Alignment.CenterHorizontally,
        modifier = modifier
    ) {
        Text(
            text = label,
            style = MaterialTheme.typography.labelSmall,
            fontSize = 10.sp,
            color = MaterialTheme.colorScheme.onSurfaceVariant
        )
        Text(
            text = value,
            style = MaterialTheme.typography.labelMedium,
            fontFamily = FontFamily.Monospace,
            fontWeight = FontWeight.Bold,
            color = MaterialTheme.colorScheme.onSurface
        )
    }
}

@Composable
private fun HoldablePedalButton(
    label: String,
    icon: androidx.compose.ui.graphics.vector.ImageVector,
    isPressed: Boolean,
    isPrimaryPedal: Boolean,
    onPressedChange: (Boolean) -> Unit,
    modifier: Modifier = Modifier
) {
    val latestOnPressedChange by rememberUpdatedState(onPressedChange)
    val containerColor = when {
        isPressed && isPrimaryPedal -> MaterialTheme.colorScheme.primary
        isPressed && !isPrimaryPedal -> MaterialTheme.colorScheme.error
        else -> MaterialTheme.colorScheme.surfaceContainerHighest
    }
    val contentColor = when {
        isPressed && isPrimaryPedal -> MaterialTheme.colorScheme.onPrimary
        isPressed && !isPrimaryPedal -> MaterialTheme.colorScheme.onError
        else -> MaterialTheme.colorScheme.onSurface
    }

    Surface(
        shape = RoundedCornerShape(12.dp),
        color = containerColor,
        contentColor = contentColor,
        border = BorderStroke(
            width = if (isPressed) 2.dp else 1.dp,
            color = if (isPressed) contentColor.copy(alpha = 0.7f) else MaterialTheme.colorScheme.outline
        ),
        modifier = modifier
            .defaultMinSize(minHeight = 52.dp)
            .pointerInput(Unit) {
                awaitEachGesture {
                    awaitFirstDown(requireUnconsumed = false)
                    latestOnPressedChange(true)
                    try {
                        waitForUpOrCancellation()
                    } finally {
                        latestOnPressedChange(false)
                    }
                }
            }
    ) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(horizontal = 12.dp, vertical = 12.dp),
            horizontalArrangement = Arrangement.Center,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Icon(
                imageVector = icon,
                contentDescription = label,
                modifier = Modifier.size(18.dp)
            )
            Spacer(Modifier.width(8.dp))
            Text(
                text = label,
                style = MaterialTheme.typography.labelLarge,
                fontWeight = FontWeight.Bold,
                maxLines = 1,
                overflow = TextOverflow.Ellipsis
            )
        }
    }
}
