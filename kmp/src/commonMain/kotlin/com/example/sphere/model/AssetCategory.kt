package com.example.sphere

import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ViewInAr
import androidx.compose.material.icons.outlined.Apartment
import androidx.compose.material.icons.outlined.Cloud
import androidx.compose.material.icons.outlined.DirectionsCar
import androidx.compose.material.icons.outlined.EditRoad
import androidx.compose.material.icons.outlined.Forest
import androidx.compose.material.icons.outlined.GraphicEq
import androidx.compose.material.icons.outlined.Landscape
import androidx.compose.material.icons.outlined.Layers
import androidx.compose.material.icons.outlined.Texture
import androidx.compose.material.icons.outlined.TextFields
import androidx.compose.material.icons.filled.Lightbulb
import androidx.compose.material.icons.outlined.LocalShipping
import androidx.compose.material.icons.outlined.RvHookup
import androidx.compose.material.icons.outlined.Water
import androidx.compose.material.icons.outlined.WbSunny
import androidx.compose.material.icons.outlined.Widgets
import androidx.compose.ui.graphics.vector.ImageVector

// -------------------------------------------------------------
// ASSET CATEGORIES DEFINITION FOR SPHERE STUDIO
// -------------------------------------------------------------
data class AssetCategory(
    val id: String,
    val title: String,
    val subtitle: String,
    val icon: ImageVector,
    val description: String
)

val availableAssetCategories = listOf(
    AssetCategory("test", "Test Screen", "Calibration Cube & Scene Viewport", Icons.Default.ViewInAr, "Interactive 3D geometry, dimensional controls & surface PBR patterns"),
    AssetCategory("commodities", "Commodities & Cargo", "Procedural Goods & Freight", Icons.Outlined.LocalShipping, "Procedural bags, crates, drums, timber, bottles, industrial pallets & perishable goods"),
    AssetCategory("trailers", "Trailers & Haulage", "Semi, Tanker, Heavy & Dollies", Icons.Outlined.RvHookup, "Procedural flatbeds, tankers, box vans, container chassis, logging, curtainsiders, hoppers & running gear"),
    AssetCategory("lighting", "Lighting Studio", "PlainRenderer Vulkan Lights", Icons.Default.Lightbulb, "Physically based lights: flashlight spot cones, filament bulbs, area softboxes & Kelvin blackbody spectrum"),
    AssetCategory("water", "Water Simulation", "Vulkan Compute SPH & SSFR", Icons.Outlined.Water, "3D Smoothed Particle Hydrodynamics, spatial hash bitonic sort, bilateral depth blur, Beer-Lambert absorption & Fresnel refraction"),
    AssetCategory("roads", "Roads & Highways", "Highway, Two-Lane & Urban KTX2", Icons.Outlined.EditRoad, "Khronos KTX 2.0 roadways: highway two-lane, white shoulders, double yellow & asphalt"),
    AssetCategory("vegetation", "Vegetation & Foliage", "L-System Trees, Grass & Biomes", Icons.Outlined.Forest, "L-system branching, blade compute simulation & species rules"),
    AssetCategory("surfaces", "PBR Surfaces", "Materials, Decals & Detail", Icons.Outlined.Texture, "Multi-layer PBR textures, micro-detail noise & Voronoi"),
    AssetCategory("text", "GlyphCore Studio", "Vulkan SDF & TextOverlay Engine", Icons.Outlined.TextFields, "Sascha Willems SDF glyph rasterizer, text overlay pipelines, 3D billboard labels & typography styling"),
    AssetCategory("buildings", "Buildings & Architecture", "Modular Facades & Kits", Icons.Outlined.Apartment, "Grammar-based building generator, socket attachment & floorplans"),
    AssetCategory("weather", "Weather Systems", "Rain, Snow & Clouds", Icons.Outlined.Cloud, "Volumetric froxel fog, raymarched clouds & lightning paths"),
    AssetCategory("terrain", "Terrain & Landscapes", "Erosion, Cliffs & Caves", Icons.Outlined.Landscape, "Hydraulic erosion, marching-cubes voxel caves & splatmaps"),
    AssetCategory("vehicles", "Lorries & Buses", "Rigid, Tractors, Coaches & Matatu", Icons.Outlined.DirectionsCar, "Procedural heavy lorries, 5th-wheel articulated tractors, city buses, coaches & 14-seater matatus"),
    AssetCategory("props", "World Props & Utility", "Power, Solar & Street Furniture", Icons.Outlined.Widgets, "Wind turbines, solar PV trackers, catenary wires & transformers"),
    AssetCategory("sky", "Atmosphere & Sky", "Preetham / Hosek-Wilkie", Icons.Outlined.WbSunny, "Atmospheric scattering LUT, star catalogs & solar disk"),
    AssetCategory("particles", "GPU Particle Systems", "VFX & Compute Simulations", Icons.Outlined.Layers, "Sort compute, smoke curl noise, spark physics & flame sheets"),
    AssetCategory("audio", "Procedural Audio", "Synthesis & Ambience", Icons.Outlined.GraphicEq, "Real-time engine oscillator, footsteps & surface acoustics")
)
