package com.example.sphere.water.tabs

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.CropSquare
import androidx.compose.material.icons.filled.Cyclone
import androidx.compose.material.icons.filled.RadioButtonChecked
import androidx.compose.material.icons.filled.ViewInAr
import androidx.compose.material.icons.filled.Waves
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.unit.dp
import com.example.sphere.model.WaterCameraState
import com.example.sphere.model.WaterTankState
import kotlin.math.roundToInt

@Composable
fun WaterTankTab(
    tank: WaterTankState,
    camera: WaterCameraState,
    onTankChange: (WaterTankState) -> Unit,
    onCameraChange: (WaterCameraState) -> Unit,
    onTriggerPulse: (Int) -> Unit
) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(horizontal = 14.dp, vertical = 10.dp)
    ) {
        // Container Bounding Box & Rendering Guides
        WaterSectionCard(
            title = "Simulation Container Bounds",
            subtitle = "AABB domain boundaries enforced during sph_integrate.comp",
            icon = Icons.Default.CropSquare
        ) {
            WaterLabeledSlider(
                label = "Tank Width (X)",
                value = tank.boundsWidth,
                valueRange = 1.6f..4.6f,
                formattedValue = "${"%.2f".format(tank.boundsWidth)} m",
                onValueChange = { onTankChange(tank.copy(boundsWidth = it)) },
                testTag = "water_tank_width_slider"
            )
            WaterLabeledSlider(
                label = "Tank Height (Y)",
                value = tank.boundsHeight,
                valueRange = 1.4f..3.6f,
                formattedValue = "${"%.2f".format(tank.boundsHeight)} m",
                onValueChange = { onTankChange(tank.copy(boundsHeight = it)) },
                testTag = "water_tank_height_slider"
            )
            WaterLabeledSlider(
                label = "Tank Depth (Z)",
                value = tank.boundsDepth,
                valueRange = 1.4f..4.2f,
                formattedValue = "${"%.2f".format(tank.boundsDepth)} m",
                onValueChange = { onTankChange(tank.copy(boundsDepth = it)) },
                testTag = "water_tank_depth_slider"
            )

            WaterToggleRow(
                title = "Container Wireframe Box",
                subtitle = "Draws container_box.vert/frag bounds outline",
                checked = tank.showContainerBox,
                onCheckedChange = { onTankChange(tank.copy(showContainerBox = it)) },
                testTag = "water_show_container_switch"
            )
            WaterToggleRow(
                title = "Caustic Pool Floor Grid",
                subtitle = "Renders refracted pool floor plane & metric grid",
                checked = tank.showFloorGrid,
                onCheckedChange = { onTankChange(tank.copy(showFloorGrid = it)) },
                testTag = "water_show_floor_grid_switch"
            )
        }

        // Wave Maker Paddle
        WaterSectionCard(
            title = "Oscillating Wave-Maker Wall",
            subtitle = "Dynamic sinusoidal left-wall piston driving standing & breaking waves",
            icon = Icons.Default.Waves
        ) {
            WaterToggleRow(
                title = "Enable Wave Maker",
                subtitle = "Modulates -X boundary wall with harmonic motion",
                checked = tank.waveMakerEnabled,
                onCheckedChange = { onTankChange(tank.copy(waveMakerEnabled = it)) },
                testTag = "water_wave_maker_switch"
            )
            if (tank.waveMakerEnabled) {
                WaterLabeledSlider(
                    label = "Wave Frequency",
                    value = tank.waveFrequency,
                    valueRange = 0.4f..3.0f,
                    formattedValue = "${"%.2f".format(tank.waveFrequency)} Hz",
                    onValueChange = { onTankChange(tank.copy(waveFrequency = it)) },
                    testTag = "water_wave_frequency_slider"
                )
                WaterLabeledSlider(
                    label = "Paddle Stroke Amplitude",
                    value = tank.waveAmplitude,
                    valueRange = 0.05f..0.75f,
                    formattedValue = "${"%.2f".format(tank.waveAmplitude)} m",
                    onValueChange = { onTankChange(tank.copy(waveAmplitude = it)) },
                    testTag = "water_wave_amplitude_slider"
                )
            }
        }

        // Submerged Sphere Obstacle
        WaterSectionCard(
            title = "Submerged Rigid Sphere Obstacle",
            subtitle = "Analytical raycasted sphere with SPH velocity-reflection contact",
            icon = Icons.Default.RadioButtonChecked
        ) {
            WaterToggleRow(
                title = "Enable Sphere Obstacle",
                subtitle = "Places a solid sphere inside the fluid basin",
                checked = tank.obstacleEnabled,
                onCheckedChange = { onTankChange(tank.copy(obstacleEnabled = it)) },
                testTag = "water_obstacle_switch"
            )
            if (tank.obstacleEnabled) {
                WaterLabeledSlider(
                    label = "Obstacle X Position",
                    value = tank.obstacleX,
                    valueRange = -1.2f..1.2f,
                    formattedValue = "${"%.2f".format(tank.obstacleX)} m",
                    onValueChange = { onTankChange(tank.copy(obstacleX = it)) },
                    testTag = "water_obstacle_x_slider"
                )
                WaterLabeledSlider(
                    label = "Obstacle Y Height",
                    value = tank.obstacleY,
                    valueRange = 0.20f..1.40f,
                    formattedValue = "${"%.2f".format(tank.obstacleY)} m",
                    onValueChange = { onTankChange(tank.copy(obstacleY = it)) },
                    testTag = "water_obstacle_y_slider"
                )
                WaterLabeledSlider(
                    label = "Obstacle Z Position",
                    value = tank.obstacleZ,
                    valueRange = -1.0f..1.0f,
                    formattedValue = "${"%.2f".format(tank.obstacleZ)} m",
                    onValueChange = { onTankChange(tank.copy(obstacleZ = it)) },
                    testTag = "water_obstacle_z_slider"
                )
                WaterLabeledSlider(
                    label = "Obstacle Radius",
                    value = tank.obstacleRadius,
                    valueRange = 0.16f..0.65f,
                    formattedValue = "${"%.2f".format(tank.obstacleRadius)} m",
                    onValueChange = { onTankChange(tank.copy(obstacleRadius = it)) },
                    testTag = "water_obstacle_radius_slider"
                )
            }
        }

        // Interactive Fluid Impulses & Orbital Camera
        WaterSectionCard(
            title = "Fluid Impulses & Orbital Camera",
            subtitle = "Ray-casted fluid stirring & view frustum controls",
            icon = Icons.Default.Cyclone
        ) {
            WaterLabeledSlider(
                label = "Impulse / Stir Strength",
                value = tank.impulseStrength,
                valueRange = 0.5f..6.0f,
                formattedValue = "${"%.1f".format(tank.impulseStrength)}x",
                onValueChange = { onTankChange(tank.copy(impulseStrength = it)) },
                testTag = "water_impulse_strength_slider"
            )

            Row(
                modifier = Modifier.fillMaxWidth().padding(bottom = 10.dp),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                FilledTonalButton(
                    onClick = { onTriggerPulse(0) },
                    modifier = Modifier.weight(1f).height(48.dp).testTag("water_tab_plunge_btn")
                ) { Text("Geyser") }
                FilledTonalButton(
                    onClick = { onTriggerPulse(1) },
                    modifier = Modifier.weight(1f).height(48.dp).testTag("water_tab_vortex_btn")
                ) { Text("Vortex") }
                FilledTonalButton(
                    onClick = { onTriggerPulse(2) },
                    modifier = Modifier.weight(1f).height(48.dp).testTag("water_tab_slosh_btn")
                ) { Text("Slosh") }
            }

            HorizontalDivider(modifier = Modifier.padding(vertical = 6.dp))

            Row(
                verticalAlignment = androidx.compose.ui.Alignment.CenterVertically,
                horizontalArrangement = Arrangement.spacedBy(6.dp),
                modifier = Modifier.padding(vertical = 4.dp)
            ) {
                Icon(Icons.Default.ViewInAr, contentDescription = null, modifier = Modifier.size(16.dp))
                Text("Orbital Camera", style = MaterialTheme.typography.titleSmall)
            }

            WaterToggleRow(
                title = "Auto-Orbit Camera",
                subtitle = "Continuously orbits camera around fluid tank",
                checked = camera.autoOrbit,
                onCheckedChange = { onCameraChange(camera.copy(autoOrbit = it)) },
                testTag = "water_auto_orbit_switch"
            )
            WaterLabeledSlider(
                label = "Camera Yaw",
                value = camera.yawDeg,
                valueRange = -180f..180f,
                formattedValue = "${camera.yawDeg.roundToInt()}°",
                onValueChange = { onCameraChange(camera.copy(yawDeg = it)) },
                testTag = "water_camera_yaw_slider"
            )
            WaterLabeledSlider(
                label = "Camera Pitch",
                value = camera.pitchDeg,
                valueRange = -12f..82f,
                formattedValue = "${camera.pitchDeg.roundToInt()}°",
                onValueChange = { onCameraChange(camera.copy(pitchDeg = it)) },
                testTag = "water_camera_pitch_slider"
            )
            WaterLabeledSlider(
                label = "Camera Distance",
                value = camera.distance,
                valueRange = 2.0f..11.0f,
                formattedValue = "${"%.2f".format(camera.distance)} m",
                onValueChange = { onCameraChange(camera.copy(distance = it)) },
                testTag = "water_camera_distance_slider"
            )
        }
        Spacer(Modifier.height(20.dp))
    }
}
