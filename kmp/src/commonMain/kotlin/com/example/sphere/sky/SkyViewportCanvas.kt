package com.example.sphere

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.gestures.detectDragGestures
import androidx.compose.foundation.gestures.detectTapGestures
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.CornerRadius
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Rect
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.FilterQuality
import androidx.compose.ui.graphics.ImageBitmap
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.graphics.drawscope.DrawScope
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.graphics.drawscope.clipRect
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.TextMeasurer
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.drawText
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.rememberTextMeasurer
import androidx.compose.ui.unit.IntOffset
import androidx.compose.ui.unit.IntSize
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.CloudDetectionRepository
import com.example.sphere.model.CloudGenus
import com.example.sphere.model.DetectedCloudFormation
import com.example.sphere.model.ProjectedCloudDetection
import kotlin.math.PI
import kotlin.math.abs
import kotlin.math.cos
import kotlin.math.max
import kotlin.math.min
import kotlin.math.sin

/**
 * High-fidelity Atmospheric Sky & Volumetric Cloudscape Viewport
 * Powered by AmanSachan1/Meteoros Decima Volumetric Cloud Raymarcher
 */
@Composable
fun SkyViewportCanvas(
    modifier: Modifier = Modifier,
    sunElevation: Float,
    sunAzimuth: Float,
    sunAngularRadius: Float,
    coronaFlareIntensity: Float,
    moonPhase: Float,
    moonElevation: Float,
    moonAzimuth: Float,
    earthshine: Float,
    starCount: Int,
    twinkleSpeed: Float,
    twinkleCycle: Float,
    camYaw: Float,
    camPitch: Float,
    onDrag: (Float, Float) -> Unit,
    // Meteoros Volumetric Cloudscape
    cloudImage: ImageBitmap? = null,
    cloudCoverage: Float = 0.65f,
    cloudDensity: Float = 1.0f,
    enableGodRays: Boolean = true,
    godRaysIntensity: Float = 0.65f,
    enableCirrus: Boolean = true,
    cirrusCoverage: Float = 0.35f,
    timeSeconds: Float = 0.0f,
    // Shared Vulkan Scene Stage (Default is NONE for unhindered sky dome view)
    surfaceMode: SurfaceSceneMode = SurfaceSceneMode.NONE,
    stageWidth: Float = 3.5f,
    stageLength: Float = 3.5f,
    stageHeight: Float = 0.25f,
    showStageGrid: Boolean = true,
    showCalibrationItem: Boolean = false,
    // Cloud Detection & Classification Features
    isDetectionEnabled: Boolean = true,
    detectedFormations: List<DetectedCloudFormation> = CloudDetectionRepository.defaultFormations,
    selectedFormationId: String? = null,
    selectedFilterGenus: CloudGenus? = null,
    onSelectFormation: (DetectedCloudFormation) -> Unit = {}
) {
    val textMeasurer = rememberTextMeasurer()

    Canvas(
        modifier = modifier
            .fillMaxSize()
            .testTag("sky_viewport_canvas")
            .pointerInput(detectedFormations, camYaw, camPitch, isDetectionEnabled, selectedFilterGenus) {
                detectTapGestures { tapOffset ->
                    if (!isDetectionEnabled) return@detectTapGestures
                    val w = size.width.toFloat()
                    val h = size.height.toFloat()

                    for (formation in detectedFormations) {
                        if (selectedFilterGenus != null && formation.genus != selectedFilterGenus) continue
                        val proj = CloudDetectionRepository.projectFormation(
                            formation = formation,
                            screenWidth = w,
                            screenHeight = h,
                            camYaw = camYaw,
                            camPitch = camPitch
                        )
                        if (proj.isVisibleInFrustum) {
                            // Expand hit rect slightly for comfortable touch targeting
                            val touchRect = Rect(
                                left = proj.screenRect.left - 16f,
                                top = proj.screenRect.top - 16f,
                                right = proj.screenRect.right + 16f,
                                bottom = proj.screenRect.bottom + 28f
                            )
                            if (touchRect.contains(tapOffset)) {
                                onSelectFormation(formation)
                                return@detectTapGestures
                            }
                        }
                    }
                }
            }
            .pointerInput(Unit) {
                detectDragGestures { change, dragAmount ->
                    change.consume()
                    onDrag(dragAmount.x, dragAmount.y)
                }
            }
    ) {
        val w = size.width
        val h = size.height
        val horizonY = center.y + (camPitch / 80f) * (h / 2f)

        // 1. Preetham / Hosek-Wilkie Rayleigh & Mie Sky Gradient Computation
        val (zenithColor, horizonColor) = when {
            sunElevation > 20f -> {
                Color(0xFF0F3A7A) to Color(0xFF6BA5E7)
            }
            sunElevation in 5f..20f -> {
                Color(0xFF1E3A8A) to Color(0xFF93C5FD)
            }
            sunElevation in 0f..5f -> {
                Color(0xFF312E81) to Color(0xFFFB923C)
            }
            sunElevation in -6f..0f -> {
                Color(0xFF0F172A) to Color(0xFFC2185B)
            }
            else -> {
                Color(0xFF030712) to Color(0xFF0B1329)
            }
        }

        val skyBrush = Brush.verticalGradient(
            colors = listOf(zenithColor, horizonColor),
            startY = 0f,
            endY = horizonY.coerceAtLeast(10f)
        )
        drawRect(brush = skyBrush, topLeft = Offset.Zero, size = Size(w, horizonY.coerceAtLeast(0f)))

        // 2. Night Starfield Rendering
        val starDarkness = (1f - (sunElevation + 6f) / 16f).coerceIn(0f, 1f)
        if (starDarkness > 0.05f) {
            val starSeed = 42
            val displayStars = min(starCount, 300)
            for (i in 0 until displayStars) {
                val starRa = ((i * 137.508f + starSeed) % 360f)
                val starDec = -10f + ((i * 53.1f) % 95f)

                val dYaw = (starRa - camYaw + 540f) % 360f - 180f
                val dPitch = starDec - camPitch

                if (abs(dYaw) < 80f && dPitch in -30f..80f) {
                    val sx = center.x + (dYaw / 80f) * (w / 2f)
                    val sy = center.y - (dPitch / 80f) * (h / 2f)

                    if (sy < horizonY) {
                        val twinkleFactor = 0.65f + 0.35f * sin(twinkleCycle * twinkleSpeed + (i * 1.7f))
                        val starAlpha = starDarkness * twinkleFactor * ((displayStars - i * 0.5f) / displayStars)

                        val starColor = when (i % 5) {
                            0 -> Color(0xFF90CAF9)
                            1 -> Color(0xFFFFE082)
                            2 -> Color(0xFFFFAB91)
                            else -> Color.White
                        }

                        val starSize = if (i % 12 == 0) 2.4f else 1.4f
                        drawCircle(
                            color = starColor.copy(alpha = starAlpha.coerceIn(0f, 1f)),
                            radius = starSize,
                            center = Offset(sx, sy)
                        )
                    }
                }
            }
        }

        // 3. Solar Disk & Projected Coordinates
        val sunDYaw = (sunAzimuth - camYaw + 540f) % 360f - 180f
        val sunDPitch = sunElevation - camPitch
        val isSunInFrustum = abs(sunDYaw) < 90f && sunDPitch in -30f..90f && sunElevation > -10f
        val sunCenter = if (isSunInFrustum) {
            val sx = center.x + (sunDYaw / 80f) * (w / 2f)
            val sy = center.y - (sunDPitch / 80f) * (h / 2f)
            Offset(sx, sy)
        } else null

        // 4. Lunar Orb & Phase Rendering
        val moonDYaw = (moonAzimuth - camYaw + 540f) % 360f - 180f
        val moonDPitch = moonElevation - camPitch

        if (abs(moonDYaw) < 90f && moonDPitch in -30f..90f && moonElevation > -10f) {
            val moonX = center.x + (moonDYaw / 80f) * (w / 2f)
            val moonY = center.y - (moonDPitch / 80f) * (h / 2f)
            if (moonY < horizonY) {
                val moonCenter = Offset(moonX, moonY)
                val moonRadius = 13f

                drawCircle(
                    color = Color(0xFF64748B).copy(alpha = (earthshine * 10f).coerceIn(0f, 0.4f)),
                    radius = moonRadius * 1.4f,
                    center = moonCenter
                )

                drawCircle(
                    color = Color(0xFF1E293B),
                    radius = moonRadius,
                    center = moonCenter
                )

                val phaseFactor = (1f - cos(moonPhase * 2f * PI.toFloat())) * 0.5f
                val moonLitColor = Color(0xFFE2E8F0)
                drawCircle(
                    color = moonLitColor.copy(alpha = phaseFactor.coerceIn(0.2f, 1f)),
                    radius = moonRadius * phaseFactor.coerceAtLeast(0.3f),
                    center = moonCenter
                )
            }
        }

        // 5. Crepuscular Sunbeams (Volumetric God Rays from Meteoros)
        if (enableGodRays && sunCenter != null && sunCenter.y < horizonY + 50f && godRaysIntensity > 0.05f) {
            val rayCount = 14
            val baseRayAlpha = (godRaysIntensity * (1.0f - abs(cloudCoverage - 0.55f) * 0.6f) * 0.35f).coerceIn(0.05f, 0.40f)
            val sunRayColor = if (sunElevation < 15f) Color(0xFFFDBA74) else Color(0xFFFFFBEB)

            clipRect(left = 0f, top = 0f, right = w, bottom = horizonY) {
                for (r in 0 until rayCount) {
                    val angleOffset = (r.toFloat() / rayCount) * 160f - 80f + sin(timeSeconds + r * 1.2f) * 3f
                    val rad = (angleOffset + 90f) * (PI.toFloat() / 180f)
                    val rayLength = h * 0.95f

                    val endX = sunCenter.x + cos(rad) * rayLength
                    val endY = sunCenter.y + sin(rad) * rayLength
                    val beamWidth = 22f + (r % 3) * 14f

                    val rayPath = Path().apply {
                        moveTo(sunCenter.x - beamWidth * 0.25f, sunCenter.y)
                        lineTo(sunCenter.x + beamWidth * 0.25f, sunCenter.y)
                        lineTo(endX + beamWidth * 1.4f, endY)
                        lineTo(endX - beamWidth * 1.4f, endY)
                        close()
                    }

                    val beamShimmer = 0.8f + 0.2f * sin(timeSeconds * 1.8f + r * 2.1f)
                    drawPath(
                        path = rayPath,
                        brush = Brush.linearGradient(
                            colors = listOf(
                                sunRayColor.copy(alpha = baseRayAlpha * beamShimmer),
                                sunRayColor.copy(alpha = baseRayAlpha * 0.45f * beamShimmer),
                                Color.Transparent
                            ),
                            start = sunCenter,
                            end = Offset(endX, endY)
                        )
                    )
                }
            }
        }

        // 6. High-Altitude Cirrus Ice Crystal Streaks
        if (enableCirrus && cirrusCoverage > 0.05f) {
            val cirrusAlpha = (cirrusCoverage * 0.45f).coerceIn(0.08f, 0.60f)
            val cirrusDrift = (timeSeconds * 6.0f) % (w * 1.5f)

            clipRect(left = 0f, top = 0f, right = w, bottom = horizonY * 0.75f) {
                for (c in 0..6) {
                    val baseY = horizonY * (0.06f + c * 0.065f)
                    val curvePath = Path().apply {
                        val startX = -w * 0.2f + (c * w * 0.28f + cirrusDrift) % (w * 1.4f)
                        moveTo(startX, baseY)
                        quadraticTo(
                            startX + w * 0.35f,
                            baseY - 18f + sin(timeSeconds * 0.5f + c) * 6f,
                            startX + w * 0.70f,
                            baseY + 14f
                        )
                    }

                    drawPath(
                        path = curvePath,
                        color = Color.White.copy(alpha = cirrusAlpha * (0.35f + (c % 3) * 0.2f)),
                        style = Stroke(width = 10f + (c % 4) * 6f, cap = StrokeCap.Round)
                    )
                }
            }
        }

        // 7. AmanSachan1/Meteoros Volumetric Raymarched Cloudscape Render
        if (cloudImage != null && cloudCoverage > 0.02f) {
            val cloudAlpha = (0.95f * cloudDensity).coerceIn(0.15f, 1.0f)
            drawImage(
                image = cloudImage,
                dstOffset = IntOffset(0, 0),
                dstSize = IntSize(w.toInt(), horizonY.toInt().coerceAtLeast(1)),
                alpha = cloudAlpha,
                filterQuality = FilterQuality.Medium
            )
        }

        // 8. Solar Disk & Corona Bloom (Layered over clouds for realistic extinction)
        if (sunCenter != null && sunCenter.y < horizonY + 30f) {
            val coronaRadius = 40f + coronaFlareIntensity * 32f
            drawCircle(
                brush = Brush.radialGradient(
                    colors = listOf(
                        Color(0xFFFFECB3).copy(alpha = (0.60f * coronaFlareIntensity).coerceIn(0f, 1f)),
                        Color(0xFFFF9800).copy(alpha = (0.22f * coronaFlareIntensity).coerceIn(0f, 1f)),
                        Color.Transparent
                    ),
                    center = sunCenter,
                    radius = coronaRadius * 2.2f
                ),
                radius = coronaRadius * 2.2f,
                center = sunCenter
            )

            val diskRadius = max(8f, sunAngularRadius * 360f)
            drawCircle(
                brush = Brush.radialGradient(
                    colors = listOf(
                        Color(0xFFFFFFFF),
                        Color(0xFFFFEE58),
                        Color(0xFFFF9800)
                    ),
                    center = sunCenter,
                    radius = diskRadius
                ),
                radius = diskRadius,
                center = sunCenter
            )
        }

        // 9. Natural Terrain Horizon or Shared Vulkan Scene Stage
        if (surfaceMode == SurfaceSceneMode.NONE) {
            // Immersive Natural Landscape Horizon (Meteoros default)
            val groundGrad = Brush.verticalGradient(
                colors = listOf(
                    Color(0xFF0F172A),
                    Color(0xFF090D16),
                    Color(0xFF04060A)
                ),
                startY = horizonY,
                endY = h
            )
            drawRect(
                brush = groundGrad,
                topLeft = Offset(0f, horizonY),
                size = Size(w, (h - horizonY).coerceAtLeast(0f))
            )

            // Distant rolling mountain silhouette / ground haze
            val mtnPath = Path().apply {
                moveTo(0f, horizonY)
                val segments = 8
                for (s in 0..segments) {
                    val px = s * (w / segments)
                    val py = horizonY - 12f - (sin(s * 1.1f + 0.4f) * 10f).coerceAtLeast(0f)
                    lineTo(px, py)
                }
                lineTo(w, horizonY)
                close()
            }
            drawPath(
                path = mtnPath,
                color = Color(0xFF090E17).copy(alpha = 0.85f)
            )

            // Atmospheric horizon haze line
            drawLine(
                brush = Brush.horizontalGradient(
                    colors = listOf(
                        Color(0x3338BDF8),
                        Color(0x6693C5FD),
                        Color(0x3338BDF8)
                    )
                ),
                start = Offset(0f, horizonY),
                end = Offset(w, horizonY),
                strokeWidth = 1.5f
            )
        } else {
            // Draw ground void backdrop
            drawRect(
                brush = Brush.verticalGradient(
                    colors = listOf(Color(0xFF0F172A), Color(0xFF05070C)),
                    startY = horizonY,
                    endY = h
                ),
                topLeft = Offset(0f, horizonY),
                size = Size(w, (h - horizonY).coerceAtLeast(0f))
            )

            // 3D Camera Projection for the Shared Vulkan Scene Stage
            val radY = (camYaw * (PI / 180.0)).toFloat()
            val radX = (camPitch * (PI / 180.0)).toFloat()
            val cosY = cos(radY)
            val sinY = sin(radY)
            val cosX = cos(radX)
            val sinX = sin(radX)
            val cameraDist = 5.2f
            val fovFactor = 520f
            val cyStage = horizonY + 95f

            fun projectStagePoint(v: Vec3D): Point2D {
                val x1 = v.x * cosY + v.z * sinY
                val z1 = -v.x * sinY + v.z * cosY

                val y2 = v.y * cosX - z1 * sinX
                val z2 = v.y * sinX + z1 * cosX

                // Sascha Willems Vulkan depth: camera at +cameraDist looking along -Z
                val zView = (cameraDist - z2).coerceAtLeast(0.1f)
                val px = center.x + (x1 / zView) * fovFactor
                val py = cyStage - (y2 / zView) * fovFactor
                return Point2D(px, py, zView)
            }

            fun transformStageNormal(rawNorm: Vec3D): Vec3D {
                val nx1 = rawNorm.x * cosY + rawNorm.z * sinY
                val nz1 = -rawNorm.x * sinY + rawNorm.z * cosY
                val ny2 = rawNorm.y * cosX - nz1 * sinX
                val nz2 = rawNorm.y * sinX + nz1 * cosX
                return Vec3D(nx1, ny2, nz2)
            }

            // Directional sun light for stage shadows & PBR shading
            val elRad = (sunElevation * (PI / 180.0)).toFloat()
            val azRad = (sunAzimuth * (PI / 180.0)).toFloat()
            val lightX = cos(elRad) * sin(azRad)
            val lightY = sin(elRad)
            val lightZ = cos(elRad) * cos(azRad)
            val normLight = Vec3D(lightX, lightY, lightZ)
            val sunLightTint = if (sunElevation < 20f) Color(0xFFFDBA74) else Color.White

            // Ground perspective grid for horizon depth cue
            if (showStageGrid && surfaceMode != SurfaceSceneMode.NONE) {
                val gridColor = Color(0x3394A3B8)
                val halfW = (stageWidth * 0.5f).coerceIn(2.0f, 25.0f)
                val halfL = (stageLength * 0.5f).coerceIn(2.0f, 25.0f)
                val steps = 8
                val stepX = (halfW * 2f) / steps
                val stepZ = (halfL * 2f) / steps

                for (i in 0..steps) {
                    val gx = -halfW + i * stepX
                    val pA = projectStagePoint(Vec3D(gx, 0f, -halfL))
                    val pB = projectStagePoint(Vec3D(gx, 0f, halfL))
                    if (pA.depth > 0.1f && pB.depth > 0.1f) {
                        drawLine(
                            color = gridColor,
                            start = Offset(pA.x, pA.y),
                            end = Offset(pB.x, pB.y),
                            strokeWidth = 1.2f
                        )
                    }
                }
                for (j in 0..steps) {
                    val gz = -halfL + j * stepZ
                    val pA = projectStagePoint(Vec3D(-halfW, 0f, gz))
                    val pB = projectStagePoint(Vec3D(halfW, 0f, gz))
                    if (pA.depth > 0.1f && pB.depth > 0.1f) {
                        drawLine(
                            color = gridColor,
                            start = Offset(pA.x, pA.y),
                            end = Offset(pB.x, pB.y),
                            strokeWidth = 1.2f
                        )
                    }
                }
            }

            // Render 3D Calibration Cube only if explicitly requested
            if (showCalibrationItem) {
                drawStageCalibrationItem(
                    projectPoint = ::projectStagePoint,
                    normLight = normLight,
                    sunLightTint = sunLightTint,
                    sunElevation = sunElevation
                )
            }
        }

        // 10. Cardinal Compass Markers along horizon
        val cardinalPoints = listOf(
            "N" to 0f, "NE" to 45f, "E" to 90f, "SE" to 135f,
            "S" to 180f, "SW" to 225f, "W" to 270f, "NW" to 315f
        )
        for ((name, az) in cardinalPoints) {
            val dAz = (az - camYaw + 540f) % 360f - 180f
            if (abs(dAz) < 70f) {
                val cx = center.x + (dAz / 80f) * (w / 2f)
                drawLine(
                    color = Color(0xFF64748B),
                    start = Offset(cx, horizonY - 4f),
                    end = Offset(cx, horizonY + 4f),
                    strokeWidth = 1.5f
                )
                val labelLayout = textMeasurer.measure(
                    text = name,
                    style = TextStyle(
                        color = if (name == "N") Color(0xFF38BDF8) else Color(0xFF94A3B8),
                        fontSize = 9.sp,
                        fontWeight = FontWeight.Bold,
                        fontFamily = FontFamily.Monospace
                    )
                )
                drawText(
                    textLayoutResult = labelLayout,
                    topLeft = Offset(cx - labelLayout.size.width / 2f, horizonY + 6f)
                )
            }
        }

        // 11. Interactive Cloud Detection HUD Overlays (Brackets, Badges, Telemetry)
        if (isDetectionEnabled) {
            for (formation in detectedFormations) {
                if (selectedFilterGenus != null && formation.genus != selectedFilterGenus) continue

                val proj = CloudDetectionRepository.projectFormation(
                    formation = formation,
                    screenWidth = w,
                    screenHeight = h,
                    camYaw = camYaw,
                    camPitch = camPitch
                )

                if (proj.isVisibleInFrustum && proj.screenRect.bottom <= horizonY + 50f) {
                    val isSelected = formation.id == selectedFormationId
                    drawCloudDetectionOverlay(
                        proj = proj,
                        textMeasurer = textMeasurer,
                        isSelected = isSelected
                    )
                }
            }
        }
    }
}

