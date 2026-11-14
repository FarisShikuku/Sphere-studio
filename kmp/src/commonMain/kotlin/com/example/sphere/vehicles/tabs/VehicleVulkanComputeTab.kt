package com.example.sphere.vehicles.tabs

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.outlined.Memory
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.VehicleCategoryType
import com.example.sphere.model.VehicleItem

@Composable
fun VehicleVulkanComputeTab(vehicle: VehicleItem) {
    val shaderFiles = when (vehicle.category) {
        VehicleCategoryType.LORRY_RIGID -> listOf(
            "lorry_rigid/shaders/include/rigid_lorry_common.glsl",
            "lorry_rigid/shaders/cab_mesh.comp",
            "lorry_rigid/shaders/body_mesh.comp",
            "lorry_rigid/shaders/lorry_albedo.comp (VK_FORMAT_BC7_SRGB_BLOCK)",
            "lorry_rigid/shaders/lorry_normal.comp (VK_FORMAT_BC5_UNORM_BLOCK)",
            "lorry_rigid/shaders/lorry_orm.comp (VK_FORMAT_BC7_UNORM_BLOCK)"
        )
        VehicleCategoryType.LORRY_ARTICULATED -> listOf(
            "lorry_articulated/shaders/include/tractor_common.glsl",
            "lorry_articulated/shaders/tractor_mesh.comp",
            "lorry_articulated/shaders/tractor_albedo.comp (VK_FORMAT_BC7_SRGB_BLOCK)",
            "lorry_articulated/shaders/tractor_normal.comp (VK_FORMAT_BC5_UNORM_BLOCK)",
            "lorry_articulated/shaders/tractor_orm.comp (VK_FORMAT_BC7_UNORM_BLOCK)"
        )
        VehicleCategoryType.BUS_CITY -> listOf(
            "bus_city/shaders/include/city_bus_common.glsl",
            "bus_city/shaders/city_bus_mesh.comp",
            "bus_city/shaders/city_bus_albedo.comp (VK_FORMAT_BC7_SRGB_BLOCK)",
            "bus_city/shaders/city_bus_normal.comp (VK_FORMAT_BC5_UNORM_BLOCK)",
            "bus_city/shaders/city_bus_orm.comp (VK_FORMAT_BC7_UNORM_BLOCK)"
        )
        VehicleCategoryType.BUS_COACH -> listOf(
            "bus_coach/shaders/include/coach_common.glsl",
            "bus_coach/shaders/coach_mesh.comp",
            "bus_coach/shaders/coach_albedo.comp (VK_FORMAT_BC7_SRGB_BLOCK)",
            "bus_coach/shaders/coach_normal.comp (VK_FORMAT_BC5_UNORM_BLOCK)",
            "bus_coach/shaders/coach_orm.comp (VK_FORMAT_BC7_UNORM_BLOCK)"
        )
        VehicleCategoryType.BUS_MINIBUS -> listOf(
            "bus_minibus/shaders/include/minibus_common.glsl",
            "bus_minibus/shaders/minibus_mesh.comp",
            "bus_minibus/shaders/minibus_albedo.comp (VK_FORMAT_BC7_SRGB_BLOCK)",
            "bus_minibus/shaders/minibus_normal.comp (VK_FORMAT_BC5_UNORM_BLOCK)",
            "bus_minibus/shaders/minibus_orm.comp (VK_FORMAT_BC7_UNORM_BLOCK)"
        )
    }

    Column(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp, vertical = 8.dp),
        verticalArrangement = Arrangement.spacedBy(12.dp)
    ) {
        Card(
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerLow),
            shape = RoundedCornerShape(14.dp),
            modifier = Modifier.testTag("card_vehicle_vulkan_compute")
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Icon(Icons.Outlined.Memory, contentDescription = null, tint = MaterialTheme.colorScheme.primary)
                    Spacer(Modifier.width(8.dp))
                    Text(
                        "Vulkan 1.3 Compute Shaders (${vehicle.category.moduleName}/)",
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.Bold
                    )
                }
                Text(
                    "Procedural vertex extrusion & BC7/BC5 PBR texture pipelines compiled under gen/vehicles/${vehicle.category.moduleName}/:",
                    style = MaterialTheme.typography.bodySmall,
                    color = MaterialTheme.colorScheme.onSurfaceVariant
                )
                shaderFiles.forEach { path ->
                    Surface(
                        shape = RoundedCornerShape(8.dp),
                        color = MaterialTheme.colorScheme.surfaceContainer,
                        modifier = Modifier.fillMaxWidth()
                    ) {
                        Text(
                            text = path,
                            fontFamily = FontFamily.Monospace,
                            fontSize = 11.sp,
                            modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp)
                        )
                    }
                }
            }
        }
    }
}
