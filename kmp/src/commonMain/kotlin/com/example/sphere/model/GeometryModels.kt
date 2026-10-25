package com.example.sphere

import androidx.compose.ui.graphics.Color

// -------------------------------------------------------------
// GEOMETRY & PERSPECTIVE DATA STRUCTURES
// -------------------------------------------------------------
data class Vec3D(val x: Float, val y: Float, val z: Float)
data class Point2D(val x: Float, val y: Float, val depth: Float)

data class FaceQuad(
    val p0: Point2D,
    val p1: Point2D,
    val p2: Point2D,
    val p3: Point2D,
    val avgDepth: Float,
    val normal: Vec3D,
    val faceIndex: Int
)

enum class SurfacePattern(val label: String) {
    SOLID("Solid PBR"),
    GRID("Wire Grid"),
    CHECKER("Checkerboard"),
    VORONOI("Voronoi Cells"),
    RIPPLE("Wave Ripple"),
    HEXAGON("Hex Mesh")
}

data class ColorOption(val name: String, val color: Color)

val paletteColors = listOf(
    ColorOption("Vulkan Cyan", Color(0xFF06B6D4)),
    ColorOption("Neon Emerald", Color(0xFF10B981)),
    ColorOption("Ruby Red", Color(0xFFF43F5E)),
    ColorOption("Sunset Amber", Color(0xFFF59E0B)),
    ColorOption("Cosmic Violet", Color(0xFF8B5CF6)),
    ColorOption("Studio White", Color(0xFFE2E8F0)),
    ColorOption("Obsidian Grey", Color(0xFF475569))
)