/**
 * Draws high-precision LiDAR & CV cloud detection bounding brackets, genus badge, and telemetry
 */
private fun DrawScope.drawCloudDetectionOverlay(
    proj: ProjectedCloudDetection,
    textMeasurer: TextMeasurer,
    isSelected: Boolean
) {
    val rect = proj.screenRect
    val genus = proj.formation.genus
    val baseColor = if (isSelected) Color(0xFFFBBF24) else genus.badgeColor
    val bracketLen = min(rect.width * 0.22f, rect.height * 0.22f).coerceAtLeast(10f)
    val strokeWidth = if (isSelected) 2.5f else 1.6f

    val l = rect.left
    val t = rect.top
    val r = rect.right
    val b = rect.bottom

    // 1. Sleek four-corner targeting brackets
    // Top-Left
    drawLine(baseColor, Offset(l, t), Offset(l + bracketLen, t), strokeWidth)
    drawLine(baseColor, Offset(l, t), Offset(l, t + bracketLen), strokeWidth)

    // Top-Right
    drawLine(baseColor, Offset(r, t), Offset(r - bracketLen, t), strokeWidth)
    drawLine(baseColor, Offset(r, t), Offset(r, t + bracketLen), strokeWidth)

    // Bottom-Left
    drawLine(baseColor, Offset(l, b), Offset(l + bracketLen, b), strokeWidth)
    drawLine(baseColor, Offset(l, b), Offset(l, b - bracketLen), strokeWidth)

    // Bottom-Right
    drawLine(baseColor, Offset(r, b), Offset(r - bracketLen, b), strokeWidth)
    drawLine(baseColor, Offset(r, b), Offset(r, b - bracketLen), strokeWidth)

    // 2. Center Targeting Reticle
    val crosshairLen = 8f
    val cx = rect.center.x
    val cy = rect.center.y
    val crossColor = baseColor.copy(alpha = if (isSelected) 0.9f else 0.5f)
    drawLine(crossColor, Offset(cx - crosshairLen, cy), Offset(cx + crosshairLen, cy), 1.2f)
    drawLine(crossColor, Offset(cx, cy - crosshairLen), Offset(cx, cy + crosshairLen), 1.2f)
    if (isSelected) {
        drawCircle(
            color = baseColor.copy(alpha = 0.25f),
            radius = 12f,
            center = rect.center,
            style = Stroke(width = 1.2f)
        )
    }

    // 3. Top Header Label Badge: [Genus Code] Name & Confidence
    val headerText = "[${genus.symbolCode}] ${proj.formation.speciesVariant}"
    val headerLayout = textMeasurer.measure(
        text = headerText,
        style = TextStyle(
            color = Color.White,
            fontSize = 9.sp,
            fontWeight = FontWeight.Bold,
            fontFamily = FontFamily.Monospace
        )
    )

    val badgePadH = 6f
    val badgePadV = 3f
    val badgeW = headerLayout.size.width + badgePadH * 2f
    val badgeH = headerLayout.size.height + badgePadV * 2f
    val badgeTop = (t - badgeH - 3f).coerceAtLeast(4f)

    drawRoundRect(
        color = baseColor.copy(alpha = if (isSelected) 0.95f else 0.85f),
        topLeft = Offset(l, badgeTop),
        size = Size(badgeW, badgeH),
        cornerRadius = CornerRadius(3f, 3f)
    )
    drawText(
        textLayoutResult = headerLayout,
        topLeft = Offset(l + badgePadH, badgeTop + badgePadV)
    )

    // 4. Telemetry Tag below bounding box: Altitudes, Optical Depth, Radar
    val subText = "${proj.formation.baseAltitudeMeters.toInt()}-${proj.formation.topAltitudeMeters.toInt()}m • τ:${proj.formation.opticalDepthTau.toInt()} • ${(proj.formation.confidenceScore * 100).toInt()}%"
    val subLayout = textMeasurer.measure(
        text = subText,
        style = TextStyle(
            color = Color(0xFFE2E8F0),
            fontSize = 8.sp,
            fontFamily = FontFamily.Monospace
        )
    )

    drawRoundRect(
        color = Color(0xDD0F172A),
        topLeft = Offset(l, b + 3f),
        size = Size(subLayout.size.width + 8f, subLayout.size.height + 4f),
        cornerRadius = CornerRadius(3f, 3f)
    )
    drawText(
        textLayoutResult = subLayout,
        topLeft = Offset(l + 4f, b + 5f)
    )
}

