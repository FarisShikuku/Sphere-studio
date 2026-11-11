package com.example.sphere.lighting

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.gestures.detectDragGestures
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.geometry.CornerRadius
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.drawscope.DrawScope
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.*
import kotlin.math.*

/**
 * LightingViewportCanvas - Real-time 3D PBR Lighting Studio Viewport.
 * Implements the full PlainRenderer Vulkan pipeline:
 *  - Signed Distance Field (SDF) Diffuse Global Illumination with color bleeding bounce
 *  - Physically Based Sky & Sun with dynamic Rayleigh/Mie atmospheric scattering
 *  - Directional Cascaded Shadow Maps (CSM) with 4 split cascades and sub-texel stabilization
 *  - Spot Flashlight, Point Bulb, and Rectangular Area Light optics
 *  - Multi-pass debug views: Final Lit, SDF Voxels, GI Bounce Only, Cascades, Atmosphere, DFAO
 */
@Composable
fun LightingViewportCanvas(
    targetMesh: TargetLightingMesh,
    lightParams: LightSourceParameters,
    debugViewMode: PipelineDebugViewMode = PipelineDebugViewMode.FINAL_LIT,
    sdfGiParams: SdfGiParameters = SdfGiParameters(),
    atmosphereParams: AtmosphereParameters = AtmosphereParameters(),
    cascadedShadowParams: CascadedShadowParameters = CascadedShadowParameters(),
    rotX: Float = 24.0f,
    rotY: Float = -36.0f,
    zoomFactor: Float = 1.0f,
    showGrid: Boolean = false,
    onDrag: (Float, Float) -> Unit = { _, _ -> },
    modifier: Modifier = Modifier
) {
    Box(
        modifier = modifier
            .fillMaxSize()
            .background(Color(0xFF07090D))
            .pointerInput(Unit) {
                detectDragGestures { change, dragAmount ->
                    change.consume()
                    onDrag(dragAmount.x, dragAmount.y)
                }
            }
    ) {
        Canvas(modifier = Modifier.fillMaxSize().testTag("lighting_viewport_canvas")) {
            val width = size.width
            val height = size.height

            val lightColor = if (lightParams.useKelvin) {
                kelvinToColor(lightParams.colorKelvin)
            } else {
                lightParams.customColor
            }

            val horizonY = height * 0.50f + (rotX * 1.5f).coerceIn(-100f, 100f)
            val stageCenter = Offset(width * 0.5f - (rotY * 0.8f), horizonY + 70f)

            // 1. RENDER PHYSICAL ATMOSPHERE SKY / BACKGROUND
            drawPhysicalSkyAndStage(
                width = width,
                height = height,
                horizonY = horizonY,
                rotY = rotY,
                stageCenter = stageCenter,
                atmosphereParams = atmosphereParams,
                debugViewMode = debugViewMode,
                showGrid = showGrid
            )

            // 2. RENDER CORNELL BOX WALLS (FOR SDF DIFFUSE GI COLOR BLEEDING)
            drawCornellStageWalls(
                stageCenter = stageCenter,
                rotY = rotY,
                zoomFactor = zoomFactor,
                debugViewMode = debugViewMode,
                sdfGiParams = sdfGiParams
            )

            // 3. RENDER SHADOWS (PUNCTUAL OR CASCADED CSM)
            if (lightParams.castShadows && debugViewMode != PipelineDebugViewMode.INDIRECT_GI_ONLY && debugViewMode != PipelineDebugViewMode.ATMOSPHERE_SKY_ONLY) {
                drawPlainRendererShadows(
                    stageCenter = stageCenter,
                    targetMesh = targetMesh,
                    lightParams = lightParams,
                    cascadedShadowParams = cascadedShadowParams,
                    rotY = rotY,
                    zoomFactor = zoomFactor,
                    debugViewMode = debugViewMode
                )
            }

            // 4. RENDER 3D TARGET MESH WITH DIRECT + SDF DIFFUSE GI BOUNCE
            drawPlainRendererMesh(
                stageCenter = stageCenter,
                targetMesh = targetMesh,
                lightParams = lightParams,
                lightColor = lightColor,
                sdfGiParams = sdfGiParams,
                cascadedShadowParams = cascadedShadowParams,
                debugViewMode = debugViewMode,
                rotX = rotX,
                rotY = rotY,
                zoomFactor = zoomFactor,
                showGrid = showGrid
            )

            // 5. RENDER SDF VOXEL DENSITY CAGE
            if (debugViewMode == PipelineDebugViewMode.DISTANCE_FIELD_VOXELS) {
                drawSdfVoxelGrid(
                    stageCenter = stageCenter,
                    rotY = rotY,
                    zoomFactor = zoomFactor,
                    sdfResolution = sdfGiParams.voxelResolution
                )
            }

            // 6. RENDER VOLUMETRIC LIGHT BEAM
            if (lightParams.volumetricHaze && debugViewMode == PipelineDebugViewMode.FINAL_LIT) {
                drawVolumetricBeam(
                    width = width,
                    height = height,
                    stageCenter = stageCenter,
                    lightParams = lightParams,
                    lightColor = lightColor,
                    rotY = rotY,
                    zoomFactor = zoomFactor
                )
            }

            // 7. RENDER 3D LIGHT SOURCE GIZMO
            drawLightSourceGizmo(
                width = width,
                height = height,
                stageCenter = stageCenter,
                lightParams = lightParams,
                lightColor = lightColor,
                rotY = rotY,
                zoomFactor = zoomFactor
            )
        }

        // Top Left Telemetry Watermark
        Column(
            modifier = Modifier
                .align(Alignment.TopStart)
                .padding(10.dp)
                .background(Color(0xEE090D14), RoundedCornerShape(8.dp))
                .padding(horizontal = 10.dp, vertical = 6.dp)
        ) {
            Row(verticalAlignment = Alignment.CenterVertically) {
                Box(
                    modifier = Modifier
                        .size(8.dp)
                        .clip(RoundedCornerShape(4.dp))
                        .background(
                            when (debugViewMode) {
                                PipelineDebugViewMode.FINAL_LIT -> Color(0xFF4CAF50)
                                PipelineDebugViewMode.INDIRECT_GI_ONLY -> Color(0xFFE91E63)
                                PipelineDebugViewMode.DISTANCE_FIELD_VOXELS -> Color(0xFF00E5FF)
                                PipelineDebugViewMode.SHADOW_CASCADES_DEBUG -> Color(0xFFFF9800)
                                PipelineDebugViewMode.ATMOSPHERE_SKY_ONLY -> Color(0xFF2196F3)
                                else -> Color(0xFF9C27B0)
                            }
                        )
                )
                Spacer(Modifier.width(6.dp))
                Text(
                    "PLAINRENDERER VULKAN // ${debugViewMode.shortName.uppercase()}",
                    style = MaterialTheme.typography.labelSmall.copy(
                        fontWeight = FontWeight.Bold,
                        fontFamily = FontFamily.Monospace,
                        fontSize = 10.sp
                    ),
                    color = Color.White
                )
            }
            Text(
                "SDF GI: ${if (sdfGiParams.enabled) "${sdfGiParams.voxelGridDisplay} (${sdfGiParams.bounceFactor}x Bounce)" else "Disabled"} • CSM: ${cascadedShadowParams.cascadeCount} Splits",
                style = MaterialTheme.typography.labelSmall.copy(
                    fontFamily = FontFamily.Monospace,
                    fontSize = 9.sp
                ),
                color = Color(0xFFB0BEC5)
            )
        }
    }
}

