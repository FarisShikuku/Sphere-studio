package com.example.sphere.texture

import android.content.Context
import android.graphics.Bitmap
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.graphics.ImageBitmap
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.platform.LocalContext
import com.example.sphere.model.MaterialChannelMode
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.util.concurrent.ConcurrentHashMap
import kotlin.math.sqrt

data class Ktx2ContainerInfo(
    val vkFormat: Int,
    val typeSize: Int,
    val pixelWidth: Int,
    val pixelHeight: Int,
    val levelCount: Int,
    val level0Offset: Long,
    val level0ByteLength: Long,
    val kvdMetadata: Map<String, String>
) {
    val vkFormatName: String
        get() = when (vkFormat) {
            43 -> "VK_FORMAT_R8G8B8A8_SRGB"
            37 -> "VK_FORMAT_R8G8B8A8_UNORM"
            else -> "VK_FORMAT($vkFormat)"
        }
}

data class DecodedKtx2Texture(
    val assetName: String,
    val info: Ktx2ContainerInfo,
    val albedoBitmap: Bitmap,
    val albedoImageBitmap: ImageBitmap
) {
    private val channelCache = ConcurrentHashMap<MaterialChannelMode, Bitmap>()

    fun bitmapForChannel(
        channelMode: MaterialChannelMode,
        roughness: Float,
        metallic: Float,
        occlusion: Float
    ): Bitmap {
        return when (channelMode) {
            MaterialChannelMode.ALBEDO_MAP,
            MaterialChannelMode.COMPOSITE_PBR,
            MaterialChannelMode.REFRACT_SSS_MAP,
            MaterialChannelMode.SPLIT_COMPARE -> albedoBitmap
            MaterialChannelMode.NORMAL_MAP -> channelCache.getOrPut(channelMode) {
                synthesizeNormalBitmap(albedoBitmap)
            }
            MaterialChannelMode.HEIGHT_MAP -> channelCache.getOrPut(channelMode) {
                synthesizeHeightBitmap(albedoBitmap)
            }
            MaterialChannelMode.ORM_MAP -> synthesizeOrmBitmap(albedoBitmap, occlusion, roughness, metallic)
        }
    }
}

object Ktx2TextureLoader {
    private val KTX2_MAGIC = byteArrayOf(
        0xAB.toByte(), 'K'.code.toByte(), 'T'.code.toByte(), 'X'.code.toByte(),
        ' '.code.toByte(), '2'.code.toByte(), '0'.code.toByte(), 0xBB.toByte(),
        '\r'.code.toByte(), '\n'.code.toByte(), 0x1A.toByte(), '\n'.code.toByte()
    )

    private const val VK_FORMAT_R8G8B8A8_UNORM = 37
    private const val VK_FORMAT_R8G8B8A8_SRGB = 43

    private val decodedCache = ConcurrentHashMap<String, DecodedKtx2Texture>()

    fun normalizeAssetFileName(assetPath: String?): String? {
        if (assetPath.isNullOrBlank()) return null
        return assetPath.removePrefix("file:///android_asset/")
            .substringAfterLast('/')
            .takeIf { it.endsWith(".ktx2", ignoreCase = true) }
    }

    fun parseContainer(bytes: ByteArray): Ktx2ContainerInfo? {
        if (bytes.size < 104) return null
        for (i in KTX2_MAGIC.indices) {
            if (bytes[i] != KTX2_MAGIC[i]) return null
        }

        val buf = ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN)
        val vkFormat = buf.getInt(12)
        val typeSize = buf.getInt(16)
        val pixelWidth = buf.getInt(20)
        val pixelHeight = buf.getInt(24)
        val levelCount = buf.getInt(40).coerceAtLeast(1)
        val kvdByteOffset = buf.getInt(56)
        val kvdByteLength = buf.getInt(60)

        if (pixelWidth <= 0 || pixelHeight <= 0) return null

        val level0Offset = buf.getLong(80)
        val level0Length = buf.getLong(88)
        if (level0Offset < 80 || level0Length <= 0 || level0Offset + level0Length > bytes.size) {
            return null
        }

