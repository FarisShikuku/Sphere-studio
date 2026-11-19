package com.example.sphere.water.tabs

import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Grain
import androidx.compose.material.icons.filled.Refresh
import androidx.compose.material.icons.filled.Science
import androidx.compose.material.icons.filled.Speed
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import com.example.sphere.model.WaterPhysicsState
import com.example.sphere.model.WaterSpawnScenarioType
import kotlin.math.roundToInt

@Composable
fun WaterPhysicsTab(
    physics: WaterPhysicsState,
    onPhysicsChange: (WaterPhysicsState) -> Unit,
    onRespawnWithScenario: (WaterSpawnScenarioType, Int) -> Unit
) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(horizontal = 14.dp, vertical = 10.dp)
    ) {
        // Spawn Scenario & Particle Budget
        WaterSectionCard(
            title = "SPH Particle Emitter & Spawn Scenario",
            subtitle = "Initial spatial distribution & particle count for GPU bitonic hash grid",
            icon = Icons.Default.Grain
        ) {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .horizontalScroll(rememberScrollState())
                    .padding(bottom = 8.dp),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                WaterSpawnScenarioType.entries.forEach { scenario ->
                    FilterChip(
                        selected = physics.scenario == scenario,
                        onClick = { onRespawnWithScenario(scenario, physics.particleCount) },
                        label = { Text(scenario.label) },
                        modifier = Modifier.testTag("water_scenario_${scenario.name.lowercase()}")
                    )
                }
            }

            Text(
                text = physics.scenario.description,
                style = MaterialTheme.typography.bodySmall,
                color = MaterialTheme.colorScheme.onSurfaceVariant,
                modifier = Modifier.padding(bottom = 6.dp)
            )

            WaterLabeledSlider(
                label = "Particle Count (GPU SSBO Elements)",
                value = physics.particleCount.toFloat(),
                valueRange = 256f..3600f,
                formattedValue = "${physics.particleCount} particles",
                onValueChange = { count ->
                    val snapped = ((count / 64f).roundToInt() * 64).coerceIn(256, 3600)
                    onPhysicsChange(physics.copy(particleCount = snapped))
                },
                testTag = "water_particle_count_slider"
            )

            Spacer(Modifier.height(4.dp))
            FilledTonalButton(
                onClick = { onRespawnWithScenario(physics.scenario, physics.particleCount) },
                modifier = Modifier
                    .fillMaxWidth()
                    .height(48.dp)
                    .testTag("water_apply_particle_respawn_button")
            ) {
                Icon(Icons.Default.Refresh, contentDescription = null, modifier = Modifier.size(18.dp))
                Spacer(Modifier.width(8.dp))
                Text("Respawn ${physics.particleCount} Particles (${physics.scenario.label})", fontWeight = FontWeight.SemiBold)
            }
        }

        // SPH Hydrodynamics Kernel Parameters
        WaterSectionCard(
            title = "SPH Hydrodynamics & Double-Density Pressure",
            subtitle = "Spiky Pow2/Pow3 pressure kernels + Poly6 viscosity & surface tension",
            icon = Icons.Default.Science
        ) {
            WaterLabeledSlider(
                label = "Smoothing Radius (h)",
                value = physics.smoothingRadius,
                valueRange = 0.16f..0.52f,
                formattedValue = "${"%.2f".format(physics.smoothingRadius)} m",
                onValueChange = { onPhysicsChange(physics.copy(smoothingRadius = it)) },
                testTag = "water_smoothing_radius_slider"
            )
            WaterLabeledSlider(
                label = "Rest Density (ρ₀)",
                value = physics.targetDensity,
                valueRange = 4.0f..24.0f,
                formattedValue = "${"%.1f".format(physics.targetDensity)} kg/m³",
                onValueChange = { onPhysicsChange(physics.copy(targetDensity = it)) },
                testTag = "water_target_density_slider"
            )
            WaterLabeledSlider(
                label = "Pressure Stiffness (k)",
                value = physics.pressureMultiplier,
                valueRange = 8.0f..110.0f,
                formattedValue = "${"%.1f".format(physics.pressureMultiplier)}",
                onValueChange = { onPhysicsChange(physics.copy(pressureMultiplier = it)) },
                testTag = "water_pressure_slider"
            )
            WaterLabeledSlider(
                label = "Near-Pressure Stiffness (k_near)",
                value = physics.nearPressureMultiplier,
                valueRange = 2.0f..48.0f,
                formattedValue = "${"%.1f".format(physics.nearPressureMultiplier)}",
                onValueChange = { onPhysicsChange(physics.copy(nearPressureMultiplier = it)) },
                testTag = "water_near_pressure_slider"
            )
            WaterLabeledSlider(
                label = "Viscosity Strength (μ)",
                value = physics.viscosityStrength,
                valueRange = 0.0f..2.0f,
                formattedValue = "${"%.2f".format(physics.viscosityStrength)}",
                onValueChange = { onPhysicsChange(physics.copy(viscosityStrength = it)) },
                testTag = "water_viscosity_slider"
            )
            WaterLabeledSlider(
                label = "Surface Tension Cohesion (σ)",
                value = physics.surfaceTension,
                valueRange = 0.0f..2.0f,
                formattedValue = "${"%.2f".format(physics.surfaceTension)}",
                onValueChange = { onPhysicsChange(physics.copy(surfaceTension = it)) },
                testTag = "water_surface_tension_slider"
            )
            WaterLabeledSlider(
                label = "Boundary Collision Restitution",
                value = physics.collisionDamping,
                valueRange = 0.1f..0.98f,
                formattedValue = "${"%.2f".format(physics.collisionDamping)}",
                onValueChange = { onPhysicsChange(physics.copy(collisionDamping = it)) },
                testTag = "water_collision_damping_slider"
            )
        }

        // External Forces & Solver Integration
        WaterSectionCard(
            title = "Gravity Vector, Wind & Solver Substeps",
            subtitle = "Symplectic Euler integration & external force fields",
            icon = Icons.Default.Speed
        ) {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .horizontalScroll(rememberScrollState())
                    .padding(bottom = 8.dp),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                OutlinedButton(
                    onClick = { onPhysicsChange(physics.copy(gravityX = 0f, gravityY = -9.81f, gravityZ = 0f)) },
                    modifier = Modifier.height(48.dp).testTag("water_gravity_earth")
                ) { Text("Earth -9.81") }
                OutlinedButton(
                    onClick = { onPhysicsChange(physics.copy(gravityX = 0f, gravityY = -1.62f, gravityZ = 0f)) },
                    modifier = Modifier.height(48.dp).testTag("water_gravity_moon")
                ) { Text("Lunar -1.62") }
                OutlinedButton(
                    onClick = { onPhysicsChange(physics.copy(gravityX = 0f, gravityY = 0f, gravityZ = 0f)) },
                    modifier = Modifier.height(48.dp).testTag("water_gravity_zero")
                ) { Text("Zero-G 0.0") }
                OutlinedButton(
                    onClick = { onPhysicsChange(physics.copy(gravityX = 4.5f, gravityY = -8.5f, gravityZ = 0f)) },
                    modifier = Modifier.height(48.dp).testTag("water_gravity_tilted")
                ) { Text("Tilt +X") }
            }

            WaterLabeledSlider(
                label = "Gravity Y (Vertical)",
                value = physics.gravityY,
                valueRange = -20.0f..6.0f,
                formattedValue = "${"%.2f".format(physics.gravityY)} m/s²",
                onValueChange = { onPhysicsChange(physics.copy(gravityY = it)) },
                testTag = "water_gravity_y_slider"
            )
            WaterLabeledSlider(
                label = "Gravity X (Lateral Tilt)",
                value = physics.gravityX,
                valueRange = -12.0f..12.0f,
                formattedValue = "${"%.2f".format(physics.gravityX)} m/s²",
                onValueChange = { onPhysicsChange(physics.copy(gravityX = it)) },
                testTag = "water_gravity_x_slider"
            )
            WaterLabeledSlider(
                label = "Wind Shear X",
                value = physics.windForceX,
                valueRange = -6.0f..6.0f,
                formattedValue = "${"%.1f".format(physics.windForceX)} m/s²",
                onValueChange = { onPhysicsChange(physics.copy(windForceX = it)) },
                testTag = "water_wind_x_slider"
            )
            WaterLabeledSlider(
                label = "Wind Shear Z",
                value = physics.windForceZ,
                valueRange = -6.0f..6.0f,
                formattedValue = "${"%.1f".format(physics.windForceZ)} m/s²",
                onValueChange = { onPhysicsChange(physics.copy(windForceZ = it)) },
                testTag = "water_wind_z_slider"
            )
            WaterLabeledSlider(
                label = "Time Scale",
                value = physics.timeScale,
                valueRange = 0.1f..2.0f,
                formattedValue = "${"%.2f".format(physics.timeScale)}x",
                onValueChange = { onPhysicsChange(physics.copy(timeScale = it)) },
                testTag = "water_time_scale_slider"
            )
            WaterLabeledSlider(
                label = "Solver Substeps / Frame",
                value = physics.substeps.toFloat(),
                valueRange = 1f..4f,
                formattedValue = "${physics.substeps} substeps",
                steps = 2,
                onValueChange = { onPhysicsChange(physics.copy(substeps = it.roundToInt().coerceIn(1, 4))) },
                testTag = "water_substeps_slider"
            )
        }
        Spacer(Modifier.height(20.dp))
    }
}