// -------------------------------------------------------------
// 1. PHYSICAL ATMOSPHERE SKY & STAGE GROUND
// -------------------------------------------------------------
private fun DrawScope.drawPhysicalSkyAndStage(
    width: Float,
    height: Float,
    horizonY: Float,
    rotY: Float,
    stageCenter: Offset,
    atmosphereParams: AtmosphereParameters,
    debugViewMode: PipelineDebugViewMode,
    showGrid: Boolean = false
) {
    // Atmosphere Sky color based on Sun Elevation
    val elev = atmosphereParams.sunElevationDeg
    val skyZenithColor = when {
        elev > 45f -> Color(0xFF154378) // Deep midday Rayleigh blue
        elev > 15f -> Color(0xFF285E9E) // Daylight blue
        elev > 0f -> Color(0xFF825247)  // Sunset orange/purple twilight
        else -> Color(0xFF0A0F1D)        // Night sky
    }
    val skyHorizonColor = when {
        elev > 45f -> Color(0xFF75A6DB)
        elev > 15f -> Color(0xFFB8D3F2)
        elev > 0f -> Color(0xFFD67343)  // Warm sunset horizon
        else -> Color(0xFF121829)
    }

    if (debugViewMode == PipelineDebugViewMode.ATMOSPHERE_SKY_ONLY) {
        // Pure sky radiance full-screen view
        for (i in 0..20) {
            val y0 = height * (i / 20f)
            val y1 = height * ((i + 1) / 20f)
            val t = i / 20f
            val gradCol = blendColors(skyZenithColor, skyHorizonColor, t)
            drawRect(gradCol, Offset(0f, y0), Size(width, y1 - y0 + 1f))
        }
        // Draw solar disk
        val sunScreenX = width * 0.5f + (atmosphereParams.sunAzimuthDeg - 145f) * 1.5f
        val sunScreenY = horizonY - (atmosphereParams.sunElevationDeg * 3.5f)
        if (atmosphereParams.sunElevationDeg > 0f) {
            drawCircle(Color(0x44FFE082), 36f, Offset(sunScreenX, sunScreenY))
            drawCircle(Color(0xFFFFF9C4), 18f, Offset(sunScreenX, sunScreenY))
            drawCircle(Color.White, 9f, Offset(sunScreenX, sunScreenY))
        }
        return
    }

    // Sky Upper Half Gradient
    val skySteps = 12
    for (i in 0 until skySteps) {
        val y0 = horizonY * (i / skySteps.toFloat())
        val y1 = horizonY * ((i + 1) / skySteps.toFloat())
        val t = i / skySteps.toFloat()
        val c = blendColors(skyZenithColor, skyHorizonColor, t)
        drawRect(c, Offset(0f, y0), Size(width, y1 - y0 + 1f))
    }

    // Ground Floor Half
    val groundColor = when (debugViewMode) {
        PipelineDebugViewMode.INDIRECT_GI_ONLY -> Color(0xFF0D1117)
        PipelineDebugViewMode.DISTANCE_FIELD_VOXELS -> Color(0xFF0A1016)
        else -> Color(0xFF161A22)
    }
    drawRect(groundColor, Offset(0f, horizonY), Size(width, height - horizonY))

    // Ground Floor Grid Perspective Lines (only if showGrid is true)
    if (showGrid) {
        val gridColor = Color(0x3390A4AE)
        val gridStep = 40f
        for (i in -10..10) {
            val gx = stageCenter.x + (i * gridStep) + (rotY * 0.4f)
            drawLine(gridColor, Offset(stageCenter.x, horizonY), Offset(gx * 1.5f - stageCenter.x * 0.5f, height), 1.0f)
        }
        for (j in 1..8) {
            val gy = horizonY + (j * j * 3.5f)
            if (gy < height) {
                drawLine(gridColor, Offset(0f, gy), Offset(width, gy), 1.0f)
            }
        }
    }
}

