package com.example.sphere.viewport

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.asAndroidPath
import androidx.compose.ui.graphics.drawscope.DrawScope
import androidx.compose.ui.graphics.drawscope.drawIntoCanvas
import androidx.compose.ui.graphics.nativeCanvas
import com.example.sphere.model.MaterialChannelMode
import com.example.sphere.model.PbrMaterial
import java.io.File
import java.util.concurrent.ConcurrentHashMap

data class SurfacePoint2D(val x: Float, val y: Float)

object TextureManager {
    private val bitmapCache = ConcurrentHashMap<String, Bitmap?>()

    /**
     * Resolves the matching photographic texture bitmap for a given material.
     */
    fun getTextureForMaterial(context: Context, material: PbrMaterial): Bitmap? {
        val key = when {
            material.id.contains("grass") || material.name.contains("Grass", ignoreCase = true) -> "mondo_grass"
            material.id.contains("foliage") || material.id.contains("earth") ||
                material.name.contains("Foliage", ignoreCase = true) || material.name.contains("Earth", ignoreCase = true) -> "earth_foliage"
            material.id.contains("brick") || material.name.contains("Brick", ignoreCase = true) -> "rusticated_red_brick"
            material.assetPath != null -> {
                val clean = material.assetPath.substringAfterLast("/").substringBeforeLast(".")
                if (clean.isNotEmpty()) clean else "rusticated_red_brick"
            }
            else -> null
        } ?: return null

        return bitmapCache.getOrPut(key) {
            loadTexture(context, key)
        }
    }

    private fun loadTexture(context: Context, baseName: String): Bitmap? {
        // High-Performance Studio: Prioritize KTX2 container, fallback to JPG if needed
        val ktx2Name = "$baseName.ktx2"
        val jpgName = "$baseName.jpg"

        // 1. Try loading KTX2 from assets
        loadFromAssets(context, ktx2Name)?.let { return it }

        // 2. Try loading KTX2 from disk paths
        loadFromDisk(ktx2Name)?.let { return it }

        // Fallback: JPG if KTX2 not found
        loadFromAssets(context, jpgName)?.let { return it }
        loadFromDisk(jpgName)?.let { return it }

        return null
    }

    private fun loadFromAssets(context: Context, filename: String): Bitmap? {
        return try {
            context.assets.open(filename).use { stream ->
                if (filename.endsWith(".ktx2", ignoreCase = true)) {
                    val bytes = stream.readBytes()
                    parseKtx2(bytes)
                } else {
                    val opts = BitmapFactory.Options().apply {
                        inPreferredConfig = Bitmap.Config.ARGB_8888
                    }
                    BitmapFactory.decodeStream(stream, null, opts)
                }
            }
        } catch (_: Exception) {
            null
        }
    }

    private fun loadFromDisk(filename: String): Bitmap? {
        val paths = listOf(
            "textures/$filename",
            "native_assets/textures/$filename",
            "../textures/$filename",
            "public/assets/aistudio/$filename",
            "../public/assets/aistudio/$filename"
        )
        for (p in paths) {
            try {
                val f = File(p)
                if (f.exists() && f.canRead()) {
                    if (filename.endsWith(".ktx2", ignoreCase = true)) {
                        val bytes = f.readBytes()
                        val bmp = parseKtx2(bytes)
                        if (bmp != null) return bmp
                    } else {
                        val bmp = BitmapFactory.decodeFile(f.absolutePath)
                        if (bmp != null) return bmp
                    }
                }
            } catch (_: Exception) {}
        }
        return null
    }

    /**
     * Parses standard Khronos KTX 2.0 binary headers and extracts level-0 RGBA texels.
     * High performance: Zero JPEG/PNG decoding overhead.
     */
    fun parseKtx2(bytes: ByteArray): Bitmap? {
        if (bytes.size < 80 + 24) return null
        // KTX2 Identifier: \xAB K T X ' ' '2' '0' \xBB \r \n \x1A \n
        val ktx2Magic = byteArrayOf(0xAB.toByte(), 0x4B, 0x54, 0x58, 0x20, 0x32, 0x30, 0xBB.toByte(), 0x0D, 0x0A, 0x1A, 0x0A)
        for (i in 0 until 12) {
            if (bytes[i] != ktx2Magic[i]) return null
        }

        val buffer = java.nio.ByteBuffer.wrap(bytes).order(java.nio.ByteOrder.LITTLE_ENDIAN)
        val pixelWidth = buffer.getInt(20)
        val pixelHeight = buffer.getInt(24)
        val levelCount = buffer.getInt(40).coerceAtLeast(1)

        // Level 0 index is at offset 80
        val byteOffset = buffer.getLong(80).toInt()
        val byteLength = buffer.getLong(88).toInt()

        if (pixelWidth <= 0 || pixelHeight <= 0) return null
        val requiredBytes = pixelWidth * pixelHeight * 4
        if (byteOffset < 0 || byteOffset + requiredBytes > bytes.size) return null

        return try {
            val bitmap = Bitmap.createBitmap(pixelWidth, pixelHeight, Bitmap.Config.ARGB_8888)
            val pixelBuffer = java.nio.ByteBuffer.wrap(bytes, byteOffset, requiredBytes)
            bitmap.copyPixelsFromBuffer(pixelBuffer)
            bitmap
        } catch (_: Exception) {
            null
        }
    }

