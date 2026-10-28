package com.example.sphere.viewport

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.gestures.detectDragGestures
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Layers
import androidx.compose.material.icons.filled.ViewInAr
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.*
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import coil.compose.AsyncImage
import com.example.sphere.model.CompositeSurface
import com.example.sphere.model.MaterialChannelMode
import com.example.sphere.model.MaterialSurfaceMode
import com.example.sphere.model.PbrMaterial
import com.example.sphere.model.SurfacePatternType
import kotlin.math.*

enum class PreviewMeshShape(val label: String) {
    SPHERE("Sphere"),
    PLANE("Surface Plane"),
    CUBE("Cube")
}

@Composable
fun MaterialPreviewViewport(
    material: PbrMaterial,
    modifier: Modifier = Modifier,
    compositeSurface: CompositeSurface? = null,
    channelMode: MaterialChannelMode = MaterialChannelMode.COMPOSITE_PBR,
    shape: PreviewMeshShape = PreviewMeshShape.SPHERE,
    onShapeChange: (PreviewMeshShape) -> Unit = {}
) {
    var rotX by remember { mutableFloatStateOf(15f) }
    var rotY by remember { mutableFloatStateOf(35f) }
    var lightAngle by remember { mutableFloatStateOf(45f) }
    var splitPosition by remember { mutableFloatStateOf(0.5f) }

    Box(
        modifier = modifier
            .background(Color(0xFF0A0D14))
            .pointerInput(channelMode) {
                detectDragGestures { change, dragAmount ->
                    change.consume()
                    if (channelMode == MaterialChannelMode.SPLIT_COMPARE) {
                        splitPosition = (splitPosition + dragAmount.x / size.width).coerceIn(0.05f, 0.95f)
                    } else {
                        rotY = (rotY + dragAmount.x * 0.5f) % 360f
                        rotX = (rotX - dragAmount.y * 0.5f).coerceIn(-75f, 75f)
                    }
                }
            }
            .testTag("material_preview_viewport")
    ) {
        // 1. Background Grid & Radial Studio Falloff
        Canvas(modifier = Modifier.fillMaxSize()) {
            drawRect(
                brush = Brush.radialGradient(
                    colors = listOf(Color(0xFF161F2E), Color(0xFF06080C)),
                    center = center,
                    radius = size.minDimension * 0.85f
                )
            )
            // Perspective Floor Grid Lines
            val gridStep = 36f
            val floorY = size.height * 0.72f
            for (i in -12..12) {
                val x = center.x + i * gridStep
                drawLine(
                    color = Color.White.copy(alpha = 0.035f),
                    start = Offset(x, floorY),
                    end = Offset(center.x + i * gridStep * 2.8f, size.height),
                    strokeWidth = 1f
                )
            }
        }

        // 2. Main Visualizer: Channel Modes or 3D Geometry
        if (channelMode == MaterialChannelMode.SPLIT_COMPARE) {
            // Split-Screen Interactive Comparison (Surface Flat vs 3D Geometry / Normal)
            Box(modifier = Modifier.fillMaxSize()) {
                // Left Side: Flat Surface Texture
                Box(
                    modifier = Modifier
                        .fillMaxSize()
                        .padding(end = (1f - splitPosition).coerceIn(0f, 1f).let { (it * 300).dp })
                ) {
                    if (material.assetPath != null) {
                        AsyncImage(
                            model = material.assetPath,
                            contentDescription = "Surface Texture",
                            modifier = Modifier.fillMaxSize().clip(RoundedCornerShape(8.dp)),
                            contentScale = ContentScale.Crop
                        )
                    } else {
                        Box(Modifier.fillMaxSize().background(material.baseColor))
                    }
                }

                // Right Side: Procedural 3D Geometry / Wireframe
                Canvas(
                    modifier = Modifier
                        .fillMaxSize()
                        .padding(start = (splitPosition * 300).dp)
                ) {
                    drawRect(Color(0xFF111827).copy(alpha = 0.85f))
                    // Draw Procedural 3D Instanced Blade Ribbons
                    val bladeCount = (material.bladeDensity / 80f).toInt().coerceIn(30, 200)
                    for (b in 0 until bladeCount) {
                        val bx = (b * 19) % size.width
                        val by = size.height * 0.9f - ((b * 7) % (size.height * 0.6f))
                        val bladeHeight = material.bladeLengthMm * 1.8f
                        val windOffset = sin(b * 0.4f + lightAngle * 0.05f) * (material.windSway * 28f)
                        
                        val path = Path().apply {
                            moveTo(bx, by)
                            quadraticTo(bx + windOffset * 0.5f, by - bladeHeight * 0.6f, bx + windOffset, by - bladeHeight)
                        }
                        drawPath(
                            path = path,
                            color = if (b % 4 == 0) Color(0xFF81C784) else Color(0xFF4CAF50),
                            style = Stroke(width = 3.5f, cap = StrokeCap.Round)
                        )
                    }
                }

                // Vertical Divider Line & Grab Handle
                Canvas(modifier = Modifier.fillMaxSize()) {
                    val splitX = size.width * splitPosition
                    drawLine(
                        color = Color(0xFF38BDF8),
                        start = Offset(splitX, 0f),
                        end = Offset(splitX, size.height),
                        strokeWidth = 3f
                    )
                    drawCircle(Color(0xFF38BDF8), radius = 14f, center = Offset(splitX, size.height * 0.5f))
                    drawCircle(Color.White, radius = 6f, center = Offset(splitX, size.height * 0.5f))
                }
            }
        } else if (material.assetPath != null && shape == PreviewMeshShape.PLANE && channelMode == MaterialChannelMode.COMPOSITE_PBR) {
            // High-Resolution Texture Plane Preview with PBR Specular Overlay
            Box(
                modifier = Modifier
                    .size(230.dp)
                    .align(Alignment.Center)
                    .clip(RoundedCornerShape(12.dp))
                    .background(Color.Black)
            ) {
                AsyncImage(
                    model = material.assetPath,
                    contentDescription = material.name,
                    modifier = Modifier.fillMaxSize(),
                    contentScale = ContentScale.Crop
                )
                Canvas(modifier = Modifier.fillMaxSize()) {
                    val lightRad = Math.toRadians(lightAngle.toDouble()).toFloat()
                    val highlightCenter = Offset(
                        size.width * (0.5f + cos(lightRad) * 0.28f),
                        size.height * (0.5f - sin(lightRad) * 0.28f)
                    )
                    drawCircle(
                        brush = Brush.radialGradient(
                            colors = listOf(
                                Color.White.copy(alpha = (1.0f - material.roughness) * 0.45f),
                                Color.Transparent
                            ),
                            center = highlightCenter,
                            radius = size.width * (0.15f + material.roughness * 0.5f)
                        ),
                        radius = size.width * 0.6f,
                        center = highlightCenter
                    )
                }
            }
        } else if (compositeSurface != null && shape == PreviewMeshShape.PLANE && channelMode == MaterialChannelMode.COMPOSITE_PBR) {
            // Procedural Composite Surface Render (Car Paint, Wood Floor, Ceramic Tile, Asphalt Road)
            Box(
                modifier = Modifier
                    .size(230.dp)
                    .align(Alignment.Center)
                    .clip(RoundedCornerShape(12.dp))
                    .background(Color.Black)
            ) {
                Canvas(modifier = Modifier.fillMaxSize()) {
                    when (compositeSurface.patternType) {
                        SurfacePatternType.DUAL_LAYER_CLEARCOAT -> {
                            // Vehicle High-Gloss Paint (Metal Substrate + Black/White Basecoat + Mirror Clearcoat)
                            drawRect(brush = Brush.linearGradient(listOf(Color(0xFF16191D), Color(0xFF0A0C0E), Color(0xFF1E2228))))
                            // Subtle metallic flake shimmer
                            for (f in 0 until 60) {
                                val fx = (f * 37) % size.width
                                val fy = (f * 53) % size.height
                                drawCircle(Color.White.copy(alpha = 0.35f), radius = 1.2f, center = Offset(fx, fy))
                            }
                            // High-index polyurethane clearcoat reflection streak
                            drawLine(
                                brush = Brush.linearGradient(listOf(Color.Transparent, Color.White.copy(alpha = 0.6f), Color.Transparent)),
                                start = Offset(0f, size.height * 0.35f),
                                end = Offset(size.width, size.height * 0.25f),
                                strokeWidth = 14f
                            )
                        }
                        SurfacePatternType.PLANK_GRAIN_FLOOR -> {
                            // Oak Wood Flooring Planks with bevel joinery seams
                            val plankHeight = size.height / 5f
                            for (p in 0 until 5) {
                                val plankColor = if (p % 2 == 0) Color(0xFFB88E5E) else Color(0xFFA57D4F)
                                drawRect(color = plankColor, topLeft = Offset(0f, p * plankHeight), size = Size(size.width, plankHeight))
                                // Seam line
                                drawLine(color = Color(0xFF3E2712), start = Offset(0f, p * plankHeight), end = Offset(size.width, p * plankHeight), strokeWidth = 2f)
                            }
                        }
                        SurfacePatternType.TILED_GROUT_GRID -> {
                            // Ceramic Tile Grid with Recessed Grout Lines
                            drawRect(Color(0xFF9FA4A8)) // Grout base
                            val tileStep = size.width / 4f
                            for (tx in 0 until 4) {
                                for (ty in 0 until 4) {
                                    drawRect(
                                        color = Color(0xFFF2F4F7),
                                        topLeft = Offset(tx * tileStep + 2f, ty * tileStep + 2f),
                                        size = Size(tileStep - 4f, tileStep - 4f)
                                    )
                                }
                            }
                        }
                        SurfacePatternType.ASPHALT_ROAD_MARKING -> {
                            // Bitumen Asphalt with Yellow Road Marking Paint
                            drawRect(Color(0xFF25282E))
                            // Yellow Road Stripe
                            drawRect(
                                color = Color(0xFFF5CF22),
                                topLeft = Offset(size.width * 0.42f, size.height * 0.15f),
                                size = Size(size.width * 0.16f, size.height * 0.7f)
                            )
                        }
                        else -> {
                            drawRect(material.baseColor)
                        }
                    }
                }
            }
        } else {
            // Channel Visualizer (PBR, Normal Map, ORM Map, Height Map, Refraction)
            Canvas(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(16.dp)
            ) {
                val radius = min(size.width, size.height) * 0.27f
                val sphereCenter = center

                when (channelMode) {
                    MaterialChannelMode.ALBEDO_MAP -> {
                        // Raw Albedo Map Channel (sRGB Diffuse without shading)
                        drawCircle(brush = Brush.linearGradient(listOf(material.baseColor, material.baseColor.copy(alpha = 0.85f))), radius = radius, center = sphereCenter)
                    }
                    MaterialChannelMode.NORMAL_MAP -> {
                        // Tangent Space Normal Map RGB (Z-up = Blue 0.5, 0.5, 1.0; perturbed X/Y = Red/Green)
                        val normalBlue = Color(0xFF8080FF)
                        val normalBevel = Color(0xFF60A0E0)
                        drawCircle(
                            brush = Brush.radialGradient(
                                colors = listOf(normalBlue, normalBevel, Color(0xFF4060C0)),
                                center = sphereCenter,
                                radius = radius
                            ),
                            radius = radius,
                            center = sphereCenter
                        )
                        // Bevel contours
                        drawCircle(color = Color(0xFF90C0FF), radius = radius * 0.7f, center = sphereCenter, style = Stroke(width = 2.5f))
                    }
                    MaterialChannelMode.ORM_MAP -> {
                        // ORM Packed Map (Red=AO, Green=Roughness, Blue=Metallic)
                        val rVal = (material.occlusion).coerceIn(0f, 1f)
                        val gVal = (material.roughness).coerceIn(0f, 1f)
                        val bVal = (material.metallic).coerceIn(0f, 1f)
                        val ormColor = Color(rVal, gVal, bVal, 1.0f)
                        drawCircle(brush = Brush.linearGradient(listOf(ormColor, ormColor.copy(alpha = 0.8f))), radius = radius, center = sphereCenter)
                    }
                    MaterialChannelMode.HEIGHT_MAP -> {
                        // Grayscale Displacement Height Map (Mortar/recess = dark, surface = light)
                        drawCircle(
                            brush = Brush.radialGradient(
                                colors = listOf(Color.White, Color(0xFF888888), Color(0xFF222222)),
                                center = sphereCenter,
                                radius = radius
                            ),
                            radius = radius,
                            center = sphereCenter
                        )
                    }
                    MaterialChannelMode.REFRACT_SSS_MAP -> {
                        // Optical Transmission & SSS Visualization
                        drawCircle(
                            brush = Brush.radialGradient(
                                colors = listOf(material.subsurfaceColor.copy(alpha = 0.85f), material.absorptionColor.copy(alpha = 0.6f), Color.Black),
                                center = sphereCenter,
                                radius = radius
                            ),
                            radius = radius,
                            center = sphereCenter
                        )
                    }
                    else -> {
                        // COMPOSITE_PBR: Full Sascha Willems Cook-Torrance BRDF + Snell Refraction
                        val lightRad = Math.toRadians(lightAngle.toDouble()).toFloat()
                        val lightDir = Offset(cos(lightRad), -sin(lightRad))

                        // Floor Shadow
                        val shadowCenter = Offset(sphereCenter.x - lightDir.x * 24f, sphereCenter.y + radius * 0.94f)
                        drawOval(
                            brush = Brush.radialGradient(
                                colors = listOf(Color.Black.copy(alpha = 0.58f), Color.Transparent),
                                center = shadowCenter,
                                radius = radius * 1.15f
                            ),
                            topLeft = Offset(shadowCenter.x - radius * 1.15f, shadowCenter.y - radius * 0.32f),
                            size = Size(radius * 2.3f, radius * 0.64f)
                        )

                        // Subsurface Glow
                        if (material.subsurface > 0.05f) {
                            val sssCenter = Offset(sphereCenter.x - lightDir.x * radius * 0.28f, sphereCenter.y - lightDir.y * radius * 0.28f)
                            drawCircle(
                                brush = Brush.radialGradient(
                                    colors = listOf(material.subsurfaceColor.copy(alpha = material.subsurface * 0.7f), Color.Transparent),
                                    center = sssCenter,
                                    radius = radius * 1.12f
                                ),
                                radius = radius * 1.12f,
                                center = sphereCenter
                            )
                        }

                        // Snell's Law Refraction caustic
                        if (material.transmission > 0.1f) {
                            val eta = 1.0f / material.ior.coerceAtLeast(1.0f)
                            val refractCenter = Offset(sphereCenter.x - lightDir.x * radius * (1f - eta) * 0.7f, sphereCenter.y - lightDir.y * radius * (1f - eta) * 0.7f)
                            drawCircle(
                                brush = Brush.radialGradient(
                                    colors = listOf(material.absorptionColor.copy(alpha = material.transmission * 0.65f), Color.Transparent),
                                    center = refractCenter,
                                    radius = radius * 0.85f
                                ),
                                radius = radius * 0.85f,
                                center = refractCenter
                            )
                        }

                        // Base Material Sphere
                        val highlightOffset = Offset(sphereCenter.x + lightDir.x * radius * 0.45f, sphereCenter.y + lightDir.y * radius * 0.45f)
                        val sphereGrad = Brush.radialGradient(
                            colors = listOf(
                                material.baseColor.copy(alpha = if (material.transmission > 0.5f) 0.35f else 1.0f),
                                material.baseColor.copy(red = material.baseColor.red * 0.35f, green = material.baseColor.green * 0.35f, blue = material.baseColor.blue * 0.35f),
                                Color.Black.copy(alpha = 0.92f)
                            ),
                            center = highlightOffset,
                            radius = radius * 1.1f
                        )

                        if (shape == PreviewMeshShape.CUBE) {
                            val half = radius * 0.72f
                            drawRect(brush = sphereGrad, topLeft = Offset(sphereCenter.x - half, sphereCenter.y - half), size = Size(half * 2, half * 2))
                        } else {
                            drawCircle(brush = sphereGrad, radius = radius, center = sphereCenter)
                        }

                        // Fresnel Rim
                        val f0 = if (material.metallic > 0.5f) 0.95f else ((material.ior - 1f) / (material.ior + 1f)).pow(2)
                        val rimAlpha = (f0 + (1f - f0) * 0.6f).coerceIn(0.1f, 0.9f)
                        drawCircle(
                            brush = Brush.radialGradient(listOf(Color.Transparent, Color.White.copy(alpha = rimAlpha * 0.65f)), center = sphereCenter, radius = radius),
                            radius = radius,
                            center = sphereCenter
                        )

                        // Cook-Torrance Specular
                        val specRadius = radius * (0.08f + material.roughness * 0.35f)
                        val specAlpha = ((1.0f - material.roughness) * (0.4f + material.metallic * 0.6f)).coerceIn(0.05f, 0.95f)
                        drawCircle(
                            brush = Brush.radialGradient(listOf(Color.White.copy(alpha = specAlpha), Color.Transparent), center = highlightOffset, radius = specRadius),
                            radius = specRadius,
                            center = highlightOffset
                        )

                        // If 3D geometry mode is active on nature, draw procedural 3D grass blade overlay!
                        if (material.surfaceMode == MaterialSurfaceMode.GEOMETRY_3D) {
                            for (b in 0 until 40) {
                                val ang = b * 0.16f
                                val bx = sphereCenter.x + cos(ang) * radius * 0.85f
                                val by = sphereCenter.y + sin(ang) * radius * 0.85f
                                val bladeLen = material.bladeLengthMm * 0.6f
                                drawLine(
                                    color = Color(0xFF76C442),
                                    start = Offset(bx, by),
                                    end = Offset(bx + sin(ang + 0.3f) * bladeLen, by - cos(ang + 0.3f) * bladeLen),
                                    strokeWidth = 2.5f,
                                    cap = StrokeCap.Round
                                )
                            }
                        }
                    }
                }

                // Wireframe outline
                drawCircle(color = Color.White.copy(alpha = 0.15f), radius = radius, center = sphereCenter, style = Stroke(width = 1.5f))
            }
        }

        // 3. Top Badges: Surface Mode & Channel Indicator
        Row(
            modifier = Modifier
                .align(Alignment.TopStart)
                .padding(12.dp),
            verticalAlignment = Alignment.CenterVertically
        ) {
            Surface(
                color = Color.Black.copy(alpha = 0.70f),
                shape = RoundedCornerShape(8.dp),
                border = BorderStroke(1.dp, Color.White.copy(alpha = 0.12f))
            ) {
                Row(modifier = Modifier.padding(horizontal = 8.dp, vertical = 5.dp), verticalAlignment = Alignment.CenterVertically) {
                    Icon(
                        imageVector = if (material.surfaceMode == MaterialSurfaceMode.SURFACE_ONLY) Icons.Default.Layers else Icons.Default.ViewInAr,
                        contentDescription = null,
                        tint = if (material.surfaceMode == MaterialSurfaceMode.SURFACE_ONLY) Color(0xFF64B5F6) else Color(0xFF81C784),
                        modifier = Modifier.size(15.dp)
                    )
                    Spacer(Modifier.width(5.dp))
                    Text(material.surfaceMode.label, style = MaterialTheme.typography.labelSmall.copy(fontWeight = FontWeight.Bold, color = Color.White))
                }
            }

            Spacer(Modifier.width(8.dp))

            Surface(
                color = Color.Black.copy(alpha = 0.70f),
                shape = RoundedCornerShape(8.dp),
                border = BorderStroke(1.dp, Color.White.copy(alpha = 0.12f))
            ) {
                Text(
                    text = channelMode.label,
                    modifier = Modifier.padding(horizontal = 8.dp, vertical = 5.dp),
                    style = MaterialTheme.typography.labelSmall.copy(fontFamily = FontFamily.Monospace, color = Color(0xFF38BDF8), fontWeight = FontWeight.Bold)
                )
            }
        }

        // 4. Shape Selection Pills
        Row(
            modifier = Modifier
                .align(Alignment.BottomEnd)
                .padding(12.dp),
            horizontalArrangement = Arrangement.spacedBy(4.dp)
        ) {
            PreviewMeshShape.entries.forEach { meshShape ->
                FilterChip(
                    selected = shape == meshShape,
                    onClick = { onShapeChange(meshShape) },
                    label = { Text(meshShape.label, fontSize = 11.sp) },
                    modifier = Modifier.testTag("mesh_${meshShape.name.lowercase()}"),
                    colors = FilterChipDefaults.filterChipColors(selectedContainerColor = MaterialTheme.colorScheme.primaryContainer)
                )
            }
        }
    }
}