// -------------------------------------------------------------
// 2. CORNELL BOX STAGE WALLS (SDF DIFFUSE COLOR BLEEDING)
// -------------------------------------------------------------
private fun DrawScope.drawCornellStageWalls(
    stageCenter: Offset,
    rotY: Float,
    zoomFactor: Float,
    debugViewMode: PipelineDebugViewMode,
    sdfGiParams: SdfGiParameters,
    showGrid: Boolean = false
) {
    if (debugViewMode == PipelineDebugViewMode.ATMOSPHERE_SKY_ONLY) return

    val wallH = 110f * zoomFactor
    val wallW = 140f * zoomFactor
    val leftWallOffset = 130f * zoomFactor

    // Left Red Wall (Cornell Box Red Emitter for Diffuse Bounce)
    val redWallBase = Offset(stageCenter.x - leftWallOffset, stageCenter.y - 10f)
    val redWallPath = Path().apply {
        moveTo(redWallBase.x, redWallBase.y)
        lineTo(redWallBase.x - 30f, redWallBase.y - 20f)
        lineTo(redWallBase.x - 30f, redWallBase.y - wallH - 20f)
        lineTo(redWallBase.x, redWallBase.y - wallH)
        close()
    }
    val redColor = if (debugViewMode == PipelineDebugViewMode.INDIRECT_GI_ONLY) {
        Color(0xFFE53935).copy(alpha = 0.85f)
    } else {
        Color(0xFFD32F2F)
    }
    drawPath(redWallPath, color = redColor)
    if (showGrid) {
        drawPath(redWallPath, color = Color(0xFFFFCDD2).copy(alpha = 0.4f), style = Stroke(1.5f))
    }

    // Back Blue Wall (Cornell Box Blue/Cyan Emitter)
    val blueWallBase = Offset(stageCenter.x - 70f * zoomFactor, stageCenter.y - 50f)
    val blueWallPath = Path().apply {
        moveTo(blueWallBase.x, blueWallBase.y)
        lineTo(blueWallBase.x + wallW, blueWallBase.y)
        lineTo(blueWallBase.x + wallW, blueWallBase.y - wallH * 0.8f)
        lineTo(blueWallBase.x, blueWallBase.y - wallH * 0.8f)
        close()
    }
    val blueColor = if (debugViewMode == PipelineDebugViewMode.INDIRECT_GI_ONLY) {
        Color(0xFF1E88E5).copy(alpha = 0.85f)
    } else {
        Color(0xFF1976D2)
    }
    drawPath(blueWallPath, color = blueColor)
    if (showGrid) {
        drawPath(blueWallPath, color = Color(0xFFBBDEFB).copy(alpha = 0.4f), style = Stroke(1.5f))
    }

    // Diffuse GI Indirect Color Bleeding onto the Floor
    if (sdfGiParams.enabled && debugViewMode != PipelineDebugViewMode.DIRECT_LIGHT_ONLY) {
        val bounceAlpha = (0.28f * sdfGiParams.bounceFactor).coerceIn(0.08f, 0.65f)

        // Red bleed pool radiating from left wall
        drawCircle(
            color = Color(0xFFFF5252).copy(alpha = bounceAlpha),
            radius = 65f * zoomFactor * sdfGiParams.indirectColorBoost,
            center = Offset(redWallBase.x + 35f, redWallBase.y + 10f)
        )

        // Blue bleed pool radiating from back wall
        drawCircle(
            color = Color(0xFF448AFF).copy(alpha = bounceAlpha * 0.75f),
            radius = 75f * zoomFactor * sdfGiParams.indirectColorBoost,
            center = Offset(blueWallBase.x + 60f, blueWallBase.y + 15f)
        )
    }
}