    /**
     * Renders a perspective-mapped photographic texture onto an arbitrary 3D quadrilateral.
     */
    fun drawTexturedQuad(
        drawScope: DrawScope,
        bitmap: Bitmap,
        pts: List<SurfacePoint2D>,
        facePath: Path,
        material: PbrMaterial,
        diffuse: Float,
        specular: Float,
        channelMode: MaterialChannelMode
    ) {
        if (pts.size < 4) return

        drawScope.drawIntoCanvas { canvas ->
            val nativeCanvas = canvas.nativeCanvas
            nativeCanvas.save()
            nativeCanvas.clipPath(facePath.asAndroidPath())

            val src = floatArrayOf(
                0f, 0f,
                bitmap.width.toFloat(), 0f,
                bitmap.width.toFloat(), bitmap.height.toFloat(),
                0f, bitmap.height.toFloat()
            )
            val dst = floatArrayOf(
                pts[0].x, pts[0].y,
                pts[1].x, pts[1].y,
                pts[2].x, pts[2].y,
                pts[3].x, pts[3].y
            )

            val matrix = android.graphics.Matrix()
            val valid = matrix.setPolyToPoly(src, 0, dst, 0, 4)

            val paint = android.graphics.Paint(android.graphics.Paint.ANTI_ALIAS_FLAG or android.graphics.Paint.FILTER_BITMAP_FLAG)
            paint.isDither = true

            when (channelMode) {
                MaterialChannelMode.ALBEDO_MAP -> {
                    // Pure unshaded photographic albedo as uploaded
                    paint.colorFilter = null
                }
                MaterialChannelMode.NORMAL_MAP -> {
                    // Normal space perturbation color
                    val nColor = android.graphics.Color.rgb(128, 128, 255)
                    paint.colorFilter = android.graphics.PorterDuffColorFilter(nColor, android.graphics.PorterDuff.Mode.MULTIPLY)
                }
                MaterialChannelMode.ORM_MAP -> {
                    val r = (material.occlusion * 255f).toInt().coerceIn(0, 255)
                    val g = (material.roughness * 255f).toInt().coerceIn(0, 255)
                    val b = (material.metallic * 255f).toInt().coerceIn(0, 255)
                    paint.colorFilter = android.graphics.PorterDuffColorFilter(
                        android.graphics.Color.rgb(r, g, b),
                        android.graphics.PorterDuff.Mode.MULTIPLY
                    )
                }
                MaterialChannelMode.HEIGHT_MAP -> {
                    val h = 180
                    paint.colorFilter = android.graphics.PorterDuffColorFilter(
                        android.graphics.Color.rgb(h, h, h),
                        android.graphics.PorterDuff.Mode.MULTIPLY
                    )
                }
                else -> {
                    // Full PBR: Direct sunlight + ambient diffuse lighting
                    val shade = diffuse.coerceIn(0.28f, 1.15f)
                    val sInt = (shade * 255f).toInt().coerceIn(0, 255)
                    val lightColor = android.graphics.Color.rgb(sInt, sInt, sInt)
                    paint.colorFilter = android.graphics.PorterDuffColorFilter(lightColor, android.graphics.PorterDuff.Mode.MULTIPLY)
                }
            }

            if (valid) {
                nativeCanvas.drawBitmap(bitmap, matrix, paint)
            }

            // Specular sheen for glossy / dielectric PBR materials
            if (channelMode == MaterialChannelMode.COMPOSITE_PBR && specular > 0.05f) {
                val specAlpha = (specular * 255f).toInt().coerceIn(0, 180)
                val specPaint = android.graphics.Paint().apply {
                    color = android.graphics.Color.argb(specAlpha, 255, 255, 255)
                    xfermode = android.graphics.PorterDuffXfermode(android.graphics.PorterDuff.Mode.SCREEN)
                }
                nativeCanvas.drawPaint(specPaint)
            }

            nativeCanvas.restore()
        }
    }

    /**
     * Renders a circular-clipped photographic texture onto a 3D sphere.
     */
    fun drawTexturedSphere(
        drawScope: DrawScope,
        bitmap: Bitmap,
        center: androidx.compose.ui.geometry.Offset,
        radius: Float
    ) {
        drawScope.drawIntoCanvas { canvas ->
            val nativeCanvas = canvas.nativeCanvas
            nativeCanvas.save()
            val spherePath = android.graphics.Path().apply {
                addCircle(center.x, center.y, radius, android.graphics.Path.Direction.CW)
            }
            nativeCanvas.clipPath(spherePath)
            val dstRect = android.graphics.RectF(center.x - radius, center.y - radius, center.x + radius, center.y + radius)
            val paint = android.graphics.Paint(android.graphics.Paint.ANTI_ALIAS_FLAG or android.graphics.Paint.FILTER_BITMAP_FLAG)
            nativeCanvas.drawBitmap(bitmap, null, dstRect, paint)
            nativeCanvas.restore()
        }
    }
}
