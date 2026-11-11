package com.example.sphere.viewport

import androidx.compose.foundation.BorderStroke
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
import androidx.compose.ui.geometry.CornerRadius
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.*
import androidx.compose.ui.graphics.drawscope.DrawScope
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.graphics.drawscope.drawIntoCanvas
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.TextMeasurer
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.drawText
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.rememberTextMeasurer
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.sphere.model.CompositeSurface
import com.example.sphere.model.MaterialChannelMode
import com.example.sphere.model.PbrMaterial
import com.example.sphere.model.SurfacePatternType
import kotlin.math.*

enum class PreviewMeshShape(val id: String, val label: String, val description: String) {
    CUBE("cube", "Test Cube", "3D Photometric Calibration Cube with face text and telemetry"),
    WALL("wall", "Architectural Wall", "3D Vertical Wall with depth, top coping & foundation"),
    SPHERE("sphere", "PBR Sphere", "Curved microfacet BRDF inspection sphere"),
    PLANE("plane", "Ground Slab", "Horizontal planar stage surface")
}

private data class Vec3D(val x: Float, val y: Float, val z: Float) {
    fun dot(other: Vec3D): Float = x * other.x + y * other.y + z * other.z
    fun length(): Float = sqrt(x * x + y * y + z * z).coerceAtLeast(0.0001f)
    fun normalized(): Vec3D {
        val l = length()
        return Vec3D(x / l, y / l, z / l)
    }
}

private data class Point2D(val x: Float, val y: Float, val zView: Float)