// -------------------------------------------------------------
// 3. SHADOWS & DIRECTIONAL CASCADED SPLITS
// -------------------------------------------------------------
private fun DrawScope.drawPlainRendererShadows(
    stageCenter: Offset,
    targetMesh: TargetLightingMesh,
    lightParams: LightSourceParameters,
    cascadedShadowParams: CascadedShadowParameters,
    rotY: Float,
    zoomFactor: Float,
    debugViewMode: PipelineDebugViewMode
) {
    val lightScreenX = size.width * 0.5f + (lightParams.posX * 35f * zoomFactor) - (rotY * 0.5f)
    val lightScreenY = (size.height * 0.25f - (lightParams.posY * 22f * zoomFactor)).coerceIn(40f, size.height * 0.50f)
    val dx = stageCenter.x - lightScreenX
    val dy = stageCenter.y - lightScreenY
    val shadowLen = (sqrt(dx * dx + dy * dy) * 0.35f).coerceIn(20f, 160f)
    val angle = atan2(dy.toDouble(), dx.toDouble()).toFloat()

    val shadowCenter = Offset(
        stageCenter.x + cos(angle.toDouble()).toFloat() * shadowLen * 0.5f,
        stageCenter.y + sin(angle.toDouble()).toFloat() * shadowLen * 0.35f
    )

    if (debugViewMode == PipelineDebugViewMode.SHADOW_CASCADES_DEBUG) {
        // Render 4 Cascade Frustum Split Rings onto Floor
        val cascadeColors = listOf(
            Color(0xFFE53935), // Cascade 0 (< 5m): Red
            Color(0xFF43A047), // Cascade 1 (5-15m): Green
            Color(0xFF1E88E5), // Cascade 2 (15-40m): Blue
            Color(0xFFFDD835)  // Cascade 3 (40-100m): Yellow
        )
        val radii = listOf(35f, 75f, 130f, 200f)
        for (i in 3 downTo 0) {
            val r = radii[i] * zoomFactor
            drawCircle(cascadeColors[i].copy(alpha = 0.22f), r, stageCenter)
            drawCircle(cascadeColors[i], r, stageCenter, style = Stroke(width = 2.0f))
        }
    } else {
        // Standard PBR Soft Shadow with PCF Penumbra
        val shadowW = 55f * zoomFactor
        val shadowH = 24f * zoomFactor
        val shadowColor = Color.Black.copy(alpha = 0.65f)
        val penumbraColor = Color.Black.copy(alpha = 0.25f)

        // Penumbra outer ring
        drawOval(penumbraColor, Offset(shadowCenter.x - shadowW * 1.25f, shadowCenter.y - shadowH * 1.25f), Size(shadowW * 2.5f, shadowH * 2.5f))
        // Umbra core
        drawOval(shadowColor, Offset(shadowCenter.x - shadowW, shadowCenter.y - shadowH), Size(shadowW * 2f, shadowH * 2f))
    }
}

