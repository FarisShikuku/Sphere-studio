package com.example.sphere.trailers.tabs

import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.CommodityItem
import com.example.sphere.model.TrailerItem

@Composable
fun TrailerRegistryCppTab(
    trailer: TrailerItem,
    selectedCargo: CommodityItem,
    cargoQuantity: Int,
    modifier: Modifier = Modifier
) {
    var selectedSection by remember { mutableIntStateOf(0) }
    val sections = listOf("Logistics Mission C++", "TrailerDef Struct", "gen/trailers/ Tree")

    val code = when (selectedSection) {
        0 -> """
#include "TrailerRegistry.h"
#include "CargoRegistry.h"

using namespace sphere::trailers;
using namespace sphere::cargo;

void StartHaulageMission(Vehicle* myTruck) {
    // 1. Spawn procedural trailer
    auto myTrailer = TrailerRegistry::Get(static_cast<TrailerID>(${trailer.id})); // ${trailer.name}

    // 2. Load cargo from CargoRegistry
    auto cargo = CargoRegistry::Get().GetCargo(static_cast<CargoID>(${selectedCargo.id}));
    myTrailer->LoadCargo(cargo, /*quantity=*/${cargoQuantity}u);

    // 3. Query combined Gross Vehicle Weight
    float gvwKg = myTrailer->GetTotalMassKg(); // Empty ${trailer.emptyWeightKg} + Payload
    myTruck->Hitch(myTrailer);
}
        """.trimIndent()
        1 -> """
struct TrailerDef {
    TrailerID id            = static_cast<TrailerID>(${trailer.id});
    std::string name        = "${trailer.name}";
    std::string category    = "${trailer.category.label}";
    float empty_weight_kg   = ${trailer.emptyWeightKg}f;
    float max_payload_kg    = ${trailer.maxPayloadKg}f;
    Vec3 dimensions         = {${trailer.lengthM}f, ${trailer.widthM}f, ${trailer.heightM}f};
    float deck_height_m     = ${trailer.deckHeightM}f;
    uint32_t axle_count     = ${trailer.axleConfig.count}u;
    bool has_lift_axle      = ${trailer.hasLiftAxle};
    bool has_steerable_axle = ${trailer.hasSteerableAxle};
};
        """.trimIndent()
        else -> """
native_assets/src/gen/trailers/
├── CMakeLists.txt (libtrailers.a -> linked with cargo)
├── library/{TrailerID, TrailerDef, TrailerRegistry, ITrailer}
├── flatbed/       ├── tanker/       ├── box_van/
├── chassis/       ├── logging/      ├── curtainsider/
├── bulk_hopper/   ├── car_carrier/  ├── heavy_haulage/
├── dolly/         ├── recreational/ ├── specialty/
└── running_gear/  (Shared axles, air/leaf susp, ABS, wheels)
        """.trimIndent()
    }

    Column(
        modifier = modifier
            .fillMaxWidth()
            .padding(14.dp)
            .testTag("trailer_registry_cpp_tab")
    ) {
        Text(
            "Native C++20 TrailerRegistry & Cargo Coupling",
            style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
            color = MaterialTheme.colorScheme.primary
        )
        Spacer(Modifier.height(8.dp))

        Row(
            modifier = Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            sections.forEachIndexed { idx, label ->
                FilterChip(
                    selected = selectedSection == idx,
                    onClick = { selectedSection = idx },
                    label = { Text(label, fontSize = 11.sp) }
                )
            }
        }

        Spacer(Modifier.height(10.dp))

        Box(
            modifier = Modifier
                .fillMaxWidth()
                .clip(RoundedCornerShape(10.dp))
                .background(Color(0xFF0B0F17))
                .padding(12.dp)
        ) {
            Text(
                text = code,
                color = Color(0xFF86EFAC),
                fontFamily = FontFamily.Monospace,
                fontSize = 11.sp,
                modifier = Modifier.horizontalScroll(rememberScrollState())
            )
        }
    }
}