@Composable
fun MaterialPreviewViewport(
    material: PbrMaterial,
    modifier: Modifier = Modifier,
    compositeSurface: CompositeSurface? = null,
    channelMode: MaterialChannelMode = MaterialChannelMode.COMPOSITE_PBR,
    shape: PreviewMeshShape = PreviewMeshShape.CUBE,
    objectScale: Float = 1.0f,
    objectWidthScale: Float = 1.0f,
    objectHeightScale: Float = 1.0f,
    objectDepthScale: Float = 1.0f,
    useRealWorldScale: Boolean = true,
    uvTilingMultiplierX: Float = 1.0f,
    uvTilingMultiplierY: Float = 1.0f,
    showWireframe: Boolean = false,
    onToggleWireframe: ((Boolean) -> Unit)? = null,
    onScaleChange: ((Float) -> Unit)? = null,
    onShapeChange: (PreviewMeshShape) -> Unit = {}
) {
    var rotX by remember { mutableFloatStateOf(18f) }
    var rotY by remember { mutableFloatStateOf(-32f) }
    var cameraZoomFactor by remember { mutableFloatStateOf(1.0f) }
    var lightAngle by remember { mutableFloatStateOf(45f) }
    var splitPosition by remember { mutableFloatStateOf(0.5f) }
    var internalWireframe by remember { mutableStateOf(showWireframe) }
    val isWireframeVisible = onToggleWireframe?.let { showWireframe } ?: internalWireframe

    Box(
        modifier = modifier
            .background(Color(0xFF070B12))
            .pointerInput(channelMode) {
                detectDragGestures { change, dragAmount ->
                    change.consume()
                    if (channelMode == MaterialChannelMode.SPLIT_COMPARE) {
                        splitPosition = (splitPosition + dragAmount.x / size.width).coerceIn(0.05f, 0.95f)
                    } else {
                        rotY = (rotY + dragAmount.x * 0.45f) % 360f
                        rotX = (rotX - dragAmount.y * 0.45f).coerceIn(-65f, 65f)
                    }
                }
            }
            .testTag("material_preview_viewport")
    ) {
        // 1. Studio Lighting Background & Stage Horizon
        Canvas(modifier = Modifier.fillMaxSize()) {
            drawRect(
                brush = Brush.radialGradient(
                    colors = listOf(Color(0xFF1E293B), Color(0xFF0F172A), Color(0xFF05070B)),
                    center = Offset(size.width * 0.5f, size.height * 0.45f),
                    radius = size.minDimension * 0.95f
                )
            )

            val floorY = size.height * 0.78f
            // Perspective ground lines only rendered when wireframe mode is explicitly enabled
            if (isWireframeVisible) {
                val cx = size.width * 0.5f
                val gridStep = 44f
                for (i in -12..12) {
                    val startX = cx + i * gridStep * 0.40f
                    val endX = cx + i * gridStep * 3.2f
                    val alpha = (1.0f - abs(i) / 13f) * 0.055f
                    drawLine(
                        color = Color(0xFF38BDF8).copy(alpha = alpha),
                        start = Offset(startX, floorY),
                        end = Offset(endX, size.height),
                        strokeWidth = 1f
                    )
                }
            }
            // Subtle Horizon Accent Line
            drawLine(
                color = Color(0xFF38BDF8).copy(alpha = if (isWireframeVisible) 0.14f else 0.06f),
                start = Offset(0f, floorY),
                end = Offset(size.width, floorY),
                strokeWidth = 1.0f
            )
        }

        // 2. 3D Object Geometry Renderer
        Canvas(modifier = Modifier.fillMaxSize()) {
            val w = size.width
            val h = size.height
            val cx = w / 2f
            val cy = h / 2f - 6f

            val radY = (rotY * (PI / 180.0)).toFloat()
            val radX = (rotX * (PI / 180.0)).toFloat()
            val cosY = cos(radY)
            val sinY = sin(radY)
            val cosX = cos(radX)
            val sinX = sin(radX)

            val lightRad = (lightAngle * (PI / 180.0)).toFloat()
            val lightDir = Vec3D(cos(lightRad) * 0.7f, 0.85f, sin(lightRad) * 0.7f).normalized()

            // Sascha Willems Vulkan Camera Look-At Orbit Coordinate System
            // Eye direction in world space pointing from origin towards camera
            val eyeDir = Vec3D(sinY * cosX, sinX, cosY * cosX).normalized()

            // Physical bounding extent across active dimensions
            val maxDimensionExtent = maxOf(
                objectWidthScale * objectScale,
                objectHeightScale * objectScale,
                objectDepthScale * objectScale,
                1.0f
            )
            // Camera distance adapts smoothly with sub-linear damping so large objects (> 100m) remain in view
            // while physical mesh dimensions clearly expand and shrink visually on screen
            val cameraDist = (3.8f + (maxDimensionExtent - 1.0f).coerceAtLeast(0f) * 0.40f)
            // Camera FOV magnification depends purely on cameraZoomFactor (optical view zoom, not physical dimension)
            val fovFactor = min(w, h) * 1.35f * cameraZoomFactor

            fun project(v: Vec3D): Point2D {
                val x1 = v.x * cosY + v.z * sinY
                val z1 = -v.x * sinY + v.z * cosY
                val y2 = v.y * cosX - z1 * sinX
                val z2 = v.y * sinX + z1 * cosX
                // In Sascha Willems Vulkan coordinate system, camera is positioned at +cameraDist looking along -Z.
                // Front face vertices (+Z) have smaller view depth (closer to camera): (cameraDist - z2)
                // Back face vertices (-Z) have larger view depth (farther from camera): (cameraDist - z2)
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

            fun isFaceFacingCamera(normal: Vec3D, corners: List<Vec3D>): Boolean {
                val rotNorm = transformNormal(normal)
                var fcX = 0f
                var fcY = 0f
                var fcZ = 0f
                val count = corners.size.toFloat()
                for (v in corners) {
                    fcX += v.x
                    fcY += v.y
                    fcZ += v.z
                }
                fcX /= count
                fcY /= count
                fcZ /= count

                // Rotate face center to view space
                val c1 = fcX * cosY + fcZ * sinY
                val cz1 = -fcX * sinY + fcZ * cosY
                val cy2 = fcY * cosX - cz1 * sinX
                val cz2 = fcY * sinX + cz1 * cosX

                // Vector from face center to camera at (0, 0, cameraDist) in view space
                val toEyeX = -c1
                val toEyeY = -cy2
                val toEyeZ = cameraDist - cz2
                val dot = rotNorm.x * toEyeX + rotNorm.y * toEyeY + rotNorm.z * toEyeZ
                return dot > 0.01f
            }

            when (shape) {
                PreviewMeshShape.CUBE -> {
                    // =================================================================
                    // BRIGHT, HIGH-RESOLUTION 3D TEST CUBE WITH TEXT TELEMETRY
                    // =================================================================
                    // Standard 1.00m Unit Cube (hw, hh, hd = 0.50m at 1.0x)
                    val baseHs = 0.50f * objectScale
                    val hw = baseHs * objectWidthScale
                    val hh = baseHs * objectHeightScale
                    val hd = baseHs * objectDepthScale

                    // 8 Vertices
                    val v0 = Vec3D(-hw,  hh,  hd) // 0: Top-Left Front
                    val v1 = Vec3D( hw,  hh,  hd) // 1: Top-Right Front
                    val v2 = Vec3D( hw, -hh,  hd) // 2: Bottom-Right Front
                    val v3 = Vec3D(-hw, -hh,  hd) // 3: Bottom-Left Front
                    val v4 = Vec3D(-hw,  hh, -hd) // 4: Top-Left Back
                    val v5 = Vec3D( hw,  hh, -hd) // 5: Top-Right Back
                    val v6 = Vec3D( hw, -hh, -hd) // 6: Bottom-Right Back
                    val v7 = Vec3D(-hw, -hh, -hd) // 7: Bottom-Left Back

                    // Stage Contact Shadow
                    val shadow0 = project(Vec3D(-hw * 1.25f, -hh - 0.02f,  hd * 1.25f))
                    val shadow1 = project(Vec3D( hw * 1.25f, -hh - 0.02f,  hd * 1.25f))
                    val shadow2 = project(Vec3D( hw * 1.25f, -hh - 0.02f, -hd * 1.25f))
                    val shadow3 = project(Vec3D(-hw * 1.25f, -hh - 0.02f, -hd * 1.25f))

                    val shadowPath = Path().apply {
                        moveTo(shadow0.x, shadow0.y)
                        lineTo(shadow1.x, shadow1.y)
                        lineTo(shadow2.x, shadow2.y)
                        lineTo(shadow3.x, shadow3.y)
                        close()
                    }
                    drawPath(
                        path = shadowPath,
                        brush = Brush.radialGradient(
                            colors = listOf(Color.Black.copy(alpha = 0.72f), Color.Transparent),
                            center = Offset((shadow0.x + shadow2.x) * 0.5f, (shadow0.y + shadow2.y) * 0.5f),
                            radius = min(w, h) * 0.32f
                        )
                    )

                    data class CubeFace(
                        val id: String,
                        val normal: Vec3D,
                        val corners: List<Vec3D>, // 0: TL, 1: TR, 2: BR, 3: BL (Clockwise from outside)
                        val faceType: Int // 0: Front, 1: Top, 2: Right, 3: Others
                    )

                    val faces = listOf(
                        CubeFace("FRONT (+Z)", Vec3D( 0f,  0f,  1f), listOf(v0, v1, v2, v3), 0),
                        CubeFace("BACK (-Z)",  Vec3D( 0f,  0f, -1f), listOf(v5, v4, v7, v6), 3),
                        CubeFace("TOP (+Y)",   Vec3D( 0f,  1f,  0f), listOf(v4, v5, v1, v0), 1),
                        CubeFace("BOTTOM (-Y)",Vec3D( 0f, -1f,  0f), listOf(v3, v2, v6, v7), 3),
                        CubeFace("RIGHT (+X)", Vec3D( 1f,  0f,  0f), listOf(v1, v5, v6, v2), 2),
                        CubeFace("LEFT (-X)",  Vec3D(-1f,  0f,  0f), listOf(v4, v0, v3, v7), 3)
                    )

                    // Depth-sort faces: analytical 3D visibility filter + Painter's Algorithm
                    val renderedFaces = faces.filter { isFaceFacingCamera(it.normal, it.corners) }
                        .map { face ->
                            val projPts = face.corners.map { project(it) }
                            val avgZ = projPts.map { it.zView }.average().toFloat()
                            Triple(face, projPts, avgZ)
                        }.sortedByDescending { it.third }

                    for ((face, projPts, _) in renderedFaces) {
                        val facePath = Path().apply {
                            moveTo(projPts[0].x, projPts[0].y)
                            lineTo(projPts[1].x, projPts[1].y)
                            lineTo(projPts[2].x, projPts[2].y)
                            lineTo(projPts[3].x, projPts[3].y)
                            close()
                        }

                        // Physical Shading
                        val nDotL = (face.normal.dot(lightDir)).coerceIn(0f, 1f)
                        val diffuse = 0.38f + 0.62f * nDotL

                        val halfV = Vec3D(lightDir.x + eyeDir.x, lightDir.y + eyeDir.y, lightDir.z + eyeDir.z).normalized()
                        val nDotH = (face.normal.dot(halfV)).coerceIn(0f, 1f)
                        val specPower = (1.0f - material.roughness).coerceIn(0.01f, 0.99f) * 48f + 4f
                        val specular = nDotH.pow(specPower) * (1.0f - material.roughness) * (0.35f + material.metallic * 0.65f)

                        val (faceW, faceH) = when (face.faceType) {
                            0 -> Pair(hw * 2f, hh * 2f) // Front (+Z)
                            1 -> Pair(hw * 2f, hd * 2f) // Top (+Y)
                            2 -> Pair(hd * 2f, hh * 2f) // Right (+X)
                            else -> Pair(hw * 2f, hh * 2f)
                        }


                            when (channelMode) {
                                MaterialChannelMode.ALBEDO_MAP -> {
                                    drawPath(path = facePath, color = material.baseColor)
                                }
                                MaterialChannelMode.NORMAL_MAP -> {
                                    val normCol = Color(
                                        (face.normal.x * 0.5f + 0.5f).coerceIn(0f, 1f),
                                        (face.normal.y * 0.5f + 0.5f).coerceIn(0f, 1f),
                                        (face.normal.z * 0.5f + 0.5f).coerceIn(0f, 1f)
                                    )
                                    drawPath(path = facePath, color = normCol)
                                }
                                MaterialChannelMode.ORM_MAP -> {
                                    val ormCol = Color(
                                        material.occlusion.coerceIn(0f, 1f),
                                        material.roughness.coerceIn(0f, 1f),
                                        material.metallic.coerceIn(0f, 1f)
                                    )
                                    drawPath(path = facePath, color = ormCol)
                                }
                                MaterialChannelMode.HEIGHT_MAP -> {
                                    val hVal = (0.4f + face.normal.y * 0.25f).coerceIn(0f, 1f)
                                    drawPath(path = facePath, color = Color(hVal, hVal, hVal))
                                }
                                else -> {
                                    // Full PBR
                                    val litRed = (material.baseColor.red * diffuse + specular).coerceIn(0f, 1f)
                                    val litGreen = (material.baseColor.green * diffuse + specular).coerceIn(0f, 1f)
                                    val litBlue = (material.baseColor.blue * diffuse + specular).coerceIn(0f, 1f)
                                    val litColor = Color(litRed, litGreen, litBlue, 1.0f)

                                    drawPath(path = facePath, color = litColor)

                                    // Procedural Texture with real-world repetition (constant metric scale, never stretched)
                                    val repX = ((faceW / material.tileScaleMeters.coerceAtLeast(0.05f)) * uvTilingMultiplierX).coerceIn(0.2f, 1000f)
                                    val repY = ((faceH / material.tileScaleMeters.coerceAtLeast(0.05f)) * uvTilingMultiplierY).coerceIn(0.2f, 1000f)
                                    drawProceduralFaceTexture(
                                        material = material,
                                        compositeSurface = compositeSurface,
                                        pts = projPts,
                                        diffuse = diffuse,
                                        repeatX = if (useRealWorldScale) repX else uvTilingMultiplierX,
                                        repeatY = if (useRealWorldScale) repY else uvTilingMultiplierY
                                    )
                                }
                        }

                        // Edge Rim Highlight (only rendered when wireframe inspection mode is active)
                        if (isWireframeVisible) {
                            drawPath(
                                path = facePath,
                                color = Color.White.copy(alpha = (0.35f + specular * 0.45f).coerceIn(0.20f, 0.85f)),
                                style = Stroke(width = 1.4f)
                            )
                        }
                    }
                }

                PreviewMeshShape.WALL -> {
                    // =================================================================
                    // BRIGHT 3D ARCHITECTURAL WALL WITH TOP COPING, THICKNESS & MARKINGS
                    // =================================================================
                    val baseW = 1.85f * objectScale
                    val baseH = 1.45f * objectScale
                    val baseT = 0.34f * objectScale

                    val hw = baseW * 0.5f * objectWidthScale
                    val hh = baseH * 0.5f * objectHeightScale
                    val ht = baseT * 0.5f * objectDepthScale

                    // Shadow
                    val wShadow0 = project(Vec3D(-hw * 1.2f, -hh - 0.08f,  ht * 1.3f))
                    val wShadow1 = project(Vec3D( hw * 1.2f, -hh - 0.08f,  ht * 1.3f))
                    val wShadow2 = project(Vec3D( hw * 1.2f, -hh - 0.08f, -ht * 1.3f))
                    val wShadow3 = project(Vec3D(-hw * 1.2f, -hh - 0.08f, -ht * 1.3f))
                    val wShadowPath = Path().apply {
                        moveTo(wShadow0.x, wShadow0.y)
                        lineTo(wShadow1.x, wShadow1.y)
                        lineTo(wShadow2.x, wShadow2.y)
                        lineTo(wShadow3.x, wShadow3.y)
                        close()
                    }
                    drawPath(
                        path = wShadowPath,
                        brush = Brush.radialGradient(
                            colors = listOf(Color.Black.copy(alpha = 0.70f), Color.Transparent),
                            center = Offset((wShadow0.x + wShadow2.x) * 0.5f, (wShadow0.y + wShadow2.y) * 0.5f),
                            radius = min(w, h) * 0.36f
                        )
                    )

                    // Main Wall Vertices
                    val w0 = Vec3D(-hw,  hh,  ht) // Front TL
                    val w1 = Vec3D( hw,  hh,  ht) // Front TR
                    val w2 = Vec3D( hw, -hh,  ht) // Front BR
                    val w3 = Vec3D(-hw, -hh,  ht) // Front BL
                    val w4 = Vec3D(-hw,  hh, -ht) // Back TL
                    val w5 = Vec3D( hw,  hh, -ht) // Back TR
                    val w6 = Vec3D( hw, -hh, -ht) // Back BR
                    val w7 = Vec3D(-hw, -hh, -ht) // Back BL

                    // Overhanging Coping Slab
                    val oh = 0.06f
                    val ch = 0.09f
                    val c0 = Vec3D(-hw - oh, hh + ch,  ht + oh)
                    val c1 = Vec3D( hw + oh, hh + ch,  ht + oh)
                    val c2 = Vec3D( hw + oh, hh,       ht + oh)
                    val c3 = Vec3D(-hw - oh, hh,       ht + oh)
                    val c4 = Vec3D(-hw - oh, hh + ch, -ht - oh)
                    val c5 = Vec3D( hw + oh, hh + ch, -ht - oh)
                    val c6 = Vec3D( hw + oh, hh,      -ht - oh)
                    val c7 = Vec3D(-hw - oh, hh,      -ht - oh)

                    data class WFace(
                        val name: String,
                        val normal: Vec3D,
                        val corners: List<Vec3D>,
                        val isFront: Boolean = false,
                        val isCoping: Boolean = false
                    )

                    val wallFaceList = listOf(
                        WFace("FRONT WALL", Vec3D( 0f,  0f,  1f), listOf(w0, w1, w2, w3), isFront = true),
                        WFace("RIGHT EDGE", Vec3D( 1f,  0f,  0f), listOf(w1, w5, w6, w2)),
                        WFace("LEFT EDGE",  Vec3D(-1f,  0f,  0f), listOf(w4, w0, w3, w7)),
                        WFace("BACK WALL",  Vec3D( 0f,  0f, -1f), listOf(w5, w4, w7, w6)),
                        // Coping
                        WFace("COPING TOP",   Vec3D( 0f,  1f,  0f), listOf(c4, c5, c1, c0), isCoping = true),
                        WFace("COPING FRONT", Vec3D( 0f,  0f,  1f), listOf(c0, c1, c2, c3), isCoping = true),
                        WFace("COPING RIGHT", Vec3D( 1f,  0f,  0f), listOf(c1, c5, c6, c2), isCoping = true),
                        WFace("COPING LEFT",  Vec3D(-1f,  0f,  0f), listOf(c4, c0, c3, c7), isCoping = true),
                        WFace("COPING BACK",  Vec3D( 0f,  0f, -1f), listOf(c5, c4, c7, c6), isCoping = true)
                    )

                    val renderedWall = wallFaceList.filter { isFaceFacingCamera(it.normal, it.corners) }
                        .map { wf ->
                            val pts = wf.corners.map { project(it) }
                            val avgZ = pts.map { it.zView }.average().toFloat()
                            Triple(wf, pts, avgZ)
                        }.sortedByDescending { it.third }

                    for ((wf, pts, _) in renderedWall) {
                        val path = Path().apply {
                            moveTo(pts[0].x, pts[0].y)
                            lineTo(pts[1].x, pts[1].y)
                            lineTo(pts[2].x, pts[2].y)
                            lineTo(pts[3].x, pts[3].y)
                            close()
                        }

                        val nDotL = (wf.normal.dot(lightDir)).coerceIn(0f, 1f)
                        val diffuse = 0.36f + 0.64f * nDotL

                        val (wallFaceW, wallFaceH) = if (wf.isFront || wf.name.contains("BACK")) {
                            Pair(hw * 2f, hh * 2f)
                        } else {
                            Pair(ht * 2f, hh * 2f)
                        }


                            val baseCol = if (wf.isCoping) Color(0xFFC5C9CD) else material.baseColor
                            val litRed = (baseCol.red * diffuse).coerceIn(0f, 1f)
                            val litGreen = (baseCol.green * diffuse).coerceIn(0f, 1f)
                            val litBlue = (baseCol.blue * diffuse).coerceIn(0f, 1f)
                            drawPath(path = path, color = Color(litRed, litGreen, litBlue))

                            if (wf.isFront) {
                                val repX = ((wallFaceW / material.tileScaleMeters.coerceAtLeast(0.05f)) * uvTilingMultiplierX).coerceIn(0.2f, 1000f)
                                val repY = ((wallFaceH / material.tileScaleMeters.coerceAtLeast(0.05f)) * uvTilingMultiplierY).coerceIn(0.2f, 1000f)
                                drawProceduralFaceTexture(
                                    material = material,
                                    compositeSurface = compositeSurface,
                                    pts = pts,
                                    diffuse = diffuse,
                                    repeatX = if (useRealWorldScale) repX else uvTilingMultiplierX,
                                    repeatY = if (useRealWorldScale) repY else uvTilingMultiplierY
                                )
                            }

                        if (isWireframeVisible) {
                            drawPath(
                                path = path,
                                color = Color.White.copy(alpha = if (wf.isFront) 0.35f else 0.20f),
                                style = Stroke(width = 1.4f)
                            )
                        }
                    }
                }

                PreviewMeshShape.SPHERE -> {
                    // Curved PBR sphere scaled with object dimension modifiers
                    val sScale = objectScale * ((objectWidthScale + objectHeightScale + objectDepthScale) / 3f)
                    val normalizedRadiusScale = (sScale / maxDimensionExtent).coerceIn(0.20f, 1.2f)
                    val sRadius = min(w, h) * 0.28f * normalizedRadiusScale
                    val sCenter = Offset(cx, cy)

                    val shadowCenter = Offset(sCenter.x - lightDir.x * 24f, sCenter.y + sRadius * 0.94f)
                    drawOval(
                        brush = Brush.radialGradient(listOf(Color.Black.copy(alpha = 0.65f), Color.Transparent), center = shadowCenter, radius = sRadius * 1.2f),
                        topLeft = Offset(shadowCenter.x - sRadius * 1.2f, shadowCenter.y - sRadius * 0.30f),
                        size = Size(sRadius * 2.4f, sRadius * 0.60f)
                    )

                    val highlight = Offset(sCenter.x + lightDir.x * sRadius * 0.40f, sCenter.y - lightDir.y * sRadius * 0.40f)

                    val sphereGrad = Brush.radialGradient(
                        colors = listOf(
                            material.baseColor,
                            material.baseColor.copy(red = material.baseColor.red * 0.42f, green = material.baseColor.green * 0.42f, blue = material.baseColor.blue * 0.42f),
                            Color.Black.copy(alpha = 0.92f)
                        ),
                        center = highlight,
                        radius = sRadius * 1.15f
                    )
                    drawCircle(brush = sphereGrad, radius = sRadius, center = sCenter)

                    val specRadius = sRadius * (0.08f + material.roughness * 0.36f)
                    val specAlpha = ((1.0f - material.roughness) * (0.5f + material.metallic * 0.5f)).coerceIn(0.1f, 0.95f)
                    drawCircle(
                        brush = Brush.radialGradient(listOf(Color.White.copy(alpha = specAlpha), Color.Transparent), center = highlight, radius = specRadius),
                        radius = specRadius,
                        center = highlight
                    )
                    if (isWireframeVisible) {
                        drawCircle(color = Color.White.copy(alpha = 0.25f), radius = sRadius, center = sCenter, style = Stroke(width = 1.4f))
                    }
                }

                PreviewMeshShape.PLANE -> {
                    // 3D Architectural Ground Slab / Floor with thickness & bevel
                    val baseFloorW = 2.4f * objectScale
                    val baseFloorD = 2.4f * objectScale
                    val slabThickness = 0.20f * objectScale * objectHeightScale

                    val hfw = baseFloorW * 0.5f * objectWidthScale
                    val hfd = baseFloorD * 0.5f * objectDepthScale
                    val topY = -0.12f
                    val botY = topY - slabThickness

                    // Ground contact shadow
                    val s0 = project(Vec3D(-hfw * 1.15f, botY - 0.04f,  hfd * 1.15f))
                    val s1 = project(Vec3D( hfw * 1.15f, botY - 0.04f,  hfd * 1.15f))
                    val s2 = project(Vec3D( hfw * 1.15f, botY - 0.04f, -hfd * 1.15f))
                    val s3 = project(Vec3D(-hfw * 1.15f, botY - 0.04f, -hfd * 1.15f))
                    val sPath = Path().apply {
                        moveTo(s0.x, s0.y)
                        lineTo(s1.x, s1.y)
                        lineTo(s2.x, s2.y)
                        lineTo(s3.x, s3.y)
                        close()
                    }
                    drawPath(
                        path = sPath,
                        brush = Brush.radialGradient(
                            colors = listOf(Color.Black.copy(alpha = 0.65f), Color.Transparent),
                            center = Offset((s0.x + s2.x) * 0.5f, (s0.y + s2.y) * 0.5f),
                            radius = min(w, h) * 0.45f
                        )
                    )

                    // Slab 8 vertices
                    val f0 = Vec3D(-hfw, topY,  hfd) // Top TL
                    val f1 = Vec3D( hfw, topY,  hfd) // Top TR
                    val f2 = Vec3D( hfw, topY, -hfd) // Top BR
                    val f3 = Vec3D(-hfw, topY, -hfd) // Top BL

                    val b0 = Vec3D(-hfw, botY,  hfd) // Bot TL
                    val b1 = Vec3D( hfw, botY,  hfd) // Bot TR
                    val b2 = Vec3D( hfw, botY, -hfd) // Bot BR
                    val b3 = Vec3D(-hfw, botY, -hfd) // Bot BL

                    data class FloorFace(
                        val name: String,
                        val normal: Vec3D,
                        val corners: List<Vec3D>,
                        val isTop: Boolean = false
                    )

                    val floorFaces = listOf(
                        FloorFace("TOP SLAB",   Vec3D( 0f,  1f,  0f), listOf(f3, f2, f1, f0), isTop = true),
                        FloorFace("FRONT EDGE", Vec3D( 0f,  0f,  1f), listOf(f0, f1, b1, b0)),
                        FloorFace("RIGHT EDGE", Vec3D( 1f,  0f,  0f), listOf(f1, f2, b2, b1)),
                        FloorFace("LEFT EDGE",  Vec3D(-1f,  0f,  0f), listOf(f3, f0, b0, b3)),
                        FloorFace("BACK EDGE",  Vec3D( 0f,  0f, -1f), listOf(f2, f3, b3, b2))
                    )

                    val renderedFloor = floorFaces.filter { isFaceFacingCamera(it.normal, it.corners) }
                        .map { ff ->
                            val pts = ff.corners.map { project(it) }
                            val avgZ = pts.map { it.zView }.average().toFloat()
                            Triple(ff, pts, avgZ)
                        }.sortedByDescending { it.third }

                    for ((ff, pts, _) in renderedFloor) {
                        val path = Path().apply {
                            moveTo(pts[0].x, pts[0].y)
                            lineTo(pts[1].x, pts[1].y)
                            lineTo(pts[2].x, pts[2].y)
                            lineTo(pts[3].x, pts[3].y)
                            close()
                        }

                        val nDotL = (ff.normal.dot(lightDir)).coerceIn(0f, 1f)
                        val diffuse = 0.35f + 0.65f * nDotL

                        if (ff.isTop) {
                            val floorFaceW = hfw * 2f
                            val floorFaceD = hfd * 2f

                                drawPath(path = path, color = material.baseColor)
                                val repX = ((floorFaceW / material.tileScaleMeters.coerceAtLeast(0.05f)) * uvTilingMultiplierX).coerceIn(0.2f, 1000f)
                                val repY = ((floorFaceD / material.tileScaleMeters.coerceAtLeast(0.05f)) * uvTilingMultiplierY).coerceIn(0.2f, 1000f)
                                drawProceduralFaceTexture(
                                    material = material,
                                    compositeSurface = compositeSurface,
                                    pts = pts,
                                    diffuse = diffuse,
                                    repeatX = if (useRealWorldScale) repX else uvTilingMultiplierX,
                                    repeatY = if (useRealWorldScale) repY else uvTilingMultiplierY
                                )
                        } else {
                            val edgeBase = Color(0xFF64748B)
                            val litCol = Color(
                                (edgeBase.red * diffuse).coerceIn(0f, 1f),
                                (edgeBase.green * diffuse).coerceIn(0f, 1f),
                                (edgeBase.blue * diffuse).coerceIn(0f, 1f)
                            )
                            drawPath(path = path, color = litCol)
                        }

                        if (isWireframeVisible) {
                            drawPath(
                                path = path,
                                color = Color.White.copy(alpha = if (ff.isTop) 0.35f else 0.20f),
                                style = Stroke(width = 1.4f)
                            )
                        }
                    }
                }
            }
        }

        // Live Physical Metrics & PBR Constant Tiling Telemetry HUD (TopStart)
        val hudActualW = when (shape) {
            PreviewMeshShape.CUBE -> 1.0f * objectWidthScale * objectScale
            PreviewMeshShape.WALL -> 2.0f * objectWidthScale * objectScale
            PreviewMeshShape.PLANE -> 2.0f * objectWidthScale * objectScale
            PreviewMeshShape.SPHERE -> 1.0f * objectWidthScale * objectScale
        }
        val hudActualH = when (shape) {
            PreviewMeshShape.CUBE -> 1.0f * objectHeightScale * objectScale
            PreviewMeshShape.WALL -> 1.5f * objectHeightScale * objectScale
            PreviewMeshShape.PLANE -> 0.15f * objectHeightScale * objectScale
            PreviewMeshShape.SPHERE -> 1.0f * objectHeightScale * objectScale
        }
        val hudActualD = when (shape) {
            PreviewMeshShape.CUBE -> 1.0f * objectDepthScale * objectScale
            PreviewMeshShape.WALL -> 0.25f * objectDepthScale * objectScale
            PreviewMeshShape.PLANE -> 2.0f * objectDepthScale * objectScale
            PreviewMeshShape.SPHERE -> 1.0f * objectDepthScale * objectScale
        }
        val hudPatternM = if (material.tileScaleMeters > 0.05f) material.tileScaleMeters else 1.0f
        val hudPatternMm = (hudPatternM * 1000).toInt()
        val hudTilesX = ((hudActualW / hudPatternM) * uvTilingMultiplierX).coerceAtLeast(1f).roundToInt()
        val hudTilesY = (((if (shape == PreviewMeshShape.PLANE) hudActualD else hudActualH) / hudPatternM) * uvTilingMultiplierY).coerceAtLeast(1f).roundToInt()

        Surface(
            modifier = Modifier
                .align(Alignment.TopStart)
                .padding(10.dp)
                .testTag("viewport_metrics_hud"),
            color = Color(0xDD090D16),
            shape = RoundedCornerShape(8.dp),
            border = BorderStroke(1.dp, Color(0xFF38BDF8).copy(alpha = 0.35f))
        ) {
            Column(
                modifier = Modifier.padding(horizontal = 9.dp, vertical = 6.dp),
                verticalArrangement = Arrangement.spacedBy(2.dp)
            ) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Box(
                        modifier = Modifier
                            .size(6.dp)
                            .background(Color(0xFF10B981), RoundedCornerShape(3.dp))
                    )
                    Spacer(Modifier.width(5.dp))
                    Text(
                        text = "${shape.label.uppercase()} MEASUREMENT",
                        color = Color(0xFF38BDF8),
                        fontSize = 9.sp,
                        fontWeight = FontWeight.Bold,
                        fontFamily = FontFamily.Monospace
                    )
                }
                Text(
                    text = "${"%.2f".format(hudActualW)}m (W) × ${"%.2f".format(hudActualH)}m (H) × ${"%.2f".format(hudActualD)}m (D)",
                    color = Color.White,
                    fontSize = 11.sp,
                    fontWeight = FontWeight.Bold,
                    fontFamily = FontFamily.Monospace
                )
                Text(
                    text = "Area: ${"%.2f".format(hudActualW * (if (shape == PreviewMeshShape.PLANE) hudActualD else hudActualH))} m²  •  Tiles: ${hudTilesX}×${hudTilesY} (${hudTilesX * hudTilesY})",
                    color = Color(0xFF94A3B8),
                    fontSize = 10.sp,
                    fontFamily = FontFamily.Monospace
                )
                Text(
                    text = "PBR Tile: ${hudPatternMm}mm (Constant physical scale)",
                    color = Color(0xFF10B981),
                    fontSize = 9.sp,
                    fontWeight = FontWeight.Medium,
                    fontFamily = FontFamily.Monospace
                )
            }
        }

        // Controls Overlay (Top End): Wireframe Toggle & Sun Light Rotation
        Surface(
            modifier = Modifier
                .align(Alignment.TopEnd)
                .padding(10.dp),
            color = Color.Black.copy(alpha = 0.70f),
            shape = RoundedCornerShape(8.dp),
            border = BorderStroke(1.dp, Color.White.copy(alpha = 0.12f))
        ) {
            Row(
                modifier = Modifier.padding(horizontal = 8.dp, vertical = 2.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                // Wireframe / Mesh visibility toggle button
                IconButton(
                    onClick = {
                        val next = !isWireframeVisible
                        if (onToggleWireframe != null) onToggleWireframe(next) else internalWireframe = next
                    },
                    modifier = Modifier.size(24.dp).testTag("toggle_wireframe_button")
                ) {
                    Icon(
                        imageVector = if (isWireframeVisible) Icons.Default.GridOn else Icons.Default.GridOff,
                        contentDescription = if (isWireframeVisible) "Hide Wireframe Mesh" else "Show Wireframe Mesh",
                        tint = if (isWireframeVisible) Color(0xFF38BDF8) else Color(0xFF94A3B8),
                        modifier = Modifier.size(13.dp)
                    )
                }
                // Camera Zoom In / Out Controls (View Zoom: Scales screen FOV without changing physical dimensions)
                IconButton(
                    onClick = { cameraZoomFactor = (cameraZoomFactor * 0.85f).coerceIn(0.25f, 4.0f) },
                    modifier = Modifier.size(24.dp).testTag("zoom_out_button")
                ) {
                    Icon(
                        Icons.Default.ZoomOut,
                        contentDescription = "Camera Zoom Out",
                        tint = Color(0xFFCBD5E1),
                        modifier = Modifier.size(13.dp)
                    )
                }
                Text(
                    "${"%.1f".format(cameraZoomFactor)}x",
                    style = MaterialTheme.typography.labelSmall.copy(
                        fontFamily = FontFamily.Monospace,
                        color = Color(0xFF38BDF8),
                        fontSize = 10.sp
                    )
                )
                IconButton(
                    onClick = { cameraZoomFactor = (cameraZoomFactor * 1.18f).coerceIn(0.25f, 4.0f) },
                    modifier = Modifier.size(24.dp).testTag("zoom_in_button")
                ) {
                    Icon(
                        Icons.Default.ZoomIn,
                        contentDescription = "Camera Zoom In",
                        tint = Color(0xFFCBD5E1),
                        modifier = Modifier.size(13.dp)
                    )
                }
                Spacer(modifier = Modifier.width(4.dp))
                IconButton(
                    onClick = { lightAngle = (lightAngle + 30f) % 360f },
                    modifier = Modifier.size(24.dp)
                ) {
                    Icon(
                        Icons.Default.WbSunny,
                        contentDescription = "Rotate Sun",
                        tint = Color(0xFFFBBF24),
                        modifier = Modifier.size(13.dp)
                    )
                }
                Text(
                    "${lightAngle.toInt()}°",
                    style = MaterialTheme.typography.labelSmall.copy(
                        fontFamily = FontFamily.Monospace,
                        color = Color.White,
                        fontSize = 11.sp
                    )
                )
            }
        }
    }
}