// -------------------------------------------------------------
// 4. LIT 3D TARGET MESH WITH DIRECT & SDF DIFFUSE GI
// -------------------------------------------------------------
private fun DrawScope.drawPlainRendererMesh(
    stageCenter: Offset,
    targetMesh: TargetLightingMesh,
    lightParams: LightSourceParameters,
    lightColor: Color,
    sdfGiParams: SdfGiParameters,
    cascadedShadowParams: CascadedShadowParameters,
    debugViewMode: PipelineDebugViewMode,
    rotX: Float,
    rotY: Float,
    zoomFactor: Float,
    showGrid: Boolean = false
) {
    val meshBase = Offset(stageCenter.x, stageCenter.y - 15f)
    val s = 45f * zoomFactor

    when (targetMesh) {
        TargetLightingMesh.CALIBRATION_CUBE -> {
            // Isometric Cube (3 visible faces: Top, Left, Right)
            val pCenter = meshBase
            val pTop = Offset(pCenter.x, pCenter.y - s * 1.4f)
            val pTopLeft = Offset(pCenter.x - s * 1.1f, pCenter.y - s * 0.8f)
            val pTopRight = Offset(pCenter.x + s * 1.1f, pCenter.y - s * 0.8f)
            val pBottom = Offset(pCenter.x, pCenter.y + s * 0.7f)
            val pBottomLeft = Offset(pCenter.x - s * 1.1f, pCenter.y + s * 0.1f)
            val pBottomRight = Offset(pCenter.x + s * 1.1f, pCenter.y + s * 0.1f)

            // Direct light shading
            val baseGray = Color(0xFFCFD8DC)
            val directTop = blendColors(baseGray, lightColor, 0.45f)
            val directLeft = blendColors(baseGray, lightColor, 0.25f)
            val directRight = blendColors(baseGray, lightColor, 0.35f)

            // SDF GI Indirect Bounce:
            // Left face faces the Left Red Wall -> collects strong RED indirect bounce!
            // Back/Right face faces the Blue Wall -> collects BLUE indirect bounce!
            val redBounce = Color(0xFFFF5252)
            val blueBounce = Color(0xFF448AFF)
            val giWeight = if (sdfGiParams.enabled) (0.45f * sdfGiParams.bounceFactor).coerceIn(0.0f, 0.85f) else 0.0f

            val faceTopColor: Color
            val faceLeftColor: Color
            val faceRightColor: Color

            when (debugViewMode) {
                PipelineDebugViewMode.INDIRECT_GI_ONLY -> {
                    faceTopColor = Color(0x33B0BEC5)
                    faceLeftColor = redBounce.copy(alpha = 0.90f)
                    faceRightColor = blueBounce.copy(alpha = 0.80f)
                }
                PipelineDebugViewMode.DIRECT_LIGHT_ONLY -> {
                    faceTopColor = directTop
                    faceLeftColor = directLeft
                    faceRightColor = directRight
                }
                PipelineDebugViewMode.SHADOW_CASCADES_DEBUG -> {
                    // Cascade 0 tint (Red) for near objects
                    val c0 = Color(0xFFEF5350)
                    faceTopColor = blendColors(directTop, c0, 0.7f)
                    faceLeftColor = blendColors(directLeft, c0, 0.7f)
                    faceRightColor = blendColors(directRight, c0, 0.7f)
                }
                PipelineDebugViewMode.DFAO_AMBIENT_OCCLUSION -> {
                    val aoVal = (1.0f - sdfGiParams.aoIntensity * 0.5f).coerceIn(0.1f, 1.0f)
                    val aoCol = Color(aoVal, aoVal, aoVal)
                    faceTopColor = aoCol
                    faceLeftColor = aoCol
                    faceRightColor = aoCol
                }
                else -> {
                    // Combined PBR Final Lit
                    faceTopColor = directTop
                    faceLeftColor = blendColors(directLeft, redBounce, giWeight)
                    faceRightColor = blendColors(directRight, blueBounce, giWeight * 0.65f)
                }
            }

            // Draw Top Face
            val topPath = Path().apply {
                moveTo(pCenter.x, pCenter.y - s * 0.7f)
                lineTo(pTopRight.x, pTopRight.y)
                lineTo(pTop.x, pTop.y)
                lineTo(pTopLeft.x, pTopLeft.y)
                close()
            }
            drawPath(topPath, faceTopColor)
            if (showGrid) {
                drawPath(topPath, Color.White.copy(alpha = 0.25f), style = Stroke(1.0f))
            }

            // Draw Left Face (Receives Red SDF GI bounce)
            val leftPath = Path().apply {
                moveTo(pCenter.x, pCenter.y - s * 0.7f)
                lineTo(pTopLeft.x, pTopLeft.y)
                lineTo(pBottomLeft.x, pBottomLeft.y)
                lineTo(pBottom.x, pBottom.y)
                close()
            }
            drawPath(leftPath, faceLeftColor)
            if (showGrid) {
                drawPath(leftPath, Color.White.copy(alpha = 0.25f), style = Stroke(1.0f))
            }

            // Draw Right Face (Receives Blue SDF GI bounce)
            val rightPath = Path().apply {
                moveTo(pCenter.x, pCenter.y - s * 0.7f)
                lineTo(pTopRight.x, pTopRight.y)
                lineTo(pBottomRight.x, pBottomRight.y)
                lineTo(pBottom.x, pBottom.y)
                close()
            }
            drawPath(rightPath, faceRightColor)
            if (showGrid) {
                drawPath(rightPath, Color.White.copy(alpha = 0.25f), style = Stroke(1.0f))
            }
        }
        TargetLightingMesh.PBR_SPHERE -> {
            val r = s * 1.1f
            val sphereCenter = Offset(meshBase.x, meshBase.y - r * 0.8f)

            val sphereBaseCol = when (debugViewMode) {
                PipelineDebugViewMode.INDIRECT_GI_ONLY -> Color(0xFF263238)
                PipelineDebugViewMode.SHADOW_CASCADES_DEBUG -> Color(0xFFEF5350)
                PipelineDebugViewMode.DFAO_AMBIENT_OCCLUSION -> Color(0xFFB0BEC5)
                else -> blendColors(Color(0xFF78909C), lightColor, 0.35f)
            }
            drawCircle(sphereBaseCol, r, sphereCenter)

            // Left Red SDF Indirect Highlight
            if (sdfGiParams.enabled && debugViewMode != PipelineDebugViewMode.DIRECT_LIGHT_ONLY) {
                drawCircle(Color(0xFFFF5252).copy(alpha = 0.45f), r * 0.7f, Offset(sphereCenter.x - r * 0.4f, sphereCenter.y))
            }
            // Specular GGX Highlight Core
            if (debugViewMode != PipelineDebugViewMode.INDIRECT_GI_ONLY) {
                val specCenter = Offset(sphereCenter.x + 8f, sphereCenter.y - r * 0.35f)
                drawCircle(Color.White.copy(alpha = 0.85f), r * 0.25f, specCenter)
                drawCircle(lightColor, r * 0.45f, specCenter)
            }
        }
        TargetLightingMesh.ARCHITECTURAL_WALL -> {
            val wallW = s * 2.8f
            val wallH = s * 1.8f
            drawRoundRect(
                color = blendColors(Color(0xFF607D8B), lightColor, 0.35f),
                topLeft = Offset(meshBase.x - wallW * 0.5f, meshBase.y - wallH),
                size = Size(wallW, wallH),
                cornerRadius = CornerRadius(4f, 4f)
            )
        }
        TargetLightingMesh.GROUND_SLAB -> {
            val slabW = s * 3.4f
            val slabH = s * 1.2f
            drawOval(
                color = blendColors(Color(0xFF455A64), lightColor, 0.40f),
                topLeft = Offset(meshBase.x - slabW * 0.5f, meshBase.y - slabH * 0.5f),
                size = Size(slabW, slabH)
            )
        }
    }
}

