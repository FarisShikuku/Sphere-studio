package com.example.sphere

import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.drawscope.DrawScope

/**
 * Directional Shadow Projection & PCF Filtering Routines
 * Reference: Sascha Willems Vulkan Shadow Mapping (https://github.com/SaschaWillems/Vulkan)
 */
fun DrawScope.drawItemDirectionalShadow(
    receiverY: Float,
    baseShadowColor: Color = Color.Black,
    intensityScale: Float = 1.0f,
    castShadowsActive: Boolean,
    shadowQuality: ShadowQuality,
    shadowIntensity: Float,
    shadowSoftness: Float,
    lightX: Float,
    lightY: Float,
    lightZ: Float,
    hw: Float,
    hd: Float,
    yGround: Float,
    height: Float,
    projectPoint: (Vec3D) -> Point2D
) {
    if (!castShadowsActive) return

    val effIntensity = (shadowIntensity * intensityScale).coerceIn(0f, 1f)
    val tRay = ((yGround + height) - receiverY) / lightY
    val clampedTRay = tRay.coerceIn(0.1f, 18.0f)

    // 4 Base Vertices on receiver plane
    val b0 = Vec3D(-hw, receiverY, -hd)
    val b1 = Vec3D(hw, receiverY, -hd)
    val b2 = Vec3D(hw, receiverY, hd)
    val b3 = Vec3D(-hw, receiverY, hd)

    // 4 Top Vertices projected along -L onto receiver plane
    val p0 = Vec3D(-hw - clampedTRay * lightX, receiverY, -hd - clampedTRay * lightZ)
    val p1 = Vec3D(hw - clampedTRay * lightX, receiverY, -hd - clampedTRay * lightZ)
    val p2 = Vec3D(hw - clampedTRay * lightX, receiverY, hd - clampedTRay * lightZ)
    val p3 = Vec3D(-hw - clampedTRay * lightX, receiverY, hd - clampedTRay * lightZ)

    val rawHull = computeConvexHull2D(listOf(b0, b1, b2, b3, p0, p1, p2, p3))
    if (rawHull.size < 3) return

    // Penumbra spread radius based on ray distance and softness setting (Sascha Willems PCF)
    val penumbraDist = clampedTRay * 0.05f * shadowSoftness

    // PCF Multi-tap rendering
    when (shadowQuality) {
        ShadowQuality.PCF_5X5, ShadowQuality.CSM_4 -> {
            // Outer Penumbra Shell (smooth falloff)
            val outerHull = rawHull.map { pt ->
                val isProjected = pt.x !in (-hw - 0.01f)..(hw + 0.01f) || pt.z !in (-hd - 0.01f)..(hd + 0.01f)
                if (isProjected) {
                    Vec3D(pt.x - lightX * penumbraDist * 0.4f, pt.y, pt.z - lightZ * penumbraDist * 0.4f)
                } else pt
            }
            val outerPath = Path().apply {
                val s0 = projectPoint(outerHull[0])
                moveTo(s0.x, s0.y)
                for (i in 1 until outerHull.size) {
                    val si = projectPoint(outerHull[i])
                    lineTo(si.x, si.y)
                }
                close()
            }
            drawPath(outerPath, color = baseShadowColor.copy(alpha = effIntensity * 0.22f))

            // Intermediate Penumbra Shell
            val midHull = rawHull.map { pt ->
                val isProjected = pt.x !in (-hw - 0.01f)..(hw + 0.01f) || pt.z !in (-hd - 0.01f)..(hd + 0.01f)
                if (isProjected) {
                    Vec3D(pt.x - lightX * penumbraDist * 0.18f, pt.y, pt.z - lightZ * penumbraDist * 0.18f)
                } else pt
            }
            val midPath = Path().apply {
                val s0 = projectPoint(midHull[0])
                moveTo(s0.x, s0.y)
                for (i in 1 until midHull.size) {
                    val si = projectPoint(midHull[i])
                    lineTo(si.x, si.y)
                }
                close()
            }
            drawPath(midPath, color = baseShadowColor.copy(alpha = effIntensity * 0.45f))

            // Core Umbra
            val corePath = Path().apply {
                val s0 = projectPoint(rawHull[0])
                moveTo(s0.x, s0.y)
                for (i in 1 until rawHull.size) {
                    val si = projectPoint(rawHull[i])
                    lineTo(si.x, si.y)
                }
                close()
            }
            drawPath(corePath, color = baseShadowColor.copy(alpha = effIntensity * 0.88f))
        }
        ShadowQuality.PCF_3X3 -> {
            val outerHull = rawHull.map { pt ->
                val isProjected = pt.x !in (-hw - 0.01f)..(hw + 0.01f) || pt.z !in (-hd - 0.01f)..(hd + 0.01f)
                if (isProjected) {
                    Vec3D(pt.x - lightX * penumbraDist * 0.25f, pt.y, pt.z - lightZ * penumbraDist * 0.25f)
                } else pt
            }
            val outerPath = Path().apply {
                val s0 = projectPoint(outerHull[0])
                moveTo(s0.x, s0.y)
                for (i in 1 until outerHull.size) {
                    val si = projectPoint(outerHull[i])
                    lineTo(si.x, si.y)
                }
                close()
            }
            drawPath(outerPath, color = baseShadowColor.copy(alpha = effIntensity * 0.35f))

            val corePath = Path().apply {
                val s0 = projectPoint(rawHull[0])
                moveTo(s0.x, s0.y)
                for (i in 1 until rawHull.size) {
                    val si = projectPoint(rawHull[i])
                    lineTo(si.x, si.y)
                }
                close()
            }
            drawPath(corePath, color = baseShadowColor.copy(alpha = effIntensity * 0.85f))
        }
        ShadowQuality.HARD -> {
            val corePath = Path().apply {
                val s0 = projectPoint(rawHull[0])
                moveTo(s0.x, s0.y)
                for (i in 1 until rawHull.size) {
                    val si = projectPoint(rawHull[i])
                    lineTo(si.x, si.y)
                }
                close()
            }
            drawPath(corePath, color = baseShadowColor.copy(alpha = effIntensity * 0.92f))
        }
        ShadowQuality.OFF -> {}
    }

    // Contact Ambient Occlusion (prevents Peter Panning at base contact)
    val c0 = projectPoint(Vec3D(-hw * 1.05f, receiverY + 0.005f, -hd * 1.05f))
    val c1 = projectPoint(Vec3D(hw * 1.05f, receiverY + 0.005f, -hd * 1.05f))
    val c2 = projectPoint(Vec3D(hw * 1.05f, receiverY + 0.005f, hd * 1.05f))
    val c3 = projectPoint(Vec3D(-hw * 1.05f, receiverY + 0.005f, hd * 1.05f))
    val contactPath = Path().apply {
        moveTo(c0.x, c0.y); lineTo(c1.x, c1.y); lineTo(c2.x, c2.y); lineTo(c3.x, c3.y); close()
    }
    drawPath(contactPath, color = baseShadowColor.copy(alpha = (effIntensity * 1.15f).coerceAtMost(0.96f)))
}
