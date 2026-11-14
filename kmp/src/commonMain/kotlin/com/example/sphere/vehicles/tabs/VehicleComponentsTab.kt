package com.example.sphere.vehicles.tabs

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.outlined.Build
import androidx.compose.material.icons.outlined.DirectionsBus
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.VehicleItem

private data class SharedPartInfo(
    val enumName: String,
    val title: String,
    val dims: String,
    val notes: String
)

@Composable
fun VehicleComponentsTab(vehicle: VehicleItem) {
    val cabParts = listOf(
        SharedPartInfo("DOOR_LEFT", "Heavy Lorry Door (Left)", "1.05m × 1.38m × 0.08m", "Steel skin, belt molding, pull latch & opaque glass"),
        SharedPartInfo("DOOR_RIGHT", "Heavy Lorry Door (Right)", "1.05m × 1.38m × 0.08m", "Flush seam gasket & curb-view corner cut"),
        SharedPartInfo("WINDSHIELD", "Panoramic Laminated Glass", "2.18m × 0.88m × 0.03m", "Opaque reflective slate glass (alpha=1.0) + wipers"),
        SharedPartInfo("GRILLE_HEAVY", "Slatted Chrome Grille", "1.72m × 0.72m × 0.06m", "Multi-slat radiator bezel over dark core backing"),
        SharedPartInfo("MIRROR_TRUCK", "Dual-Arm Wing Mirrors", "0.34m × 0.56m × 0.06m", "Upper/lower steel arms & wide-angle spotters")
    )

    val chassisParts = listOf(
        SharedPartInfo("FUEL_TANK", "600L Alloy Fuel Tank", "1.55m × 0.62m × 0.62m", "Watertight cylinder with shallow dished end caps"),
        SharedPartInfo("EXHAUST_STACK", "Vertical Chrome Stacks", "0.22m × 0.22m × 2.55m", "Dual vertical risers with perforated heat shields"),
        SharedPartInfo("BATTERY_BOX", "Battery & AdBlue Pack", "0.72m × 0.48m × 0.42m", "Steel enclosure with blue DEF filler cap"),
        SharedPartInfo("ENGINE_BLOCK", "12.8L I6 Turbo-Diesel", "1.35m × 0.85m × 1.05m", "Mounted above ${vehicle.groundClearanceM}m clearance plane"),
        SharedPartInfo("DRIVETRAIN", "Propeller Shaft & Diff", "3.60m × 0.44m × 0.44m", "Tubular driveshaft & cast-iron axle differentials")
    )

    Column(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp, vertical = 8.dp),
        verticalArrangement = Arrangement.spacedBy(12.dp)
    ) {
        Card(
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerLow),
            shape = RoundedCornerShape(14.dp),
            modifier = Modifier.testTag("card_shared_cab_components")
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Icon(Icons.Outlined.DirectionsBus, contentDescription = null, tint = MaterialTheme.colorScheme.primary)
                    Spacer(Modifier.width(8.dp))
                    Text("Shared Cab Components (cab_components/)", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.Bold)
                }
                cabParts.forEach { part ->
                    HorizontalDivider()
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                        Text(part.enumName, fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold, fontSize = 11.sp, color = MaterialTheme.colorScheme.primary)
                        Text(part.dims, fontFamily = FontFamily.Monospace, fontSize = 11.sp)
                    }
                    Text("${part.title} — ${part.notes}", style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.onSurfaceVariant)
                }
            }
        }

        Card(
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerLow),
            shape = RoundedCornerShape(14.dp),
            modifier = Modifier.testTag("card_shared_chassis_components")
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Icon(Icons.Outlined.Build, contentDescription = null, tint = MaterialTheme.colorScheme.secondary)
                    Spacer(Modifier.width(8.dp))
                    Text("Shared Chassis Components (chassis_components/)", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.Bold)
                }
                chassisParts.forEach { part ->
                    HorizontalDivider()
                    Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceBetween) {
                        Text(part.enumName, fontFamily = FontFamily.Monospace, fontWeight = FontWeight.Bold, fontSize = 11.sp, color = MaterialTheme.colorScheme.secondary)
                        Text(part.dims, fontFamily = FontFamily.Monospace, fontSize = 11.sp)
                    }
                    Text("${part.title} — ${part.notes}", style = MaterialTheme.typography.bodySmall, color = MaterialTheme.colorScheme.onSurfaceVariant)
                }
            }
        }
    }
}