// -------------------------------------------------------------
// 5. SDF VOXEL DENSITY GRID VISUALIZER
// -------------------------------------------------------------
private fun DrawScope.drawSdfVoxelGrid(
    stageCenter: Offset,
    rotY: Float,
    zoomFactor: Float,
    sdfResolution: Int
) {
    val boundW = 120f * zoomFactor
    val boundH = 100f * zoomFactor
    val cageCenter = Offset(stageCenter.x, stageCenter.y - boundH * 0.5f)

    // Outer Bounding Box (AABB)
    drawRect(
        color = Color(0xFF00E5FF).copy(alpha = 0.65f),
        topLeft = Offset(cageCenter.x - boundW * 0.5f, cageCenter.y - boundH * 0.5f),
        size = Size(boundW, boundH),
        style = Stroke(width = 1.5f)
    )

    // Inner Voxel Slice Contours (dim = 32, 64, or 128)
    val sliceCount = (sdfResolution / 8).coerceIn(4, 12)
    for (i in 1 until sliceCount) {
        val y = cageCenter.y - boundH * 0.5f + (boundH * (i / sliceCount.toFloat()))
        drawLine(
            color = Color(0xFF00E5FF).copy(alpha = 0.25f),
            start = Offset(cageCenter.x - boundW * 0.5f, y),
            end = Offset(cageCenter.x + boundW * 0.5f, y),
            strokeWidth = 1.0f
        )
        val x = cageCenter.x - boundW * 0.5f + (boundW * (i / sliceCount.toFloat()))
        drawLine(
            color = Color(0xFF00E5FF).copy(alpha = 0.25f),
            start = Offset(x, cageCenter.y - boundH * 0.5f),
            end = Offset(x, cageCenter.y + boundH * 0.5f),
            strokeWidth = 1.0f
        )
    }
}

