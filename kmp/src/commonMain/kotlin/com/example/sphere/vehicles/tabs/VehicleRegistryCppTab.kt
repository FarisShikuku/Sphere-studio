package com.example.sphere.vehicles.tabs

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.CommodityItem
import com.example.sphere.model.TrailerItem
import com.example.sphere.model.VehicleItem
import com.example.sphere.nativebridge.SphereNativeEngine

@Composable
fun VehicleRegistryCppTab(
    vehicle: VehicleItem,
    hitchEnabled: Boolean,
    hitchedTrailer: TrailerItem,
    loadedCargo: CommodityItem,
    cargoQuantity: Int
) {
    val nativeCount = SphereNativeEngine.getVehiclesCount()
    val nativeName = SphereNativeEngine.getVehicleName(vehicle.id)
    val nativeCategory = SphereNativeEngine.getVehicleCategory(vehicle.id)
    val nativeSpecs = SphereNativeEngine.getVehicleSpecs(vehicle.id)

    val cppSnippet = buildString {
        appendLine("// C++20 / Vulkan 1.3 Procedural Vehicle Rigging")
        appendLine("#include \"VehicleRegistry.h\"")
        if (vehicle.canHitchTrailer && hitchEnabled) {
            appendLine("#include \"TrailerRegistry.h\"")
            appendLine("#include \"CargoRegistry.h\"")
        }
        appendLine()
        appendLine("auto& reg = sphere::vehicles::VehicleRegistry::Get();")
        appendLine("auto veh = reg.GetVehicle(VehicleID::${vehicle.cppEnumName});")
        appendLine("veh->SetLivery(LiveryID::${vehicle.livery.name});")
        if (vehicle.livery.name == "NAIROBI_NGANYA") {
            appendLine("veh->SetRandomLivery(0x${vehicle.randomSeed.toUInt().toString(16).uppercase()}u);")
        }
        if (vehicle.canHitchTrailer && hitchEnabled) {
            appendLine()
            appendLine("auto trailer = TrailerRegistry::Get().GetTrailer(static_cast<TrailerID>(${hitchedTrailer.id})); // ${hitchedTrailer.name}")
            appendLine("auto cargo   = CargoRegistry::Get().GetCargo(static_cast<CargoID>(${loadedCargo.id})); // ${loadedCargo.name}")
            appendLine("trailer->LoadCargo(cargo, ${cargoQuantity}u);")
            appendLine("veh->Hitch(trailer); // Aligns kingpin to +${String.format("%.2f", vehicle.fifthWheelOffsetM)}m 5th wheel")
        }
        appendLine("float gvwKg = veh->GetTotalMassKg();")
        append("auto mesh = veh->GenerateMesh(0);")
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
            modifier = Modifier.testTag("card_vehicle_jni_stats")
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(6.dp)) {
                Text("Live C++ VehicleRegistry Inspection", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.Bold)
                Text("Registered Vehicles (C++): $nativeCount", fontFamily = FontFamily.Monospace, fontSize = 12.sp)
                Text("Name: $nativeName ($nativeCategory)", fontFamily = FontFamily.Monospace, fontSize = 12.sp)
                Text(
                    "Native Specs: ${nativeSpecs[0].toInt()}kg kerb • ${nativeSpecs[4].toInt()}Nm • ${nativeSpecs[5].toInt()}HP • ${nativeSpecs[7]}m×${nativeSpecs[8]}m×${nativeSpecs[9]}m",
                    fontFamily = FontFamily.Monospace,
                    fontSize = 11.sp,
                    color = MaterialTheme.colorScheme.primary
                )
            }
        }

        Card(
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.surfaceContainerLow),
            shape = RoundedCornerShape(14.dp),
            modifier = Modifier.testTag("card_vehicle_cpp_snippet")
        ) {
            Column(modifier = Modifier.padding(14.dp), verticalArrangement = Arrangement.spacedBy(8.dp)) {
                Text("VehicleRegistry & Coupling API", style = MaterialTheme.typography.titleSmall, fontWeight = FontWeight.Bold)
                Surface(
                    shape = RoundedCornerShape(10.dp),
                    color = MaterialTheme.colorScheme.surfaceContainer,
                    modifier = Modifier.fillMaxWidth()
                ) {
                    Text(
                        text = cppSnippet,
                        fontFamily = FontFamily.Monospace,
                        fontSize = 11.sp,
                        lineHeight = 16.sp,
                        modifier = Modifier.padding(12.dp)
                    )
                }
            }
        }
    }
}
