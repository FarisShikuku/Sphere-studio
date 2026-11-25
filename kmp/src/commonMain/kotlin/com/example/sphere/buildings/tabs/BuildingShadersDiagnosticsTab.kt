package com.example.sphere.buildings.tabs

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.buildings.BuildingMeshStats
import com.example.sphere.model.BuildingPresetItem
import com.example.sphere.model.BuildingStudioState
import com.example.sphere.nativebridge.SphereNativeEngine
import kotlin.math.roundToInt

@Composable
fun BuildingShadersDiagnosticsTab(
    preset: BuildingPresetItem,
    state: BuildingStudioState,
    stats: BuildingMeshStats,
    modifier: Modifier = Modifier
) {
    val nativeDiagnostics = remember { SphereNativeEngine.getBuildingDiagnostics() }

    Column(
        modifier = modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp, vertical = 12.dp),
        verticalArrangement = Arrangement.spacedBy(12.dp)
    ) {
        SectionHeader("Active Architectural Pipeline", "gen/buildings C++20 Generators & GLSL Shaders")

        ElevatedCard(shape = RoundedCornerShape(14.dp)) {
            Column(
                modifier = Modifier.padding(14.dp),
                verticalArrangement = Arrangement.spacedBy(6.dp)
            ) {
                Text("Active Variant", style = MaterialTheme.typography.labelLarge, color = MaterialTheme.colorScheme.primary)
                Text(preset.name, style = MaterialTheme.typography.titleMedium, fontWeight = FontWeight.Bold)
                Text(preset.subtitle, style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.onSurfaceVariant)
                HorizontalDivider(modifier = Modifier.padding(vertical = 4.dp))
                DiagnosticKvRow("Domain ID / Subtype", "${state.domainOrdinal} / #${state.subTypeOrdinal}")
                DiagnosticKvRow("Volume & Corner", "${state.shape.displayName} • ${state.cornerType.displayName}")
                DiagnosticKvRow("Cladding & Glazing", "${state.wallMaterial.displayName} • ${state.glassType.displayName}")
                DiagnosticKvRow("Glass Privacy IOR", String.format("%.2f", state.windowIor))
                DiagnosticKvRow("Roof Crown", "${state.roofType.displayName} (${String.format("%.1f", state.roofHeight)}m)")
                DiagnosticKvRow("Gross Floor Area", "${stats.grossFloorAreaSqM.roundToInt()} m² (${stats.footprintAreaSqM.roundToInt()} m² plot)")
                DiagnosticKvRow("Generated Geometry", "${stats.triangleCount} tris / ${stats.vertexCount} vertices")
            }
        }

        SectionHeader("Vulkan Shader & Compute Kernels", "gen/buildings/shaders/*.vert|frag|comp")
        ElevatedCard(shape = RoundedCornerShape(14.dp)) {
            Column(
                modifier = Modifier.padding(14.dp),
                verticalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                ShaderSpecRow("building_surface.vert / .frag", "Flat MaterialSlot (0..49) routing, procedural brick mortar UV, and Schlick Fresnel glass refraction + skyline reflection.")
                ShaderSpecRow("facade.comp", "Compute shader (16×16 workgroups) generating procedural facade atlases with brick, corten steel, basalt & glazed bay apertures.")
                ShaderSpecRow("socket_attach.comp", "Compute shader (64×1 workgroups) resolving rooftop & facade socket matrices for HVAC, antennas, and solar props.")
            }
        }

        SectionHeader("C++ BuildingSystem Registry", "12 Registered Architectural Generators")
        Surface(
            shape = RoundedCornerShape(12.dp),
            color = MaterialTheme.colorScheme.surfaceContainerLow,
            modifier = Modifier.fillMaxWidth().testTag("building_native_diagnostics_card")
        ) {
            Text(
                text = nativeDiagnostics,
                fontFamily = FontFamily.Monospace,
                fontSize = 11.sp,
                lineHeight = 16.sp,
                modifier = Modifier.padding(12.dp)
            )
        }
    }
}

@Composable
private fun DiagnosticKvRow(label: String, value: String) {
    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
        Text(label, style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.onSurfaceVariant)
        Text(value, style = MaterialTheme.typography.bodySmall, fontWeight = FontWeight.SemiBold)
    }
}

@Composable
private fun ShaderSpecRow(name: String, summary: String) {
    Column(verticalArrangement = Arrangement.spacedBy(2.dp)) {
        Text(name, fontFamily = FontFamily.Monospace, style = MaterialTheme.typography.labelMedium, color = MaterialTheme.colorScheme.primary)
        Text(summary, style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.onSurfaceVariant)
    }
}