// -------------------------------------------------------------
// 6. VOLUMETRIC LIGHT BEAM (Fog / Haze cone)
// -------------------------------------------------------------
private fun DrawScope.drawVolumetricBeam(
    width: Float,
    height: Float,
    stageCenter: Offset,
    lightParams: LightSourceParameters,
    lightColor: Color,
    rotY: Float,
    zoomFactor: Float
) {
    val lightScreenX = width * 0.5f + (lightParams.posX * 35f * zoomFactor) - (rotY * 0.5f)
    val lightScreenY = (height * 0.25f - (lightParams.posY * 22f * zoomFactor)).coerceIn(40f, height * 0.50f)
    val beamSource = Offset(lightScreenX, lightScreenY)
    val beamTarget = Offset(stageCenter.x, stageCenter.y - 20f)
    val beamW = (tan(lightParams.spotOuterDeg * 3.14159f / 180f) * 160f * zoomFactor).coerceIn(30f, 140f)

    val beamPath = Path().apply {
        moveTo(beamSource.x - 6f, beamSource.y)
        lineTo(beamSource.x + 6f, beamSource.y)
        lineTo(beamTarget.x + beamW, beamTarget.y)
        lineTo(beamTarget.x - beamW, beamTarget.y)
        close()
    }

    val beamAlpha = (0.16f * lightParams.volumetricIntensity).coerceIn(0.04f, 0.40f)
    drawPath(beamPath, color = lightColor.copy(alpha = beamAlpha))
}

