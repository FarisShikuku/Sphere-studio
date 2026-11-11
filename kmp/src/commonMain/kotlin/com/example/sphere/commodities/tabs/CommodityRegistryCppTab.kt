package com.example.sphere.commodities.tabs

import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.outlined.Check
import androidx.compose.material.icons.outlined.Code
import androidx.compose.material.icons.outlined.DataObject
import androidx.compose.material.icons.outlined.FolderZip
import androidx.compose.material.icons.outlined.Layers
import androidx.compose.material.icons.outlined.Memory
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.Divider
import androidx.compose.material3.FilterChip
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.CommodityItem

@Composable
fun CommodityRegistryCppTab(
    commodity: CommodityItem,
    modifier: Modifier = Modifier
) {
    var selectedSectionIndex by remember { mutableIntStateOf(0) }

    val sectionTitles = listOf(
        "Registry API (C++)",
        "CargoDef Struct",
        "Pipeline Integration"
    )

    val cppSourceCode = when (selectedSectionIndex) {
        0 -> """
// C++20 CargoRegistry Singleton Usage (include/CargoRegistry.h)
#include "CargoRegistry.h"
#include "CargoID.h"

using namespace sphere::cargo;

void SpawnWarehouseProp(VulkanDevice* device, VkCommandBuffer cmd) {
    // 1. Obtain singleton instance
    auto& registry = CargoRegistry::Get();
    
    // 2. Query definitions without loading textures
    CargoDef def = registry.GetDef(CargoID::${commodity.id});
    printf("Spawning %s | Weight: %.1f kg | Dimensions: %.2fx%.2fx%.2f m\n",
           def.name.c_str(), def.weight_kg, def.length_m, def.width_m, def.height_m);
           
    // 3. Request procedurally generated mesh (lazily allocated & cached)
    const Mesh* mesh = registry.RequestMesh(CargoID::${commodity.id}, device);
    
    // 4. Request Vulkan BC7 diffuse texture
    VkImageView albedoView = registry.RequestImage(
        CargoID::${commodity.id},
        CargoImageType::ALBEDO,
        device
    );
    
    // 5. Bind into Vulkan render pass
    VkBuffer vbo = mesh->GetVertexBuffer();
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmd, 0, 1, &vbo, offsets);
    vkCmdBindIndexBuffer(cmd, mesh->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(cmd, mesh->GetIndexCount(), 1, 0, 0, 0);
}
        """.trimIndent()
        1 -> """
// CargoDef.h Memory Layout (PBR Optics + Logistics Data)
namespace sphere::cargo {

struct CargoDef {
    CargoID     id            = CargoID::${commodity.id};
    std::string name          = "${commodity.name}";
    CargoCategory category    = CargoCategory::${commodity.category.name};
    
    // Logistics physical characteristics
    float weight_kg           = ${commodity.weightKg}f;
    float volume_m3           = ${commodity.volumeM3}f;
    float length_m            = ${commodity.lengthM}f;
    float width_m             = ${commodity.widthM}f;
    float height_m            = ${commodity.heightM}f;
    float density_kg_m3       = ${commodity.densityKgM3}f;
    float max_stack_load_kg   = ${commodity.maxStackLoadKg}f;
    
    // Procedural PBR optics
    Vec4  base_albedo_tint    = Vec4(${commodity.baseColor.red}f, ${commodity.baseColor.green}f, ${commodity.baseColor.blue}f, 1.0f);
    float roughness           = ${commodity.roughness}f;
    float metallic            = ${commodity.metallic}f;
    float opacity_bias        = ${commodity.opacityBias}f;
    float wrinkle_relief      = ${commodity.wrinkleCreaseDepth}f;
    float wear_and_tear       = ${commodity.wearAndTear}f;
    
    // Vulkan hardware target formats
    VkFormat albedo_format    = VK_FORMAT_BC7_SRGB_BLOCK;
    VkFormat normal_format    = VK_FORMAT_BC5_UNORM_BLOCK;
    VkFormat orm_format       = VK_FORMAT_BC7_UNORM_BLOCK;
    uint32_t texture_res      = 1024;
};

} // namespace sphere::cargo
        """.trimIndent()
        else -> """
// CMake & Subdirectory Linking (CMakeLists.txt)
// Add to root CMakeLists.txt:
// add_subdirectory(src/gen/cargo)
// target_link_libraries(SphereStudioEngine PRIVATE cargo)

// Imported project headers (Zero duplicated code):
#include "scene/Mesh.h"
#include "scene/Transform.h"
#include "scene/AABB.h"
#include "shared/text/TextDef.h"

// In JNI bridge (SphereStudioJni.cpp):
JNIEXPORT jint JNICALL
Java_com_example_sphere_nativebridge_SphereNativeEngine_nativeGetCargoCommoditiesCount(
    JNIEnv* env, jobject thiz) {
    return static_cast<jint>(sphere::cargo::CargoRegistry::Get().GetTotalCargoCount());
}
        """.trimIndent()
    }

    Column(
        modifier = modifier
            .fillMaxWidth()
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(16.dp)
    ) {
        // C++ Architecture Info Card
        Card(
            modifier = Modifier
                .fillMaxWidth()
                .testTag("commodity_cpp_architecture_card"),
            shape = RoundedCornerShape(14.dp),
            colors = CardDefaults.cardColors(
                containerColor = MaterialTheme.colorScheme.surfaceVariant.copy(alpha = 0.5f)
            )
        ) {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(16.dp),
                verticalArrangement = Arrangement.spacedBy(12.dp)
            ) {
                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.spacedBy(10.dp)
                ) {
                    Icon(
                        imageVector = Icons.Outlined.FolderZip,
                        contentDescription = null,
                        tint = MaterialTheme.colorScheme.primary,
                        modifier = Modifier.size(24.dp)
                    )
                    Column {
                        Text(
                            text = "libcargo.a Native Shared Library",
                            style = MaterialTheme.typography.titleMedium,
                            fontWeight = FontWeight.SemiBold
                        )
                        Text(
                            text = "7 procedural modules under native_assets/src/gen/cargo/",
                            style = MaterialTheme.typography.bodySmall,
                            color = MaterialTheme.colorScheme.onSurfaceVariant
                        )
                    }
                }

                Text(
                    text = "Imports existing scene/Mesh.h, scene/Transform.h, and shared/text/TextDef.h without recreating mesh structures or rasterizers.",
                    style = MaterialTheme.typography.bodySmall,
                    color = MaterialTheme.colorScheme.onSurfaceVariant
                )
            }
        }

        // Section Tabs
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            sectionTitles.forEachIndexed { index, title ->
                val isSelected = selectedSectionIndex == index
                FilterChip(
                    selected = isSelected,
                    onClick = { selectedSectionIndex = index },
                    label = { Text(title, style = MaterialTheme.typography.bodySmall) },
                    modifier = Modifier
                        .weight(1f)
                        .testTag("chip_cpp_section_$index")
                )
            }
        }

        // C++ Code Card
        Card(
            modifier = Modifier
                .fillMaxWidth()
                .testTag("commodity_cpp_code_card"),
            shape = RoundedCornerShape(14.dp),
            colors = CardDefaults.cardColors(
                containerColor = Color(0xFF14141E)
            )
        ) {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(14.dp)
            ) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Row(
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.spacedBy(6.dp)
                    ) {
                        Icon(
                            imageVector = Icons.Outlined.DataObject,
                            contentDescription = null,
                            tint = Color(0xFFA5D6A7),
                            modifier = Modifier.size(16.dp)
                        )
                        Text(
                            text = "${sectionTitles[selectedSectionIndex]} :: C++20",
                            style = MaterialTheme.typography.labelMedium,
                            color = Color(0xFFA5D6A7),
                            fontFamily = FontFamily.Monospace
                        )
                    }

                    Text(
                        text = "Clang / NDK r26",
                        style = MaterialTheme.typography.labelSmall,
                        color = Color.Gray,
                        fontFamily = FontFamily.Monospace
                    )
                }

                Spacer(modifier = Modifier.height(10.dp))

                Box(
                    modifier = Modifier
                        .fillMaxWidth()
                        .horizontalScroll(rememberScrollState())
                ) {
                    Text(
                        text = cppSourceCode,
                        style = MaterialTheme.typography.bodySmall.copy(
                            fontSize = 11.sp,
                            lineHeight = 16.sp
                        ),
                        fontFamily = FontFamily.Monospace,
                        color = Color(0xFFE0E0E0)
                    )
                }
            }
        }
    }
}
