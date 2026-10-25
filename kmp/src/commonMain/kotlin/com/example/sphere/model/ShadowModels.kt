package com.example.sphere

// -------------------------------------------------------------
// SHADOW QUALITY & CONVEX HULL PROJECTION ALGORITHM
// (Referencing SaschaWillems/Vulkan shadowmapping)
// -------------------------------------------------------------
enum class ShadowQuality(
    val label: String,
    val shortLabel: String,
    val kernelDesc: String,
    val pcfRange: Int
) {
    OFF("Shadows Disabled", "Off", "Direct sun lighting only", 0),
    HARD("Hard Shadow (1-Tap)", "Hard", "Raw depth test (aliased edges)", 0),
    PCF_3X3("PCF 3×3 (Medium)", "PCF 3×3", "9-tap bilinear PCF filter", 1),
    PCF_5X5("PCF 5×5 (Sascha Willems)", "PCF 5×5", "25-tap smooth penumbra kernel", 2),
    CSM_4("Cascaded Shadow Maps", "CSM 4-Split", "4 frustum depth cascade splits", 2)
}

// 2D Monotone Chain Convex Hull algorithm for projected ground shadows
fun computeConvexHull2D(points: List<Vec3D>): List<Vec3D> {
    if (points.size <= 3) return points
    val sorted = points.sortedWith(compareBy({ it.x }, { it.z }))
    val lower = ArrayList<Vec3D>(points.size)
    for (p in sorted) {
        while (lower.size >= 2 && cross2D(lower[lower.size - 2], lower[lower.size - 1], p) <= 0f) {
            lower.removeAt(lower.size - 1)
        }
        lower.add(p)
    }
    val upper = ArrayList<Vec3D>(points.size)
    for (i in sorted.indices.reversed()) {
        val p = sorted[i]
        while (upper.size >= 2 && cross2D(upper[upper.size - 2], upper[upper.size - 1], p) <= 0f) {
            upper.removeAt(upper.size - 1)
        }
        upper.add(p)
    }
    if (lower.isNotEmpty()) lower.removeAt(lower.size - 1)
    if (upper.isNotEmpty()) upper.removeAt(upper.size - 1)
    return lower + upper
}

private fun cross2D(o: Vec3D, a: Vec3D, b: Vec3D): Float {
    return (a.x - o.x) * (b.z - o.z) - (a.z - o.z) * (b.x - o.x)
}