/**
 * Renders a 3D PBR calibration cube resting on top of the shared Vulkan Stage platform.
 * Catches directional sunlight and celestial skylight matching the Preetham/Hosek-Wilkie model.
 */
private fun DrawScope.drawStageCalibrationItem(
    projectPoint: (Vec3D) -> Point2D,
    normLight: Vec3D,
    sunLightTint: Color,
    sunElevation: Float
) {
    val s = 0.55f
    val yBase = 0f
    val yTop = yBase + 1.1f

    val v0 = projectPoint(Vec3D(-s, yTop, -s))
    val v1 = projectPoint(Vec3D(s, yTop, -s))
    val v2 = projectPoint(Vec3D(s, yTop, s))
    val v3 = projectPoint(Vec3D(-s, yTop, s))

    val v4 = projectPoint(Vec3D(-s, yBase, -s))
    val v5 = projectPoint(Vec3D(s, yBase, -s))
    val v6 = projectPoint(Vec3D(s, yBase, s))
    val v7 = projectPoint(Vec3D(-s, yBase, s))

    val quads = listOf(
        FaceQuad(v3, v2, v6, v7, (v3.depth + v2.depth + v6.depth + v7.depth) / 4f, Vec3D(0f, 0f, 1f), 0),
        FaceQuad(v1, v0, v4, v5, (v1.depth + v0.depth + v4.depth + v5.depth) / 4f, Vec3D(0f, 0f, -1f), 1),
        FaceQuad(v0, v1, v2, v3, (v0.depth + v1.depth + v2.depth + v3.depth) / 4f, Vec3D(0f, 1f, 0f), 2),
        FaceQuad(v0, v3, v7, v4, (v0.depth + v3.depth + v7.depth + v4.depth) / 4f, Vec3D(-1f, 0f, 0f), 3),
        FaceQuad(v2, v1, v5, v6, (v2.depth + v1.depth + v5.depth + v6.depth) / 4f, Vec3D(1f, 0f, 0f), 4)
    ).sortedByDescending { it.avgDepth }

    val baseMatColor = Color(0xFF0284C7)
    val castShadowsActive = sunElevation > 0f

    for (quad in quads) {
        val dot = (quad.normal.x * normLight.x + quad.normal.y * normLight.y + quad.normal.z * normLight.z)
            .coerceAtLeast(0.0f)
        val ambient = 0.35f
        val diffuse = if (castShadowsActive && dot <= 0.001f) 0.0f else 0.65f * dot
        val shade = (ambient + diffuse).coerceIn(0.2f, 1.0f)

        val faceColor = Color(
            red = (baseMatColor.red * sunLightTint.red * shade).coerceIn(0f, 1f),
            green = (baseMatColor.green * sunLightTint.green * shade).coerceIn(0f, 1f),
            blue = (baseMatColor.blue * sunLightTint.blue * shade).coerceIn(0f, 1f),
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

        // Draw calibration wire grid on faces
        val wireColor = Color.White.copy(alpha = 0.30f)
        val mid01 = Offset((quad.p0.x + quad.p1.x) / 2f, (quad.p0.y + quad.p1.y) / 2f)
        val mid23 = Offset((quad.p2.x + quad.p3.x) / 2f, (quad.p2.y + quad.p3.y) / 2f)
        val mid12 = Offset((quad.p1.x + quad.p2.x) / 2f, (quad.p1.y + quad.p2.y) / 2f)
        val mid30 = Offset((quad.p3.x + quad.p0.x) / 2f, (quad.p3.y + quad.p0.y) / 2f)
        drawLine(wireColor, mid01, mid23, strokeWidth = 1.0f)
        drawLine(wireColor, mid12, mid30, strokeWidth = 1.0f)

        // Face outline
        drawPath(path = path, color = Color.Black.copy(alpha = 0.4f), style = Stroke(width = 1.2f))
    }
}
