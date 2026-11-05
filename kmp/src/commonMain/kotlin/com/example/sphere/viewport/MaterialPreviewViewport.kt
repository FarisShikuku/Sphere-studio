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
    onScaleChange: ((Float) -> Unit)? = null,
    onShapeChange: (PreviewMeshShape) -> Unit = {}
) {
    var rotX by remember { mutableFloatStateOf(18f) }
    var rotY by remember { mutableFloatStateOf(-32f) }
    var lightAngle by remember { mutableFloatStateOf(45f) }
    var splitPosition by remember { mutableFloatStateOf(0.5f) }

    val context = androidx.compose.ui.platform.LocalContext.current
    val textureBitmap = remember(material.id, context) {
        TextureManager.getTextureForMaterial(context, material)
    }

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

            // Perspective Studio Ground Stage Lines
            val floorY = size.height * 0.78f
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
            // Horizon Accent Line
            drawLine(
                color = Color(0xFF38BDF8).copy(alpha = 0.14f),
                start = Offset(0f, floorY),
                end = Offset(size.width, floorY),
                strokeWidth = 1.2f
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

            val cameraDist = 4.2f
            val fovFactor = min(w, h) * 1.55f

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
                    val baseHs = 0.96f * objectScale
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

                        if (textureBitmap != null) {
                            val surfacePts = projPts.map { SurfacePoint2D(it.x, it.y) }
                            TextureManager.drawTexturedQuad(
                                drawScope = this,
                                bitmap = textureBitmap,
                                pts = surfacePts,
                                facePath = facePath,
                                material = material,
                                diffuse = diffuse,
                                specular = specular,
                                channelMode = channelMode,
                                surfaceWidthMeters = faceW,
                                surfaceHeightMeters = faceH,
                                useRealWorldScale = useRealWorldScale,
                                uvTilingMultiplierX = uvTilingMultiplierX,
                                uvTilingMultiplierY = uvTilingMultiplierY
                            )
                        } else {
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

                                    // Procedural Texture with real-world repetition
                                    val repX = ((faceW / material.tileScaleMeters.coerceAtLeast(0.1f)) * uvTilingMultiplierX).coerceIn(0.2f, 24f)
                                    val repY = ((faceH / material.tileScaleMeters.coerceAtLeast(0.1f)) * uvTilingMultiplierY).coerceIn(0.2f, 24f)
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
                        }

                        // Chamfer Bevel Rim Highlight
                        drawPath(
                            path = facePath,
                            color = Color.White.copy(alpha = (0.35f + specular * 0.45f).coerceIn(0.20f, 0.85f)),
                            style = Stroke(width = 1.8f)
                        )
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

                        if (textureBitmap != null && !wf.isCoping) {
                            val surfacePts = pts.map { SurfacePoint2D(it.x, it.y) }
                            TextureManager.drawTexturedQuad(
                                drawScope = this,
                                bitmap = textureBitmap,
                                pts = surfacePts,
                                facePath = path,
                                material = material,
                                diffuse = diffuse,
                                specular = 0f,
                                channelMode = channelMode,
                                surfaceWidthMeters = wallFaceW,
                                surfaceHeightMeters = wallFaceH,
                                useRealWorldScale = useRealWorldScale,
                                uvTilingMultiplierX = uvTilingMultiplierX,
                                uvTilingMultiplierY = uvTilingMultiplierY
                            )
                        } else {
                            val baseCol = if (wf.isCoping) Color(0xFFC5C9CD) else material.baseColor
                            val litRed = (baseCol.red * diffuse).coerceIn(0f, 1f)
                            val litGreen = (baseCol.green * diffuse).coerceIn(0f, 1f)
                            val litBlue = (baseCol.blue * diffuse).coerceIn(0f, 1f)
                            drawPath(path = path, color = Color(litRed, litGreen, litBlue))

                            if (wf.isFront) {
                                val repX = ((wallFaceW / material.tileScaleMeters.coerceAtLeast(0.1f)) * uvTilingMultiplierX).coerceIn(0.2f, 24f)
                                val repY = ((wallFaceH / material.tileScaleMeters.coerceAtLeast(0.1f)) * uvTilingMultiplierY).coerceIn(0.2f, 24f)
                                drawProceduralFaceTexture(
                                    material = material,
                                    compositeSurface = compositeSurface,
                                    pts = pts,
                                    diffuse = diffuse,
                                    repeatX = if (useRealWorldScale) repX else uvTilingMultiplierX,
                                    repeatY = if (useRealWorldScale) repY else uvTilingMultiplierY
                                )
                            }
                        }

                        drawPath(
                            path = path,
                            color = Color.White.copy(alpha = if (wf.isFront) 0.35f else 0.20f),
                            style = Stroke(width = 1.6f)
                        )
                    }
                }

                PreviewMeshShape.SPHERE -> {
                    // Curved PBR sphere scaled with object dimension modifiers
                    val sScale = (objectScale * ((objectWidthScale + objectHeightScale + objectDepthScale) / 3f)).coerceIn(0.35f, 2.5f)
                    val sRadius = min(w, h) * 0.28f * sScale
                    val sCenter = Offset(cx, cy)

                    val shadowCenter = Offset(sCenter.x - lightDir.x * 24f, sCenter.y + sRadius * 0.94f)
                    drawOval(
                        brush = Brush.radialGradient(listOf(Color.Black.copy(alpha = 0.65f), Color.Transparent), center = shadowCenter, radius = sRadius * 1.2f),
                        topLeft = Offset(shadowCenter.x - sRadius * 1.2f, shadowCenter.y - sRadius * 0.30f),
                        size = Size(sRadius * 2.4f, sRadius * 0.60f)
                    )

                    val highlight = Offset(sCenter.x + lightDir.x * sRadius * 0.40f, sCenter.y - lightDir.y * sRadius * 0.40f)

                    if (textureBitmap != null) {
                        val sphDiam = sScale * 1.6f
                        TextureManager.drawTexturedSphere(
                            drawScope = this,
                            bitmap = textureBitmap,
                            center = sCenter,
                            radius = sRadius,
                            sphereDiameterMeters = sphDiam,
                            material = material,
                            useRealWorldScale = useRealWorldScale,
                            uvTilingMultiplier = uvTilingMultiplierX
                        )
                        val sphereShade = Brush.radialGradient(
                            colors = listOf(
                                Color.Transparent,
                                Color.Black.copy(alpha = 0.42f),
                                Color.Black.copy(alpha = 0.88f)
                            ),
                            center = highlight,
                            radius = sRadius * 1.15f
                        )
                        drawCircle(brush = sphereShade, radius = sRadius, center = sCenter)
                    } else {
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
                    }

                    val specRadius = sRadius * (0.08f + material.roughness * 0.36f)
                    val specAlpha = ((1.0f - material.roughness) * (0.5f + material.metallic * 0.5f)).coerceIn(0.1f, 0.95f)
                    drawCircle(
                        brush = Brush.radialGradient(listOf(Color.White.copy(alpha = specAlpha), Color.Transparent), center = highlight, radius = specRadius),
                        radius = specRadius,
                        center = highlight
                    )
                    drawCircle(color = Color.White.copy(alpha = 0.25f), radius = sRadius, center = sCenter, style = Stroke(width = 1.6f))
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
                            if (textureBitmap != null) {
                                val surfacePts = pts.map { SurfacePoint2D(it.x, it.y) }
                                TextureManager.drawTexturedQuad(
                                    drawScope = this,
                                    bitmap = textureBitmap,
                                    pts = surfacePts,
                                    facePath = path,
                                    material = material,
                                    diffuse = diffuse,
                                    specular = 0f,
                                    channelMode = channelMode,
                                    surfaceWidthMeters = floorFaceW,
                                    surfaceHeightMeters = floorFaceD,
                                    useRealWorldScale = useRealWorldScale,
                                    uvTilingMultiplierX = uvTilingMultiplierX,
                                    uvTilingMultiplierY = uvTilingMultiplierY
                                )
                            } else {
                                drawPath(path = path, color = material.baseColor)
                                val repX = ((floorFaceW / material.tileScaleMeters.coerceAtLeast(0.1f)) * uvTilingMultiplierX).coerceIn(0.2f, 24f)
                                val repY = ((floorFaceD / material.tileScaleMeters.coerceAtLeast(0.1f)) * uvTilingMultiplierY).coerceIn(0.2f, 24f)
                                drawProceduralFaceTexture(
                                    material = material,
                                    compositeSurface = compositeSurface,
                                    pts = pts,
                                    diffuse = diffuse,
                                    repeatX = if (useRealWorldScale) repX else uvTilingMultiplierX,
                                    repeatY = if (useRealWorldScale) repY else uvTilingMultiplierY
                                )
                            }
                        } else {
                            val edgeBase = Color(0xFF64748B)
                            val litCol = Color(
                                (edgeBase.red * diffuse).coerceIn(0f, 1f),
                                (edgeBase.green * diffuse).coerceIn(0f, 1f),
                                (edgeBase.blue * diffuse).coerceIn(0f, 1f)
                            )
                            drawPath(path = path, color = litCol)
                        }

                        drawPath(
                            path = path,
                            color = Color.White.copy(alpha = if (ff.isTop) 0.35f else 0.20f),
                            style = Stroke(width = 1.6f)
                        )
                    }
                }
            }
        }

        // Sun Light Rotation Control (Top End)
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
        material.id.contains("brick") -> SurfacePatternType.BRICK_BOND_MASONRY
        material.id.contains("tile") -> SurfacePatternType.TILED_GROUT_GRID
        material.id.contains("wood") -> SurfacePatternType.PLANK_GRAIN_FLOOR
        material.id.contains("asphalt") -> SurfacePatternType.ASPHALT_ROAD_MARKING
        else -> SurfacePatternType.HOMOGENEOUS
    }

    when (patternType) {
        SurfacePatternType.BRICK_BOND_MASONRY -> {
            val rows = (6f * repeatY).roundToInt().coerceIn(3, 26)
            val cols = (4f * repeatX).roundToInt().coerceIn(2, 22)
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
            val divs = (4f * ((repeatX + repeatY) * 0.5f)).roundToInt().coerceIn(2, 18)
            val groutCol = Color(0xFF94A3B8).copy(alpha = 0.75f)
            for (i in 1 until divs) {
                val t = i.toFloat() / divs
                drawLine(color = groutCol, start = lerp(0f, t), end = lerp(1f, t), strokeWidth = 1.8f)
                drawLine(color = groutCol, start = lerp(t, 0f), end = lerp(t, 1f), strokeWidth = 1.8f)
            }
        }

        SurfacePatternType.PLANK_GRAIN_FLOOR -> {
            val planks = (5f * repeatY).roundToInt().coerceIn(3, 22)
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
                // 4 Formwork Tie Holes
                val holes = listOf(lerp(0.25f, 0.25f), lerp(0.75f, 0.25f), lerp(0.25f, 0.75f), lerp(0.75f, 0.75f))
                for (h in holes) {
                    drawCircle(color = Color.Black.copy(alpha = 0.75f), radius = 4f, center = h)
                    drawCircle(color = Color.White.copy(alpha = 0.40f), radius = 5.5f, center = h, style = Stroke(width = 1f))
                }
            }
        }
    }
}
