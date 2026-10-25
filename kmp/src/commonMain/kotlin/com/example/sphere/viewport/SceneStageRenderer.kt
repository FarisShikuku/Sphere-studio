package com.example.sphere

import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.PathEffect
import androidx.compose.ui.graphics.drawscope.DrawScope
import androidx.compose.ui.graphics.drawscope.Stroke
import kotlin.math.cos
import kotlin.math.sin

/**
 * Renders the 5 surface scene modes in the 3D viewport:
 * ENDLESS, NONE, FLAT_PIECE, WATER_BODY, BROKEN_SLAB.
 */
fun DrawScope.renderSceneStage(
    surfaceMode: SurfaceSceneMode,
    stageWidth: Float,
    stageLength: Float,
    stageHeight: Float,
    yGround: Float,
    hw: Float,
    hd: Float,
    height: Float,
    castShadowsActive: Boolean,
    shadowQuality: ShadowQuality,
    shadowIntensity: Float,
    shadowSoftness: Float,
    lightX: Float,
    lightY: Float,
    lightZ: Float,
    normLight: Vec3D,
    sunLightTint: Color,
    showGrid: Boolean,
    enableSky: Boolean,
    w: Float,
    h: Float,
    cx: Float,
    cy: Float,
    projectPoint: (Vec3D) -> Point2D,
    transformNormal: (Vec3D) -> Vec3D
) {
    fun callShadow(receiverY: Float, baseShadowColor: Color = Color.Black, intensityScale: Float = 1.0f) {
        drawItemDirectionalShadow(
            receiverY = receiverY,
            baseShadowColor = baseShadowColor,
            intensityScale = intensityScale,
            castShadowsActive = castShadowsActive,
            shadowQuality = shadowQuality,
            shadowIntensity = shadowIntensity,
            shadowSoftness = shadowSoftness,
            lightX = lightX,
            lightY = lightY,
            lightZ = lightZ,
            hw = hw,
            hd = hd,
            yGround = yGround,
            height = height,
            projectPoint = projectPoint
        )
    }

    when (surfaceMode) {
        SurfaceSceneMode.NONE -> {
            if (!enableSky) {
                val refRadius = (maxOf(hw, hd) * 1.8f).coerceAtLeast(1.4f)
                val numPts = 32
                val refPoints = (0..numPts).map { i ->
                    val ang = (i.toFloat() / numPts) * 2f * 3.14159265f
                    projectPoint(Vec3D(refRadius * cos(ang), yGround, refRadius * sin(ang)))
                }
                val refPath = Path().apply {
                    moveTo(refPoints[0].x, refPoints[0].y)
                    for (pt in refPoints.drop(1)) lineTo(pt.x, pt.y)
                    close()
                }
                drawPath(
                    path = refPath,
                    color = Color(0xFF38BDF8).copy(alpha = 0.18f),
                    style = Stroke(width = 1f, pathEffect = PathEffect.dashPathEffect(floatArrayOf(5f, 5f), 0f))
                )
            }
        }

        SurfaceSceneMode.ENDLESS -> {
            if (showGrid) {
                val gridColor = if (enableSky) Color(0xFF38BDF8).copy(alpha = 0.22f) else Color(0xFF1E293B).copy(alpha = 0.4f)
                val gridAxisColor = if (enableSky) Color(0xFF38BDF8).copy(alpha = 0.6f) else Color(0xFF0284C7).copy(alpha = 0.6f)
                val dashEffect = PathEffect.dashPathEffect(floatArrayOf(4f, 4f), 0f)

                val lines = 12
                val spacing = w / (lines + 2)

                for (i in -lines / 2..lines / 2) {
                    val offset = i * spacing
                    val isCenter = i == 0
                    val pY = cy + 130f
                    drawLine(
                        color = if (isCenter) gridAxisColor else gridColor,
                        start = Offset(cx + offset * 0.4f, pY - 50f),
                        end = Offset(cx + offset * 2.1f, h + 20f),
                        strokeWidth = if (isCenter) 2f else 1f,
                        pathEffect = if (!isCenter) dashEffect else null
                    )
                }

                for (j in 1..6) {
                    val lineY = cy + 60f + (j * j * 9f)
                    if (lineY <= h) {
                        drawLine(
                            color = gridColor,
                            start = Offset(0f, lineY),
                            end = Offset(w, lineY),
                            strokeWidth = 1f
                        )
                    }
                }
            }
            callShadow(receiverY = yGround, baseShadowColor = Color.Black)
        }

        SurfaceSceneMode.FLAT_PIECE -> {
            val sw = stageWidth * 0.5f
            val sd = stageLength * 0.5f
            val sh = stageHeight
            val yTop = yGround
            val yBtm = yGround - sh

            val pT0 = projectPoint(Vec3D(-sw, yTop, -sd))
            val pT1 = projectPoint(Vec3D(sw, yTop, -sd))
            val pT2 = projectPoint(Vec3D(sw, yTop, sd))
            val pT3 = projectPoint(Vec3D(-sw, yTop, sd))

            val pB0 = projectPoint(Vec3D(-sw, yBtm, -sd))
            val pB1 = projectPoint(Vec3D(sw, yBtm, -sd))
            val pB2 = projectPoint(Vec3D(sw, yBtm, sd))
            val pB3 = projectPoint(Vec3D(-sw, yBtm, sd))

            // Directional drop shadow of platform on void below
            if (castShadowsActive) {
                val tPlat = (sh + 0.6f) / lightY
                val clPlat = tPlat.coerceIn(0.2f, 16.0f)
                val platHull = computeConvexHull2D(listOf(
                    Vec3D(-sw, yBtm - 0.4f, -sd),
                    Vec3D(sw, yBtm - 0.4f, -sd),
                    Vec3D(sw, yBtm - 0.4f, sd),
                    Vec3D(-sw, yBtm - 0.4f, sd),
                    Vec3D(-sw - clPlat * lightX, yBtm - 0.4f, -sd - clPlat * lightZ),
                    Vec3D(sw - clPlat * lightX, yBtm - 0.4f, -sd - clPlat * lightZ),
                    Vec3D(sw - clPlat * lightX, yBtm - 0.4f, sd - clPlat * lightZ),
                    Vec3D(-sw - clPlat * lightX, yBtm - 0.4f, sd - clPlat * lightZ)
                ))
                if (platHull.size >= 3) {
                    val platShadowPath = Path().apply {
                        val s0 = projectPoint(platHull[0])
                        moveTo(s0.x, s0.y)
                        for (i in 1 until platHull.size) {
                            val si = projectPoint(platHull[i])
                            lineTo(si.x, si.y)
                        }
                        close()
                    }
                    drawPath(platShadowPath, color = Color.Black.copy(alpha = (shadowIntensity * 0.45f).coerceIn(0f, 0.65f)))
                }
            }

            // Side Skirts
            val sideFaces = listOf(
                Triple(listOf(pT0, pT1, pB1, pB0), Vec3D(0f, 0f, -1f), "Front"),
                Triple(listOf(pT1, pT2, pB2, pB1), Vec3D(1f, 0f, 0f), "Right"),
                Triple(listOf(pT2, pT3, pB3, pB2), Vec3D(0f, 0f, 1f), "Back"),
                Triple(listOf(pT3, pT0, pB0, pB3), Vec3D(-1f, 0f, 0f), "Left")
            )

            for ((pts, normal, _) in sideFaces) {
                val rotNorm = transformNormal(normal)
                if (rotNorm.z > -0.05f) {
                    val dot = (rotNorm.x * normLight.x + rotNorm.y * normLight.y + rotNorm.z * normLight.z).coerceAtLeast(0f)
                    val skirtShade = (0.25f + 0.55f * dot).coerceIn(0.15f, 0.85f)
                    val skirtColor = Color(
                        red = (0.12f * sunLightTint.red * skirtShade).coerceIn(0f, 1f),
                        green = (0.16f * sunLightTint.green * skirtShade).coerceIn(0f, 1f),
                        blue = (0.24f * sunLightTint.blue * skirtShade).coerceIn(0f, 1f),
                        alpha = 0.98f
                    )
                    val skirtPath = Path().apply {
                        moveTo(pts[0].x, pts[0].y)
                        lineTo(pts[1].x, pts[1].y)
                        lineTo(pts[2].x, pts[2].y)
                        lineTo(pts[3].x, pts[3].y)
                        close()
                    }
                    drawPath(path = skirtPath, color = skirtColor)
                    drawPath(path = skirtPath, color = Color(0xFF1E293B), style = Stroke(width = 1f))
                }
            }

            // Top Quad
            val topQuadPath = Path().apply {
                moveTo(pT0.x, pT0.y)
                lineTo(pT1.x, pT1.y)
                lineTo(pT2.x, pT2.y)
                lineTo(pT3.x, pT3.y)
                close()
            }

            val topNormal = transformNormal(Vec3D(0f, 1f, 0f))
            val topDot = (topNormal.x * normLight.x + topNormal.y * normLight.y + topNormal.z * normLight.z).coerceAtLeast(0f)
            val topShade = (0.35f + 0.65f * topDot).coerceIn(0.25f, 1.0f)
            val platformTopColor = Color(
                red = (0.13f * sunLightTint.red * topShade).coerceIn(0f, 1f),
                green = (0.17f * sunLightTint.green * topShade).coerceIn(0f, 1f),
                blue = (0.25f * sunLightTint.blue * topShade).coerceIn(0f, 1f),
                alpha = 0.98f
            )
            drawPath(path = topQuadPath, color = platformTopColor)

            if (showGrid) {
                val stepSize = if (stageWidth <= 4.0f && stageLength <= 4.0f) 0.5f else 1.0f
                var gx = -sw + stepSize
                while (gx < sw) {
                    val pA = projectPoint(Vec3D(gx, yTop, -sd))
                    val pB = projectPoint(Vec3D(gx, yTop, sd))
                    val isCenter = kotlin.math.abs(gx) < 0.1f
                    drawLine(
                        color = if (isCenter) Color(0xFF38BDF8).copy(alpha = 0.7f) else Color(0xFF38BDF8).copy(alpha = 0.22f),
                        start = Offset(pA.x, pA.y),
                        end = Offset(pB.x, pB.y),
                        strokeWidth = if (isCenter) 1.8f else 1.0f
                    )
                    gx += stepSize
                }

                var gz = -sd + stepSize
                while (gz < sd) {
                    val pC = projectPoint(Vec3D(-sw, yTop, gz))
                    val pD = projectPoint(Vec3D(sw, yTop, gz))
                    val isCenter = kotlin.math.abs(gz) < 0.1f
                    drawLine(
                        color = if (isCenter) Color(0xFF38BDF8).copy(alpha = 0.7f) else Color(0xFF38BDF8).copy(alpha = 0.22f),
                        start = Offset(pC.x, pC.y),
                        end = Offset(pD.x, pD.y),
                        strokeWidth = if (isCenter) 1.8f else 1.0f
                    )
                    gz += stepSize
                }
            }

            drawPath(path = topQuadPath, color = Color(0xFF38BDF8).copy(alpha = 0.65f), style = Stroke(width = 2.0f))
            listOf(pT0, pT1, pT2, pT3).forEach { pt ->
                drawCircle(Color(0xFF38BDF8), radius = 3.5f, center = Offset(pt.x, pt.y))
            }
            callShadow(receiverY = yTop, baseShadowColor = Color.Black)
        }

        SurfaceSceneMode.WATER_BODY -> {
            val sw = stageWidth * 0.5f
            val sd = stageLength * 0.5f
            val sh = stageHeight
            val yTop = yGround
            val yBtm = yGround - sh

            val pT0 = projectPoint(Vec3D(-sw, yTop, -sd))
            val pT1 = projectPoint(Vec3D(sw, yTop, -sd))
            val pT2 = projectPoint(Vec3D(sw, yTop, sd))
            val pT3 = projectPoint(Vec3D(-sw, yTop, sd))

            val pB0 = projectPoint(Vec3D(-sw, yBtm, -sd))
            val pB1 = projectPoint(Vec3D(sw, yBtm, -sd))
            val pB2 = projectPoint(Vec3D(sw, yBtm, sd))
            val pB3 = projectPoint(Vec3D(-sw, yBtm, sd))

            val basinFaces = listOf(
                listOf(pT0, pT1, pB1, pB0),
                listOf(pT1, pT2, pB2, pB1),
                listOf(pT2, pT3, pB3, pB2),
                listOf(pT3, pT0, pB0, pB3)
            )
            for (pts in basinFaces) {
                val p = Path().apply {
                    moveTo(pts[0].x, pts[0].y); lineTo(pts[1].x, pts[1].y); lineTo(pts[2].x, pts[2].y); lineTo(pts[3].x, pts[3].y); close()
                }
                drawPath(path = p, color = Color(0xDD075985))
                drawPath(path = p, color = Color(0xFF0284C7).copy(alpha = 0.7f), style = Stroke(width = 1.2f))
            }

            val waterPath = Path().apply {
                moveTo(pT0.x, pT0.y); lineTo(pT1.x, pT1.y); lineTo(pT2.x, pT2.y); lineTo(pT3.x, pT3.y); close()
            }
            drawPath(
                path = waterPath,
                brush = Brush.linearGradient(
                    colors = listOf(Color(0xFA0284C7), Color(0xFA0369A1), Color(0xFA0C4A6E)),
                    start = Offset(pT0.x, pT0.y),
                    end = Offset(pT2.x, pT2.y)
                )
            )

            callShadow(receiverY = yTop, baseShadowColor = Color(0xFF031422), intensityScale = 0.85f)

            for (r in 1..6) {
                val rz = -sd + (sd * 2f) * (r / 7f)
                val rA = projectPoint(Vec3D(-sw * 0.9f, yTop, rz))
                val rB = projectPoint(Vec3D(sw * 0.9f, yTop, rz))
                drawLine(
                    color = Color(0xFFE0F2FE).copy(alpha = 0.35f),
                    start = Offset(rA.x, rA.y),
                    end = Offset(rB.x, rB.y),
                    strokeWidth = 1.2f
                )
            }

            drawPath(path = waterPath, color = Color(0xFFBAE6FD).copy(alpha = 0.85f), style = Stroke(width = 2.5f))

            val cW0 = projectPoint(Vec3D(-hw * 1.15f, yTop, -hd * 1.15f))
            val cW1 = projectPoint(Vec3D(hw * 1.15f, yTop, -hd * 1.15f))
            val cW2 = projectPoint(Vec3D(hw * 1.15f, yTop, hd * 1.15f))
            val cW3 = projectPoint(Vec3D(-hw * 1.15f, yTop, hd * 1.15f))
            val foamPath = Path().apply {
                moveTo(cW0.x, cW0.y); lineTo(cW1.x, cW1.y); lineTo(cW2.x, cW2.y); lineTo(cW3.x, cW3.y); close()
            }
            drawPath(path = foamPath, color = Color(0xFFE0F2FE).copy(alpha = 0.85f), style = Stroke(width = 2f))
            drawPath(path = foamPath, color = Color(0x66082F49))
        }

        SurfaceSceneMode.BROKEN_SLAB -> {
            val sw = stageWidth * 0.5f
            val sd = stageLength * 0.5f
            val sh = stageHeight * 1.4f
            val yTop = yGround
            val yBtm = yGround - sh

            val radii = floatArrayOf(
                1.0f, 0.85f, 1.08f, 0.78f, 1.12f, 0.90f,
                1.04f, 0.80f, 1.10f, 0.86f, 1.05f, 0.88f
            )
            val numPts = radii.size
            val topPts = mutableListOf<Point2D>()
            val btmPts = mutableListOf<Point2D>()

            for (k in 0 until numPts) {
                val angle = (k.toFloat() / numPts) * 2f * 3.14159265f
                val r = sw * radii[k]
                val fx = r * cos(angle)
                val fz = (r * (sd / sw)) * sin(angle)
                topPts.add(projectPoint(Vec3D(fx, yTop, fz)))
                btmPts.add(projectPoint(Vec3D(fx * 0.72f, yBtm, fz * 0.72f)))
            }

            val btmCenter = projectPoint(Vec3D(0f, yBtm - 0.5f, 0f))
            drawCircle(
                brush = Brush.radialGradient(
                    colors = listOf(Color.Black.copy(alpha = 0.5f), Color.Transparent),
                    center = Offset(btmCenter.x, btmCenter.y),
                    radius = (sw * 45f).coerceAtLeast(20f)
                ),
                radius = (sw * 45f).coerceAtLeast(20f),
                center = Offset(btmCenter.x, btmCenter.y)
            )

            for (k in 0 until numPts) {
                val next = (k + 1) % numPts
                val cliffQuad = Path().apply {
                    moveTo(topPts[k].x, topPts[k].y)
                    lineTo(topPts[next].x, topPts[next].y)
                    lineTo(btmPts[next].x, btmPts[next].y)
                    lineTo(btmPts[k].x, btmPts[k].y)
                    close()
                }
                val facetShade = if (k % 2 == 0) 0.35f else 0.22f
                drawPath(
                    path = cliffQuad,
                    color = Color(
                        red = (0.20f * sunLightTint.red * facetShade).coerceIn(0f, 1f),
                        green = (0.24f * sunLightTint.green * facetShade).coerceIn(0f, 1f),
                        blue = (0.32f * sunLightTint.blue * facetShade).coerceIn(0f, 1f),
                        alpha = 0.98f
                    )
                )
                drawPath(path = cliffQuad, color = Color(0xFF1E293B), style = Stroke(width = 1f))
            }

            val topRockPath = Path().apply {
                moveTo(topPts[0].x, topPts[0].y)
                for (pt in topPts.drop(1)) lineTo(pt.x, pt.y)
                close()
            }
            drawPath(path = topRockPath, color = Color(0xFF293548))
            drawPath(path = topRockPath, color = Color(0xFF64748B), style = Stroke(width = 1.5f))

            val centerPt = projectPoint(Vec3D(0f, yTop, 0f))
            for (k in listOf(1, 4, 7, 10)) {
                drawLine(
                    color = Color(0xFF0F172A).copy(alpha = 0.6f),
                    start = Offset(centerPt.x, centerPt.y),
                    end = Offset(topPts[k].x, topPts[k].y),
                    strokeWidth = 1.2f
                )
            }

            val frag1 = projectPoint(Vec3D(sw * 0.7f, yBtm - 0.25f, sd * 0.5f))
            val frag2 = projectPoint(Vec3D(-sw * 0.65f, yBtm - 0.35f, -sd * 0.4f))
            drawCircle(Color(0xFF334155), radius = 5f, center = Offset(frag1.x, frag1.y))
            drawCircle(Color(0xFF1E293B), radius = 4f, center = Offset(frag2.x, frag2.y))

            callShadow(receiverY = yTop, baseShadowColor = Color(0xFF090E17))
        }
    }
}