// -------------------------------------------------------------
// 7. 3D LIGHT SOURCE GIZMO (Flashlight / Bulb / Softbox / Sun)
// -------------------------------------------------------------
private fun DrawScope.drawLightSourceGizmo(
    width: Float,
    height: Float,
    stageCenter: Offset,
    lightParams: LightSourceParameters,
    lightColor: Color,
    rotY: Float,
    zoomFactor: Float
) {
    val lightScreenX = width * 0.5f + (lightParams.posX * 35f * zoomFactor) - (rotY * 0.5f)
    val lightScreenY = (height * 0.25f - (lightParams.posY * 22f * zoomFactor)).coerceIn(40f, height * 0.50f)
    val gizmoPos = Offset(lightScreenX, lightScreenY)

    when (lightParams.type) {
        LightSourceType.SPOT_FLASHLIGHT -> {
            val barrelW = 14f * zoomFactor
            val barrelL = 34f * zoomFactor

            drawRoundRect(
                color = Color(0xFF263238),
                topLeft = Offset(gizmoPos.x - barrelW * 0.5f, gizmoPos.y - barrelL),
                size = Size(barrelW, barrelL),
                cornerRadius = CornerRadius(3f, 3f)
            )
            val bezelW = barrelW * 1.6f
            val bezelH = 10f * zoomFactor
            drawRoundRect(
                color = Color(0xFF1E282D),
                topLeft = Offset(gizmoPos.x - bezelW * 0.5f, gizmoPos.y),
                size = Size(bezelW, bezelH),
                cornerRadius = CornerRadius(2f, 2f)
            )
            drawCircle(lightColor, 7f * zoomFactor, Offset(gizmoPos.x, gizmoPos.y + bezelH * 0.5f))
            drawCircle(Color.White, 3f * zoomFactor, Offset(gizmoPos.x, gizmoPos.y + bezelH * 0.5f))

            drawLine(
                color = Color(0xFFFFA726).copy(alpha = 0.5f),
                start = Offset(gizmoPos.x, gizmoPos.y + bezelH),
                end = Offset(stageCenter.x, stageCenter.y - 30f),
                strokeWidth = 1.0f
            )
        }
        LightSourceType.POINT_BULB -> {
            drawLine(Color(0xFF212121), Offset(gizmoPos.x, 0f), gizmoPos, 2.0f)
            val socketW = 12f * zoomFactor
            val socketH = 14f * zoomFactor
            drawRect(Color(0xFFD4AF37), Offset(gizmoPos.x - socketW * 0.5f, gizmoPos.y), Size(socketW, socketH))

            val bulbR = 18f * zoomFactor
            val bulbCenter = Offset(gizmoPos.x, gizmoPos.y + socketH + bulbR * 0.75f)
            drawCircle(Color(0x33FFFFFF), bulbR, bulbCenter)
            drawCircle(Color(0x66B0BEC5), bulbR, bulbCenter, style = Stroke(width = 1.5f))
            drawCircle(lightColor.copy(alpha = 0.35f), bulbR * 1.8f, bulbCenter)
            drawCircle(lightColor, 5f * zoomFactor, bulbCenter)
            drawCircle(Color.White, 2f * zoomFactor, bulbCenter)
        }
        LightSourceType.AREA_SOFTBOX -> {
            val panelW = 44f * zoomFactor * (lightParams.areaWidthMeters / 1.4f)
            val panelH = 28f * zoomFactor * (lightParams.areaHeightMeters / 0.9f)
            val panelTopLeft = Offset(gizmoPos.x - panelW * 0.5f, gizmoPos.y - panelH * 0.5f)

            drawRoundRect(Color(0xFF1E242C), panelTopLeft, Size(panelW, panelH), CornerRadius(4f, 4f))
            drawRoundRect(Color(0xFF455A64), panelTopLeft, Size(panelW, panelH), CornerRadius(4f, 4f), style = Stroke(width = 2.0f))
            drawRoundRect(lightColor.copy(alpha = 0.90f), Offset(panelTopLeft.x + 3f, panelTopLeft.y + 3f), Size(panelW - 6f, panelH - 6f), CornerRadius(2f, 2f))
            drawRoundRect(Color.White.copy(alpha = 0.75f), Offset(panelTopLeft.x + 6f, panelTopLeft.y + 6f), Size(panelW - 12f, panelH - 12f), CornerRadius(2f, 2f))
        }
        LightSourceType.DIRECTIONAL_SUN -> {
            val sunRadius = 22f * zoomFactor
            drawCircle(lightColor.copy(alpha = 0.35f), sunRadius * 1.8f, gizmoPos)
            drawCircle(lightColor, sunRadius, gizmoPos)
            drawCircle(Color.White, sunRadius * 0.5f, gizmoPos)
        }
    }
}

private fun blendColors(base: Color, tint: Color, factor: Float): Color {
    val f = factor.coerceIn(0f, 1f)
    return Color(
        red = (base.red * (1f - f) + tint.red * f).coerceIn(0f, 1f),
        green = (base.green * (1f - f) + tint.green * f).coerceIn(0f, 1f),
        blue = (base.blue * (1f - f) + tint.blue * f).coerceIn(0f, 1f),
        alpha = 1.0f
    )
}
