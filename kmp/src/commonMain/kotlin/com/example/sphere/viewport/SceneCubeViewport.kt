package com.example.sphere

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.gestures.detectDragGestures
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.platform.testTag
import kotlin.math.cos
import kotlin.math.sin
import kotlin.math.sqrt

// -------------------------------------------------------------
// 3D PERSPECTIVE CANVAS WITH CUBE, ATMOSPHERIC SKY & SUN
// -------------------------------------------------------------
@Composable
fun SceneCubeViewport(
    modifier: Modifier = Modifier,
    width: Float,
    height: Float,
    depth: Float,
    baseColor: Color,
    pattern: SurfacePattern,
    rotX: Float,
    rotY: Float,
    showGrid: Boolean,
    enableSky: Boolean,
    enableSun: Boolean,
    sunElevation: Float,
    sunAzimuth: Float,
    turbidity: Float,
    surfaceMode: SurfaceSceneMode = SurfaceSceneMode.FLAT_PIECE,
    stageWidth: Float = 10.0f,
    stageLength: Float = 10.0f,
    stageHeight: Float = 0.35f,
    enableShadows: Boolean = true,
    shadowQuality: ShadowQuality = ShadowQuality.PCF_5X5,
    shadowIntensity: Float = 0.72f,
    shadowSoftness: Float = 1.8f,
    depthBias: Float = 0.005f,
    showShadowMapDebug: Boolean = false,
    onDrag: (Float, Float) -> Unit
) {
    Canvas(
        modifier = modifier
            .testTag("scene_cube_viewport")
            .pointerInput(Unit) {
                detectDragGestures { change, dragAmount ->
                    change.consume()
                    onDrag(dragAmount.x, dragAmount.y)
                }
            }
    ) {
        val w = size.width
        val h = size.height
        val cx = w / 2f
        val cy = h / 2f - 15f

        // 1. SKY AND HORIZON EVALUATION
        val radY = (rotY * (kotlin.math.PI / 180.0)).toFloat()
        val radX = (rotX * (kotlin.math.PI / 180.0)).toFloat()
        val cosY = cos(radY)
        val sinY = sin(radY)
        val cosX = cos(radX)
        val sinX = sin(radX)

        val elRad = (sunElevation * (kotlin.math.PI / 180.0)).toFloat()
        val azRad = (sunAzimuth * (kotlin.math.PI / 180.0)).toFloat()

        // 3D Sun direction vector in world space
        val sunDir = Vec3D(
            x = cos(elRad) * sin(azRad),
            y = sin(elRad),
            z = cos(elRad) * cos(azRad)
        )

        val cameraDist = 4.8f
        val fovFactor = 540f

        fun projectPoint(v: Vec3D): Point2D {
            val x1 = v.x * cosY + v.z * sinY
            val z1 = -v.x * sinY + v.z * cosY

            val y2 = v.y * cosX - z1 * sinX
            val z2 = v.y * sinX + z1 * cosX

            // Sascha Willems Vulkan depth: camera at +cameraDist looking along -Z
            val zView = (cameraDist - z2).coerceAtLeast(0.1f)
            val px = cx + (x1 / zView) * fovFactor
            val py = cy - (y2 / zView) * fovFactor
            return Point2D(px, py, zView)
        }

        fun transformNormal(rawNorm: Vec3D): Vec3D {
            val nx1 = rawNorm.x * cosY + rawNorm.z * sinY
            val nz1 = -rawNorm.x * sinY + rawNorm.z * cosY
            val ny2 = rawNorm.y * cosX - nz1 * sinX
            val nz2 = rawNorm.y * sinX + nz1 * cosX
            return Vec3D(nx1, ny2, nz2)
        }

        val horizonY = cy + 45f + (sinX * 180f)

        val lightDir = if (enableSun) sunDir else Vec3D(0.5f, 0.85f, 0.4f)
        val lightLen = sqrt(lightDir.x * lightDir.x + lightDir.y * lightDir.y + lightDir.z * lightDir.z).coerceAtLeast(0.001f)
        val normLight = Vec3D(lightDir.x / lightLen, lightDir.y / lightLen, lightDir.z / lightLen)
        val sunLightTint = if (enableSun && sunElevation < 20f) Color(0xFFFDBA74) else Color.White

        // 2. BACKGROUND RENDERING (Sky vs Studio Dark vs Void)
        if (enableSky) {
            val zenithColor: Color
            val horizonColor: Color

            if (sunElevation > 20.0f) {
                val factor = (sunElevation - 20.0f) / 70.0f
                zenithColor = Color(
                    red = (0.10f + 0.05f * factor).coerceIn(0f, 1f),
                    green = (0.35f + 0.15f * factor).coerceIn(0f, 1f),
                    blue = (0.78f + 0.18f * factor).coerceIn(0f, 1f)
                )
                horizonColor = Color(
                    red = (0.55f + 0.15f * (turbidity / 10f)).coerceIn(0f, 1f),
                    green = (0.70f + 0.15f * (turbidity / 10f)).coerceIn(0f, 1f),
                    blue = (0.85f + 0.10f * (turbidity / 10f)).coerceIn(0f, 1f)
                )
            } else if (sunElevation >= 0.0f) {
                val sunsetRatio = 1.0f - (sunElevation / 20.0f)
                zenithColor = Color(
                    red = 0.18f + 0.15f * sunsetRatio,
                    green = 0.28f + 0.05f * sunsetRatio,
                    blue = 0.65f - 0.25f * sunsetRatio
                )
                horizonColor = Color(
                    red = 0.85f + 0.12f * sunsetRatio,
                    green = 0.52f - 0.12f * sunsetRatio,
                    blue = 0.22f - 0.10f * sunsetRatio
                )
            } else {
                val nightRatio = (-sunElevation / 10.0f).coerceIn(0f, 1f)
                zenithColor = Color(0xFF04060C)
                horizonColor = Color(
                    red = (0.22f * (1f - nightRatio) + 0.05f).coerceIn(0f, 1f),
                    green = (0.15f * (1f - nightRatio) + 0.07f).coerceIn(0f, 1f),
                    blue = (0.35f * (1f - nightRatio) + 0.12f).coerceIn(0f, 1f)
                )
            }

            if (surfaceMode == SurfaceSceneMode.NONE) {
                drawRect(
                    brush = Brush.verticalGradient(
                        colors = listOf(
                            zenithColor,
                            horizonColor,
                            Color(0xFF0F172A),
                            Color(0xFF050811)
                        )
                    )
                )
            } else {
                drawRect(
                    brush = Brush.verticalGradient(
                        colorStops = arrayOf(
                            0.0f to zenithColor,
                            0.75f to horizonColor,
                            1.0f to horizonColor.copy(alpha = 0.9f)
                        ),
                        startY = 0f,
                        endY = horizonY.coerceAtLeast(10f)
                    ),
                    topLeft = Offset(0f, 0f),
                    size = Size(w, horizonY.coerceAtLeast(10f))
                )

                if (surfaceMode == SurfaceSceneMode.ENDLESS) {
                    drawRect(
                        brush = Brush.verticalGradient(
                            colors = listOf(
                                horizonColor.copy(alpha = 0.45f),
                                Color(0xFF0E121B),
                                Color(0xFF08090D)
                            ),
                            startY = horizonY,
                            endY = h
                        ),
                        topLeft = Offset(0f, horizonY),
                        size = Size(w, (h - horizonY).coerceAtLeast(0f))
                    )
                } else {
                    drawRect(
                        brush = Brush.verticalGradient(
                            colors = listOf(
                                horizonColor.copy(alpha = 0.35f),
                                Color(0xFF0F172A),
                                Color(0xFF090D1A)
                            ),
                            startY = horizonY,
                            endY = h
                        ),
                        topLeft = Offset(0f, horizonY),
                        size = Size(w, (h - horizonY).coerceAtLeast(0f))
                    )
                }
            }

            // Sun Disk & Corona Glow
            if (enableSun && sunElevation > -2.0f) {
                val sunX1 = sunDir.x * cosY + sunDir.z * sinY
                val sunZ1 = -sunDir.x * sinY + sunDir.z * cosY
                val sunY2 = sunDir.y * cosX - sunZ1 * sinX
                val sunZ2 = sunDir.y * sinX + sunZ1 * cosX

                if (sunZ2 > -0.2f) {
                    val sunProjDist = 600f
                    val sunPx = cx + (sunX1 / (sunZ2 + 2.5f)) * sunProjDist
                    val sunPy = cy - (sunY2 / (sunZ2 + 2.5f)) * sunProjDist
                    val coronaRadius = 140f
                    val sunColorRGB = if (sunElevation < 15f) Color(0xFFF97316) else Color(0xFFFDE047)

                    drawCircle(
                        brush = Brush.radialGradient(
                            colors = listOf(
                                sunColorRGB.copy(alpha = 0.45f),
                                sunColorRGB.copy(alpha = 0.18f),
                                Color.Transparent
                            ),
                            center = Offset(sunPx, sunPy),
                            radius = coronaRadius
                        ),
                        radius = coronaRadius,
                        center = Offset(sunPx, sunPy)
                    )

                    drawCircle(Color.White, radius = 18f, center = Offset(sunPx, sunPy))
                    drawCircle(
                        color = sunColorRGB.copy(alpha = 0.85f),
                        radius = 21f,
                        center = Offset(sunPx, sunPy),
                        style = Stroke(width = 3f)
                    )
                }
            }
        } else {
            drawRect(
                brush = Brush.verticalGradient(
                    colors = listOf(
                        Color(0xFF090B10),
                        Color(0xFF10141F),
                        Color(0xFF080A0E)
                    )
                )
            )
        }

        // 3. STAGE / SURFACE GEOMETRY RENDERING & DIRECTIONAL SHADOWS
        val hw = width * 0.5f
        val hh = height * 0.5f
        val hd = depth * 0.5f
        val yGround = -hh

        val castShadowsActive = enableShadows && enableSun && shadowQuality != ShadowQuality.OFF && sunElevation > -2.0f
        val sunElevationClamped = sunElevation.coerceAtLeast(1.5f)
        val elRadClamped = (sunElevationClamped * (kotlin.math.PI / 180.0)).toFloat()
        val lightY = sin(elRadClamped).coerceAtLeast(0.04f)
        val lightX = cos(elRadClamped) * sin(azRad)
        val lightZ = cos(elRadClamped) * cos(azRad)

        renderSceneStage(
            surfaceMode = surfaceMode,
            stageWidth = stageWidth,
            stageLength = stageLength,
            stageHeight = stageHeight,
            yGround = yGround,
            hw = hw,
            hd = hd,
            height = height,
            castShadowsActive = castShadowsActive,
            shadowQuality = shadowQuality,
            shadowIntensity = shadowIntensity,
            shadowSoftness = shadowSoftness,
            lightX = lightX,
            lightY = lightY,
            lightZ = lightZ,
            normLight = normLight,
            sunLightTint = sunLightTint,
            showGrid = showGrid,
            enableSky = enableSky,
            w = w,
            h = h,
            cx = cx,
            cy = cy,
            projectPoint = { projectPoint(it) },
            transformNormal = { transformNormal(it) }
        )

        // 4. 3D CUBE PROJECTION & RENDERING
        val rawVertices = listOf(
            Vec3D(-hw,  hh, -hd),
            Vec3D(-hw, -hh, -hd),
            Vec3D( hw, -hh, -hd),
            Vec3D( hw,  hh, -hd),
            Vec3D(-hw,  hh,  hd),
            Vec3D(-hw, -hh,  hd),
            Vec3D( hw, -hh,  hd),
            Vec3D( hw,  hh,  hd)
        )

        val projected = rawVertices.map { projectPoint(it) }

        val facesData = listOf(
            Triple(listOf(3, 0, 1, 2), Vec3D( 0f,  0f, -1f), 0), // Back (-Z)
            Triple(listOf(4, 7, 6, 5), Vec3D( 0f,  0f,  1f), 1), // Front (+Z)
            Triple(listOf(0, 3, 7, 4), Vec3D( 0f,  1f,  0f), 2), // Top (+Y)
            Triple(listOf(5, 6, 2, 1), Vec3D( 0f, -1f,  0f), 3), // Bottom (-Y)
            Triple(listOf(7, 3, 2, 6), Vec3D( 1f,  0f,  0f), 4), // Right (+X)
            Triple(listOf(0, 4, 5, 1), Vec3D(-1f,  0f,  0f), 5)  // Left (-X)
        )

        val quads = mutableListOf<FaceQuad>()

        for ((indices, rawNorm, faceIdx) in facesData) {
            val rotNorm = transformNormal(rawNorm)
            val vA = rawVertices[indices[0]]
            val vB = rawVertices[indices[2]]
            val fcX = (vA.x + vB.x) * 0.5f
            val fcY = (vA.y + vB.y) * 0.5f
            val fcZ = (vA.z + vB.z) * 0.5f

            val c1 = fcX * cosY + fcZ * sinY
            val cz1 = -fcX * sinY + fcZ * cosY
            val cy2 = fcY * cosX - cz1 * sinX
            val cz2 = fcY * sinX + cz1 * cosX

            val toEyeX = -c1
            val toEyeY = -cy2
            val toEyeZ = cameraDist - cz2
            val isVis = (rotNorm.x * toEyeX + rotNorm.y * toEyeY + rotNorm.z * toEyeZ) > 0.01f

            if (isVis) {
                val p0 = projected[indices[0]]
                val p1 = projected[indices[1]]
                val p2 = projected[indices[2]]
                val p3 = projected[indices[3]]
                val avgDepth = (p0.depth + p1.depth + p2.depth + p3.depth) / 4f

                quads.add(
                    FaceQuad(
                        p0 = p0,
                        p1 = p1,
                        p2 = p2,
                        p3 = p3,
                        avgDepth = avgDepth,
                        normal = rotNorm,
                        faceIndex = faceIdx
                    )
                )
            }
        }

        quads.sortByDescending { it.avgDepth }

        for (quad in quads) {
            val dot = (quad.normal.x * normLight.x + quad.normal.y * normLight.y + quad.normal.z * normLight.z)
                .coerceAtLeast(0.0f)
            val ambient = if (enableSky) 0.32f else 0.26f
            val diffuse = if (castShadowsActive && dot <= 0.001f) 0.0f else 0.68f * dot
            val shade = (ambient + diffuse).coerceIn(0.18f, 1.0f)

            val faceColor = Color(
                red = (baseColor.red * sunLightTint.red * shade).coerceIn(0f, 1f),
                green = (baseColor.green * sunLightTint.green * shade).coerceIn(0f, 1f),
                blue = (baseColor.blue * sunLightTint.blue * shade).coerceIn(0f, 1f),
                alpha = 0.95f
            )

            val path = Path().apply {
                moveTo(quad.p0.x, quad.p0.y)
                lineTo(quad.p1.x, quad.p1.y)
                lineTo(quad.p2.x, quad.p2.y)
                lineTo(quad.p3.x, quad.p3.y)
                close()
            }

            drawPath(path = path, color = faceColor)

            when (pattern) {
                SurfacePattern.GRID -> {
                    val lineColor = Color.White.copy(alpha = 0.35f)
                    val mid01 = Offset((quad.p0.x + quad.p1.x) / 2f, (quad.p0.y + quad.p1.y) / 2f)
                    val mid23 = Offset((quad.p2.x + quad.p3.x) / 2f, (quad.p2.y + quad.p3.y) / 2f)
                    val mid12 = Offset((quad.p1.x + quad.p2.x) / 2f, (quad.p1.y + quad.p2.y) / 2f)
                    val mid30 = Offset((quad.p3.x + quad.p0.x) / 2f, (quad.p3.y + quad.p0.y) / 2f)
                    drawLine(lineColor, mid01, mid23, strokeWidth = 1.2f)
                    drawLine(lineColor, mid12, mid30, strokeWidth = 1.2f)
                }
                SurfacePattern.CHECKER -> {
                    val subPath = Path().apply {
                        val center = Offset(
                            (quad.p0.x + quad.p1.x + quad.p2.x + quad.p3.x) / 4f,
                            (quad.p0.y + quad.p1.y + quad.p2.y + quad.p3.y) / 4f
                        )
                        moveTo(quad.p0.x, quad.p0.y); lineTo(center.x, center.y); lineTo(quad.p1.x, quad.p1.y); close()
                        moveTo(quad.p2.x, quad.p2.y); lineTo(center.x, center.y); lineTo(quad.p3.x, quad.p3.y); close()
                    }
                    drawPath(subPath, color = Color.Black.copy(alpha = 0.22f))
                }
                SurfacePattern.RIPPLE -> {
                    val center = Offset(
                        (quad.p0.x + quad.p1.x + quad.p2.x + quad.p3.x) / 4f,
                        (quad.p0.y + quad.p1.y + quad.p2.y + quad.p3.y) / 4f
                    )
                    drawCircle(Color.White.copy(alpha = 0.25f), 22f, center, style = Stroke(width = 1.5f))
                    drawCircle(Color.White.copy(alpha = 0.15f), 42f, center, style = Stroke(width = 1.5f))
                }
                SurfacePattern.HEXAGON -> {
                    val center = Offset(
                        (quad.p0.x + quad.p1.x + quad.p2.x + quad.p3.x) / 4f,
                        (quad.p0.y + quad.p1.y + quad.p2.y + quad.p3.y) / 4f
                    )
                    val spokeColor = Color.White.copy(alpha = 0.28f)
                    drawLine(spokeColor, center, Offset(quad.p0.x, quad.p0.y), strokeWidth = 1f)
                    drawLine(spokeColor, center, Offset(quad.p1.x, quad.p1.y), strokeWidth = 1f)
                    drawLine(spokeColor, center, Offset(quad.p2.x, quad.p2.y), strokeWidth = 1f)
                    drawLine(spokeColor, center, Offset(quad.p3.x, quad.p3.y), strokeWidth = 1f)
                }
                SurfacePattern.VORONOI -> {
                    val center = Offset(
                        (quad.p0.x + quad.p1.x + quad.p2.x + quad.p3.x) / 4f,
                        (quad.p0.y + quad.p1.y + quad.p2.y + quad.p3.y) / 4f
                    )
                    drawCircle(Color.Black.copy(alpha = 0.3f), 14f, center)
                    drawCircle(Color.White.copy(alpha = 0.4f), 4f, center)
                }
                SurfacePattern.SOLID -> {}
            }

            drawPath(path = path, color = Color.White.copy(alpha = 0.45f), style = Stroke(width = 1.5f))
        }

        // Coordinate axes gizmo at top-left
        val axisOrigin = Offset(52f, 52f)
        val axisLen = 28f
        val xEnd = Offset(axisOrigin.x + (cosY * axisLen), axisOrigin.y + (sinY * sinX * axisLen * 0.5f))
        val zEnd = Offset(axisOrigin.x - (sinY * axisLen), axisOrigin.y + (cosY * sinX * axisLen * 0.5f))
        val yEnd = Offset(axisOrigin.x, axisOrigin.y - (cosX * axisLen))

        drawLine(Color(0xFFEF4444), axisOrigin, xEnd, strokeWidth = 2.5f)
        drawLine(Color(0xFF10B981), axisOrigin, yEnd, strokeWidth = 2.5f)
        drawLine(Color(0xFF3B82F6), axisOrigin, zEnd, strokeWidth = 2.5f)

        // Light-Space Depth Map HUD
        if (showShadowMapDebug && castShadowsActive) {
            val hudW = 120f
            val hudH = 92f
            val hudX = w - hudW - 14f
            val hudY = 56f

            drawRoundRect(
                color = Color(0xFF070A11).copy(alpha = 0.92f),
                topLeft = Offset(hudX, hudY),
                size = Size(hudW, hudH),
                cornerRadius = androidx.compose.ui.geometry.CornerRadius(6f, 6f)
            )
            drawRoundRect(
                color = Color(0xFF6366F1).copy(alpha = 0.80f),
                topLeft = Offset(hudX, hudY),
                size = Size(hudW, hudH),
                cornerRadius = androidx.compose.ui.geometry.CornerRadius(6f, 6f),
                style = Stroke(width = 1.2f)
            )

            val lccx = hudX + hudW / 2f
            val lccy = hudY + hudH / 2f + 4f
            drawRoundRect(
                brush = Brush.verticalGradient(
                    colors = listOf(Color(0xFFE2E8F0), Color(0xFF64748B), Color(0xFF1E293B))
                ),
                topLeft = Offset(lccx - 24f, lccy - 18f),
                size = Size(48f, 36f),
                cornerRadius = androidx.compose.ui.geometry.CornerRadius(3f, 3f)
            )
            drawRoundRect(
                color = Color(0xFFA5B4FC),
                topLeft = Offset(lccx - 24f, lccy - 18f),
                size = Size(48f, 36f),
                cornerRadius = androidx.compose.ui.geometry.CornerRadius(3f, 3f),
                style = Stroke(width = 1f)
            )

            val arrowLen = 22f
            val endPt = Offset(lccx - lightX * arrowLen, lccy + lightZ * arrowLen)
            drawLine(Color(0xFFFBBF24), Offset(lccx, lccy), endPt, strokeWidth = 1.8f)
            drawCircle(Color(0xFFFBBF24), radius = 2.5f, center = endPt)
        }
    }
}
