package com.example.sphere

/**
 * Surface Scene Options for 3D Viewport and Weather Simulation:
 * - ENDLESS: Infinite horizon ground plane receding into the distance
 * - NONE: Floating freely in 3D sky / void with zero ground surface obstruction
 * - FLAT_PIECE: Adjustable rectangular platform (default 10×10m, or 3×3m for small ~1m items)
 * - WATER_BODY: Bounded water surface (10×10m default) with caustics and wave ripples
 * - BROKEN_SLAB: Fractured floating rock diorama piece with chiseled cliff edges
 */
enum class SurfaceSceneMode(
    val title: String,
    val shortLabel: String,
    val icon: String,
    val description: String,
    val defaultWidth: Float,
    val defaultLength: Float
) {
    ENDLESS(
        title = "Endless Surface",
        shortLabel = "Endless",
        icon = "🌐",
        description = "Infinite ground plane receding to the horizon with perspective grid",
        defaultWidth = 50.0f,
        defaultLength = 50.0f
    ),
    NONE(
        title = "No Surface (Void)",
        shortLabel = "No Surface",
        icon = "🌌",
        description = "Floating in space/sky with zero ground or horizon obstruction",
        defaultWidth = 0.0f,
        defaultLength = 0.0f
    ),
    FLAT_PIECE(
        title = "Flat Piece (10×10m / 3×3m)",
        shortLabel = "Flat Piece",
        icon = "⏹",
        description = "Adjustable flat ground platform (10×10m default, 3×3m for ~1m items)",
        defaultWidth = 10.0f,
        defaultLength = 10.0f
    ),
    WATER_BODY(
        title = "Water Piece (10×10m)",
        shortLabel = "Water Piece",
        icon = "🌊",
        description = "10×10m default water surface with animated ripples & aquatic depth",
        defaultWidth = 10.0f,
        defaultLength = 10.0f
    ),
    BROKEN_SLAB(
        title = "Broken Slab",
        shortLabel = "Broken Slab",
        icon = "🪨",
        description = "Fractured floating rock diorama with chiseled cliff edges",
        defaultWidth = 8.0f,
        defaultLength = 8.0f
    )
}
