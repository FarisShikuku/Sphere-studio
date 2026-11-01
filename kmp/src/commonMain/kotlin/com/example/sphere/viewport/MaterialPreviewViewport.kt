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
    onShapeChange: (PreviewMeshShape) -> Unit = {}
) {
    var rotX by remember { mutableFloatStateOf(18f) }
    var rotY by remember { mutableFloatStateOf(-32f) }
    var lightAngle by remember { mutableFloatStateOf(45f) }
    var splitPosition by remember { mutableFloatStateOf(0.5f) }

    val textMeasurer = rememberTextMeasurer()
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
                    val hs = 0.96f // Half-size of the 1.0m+ reference cube

                    // 8 Vertices
                    val v0 = Vec3D(-hs,  hs,  hs) // 0: Top-Left Front
                    val v1 = Vec3D( hs,  hs,  hs) // 1: Top-Right Front
                    val v2 = Vec3D( hs, -hs,  hs) // 2: Bottom-Right Front
                    val v3 = Vec3D(-hs, -hs,  hs) // 3: Bottom-Left Front
                    val v4 = Vec3D(-hs,  hs, -hs) // 4: Top-Left Back
                    val v5 = Vec3D( hs,  hs, -hs) // 5: Top-Right Back
                    val v6 = Vec3D( hs, -hs, -hs) // 6: Bottom-Right Back
                    val v7 = Vec3D(-hs, -hs, -hs) // 7: Bottom-Left Back

                    // Stage Contact Shadow
                    val shadow0 = project(Vec3D(-hs * 1.25f, -hs - 0.02f,  hs * 1.25f))
                    val shadow1 = project(Vec3D( hs * 1.25f, -hs - 0.02f,  hs * 1.25f))
                    val shadow2 = project(Vec3D( hs * 1.25f, -hs - 0.02f, -hs * 1.25f))
                    val shadow3 = project(Vec3D(-hs * 1.25f, -hs - 0.02f, -hs * 1.25f))

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
                                channelMode = channelMode
                            )

                            if (channelMode == MaterialChannelMode.COMPOSITE_PBR || channelMode == MaterialChannelMode.ALBEDO_MAP) {
                                drawCubeFaceText(
                                    textMeasurer = textMeasurer,
                                    face = face,
                                    pts = projPts,
                                    material = material
                                )
                            }
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

                                    // Procedural Texture
                                    drawProceduralFaceTexture(
                                        material = material,
                                        compositeSurface = compositeSurface,
                                        pts = projPts,
                                        diffuse = diffuse
                                    )

                                    // Real Text & Telemetry overlay on faces!
                                    drawCubeFaceText(
                                        textMeasurer = textMeasurer,
                                        face = face,
                                        pts = projPts,
                                        material = material
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
                    val wallW = 1.85f
                    val wallH = 1.45f
                    val wallT = 0.34f

                    val hw = wallW * 0.5f
                    val hh = wallH * 0.5f
                    val ht = wallT * 0.5f

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
                                channelMode = channelMode
                            )
                            if (wf.isFront && (channelMode == MaterialChannelMode.COMPOSITE_PBR || channelMode == MaterialChannelMode.ALBEDO_MAP)) {
                                drawWallBlueprintText(
                                    textMeasurer = textMeasurer,
                                    pts = pts,
                                    material = material
                                )
                            }
                        } else {
                            val baseCol = if (wf.isCoping) Color(0xFFC5C9CD) else material.baseColor
                            val litRed = (baseCol.red * diffuse).coerceIn(0f, 1f)
                            val litGreen = (baseCol.green * diffuse).coerceIn(0f, 1f)
                            val litBlue = (baseCol.blue * diffuse).coerceIn(0f, 1f)
                            drawPath(path = path, color = Color(litRed, litGreen, litBlue))

                            if (wf.isFront) {
                                drawProceduralFaceTexture(
                                    material = material,
                                    compositeSurface = compositeSurface,
                                    pts = pts,
                                    diffuse = diffuse
                                )

                                drawWallBlueprintText(
                                    textMeasurer = textMeasurer,
                                    pts = pts,
                                    material = material
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
                    // Curved PBR sphere
                    val sRadius = min(w, h) * 0.28f
                    val sCenter = Offset(cx, cy)

                    val shadowCenter = Offset(sCenter.x - lightDir.x * 24f, sCenter.y + sRadius * 0.94f)
                    drawOval(
                        brush = Brush.radialGradient(listOf(Color.Black.copy(alpha = 0.65f), Color.Transparent), center = shadowCenter, radius = sRadius * 1.2f),
                        topLeft = Offset(shadowCenter.x - sRadius * 1.2f, shadowCenter.y - sRadius * 0.30f),
                        size = Size(sRadius * 2.4f, sRadius * 0.60f)
                    )

                    val highlight = Offset(sCenter.x + lightDir.x * sRadius * 0.40f, sCenter.y - lightDir.y * sRadius * 0.40f)

                    if (textureBitmap != null) {
                        TextureManager.drawTexturedSphere(this, textureBitmap, sCenter, sRadius)
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
                    // Ground Plane
                    val p0 = project(Vec3D(-1.7f, -0.2f,  1.7f))
                    val p1 = project(Vec3D( 1.7f, -0.2f,  1.7f))
                    val p2 = project(Vec3D( 1.7f, -0.2f, -1.7f))
                    val p3 = project(Vec3D(-1.7f, -0.2f, -1.7f))

                    val slabPath = Path().apply {
                        moveTo(p0.x, p0.y)
                        lineTo(p1.x, p1.y)
                        lineTo(p2.x, p2.y)
                        lineTo(p3.x, p3.y)
                        close()
                    }

                    if (textureBitmap != null) {
                        val surfacePts = listOf(p3, p2, p1, p0).map { SurfacePoint2D(it.x, it.y) }
                        TextureManager.drawTexturedQuad(
                            drawScope = this,
                            bitmap = textureBitmap,
                            pts = surfacePts,
                            facePath = slabPath,
                            material = material,
                            diffuse = 0.88f,
                            specular = 0f,
                            channelMode = channelMode
                        )
                    } else {
                        drawPath(path = slabPath, color = material.baseColor)
                        drawProceduralFaceTexture(material, compositeSurface, listOf(p3, p2, p1, p0), 0.85f)
                    }
                    drawPath(path = slabPath, color = Color.White.copy(alpha = 0.35f), style = Stroke(width = 1.8f))
                }
            }
        }

        // 3. Top Badges (Object & Channel)
        Row(
            modifier = Modifier
                .align(Alignment.TopStart)
                .padding(10.dp),
            verticalAlignment = Alignment.CenterVertically
        ) {
            Surface(
                color = Color.Black.copy(alpha = 0.78f),
                shape = RoundedCornerShape(6.dp),
                border = BorderStroke(1.dp, MaterialTheme.colorScheme.primary.copy(alpha = 0.5f))
            ) {
                Row(modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp), verticalAlignment = Alignment.CenterVertically) {
                    Icon(
                        imageVector = when (shape) {
                            PreviewMeshShape.CUBE -> Icons.Default.ViewInAr
                            PreviewMeshShape.WALL -> Icons.Default.Layers
                            PreviewMeshShape.SPHERE -> Icons.Default.Circle
                            PreviewMeshShape.PLANE -> Icons.Default.SquareFoot
                        },
                        contentDescription = null,
                        tint = MaterialTheme.colorScheme.primary,
                        modifier = Modifier.size(14.dp)
                    )
                    Spacer(Modifier.width(5.dp))
                    Text(
                        shape.label,
                        style = MaterialTheme.typography.labelSmall.copy(fontWeight = FontWeight.Bold, color = Color.White)
                    )
                }
            }

            Spacer(Modifier.width(6.dp))

            Surface(
                color = Color.Black.copy(alpha = 0.78f),
                shape = RoundedCornerShape(6.dp),
                border = BorderStroke(1.dp, Color.White.copy(alpha = 0.15f))
            ) {
                Text(
                    text = channelMode.label,
                    modifier = Modifier.padding(horizontal = 8.dp, vertical = 4.dp),
                    style = MaterialTheme.typography.labelSmall.copy(
                        fontFamily = FontFamily.Monospace,
                        color = Color(0xFF38BDF8),
                        fontWeight = FontWeight.Bold
                    )
                )
            }
        }

        // 4. Sun Light Rotation Button
        Surface(
            modifier = Modifier
                .align(Alignment.TopEnd)
                .padding(10.dp),
            color = Color.Black.copy(alpha = 0.78f),
            shape = RoundedCornerShape(6.dp),
            border = BorderStroke(1.dp, Color.White.copy(alpha = 0.15f))
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
// TEXT DRAWING ON THE 3D TEST CUBE FACES
// -------------------------------------------------------------
private fun DrawScope.drawCubeFaceText(
    textMeasurer: TextMeasurer,
    face: Any,
    pts: List<Point2D>, // 0: TL, 1: TR, 2: BR, 3: BL
    material: PbrMaterial
) {
    if (pts.size < 4) return

    try {
        fun lerp(u: Float, v: Float): Offset {
            val topX = pts[0].x + (pts[1].x - pts[0].x) * u
            val topY = pts[0].y + (pts[1].y - pts[0].y) * u
            val botX = pts[3].x + (pts[2].x - pts[3].x) * u
            val botY = pts[3].y + (pts[2].y - pts[3].y) * u
            return Offset(topX + (botX - topX) * v, topY + (botY - topY) * v)
        }

        val centerPt = lerp(0.5f, 0.5f)
        val topSpan = sqrt((pts[1].x - pts[0].x).pow(2) + (pts[1].y - pts[0].y).pow(2))
        val sideSpan = sqrt((pts[3].x - pts[0].x).pow(2) + (pts[3].y - pts[0].y).pow(2))

        // Center Crosshair
        val cSize = 12f
        drawLine(Color.White.copy(alpha = 0.70f), Offset(centerPt.x - cSize, centerPt.y), Offset(centerPt.x + cSize, centerPt.y), strokeWidth = 1.4f)
        drawLine(Color.White.copy(alpha = 0.70f), Offset(centerPt.x, centerPt.y - cSize), Offset(centerPt.x, centerPt.y + cSize), strokeWidth = 1.4f)
        drawCircle(Color.White.copy(alpha = 0.60f), radius = 6f, center = centerPt, style = Stroke(width = 1.2f))

        // Corner L-brackets
        val bLen = 10f
        val c0 = lerp(0.08f, 0.08f)
        drawLine(Color.White.copy(alpha = 0.75f), c0, Offset(c0.x + bLen, c0.y), strokeWidth = 1.5f)
        drawLine(Color.White.copy(alpha = 0.75f), c0, Offset(c0.x, c0.y + bLen), strokeWidth = 1.5f)

        val c1 = lerp(0.92f, 0.08f)
        drawLine(Color.White.copy(alpha = 0.75f), c1, Offset(c1.x - bLen, c1.y), strokeWidth = 1.5f)
        drawLine(Color.White.copy(alpha = 0.75f), c1, Offset(c1.x, c1.y + bLen), strokeWidth = 1.5f)

        if (topSpan >= 85f && sideSpan >= 65f) {
            val badgePos = lerp(0.12f, 0.18f)
            val badgeW = (topSpan * 0.72f).coerceIn(60f, 170f)
            val badgeH = 46f

            // Bounds check against viewport dimensions
            if (badgePos.x > -badgeW && badgePos.x < size.width &&
                badgePos.y > -badgeH && badgePos.y < size.height) {

                drawRoundRect(
                    color = Color.Black.copy(alpha = 0.75f),
                    topLeft = Offset(badgePos.x, badgePos.y),
                    size = Size(badgeW, badgeH),
                    cornerRadius = CornerRadius(4f, 4f)
                )

                // Title Text (Pre-measured to prevent negative constraint crash)
                val titleLayout = textMeasurer.measure(
                    text = "TEST CUBE 1.0m",
                    style = TextStyle(
                        color = Color.White,
                        fontSize = 11.sp,
                        fontWeight = FontWeight.Bold,
                        fontFamily = FontFamily.Monospace
                    )
                )
                drawText(
                    textLayoutResult = titleLayout,
                    topLeft = Offset(badgePos.x + 6f, badgePos.y + 4f)
                )

                // Telemetry Subtitle
                val teleLayout = textMeasurer.measure(
                    text = "ROUGH: ${(material.roughness * 100).toInt()}%  MET: ${(material.metallic * 100).toInt()}%",
                    style = TextStyle(
                        color = Color(0xFF38BDF8),
                        fontSize = 9.sp,
                        fontWeight = FontWeight.Bold,
                        fontFamily = FontFamily.Monospace
                    )
                )
                drawText(
                    textLayoutResult = teleLayout,
                    topLeft = Offset(badgePos.x + 6f, badgePos.y + 24f)
                )

                // Material Name Badge at bottom
                val bottomBadgePos = lerp(0.12f, 0.75f)
                if (bottomBadgePos.x > -badgeW && bottomBadgePos.x < size.width &&
                    bottomBadgePos.y > -20f && bottomBadgePos.y < size.height) {

                    drawRoundRect(
                        color = Color.Black.copy(alpha = 0.75f),
                        topLeft = Offset(bottomBadgePos.x, bottomBadgePos.y),
                        size = Size(badgeW, 20f),
                        cornerRadius = CornerRadius(4f, 4f)
                    )

                    val nameLayout = textMeasurer.measure(
                        text = material.name.take(20),
                        style = TextStyle(
                            color = Color(0xFFFBBF24),
                            fontSize = 9.sp,
                            fontWeight = FontWeight.Medium,
                            fontFamily = FontFamily.Monospace
                        )
                    )
                    drawText(
                        textLayoutResult = nameLayout,
                        topLeft = Offset(bottomBadgePos.x + 6f, bottomBadgePos.y + 3f)
                    )
                }
            }
        }
    } catch (_: Exception) {
        // Safe fallback
    }
}

// -------------------------------------------------------------
// TEXT DRAWING ON THE 3D ARCHITECTURAL WALL
// -------------------------------------------------------------
private fun DrawScope.drawWallBlueprintText(
    textMeasurer: TextMeasurer,
    pts: List<Point2D>,
    material: PbrMaterial
) {
    if (pts.size < 4) return

    try {
        fun lerp(u: Float, v: Float): Offset {
            val topX = pts[0].x + (pts[1].x - pts[0].x) * u
            val topY = pts[0].y + (pts[1].y - pts[0].y) * u
            val botX = pts[3].x + (pts[2].x - pts[3].x) * u
            val botY = pts[3].y + (pts[2].y - pts[3].y) * u
            return Offset(topX + (botX - topX) * v, topY + (botY - topY) * v)
        }

        val widthPx = abs(pts[1].x - pts[0].x)

        // Architectural Header Plaque
        val plaquePos = lerp(0.10f, 0.15f)
        val plaqueW = (widthPx * 0.80f).coerceAtLeast(80f)
        val plaqueH = 46f

        if (plaquePos.x > -plaqueW && plaquePos.x < size.width &&
            plaquePos.y > -plaqueH && plaquePos.y < size.height) {

            drawRoundRect(
                color = Color.Black.copy(alpha = 0.80f),
                topLeft = Offset(plaquePos.x, plaquePos.y),
                size = Size(plaqueW, plaqueH),
                cornerRadius = CornerRadius(5f, 5f)
            )

            val titleLayout = textMeasurer.measure(
                text = "ARCHITECTURAL WALL • 3.0m x 2.2m",
                style = TextStyle(
                    color = Color.White,
                    fontSize = 11.sp,
                    fontWeight = FontWeight.Bold,
                    fontFamily = FontFamily.Monospace
                )
            )
            drawText(
                textLayoutResult = titleLayout,
                topLeft = Offset(plaquePos.x + 8f, plaquePos.y + 5f)
            )

            val detailLayout = textMeasurer.measure(
                text = "THICKNESS: 300mm  •  ${material.name.take(22)}",
                style = TextStyle(
                    color = Color(0xFF38BDF8),
                    fontSize = 9.sp,
                    fontWeight = FontWeight.Medium,
                    fontFamily = FontFamily.Monospace
                )
            )
            drawText(
                textLayoutResult = detailLayout,
                topLeft = Offset(plaquePos.x + 8f, plaquePos.y + 24f)
            )
        }

        // Foundation Base line
        val b0 = lerp(0.04f, 0.94f)
        val b1 = lerp(0.96f, 0.94f)
        drawLine(Color.White.copy(alpha = 0.5f), b0, b1, strokeWidth = 1.5f)
    } catch (_: Exception) {
        // Safe fallback
    }
}

// -------------------------------------------------------------
// PROCEDURAL PATTERNS ON FACES (Bricks, Concrete, Wood, Tiles)
// -------------------------------------------------------------
private fun DrawScope.drawProceduralFaceTexture(
    material: PbrMaterial,
    compositeSurface: CompositeSurface?,
    pts: List<Point2D>,
    diffuse: Float
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
            val rows = 6
            val cols = 4
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
            val divs = 4
            val groutCol = Color(0xFF94A3B8).copy(alpha = 0.75f)
            for (i in 1 until divs) {
                val t = i.toFloat() / divs
                drawLine(color = groutCol, start = lerp(0f, t), end = lerp(1f, t), strokeWidth = 1.8f)
                drawLine(color = groutCol, start = lerp(t, 0f), end = lerp(t, 1f), strokeWidth = 1.8f)
            }
        }

        SurfacePatternType.PLANK_GRAIN_FLOOR -> {
            val planks = 5
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