        val metadata = parseKvd(bytes, kvdByteOffset, kvdByteLength)
        return Ktx2ContainerInfo(
            vkFormat = vkFormat,
            typeSize = typeSize,
            pixelWidth = pixelWidth,
            pixelHeight = pixelHeight,
            levelCount = levelCount,
            level0Offset = level0Offset,
            level0ByteLength = level0Length,
            kvdMetadata = metadata
        )
    }

    fun decodeFromBytes(assetName: String, bytes: ByteArray): DecodedKtx2Texture? {
        val info = parseContainer(bytes) ?: return null
        if (info.vkFormat != VK_FORMAT_R8G8B8A8_SRGB && info.vkFormat != VK_FORMAT_R8G8B8A8_UNORM) {
            return null
        }
        val expectedBytes = info.pixelWidth.toLong() * info.pixelHeight.toLong() * 4L
        if (info.level0ByteLength < expectedBytes) return null

        val pixelCount = info.pixelWidth * info.pixelHeight
        val argbPixels = IntArray(pixelCount)
        val offset = info.level0Offset.toInt()
        for (i in 0 until pixelCount) {
            val base = offset + i * 4
            val r = bytes[base].toInt() and 0xFF
            val g = bytes[base + 1].toInt() and 0xFF
            val b = bytes[base + 2].toInt() and 0xFF
            val a = bytes[base + 3].toInt() and 0xFF
            argbPixels[i] = (a shl 24) or (r shl 16) or (g shl 8) or b
        }

        val bitmap = Bitmap.createBitmap(info.pixelWidth, info.pixelHeight, Bitmap.Config.ARGB_8888)
        bitmap.setPixels(argbPixels, 0, info.pixelWidth, 0, 0, info.pixelWidth, info.pixelHeight)
        return DecodedKtx2Texture(
            assetName = assetName,
            info = info,
            albedoBitmap = bitmap,
            albedoImageBitmap = bitmap.asImageBitmap()
        )
    }

    fun loadSyncFromAssets(context: Context, assetPath: String?): DecodedKtx2Texture? {
        val fileName = normalizeAssetFileName(assetPath) ?: return null
        decodedCache[fileName]?.let { return it }
        return try {
            val bytes = context.assets.open(fileName).use { it.readBytes() }
            decodeFromBytes(fileName, bytes)?.also { decoded ->
                decodedCache[fileName] = decoded
            }
        } catch (_: Throwable) {
            null
        }
    }

    private fun parseKvd(bytes: ByteArray, offset: Int, length: Int): Map<String, String> {
        if (offset <= 0 || length <= 0 || offset + length > bytes.size) return emptyMap()
        val buf = ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN)
        val result = linkedMapOf<String, String>()
        var cursor = offset
        val end = offset + length
        while (cursor + 4 <= end) {
            val entryLen = buf.getInt(cursor)
            cursor += 4
            if (entryLen <= 0 || cursor + entryLen > end) break
            val entryBytes = bytes.copyOfRange(cursor, cursor + entryLen)
            val nullIdx = entryBytes.indexOf(0.toByte())
            if (nullIdx > 0) {
                val key = String(entryBytes, 0, nullIdx, Charsets.UTF_8)
                val valEnd = if (entryBytes.last() == 0.toByte()) entryBytes.size - 1 else entryBytes.size
                val value = if (nullIdx + 1 <= valEnd) {
                    String(entryBytes, nullIdx + 1, valEnd - (nullIdx + 1), Charsets.UTF_8)
                } else ""
                result[key] = value
            }
            val pad = (4 - (entryLen % 4)) % 4
            cursor += entryLen + pad
        }
        return result
    }
}

@Composable
fun rememberDecodedKtx2Texture(assetPath: String?): DecodedKtx2Texture? {
    val context = LocalContext.current
    val normalized = remember(assetPath) { Ktx2TextureLoader.normalizeAssetFileName(assetPath) }
    var decoded by remember(normalized) { mutableStateOf<DecodedKtx2Texture?>(null) }

    LaunchedEffect(normalized) {
        decoded = if (normalized == null) {
            null
        } else {
            withContext(Dispatchers.IO) {
                Ktx2TextureLoader.loadSyncFromAssets(context, normalized)
            }
        }
    }

    return decoded
}

