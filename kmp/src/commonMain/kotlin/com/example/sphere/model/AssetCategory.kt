package com.example.sphere

import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ViewInAr
import androidx.compose.material.icons.outlined.Agriculture
import androidx.compose.material.icons.outlined.Apartment
import androidx.compose.material.icons.outlined.Cloud
import androidx.compose.material.icons.outlined.DirectionsCar
import androidx.compose.material.icons.outlined.Forest
import androidx.compose.material.icons.outlined.GraphicEq
import androidx.compose.material.icons.outlined.Landscape
import androidx.compose.material.icons.outlined.Layers
import androidx.compose.material.icons.outlined.Texture
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
    AssetCategory("water", "Water Bodies", "FFT Ocean, Lakes & Rivers", Icons.Outlined.Water, "Tessendorf FFT ocean spectrum, river splines & shore foam"),
    AssetCategory("roads", "Roads & Pathways", "Splines, Markings & States", Icons.Outlined.Agriculture, "Procedural camber, asphalt PBR, sign SDFs & 10 surface states"),
    AssetCategory("vegetation", "Vegetation & Foliage", "L-System Trees, Grass & Biomes", Icons.Outlined.Forest, "L-system branching, blade compute simulation & species rules"),
    AssetCategory("surfaces", "PBR Surfaces", "Materials, Decals & Detail", Icons.Outlined.Texture, "Multi-layer PBR textures, micro-detail noise & Voronoi"),
    AssetCategory("buildings", "Buildings & Architecture", "Modular Facades & Kits", Icons.Outlined.Apartment, "Grammar-based building generator, socket attachment & floorplans"),
    AssetCategory("weather", "Weather Systems", "Rain, Snow & Clouds", Icons.Outlined.Cloud, "Volumetric froxel fog, raymarched clouds & lightning paths"),
    AssetCategory("terrain", "Terrain & Landscapes", "Erosion, Cliffs & Caves", Icons.Outlined.Landscape, "Hydraulic erosion, marching-cubes voxel caves & splatmaps"),
    AssetCategory("vehicles", "Vehicles & Transports", "Cars, Trucks & Parts", Icons.Outlined.DirectionsCar, "Silhouette extrusion, wheel assemblies & emergency lightbars"),
    AssetCategory("props", "World Props & Utility", "Power, Solar & Street Furniture", Icons.Outlined.Widgets, "Wind turbines, solar PV trackers, catenary wires & transformers"),
    AssetCategory("sky", "Atmosphere & Sky", "Preetham / Hosek-Wilkie", Icons.Outlined.WbSunny, "Atmospheric scattering LUT, star catalogs & solar disk"),
    AssetCategory("particles", "GPU Particle Systems", "VFX & Compute Simulations", Icons.Outlined.Layers, "Sort compute, smoke curl noise, spark physics & flame sheets"),
    AssetCategory("audio", "Procedural Audio", "Synthesis & Ambience", Icons.Outlined.GraphicEq, "Real-time engine oscillator, footsteps & surface acoustics")
)