// -------------------------------------------------------------
// PROCEDURAL PATTERNS ON FACES (Bricks, Concrete, Wood, Tiles)
// -------------------------------------------------------------
private fun DrawScope.drawProceduralFaceTexture(
    material: PbrMaterial,
    compositeSurface: CompositeSurface?,
    pts: List<Point2D>,
    diffuse: Float,
    repeatX: Float = 1.0f,
    repeatY: Float = 1.0f
) {
    if (pts.size < 4) return

    fun lerp(u: Float, v: Float): Offset {
        val topX = pts[0].x + (pts[1].x - pts[0].x) * u
        val topY = pts[0].y + (pts[1].y - pts[0].y) * u
        val botX = pts[3].x + (pts[2].x - pts[3].x) * u
        val botY = pts[3].y + (pts[2].y - pts[3].y) * u
        return Offset(topX + (botX - topX) * v, topY + (botY - topY) * v)
    }

    val patternType = compositeSurface?.patternType ?: when {
        material.id.contains("herringbone") -> SurfacePatternType.BRICK_HERRINGBONE
        material.id.contains("four_leaf") || material.id.contains("four-leaf") -> SurfacePatternType.GRASS_PAVER_FOUR_LEAF
        material.id.contains("picket") -> SurfacePatternType.GRASS_PAVER_PICKET_SQUARE
        material.id.contains("asphalt") || material.id.contains("macadam") -> SurfacePatternType.ASPHALT_AGGREGATE_MACADAM
        material.id.contains("chevron") -> SurfacePatternType.CHEVRON_PARQUET_WOOD
        material.id.contains("paver") || material.id.contains("bowtie") -> SurfacePatternType.PAVER_INTERLOCKING_GEOMETRIC
        material.id.contains("granite") && material.id.contains("sett") -> SurfacePatternType.GRANITE_SETTS_COBBLE
        material.id.contains("flagstone") || material.id.contains("rubble") || material.id.contains("crazy") -> SurfacePatternType.STONE_RUBBLE_FLAGSTONE
        material.id.contains("terrazzo") || material.id.contains("mosaic") || material.id.contains("inlay") -> SurfacePatternType.TERRAZZO_MOSAIC_CHIPS
        material.id.contains("shingle") || material.id.contains("roof") -> SurfacePatternType.ROOF_SHINGLE_SCALLOP
        material.id.contains("corrugated") || material.id.contains("profile") -> SurfacePatternType.METAL_CORRUGATED_RIBS
        material.id.contains("tread") || material.id.contains("diamond") -> SurfacePatternType.METAL_DIAMOND_TREAD
        material.id.contains("soil") || material.id.contains("mud") -> SurfacePatternType.ORGANIC_SOIL_LOAM
        material.id.contains("grass") || material.id.contains("turf") -> SurfacePatternType.TURF_BLADE_ORGANIC
        material.id.contains("brick") -> SurfacePatternType.BRICK_BOND_MASONRY
        material.id.contains("tile") -> SurfacePatternType.TILED_GROUT_GRID
        material.id.contains("wood") -> SurfacePatternType.PLANK_GRAIN_FLOOR
        else -> SurfacePatternType.HOMOGENEOUS
    }

    when (patternType) {
        SurfacePatternType.BRICK_HERRINGBONE -> {
            // User Image 1: Pilotage Herringbone 45-degree terracotta bricks with sand joints
            val steps = (8f * ((repeatX + repeatY) * 0.5f)).roundToInt().coerceIn(6, 200)
            val mortarColor = Color(0xFFC4B8A6).copy(alpha = 0.85f)
            val brickBevel = Color(0xFFD4684A).copy(alpha = 0.55f)

            // Diagonal lines in 45-degree alternating zigzag
            for (i in 0..steps) {
                val t0 = i.toFloat() / steps
                // Diagonal positive slope
                drawLine(color = mortarColor, start = lerp(t0, 0f), end = lerp(1f, 1f - t0), strokeWidth = 2.0f)
                drawLine(color = mortarColor, start = lerp(0f, t0), end = lerp(1f - t0, 1f), strokeWidth = 2.0f)
                // Intersecting perpendicular cross-joints every 2 units
                if (i % 2 == 0) {
                    val mid = (i + 0.5f) / steps
                    if (mid < 1.0f) {
                        drawLine(color = brickBevel, start = lerp(mid, 0f), end = lerp(0f, mid), strokeWidth = 1.2f)
                    }
                }
            }
        }

        SurfacePatternType.GRASS_PAVER_FOUR_LEAF -> {
            // User Image 2: Concrete paver grid with four-leaf clover lawn grass insets
            val gridCount = (4f * ((repeatX + repeatY) * 0.5f)).roundToInt().coerceIn(3, 80)
            val frameColor = Color(0xFFC8CBCE).copy(alpha = 0.90f) // Light grey concrete
            val grassColor = Color(0xFF4C8230).copy(alpha = 0.95f) // Deep lawn green
            val innerGrass = Color(0xFF386622).copy(alpha = 0.95f)

            // Draw concrete grid lines
            for (i in 0..gridCount) {
                val t = i.toFloat() / gridCount
                drawLine(color = frameColor, start = lerp(0f, t), end = lerp(1f, t), strokeWidth = 3.0f)
                drawLine(color = frameColor, start = lerp(t, 0f), end = lerp(t, 1f), strokeWidth = 3.0f)
            }

            // Draw four-leaf clover grass clusters in each grid cell
            val step = 1.0f / gridCount
            for (gx in 0 until gridCount) {
                for (gy in 0 until gridCount) {
                    val cx = (gx + 0.5f) * step
                    val cy = (gy + 0.5f) * step
                    val radius = step * 0.28f

                    // 4 clover petals
                    val p0 = lerp(cx, cy - radius * 0.55f)
                    val p1 = lerp(cx + radius * 0.55f, cy)
                    val p2 = lerp(cx, cy + radius * 0.55f)
                    val p3 = lerp(cx - radius * 0.55f, cy)
                    val centerPt = lerp(cx, cy)

                    drawCircle(color = grassColor, radius = radius * 45f, center = p0)
                    drawCircle(color = grassColor, radius = radius * 45f, center = p1)
                    drawCircle(color = grassColor, radius = radius * 45f, center = p2)
                    drawCircle(color = grassColor, radius = radius * 45f, center = p3)
                    drawCircle(color = innerGrass, radius = radius * 26f, center = centerPt)
                }
            }
        }

        SurfacePatternType.GRASS_PAVER_PICKET_SQUARE -> {
            // User Image 3: Permeable octagonal concrete pavers with square turf insets
            val gridCount = (4f * ((repeatX + repeatY) * 0.5f)).roundToInt().coerceIn(3, 80)
            val concreteColor = Color(0xFFD0D4D6).copy(alpha = 0.95f)
            val seamColor = Color(0xFF8A8E92).copy(alpha = 0.70f)
            val turfSquareColor = Color(0xFF467B2A).copy(alpha = 0.95f)
            val step = 1.0f / gridCount

            for (gx in 0 until gridCount) {
                for (gy in 0 until gridCount) {
                    val u0 = gx * step
                    val v0 = gy * step
                    val u1 = (gx + 1) * step
                    val v1 = (gy + 1) * step

                    // Draw octagonal border seam
                    drawLine(color = seamColor, start = lerp(u0, v0), end = lerp(u1, v0), strokeWidth = 1.6f)
                    drawLine(color = seamColor, start = lerp(u0, v0), end = lerp(u0, v1), strokeWidth = 1.6f)

                    // Central square turf cutout
                    val sqMargin = step * 0.25f
                    val sq0 = lerp(u0 + sqMargin, v0 + sqMargin)
                    val sq1 = lerp(u1 - sqMargin, v0 + sqMargin)
                    val sq2 = lerp(u1 - sqMargin, v1 - sqMargin)
                    val sq3 = lerp(u0 + sqMargin, v1 - sqMargin)

                    val sqPath = Path().apply {
                        moveTo(sq0.x, sq0.y)
                        lineTo(sq1.x, sq1.y)
                        lineTo(sq2.x, sq2.y)
                        lineTo(sq3.x, sq3.y)
                        close()
                    }
                    drawPath(path = sqPath, color = turfSquareColor)
                }
            }
        }

        SurfacePatternType.ASPHALT_AGGREGATE_MACADAM -> {
            // High-detail bitumen matrix with embedded black basalt chips and mineral aggregate
            val aggregateRows = (10f * repeatY).roundToInt().coerceIn(8, 120)
            val aggregateCols = (10f * repeatX).roundToInt().coerceIn(8, 120)
            val basaltChip = Color(0xFF141516).copy(alpha = 0.90f)
            val stoneGrit = Color(0xFF787B80).copy(alpha = 0.75f)
            val tarSheen = Color(0xFF383A3D).copy(alpha = 0.40f)

            for (r in 0..aggregateRows) {
                for (c in 0..aggregateCols) {
                    val jitterU = (sin(r * 12.9898f + c * 78.233f) * 0.5f + 0.5f)
                    val jitterV = (cos(r * 43.123f + c * 93.712f) * 0.5f + 0.5f)
                    val u = ((c + jitterU * 0.8f) / aggregateCols).coerceIn(0.02f, 0.98f)
                    val v = ((r + jitterV * 0.8f) / aggregateRows).coerceIn(0.02f, 0.98f)
                    val pt = lerp(u, v)

                    val chipSize = if ((r + c) % 3 == 0) 3.5f else 2.0f
                    val chipColor = if ((r + c) % 2 == 0) basaltChip else stoneGrit
                    drawCircle(color = chipColor, radius = chipSize, center = pt)
                }
            }

            // Subtle road wear lines
            val wearCol = tarSheen
            drawLine(color = wearCol, start = lerp(0.15f, 0f), end = lerp(0.15f, 1f), strokeWidth = 14f)
            drawLine(color = wearCol, start = lerp(0.85f, 0f), end = lerp(0.85f, 1f), strokeWidth = 14f)
        }

        SurfacePatternType.PAVER_INTERLOCKING_GEOMETRIC -> {
            // Basalt V-pavers, bowtie pavers, concrete interlocking units
            val rows = (6f * repeatY).roundToInt().coerceIn(4, 200)
            val cols = (5f * repeatX).roundToInt().coerceIn(3, 200)
            val jointColor = Color(0xFF3C3E40).copy(alpha = 0.85f)
            val bevelColor = Color(0xFF888B90).copy(alpha = 0.45f)

            for (r in 0 until rows) {
                val v0 = r.toFloat() / rows
                val v1 = (r + 1).toFloat() / rows
                val shift = if (r % 2 == 1) 0.5f / cols else 0f
                for (c in 0 until cols) {
                    val u0 = (c.toFloat() / cols + shift) % 1.0f
                    val u1 = ((c + 1).toFloat() / cols + shift) % 1.0f
                    // V-chevron paver seam
                    val pMid = lerp((u0 + u1) * 0.5f, (v0 + v1) * 0.5f)
                    drawLine(color = jointColor, start = lerp(u0, v0), end = pMid, strokeWidth = 1.8f)
                    drawLine(color = jointColor, start = pMid, end = lerp(u1, v0), strokeWidth = 1.8f)
                    drawLine(color = bevelColor, start = pMid, end = lerp((u0 + u1) * 0.5f, v1), strokeWidth = 1.2f)
                }
            }
        }

        SurfacePatternType.STONE_RUBBLE_FLAGSTONE -> {
            // Natural flagstone rubble and granite crazy paving
            val divCount = (6f * ((repeatX + repeatY) * 0.5f)).roundToInt().coerceIn(4, 150)
            val jointColor = Color(0xFF6B655D).copy(alpha = 0.90f)
            val flagHighlight = Color(0xFFC0BAB0).copy(alpha = 0.35f)

            for (i in 1 until divCount) {
                val t = i.toFloat() / divCount
                // Organic wandering mortar joints
                val p0 = lerp(0f, t)
                val p1 = lerp(0.35f, (t + 0.08f * sin(i * 3.14f)).coerceIn(0.05f, 0.95f))
                val p2 = lerp(0.70f, (t - 0.06f * cos(i * 2.71f)).coerceIn(0.05f, 0.95f))
                val p3 = lerp(1.0f, t)
                drawLine(color = jointColor, start = p0, end = p1, strokeWidth = 2.2f)
                drawLine(color = jointColor, start = p1, end = p2, strokeWidth = 2.2f)
                drawLine(color = jointColor, start = p2, end = p3, strokeWidth = 2.2f)
            }
        }

        SurfacePatternType.GRANITE_SETTS_COBBLE -> {
            // Scottish granite setts in staggered courses
            val rows = (8f * repeatY).roundToInt().coerceIn(5, 200)
            val cols = (6f * repeatX).roundToInt().coerceIn(4, 200)
            val groutColor = Color(0xFF55595C).copy(alpha = 0.85f)

            for (r in 1 until rows) {
                val v = r.toFloat() / rows
                drawLine(color = groutColor, start = lerp(0f, v), end = lerp(1f, v), strokeWidth = 2.0f)
            }
            for (r in 0 until rows) {
                val v0 = r.toFloat() / rows
                val v1 = (r + 1).toFloat() / rows
                val shift = if (r % 2 == 1) 0.5f / cols else 0f
                for (c in 1 until cols) {
                    val u = (c.toFloat() / cols + shift) % 1.0f
                    drawLine(color = groutColor, start = lerp(u, v0), end = lerp(u, v1), strokeWidth = 1.8f)
                }
            }
        }

        SurfacePatternType.TERRAZZO_MOSAIC_CHIPS -> {
            // Galletta terrazzo & marble star/hex mosaic
            val chipCount = (12f * ((repeatX + repeatY) * 0.5f)).roundToInt().coerceIn(8, 60)
            val amberChip = Color(0xFFC49A58).copy(alpha = 0.80f)
            val greyChip = Color(0xFF6E7278).copy(alpha = 0.80f)
            val whiteChip = Color(0xFFFFFFFF).copy(alpha = 0.85f)

            for (i in 0 until chipCount) {
                for (j in 0 until chipCount) {
                    val u = (i + 0.5f + sin(i * 9.2f) * 0.35f) / chipCount
                    val v = (j + 0.5f + cos(j * 8.4f) * 0.35f) / chipCount
                    val pt = lerp(u.coerceIn(0.02f, 0.98f), v.coerceIn(0.02f, 0.98f))
                    val col = when ((i * 3 + j) % 3) {
                        0 -> amberChip
                        1 -> greyChip
                        else -> whiteChip
                    }
                    drawCircle(color = col, radius = 2.5f, center = pt)
                }
            }
        }

        SurfacePatternType.CHEVRON_PARQUET_WOOD -> {
            // Wenge chevron timber parquet flooring
            val chevrons = (6f * repeatY).roundToInt().coerceIn(4, 200)
            val seamCol = Color(0xFF221712).copy(alpha = 0.90f)
            val grainCol = Color(0xFF523D2E).copy(alpha = 0.45f)

            for (c in 0 until chevrons) {
                val v0 = c.toFloat() / chevrons
                val v1 = (c + 1).toFloat() / chevrons
                val vMid = (v0 + v1) * 0.5f
                drawLine(color = seamCol, start = lerp(0f, v0), end = lerp(0.5f, vMid), strokeWidth = 2.0f)
                drawLine(color = seamCol, start = lerp(0.5f, vMid), end = lerp(1f, v0), strokeWidth = 2.0f)
                // Center seam
                drawLine(color = seamCol, start = lerp(0.5f, 0f), end = lerp(0.5f, 1f), strokeWidth = 1.8f)
            }
        }

        SurfacePatternType.ROOF_SHINGLE_SCALLOP -> {
            // Bitumen hexagonal roof shingles
            val courses = (6f * repeatY).roundToInt().coerceIn(4, 200)
            val shingleLine = Color(0xFF2B2D2F).copy(alpha = 0.95f)
            val shingleShad = Color(0xFF181A1B).copy(alpha = 0.60f)

            for (k in 1 until courses) {
                val v = k.toFloat() / courses
                drawLine(color = shingleLine, start = lerp(0f, v), end = lerp(1f, v), strokeWidth = 2.5f)
                drawLine(color = shingleShad, start = lerp(0f, v + 0.02f), end = lerp(1f, v + 0.02f), strokeWidth = 1.5f)
            }
        }

        SurfacePatternType.METAL_CORRUGATED_RIBS -> {
            // Box profile steel and corrugated aluminium ribs
            val ribs = (8f * repeatX).roundToInt().coerceIn(4, 200)
            val lightRib = Color.White.copy(alpha = 0.65f)
            val shadowRib = Color.Black.copy(alpha = 0.50f)

            for (r in 0 until ribs) {
                val u = r.toFloat() / ribs
                drawLine(color = lightRib, start = lerp(u, 0f), end = lerp(u, 1f), strokeWidth = 2.0f)
                drawLine(color = shadowRib, start = lerp(u + 0.02f, 0f), end = lerp(u + 0.02f, 1f), strokeWidth = 2.5f)
            }
        }

        SurfacePatternType.METAL_DIAMOND_TREAD -> {
            // Single and four bar diamond tread plate
            val studs = (6f * ((repeatX + repeatY) * 0.5f)).roundToInt().coerceIn(4, 150)
            val lugLight = Color.White.copy(alpha = 0.70f)
            val lugDark = Color.Black.copy(alpha = 0.50f)

            for (i in 0 until studs) {
                for (j in 0 until studs) {
                    val u = (i + 0.5f) / studs
                    val v = (j + 0.5f) / studs
                    val p0 = lerp(u - 0.03f, v - 0.02f)
                    val p1 = lerp(u + 0.03f, v + 0.02f)
                    drawLine(color = lugLight, start = p0, end = p1, strokeWidth = 2.4f)
                    drawLine(color = lugDark, start = p0, end = lerp(u - 0.02f, v - 0.03f), strokeWidth = 1.5f)
                }
            }
        }

        SurfacePatternType.ORGANIC_SOIL_LOAM, SurfacePatternType.FOLIAGE_SOIL_SPLATMAP -> {
            // Rich loam soil with pebbles and scattered foliage
            val pebbles = 16
            val pebbleCol = Color(0xFF7E7262).copy(alpha = 0.85f)
            val mossGreen = Color(0xFF5B7828).copy(alpha = 0.75f)

            for (i in 0 until pebbles) {
                val u = (sin(i * 14.3f) * 0.45f + 0.5f)
                val v = (cos(i * 11.7f) * 0.45f + 0.5f)
                drawCircle(color = if (i % 2 == 0) pebbleCol else mossGreen, radius = 3.0f, center = lerp(u, v))
            }
        }

        SurfacePatternType.TURF_BLADE_ORGANIC -> {
            // Live lawn grass blade tufts
            val tufts = (10f * repeatX).roundToInt().coerceIn(6, 120)
            val bladeGreen = Color(0xFF68A33A).copy(alpha = 0.85f)
            val darkGreen = Color(0xFF385E1E).copy(alpha = 0.90f)

            for (i in 0 until tufts) {
                for (j in 0 until tufts) {
                    val u = (i + 0.5f) / tufts
                    val v = (j + 0.5f) / tufts
                    val pt0 = lerp(u, v)
                    val pt1 = lerp(u + 0.02f, (v - 0.04f).coerceAtLeast(0f))
                    drawLine(color = if ((i + j) % 2 == 0) bladeGreen else darkGreen, start = pt0, end = pt1, strokeWidth = 1.6f)
                }
            }
        }

        SurfacePatternType.BRICK_BOND_MASONRY -> {
            val rows = (6f * repeatY).roundToInt().coerceIn(3, 200)
            val cols = (4f * repeatX).roundToInt().coerceIn(2, 200)
            val mortarColor = Color(0xFFD4C8B8).copy(alpha = 0.70f) // Light sandy mortar

            // Horizontal Mortar Seams
            for (r in 1 until rows) {
                val v = r.toFloat() / rows
                drawLine(color = mortarColor, start = lerp(0f, v), end = lerp(1f, v), strokeWidth = 2.0f)
            }

            // Staggered Vertical Joints
            for (r in 0 until rows) {
                val v0 = r.toFloat() / rows
                val v1 = (r + 1).toFloat() / rows
                val shift = if (r % 2 == 1) 0.5f / cols else 0f
                for (c in 1 until cols) {
                    val u = (c.toFloat() / cols + shift) % 1.0f
                    drawLine(color = mortarColor, start = lerp(u, v0), end = lerp(u, v1), strokeWidth = 2.0f)
                }
            }
        }

        SurfacePatternType.TILED_GROUT_GRID -> {
            // Physical tile behavior: If a 1m floor has 4 tiles (2x2), then 4 m² (2m x 2m) has 16 tiles (4x4)
            // Each physical tile maintains fixed physical dimensions (e.g. 0.5m x 0.5m), tiling more units across the surface.
            val tilesX = (2f * repeatX).roundToInt().coerceIn(2, 240)
            val tilesY = (2f * repeatY).roundToInt().coerceIn(2, 240)
            val groutCol = Color(0xFF94A3B8).copy(alpha = 0.75f)
            for (i in 1 until tilesX) {
                val t = i.toFloat() / tilesX
                drawLine(color = groutCol, start = lerp(t, 0f), end = lerp(t, 1f), strokeWidth = 1.8f)
            }
            for (j in 1 until tilesY) {
                val t = j.toFloat() / tilesY
                drawLine(color = groutCol, start = lerp(0f, t), end = lerp(1f, t), strokeWidth = 1.8f)
            }
        }

        SurfacePatternType.PLANK_GRAIN_FLOOR -> {
            val planks = (5f * repeatY).roundToInt().coerceIn(3, 200)
            val seamCol = Color(0xFF382212).copy(alpha = 0.85f)
            for (p in 1 until planks) {
                val v = p.toFloat() / planks
                drawLine(color = seamCol, start = lerp(0f, v), end = lerp(1f, v), strokeWidth = 2.0f)
            }
        }

        SurfacePatternType.ASPHALT_ROAD_MARKING -> {
            val yPath = Path().apply {
                val t0 = lerp(0.44f, 0.10f)
                val t1 = lerp(0.56f, 0.10f)
                val t2 = lerp(0.56f, 0.90f)
                val t3 = lerp(0.44f, 0.90f)
                moveTo(t0.x, t0.y)
                lineTo(t1.x, t1.y)
                lineTo(t2.x, t2.y)
                lineTo(t3.x, t3.y)
                close()
            }
            drawPath(path = yPath, color = Color(0xFFFBBF24))
        }

        else -> {
            if (material.id.contains("concrete")) {
                val holes = listOf(lerp(0.25f, 0.25f), lerp(0.75f, 0.25f), lerp(0.25f, 0.75f), lerp(0.75f, 0.75f))
                for (h in holes) {
                    drawCircle(color = Color.Black.copy(alpha = 0.75f), radius = 4f, center = h)
                    drawCircle(color = Color.White.copy(alpha = 0.40f), radius = 5.5f, center = h, style = Stroke(width = 1f))
                }
            }
        }
    }
}