private fun synthesizeHeightBitmap(src: Bitmap): Bitmap {
    val w = src.width
    val h = src.height
    val pixels = IntArray(w * h)
    src.getPixels(pixels, 0, w, 0, 0, w, h)
    val out = IntArray(w * h)
    for (i in pixels.indices) {
        val c = pixels[i]
        val r = (c ushr 16) and 0xFF
        val g = (c ushr 8) and 0xFF
        val b = c and 0xFF
        val lum = ((r * 77 + g * 150 + b * 29) ushr 8).coerceIn(0, 255)
        out[i] = (0xFF shl 24) or (lum shl 16) or (lum shl 8) or lum
    }
    return Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888).apply {
        setPixels(out, 0, w, 0, 0, w, h)
    }
}

private fun synthesizeNormalBitmap(src: Bitmap): Bitmap {
    val w = src.width
    val h = src.height
    val pixels = IntArray(w * h)
    src.getPixels(pixels, 0, w, 0, 0, w, h)
    val lum = FloatArray(w * h)
    for (i in pixels.indices) {
        val c = pixels[i]
        val r = (c ushr 16) and 0xFF
        val g = (c ushr 8) and 0xFF
        val b = c and 0xFF
        lum[i] = (0.299f * r + 0.587f * g + 0.114f * b) / 255f
    }

    val out = IntArray(w * h)
    val strength = 3.2f
    for (y in 0 until h) {
        val ym = (y - 1 + h) % h
        val yp = (y + 1) % h
        for (x in 0 until w) {
            val xm = (x - 1 + w) % w
            val xp = (x + 1) % w
            val dx = (lum[y * w + xp] - lum[y * w + xm]) * strength
            val dy = (lum[yp * w + x] - lum[ym * w + x]) * strength
            val nz = 1.0f
            val invLen = 1.0f / sqrt(dx * dx + dy * dy + nz * nz)
            val nr = (((-dx * invLen) * 0.5f + 0.5f) * 255f).toInt().coerceIn(0, 255)
            val ng = (((-dy * invLen) * 0.5f + 0.5f) * 255f).toInt().coerceIn(0, 255)
            val nb = (((nz * invLen) * 0.5f + 0.5f) * 255f).toInt().coerceIn(0, 255)
            out[y * w + x] = (0xFF shl 24) or (nr shl 16) or (ng shl 8) or nb
        }
    }
    return Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888).apply {
        setPixels(out, 0, w, 0, 0, w, h)
    }
}

private fun synthesizeOrmBitmap(
    src: Bitmap,
    occlusion: Float,
    roughness: Float,
    metallic: Float
): Bitmap {
    val w = src.width
    val h = src.height
    val pixels = IntArray(w * h)
    src.getPixels(pixels, 0, w, 0, 0, w, h)
    val out = IntArray(w * h)
    val bVal = (metallic.coerceIn(0f, 1f) * 255f).toInt().coerceIn(0, 255)
    for (i in pixels.indices) {
        val c = pixels[i]
        val r = (c ushr 16) and 0xFF
        val g = (c ushr 8) and 0xFF
        val b = c and 0xFF
        val lum = (0.299f * r + 0.587f * g + 0.114f * b) / 255f
        val ao = ((0.55f + 0.45f * lum) * occlusion).coerceIn(0f, 1f)
        val rough = (roughness + (0.5f - lum) * 0.18f).coerceIn(0.02f, 1f)
        val oByte = (ao * 255f).toInt().coerceIn(0, 255)
        val rByte = (rough * 255f).toInt().coerceIn(0, 255)
        out[i] = (0xFF shl 24) or (oByte shl 16) or (rByte shl 8) or bVal
    }
    return Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888).apply {
        setPixels(out, 0, w, 0, 0, w, h)
    }
}
