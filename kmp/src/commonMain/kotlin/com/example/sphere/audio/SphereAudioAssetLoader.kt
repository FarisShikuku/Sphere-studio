package com.example.sphere.audio

import android.content.Context
import android.content.res.AssetFileDescriptor
import android.media.MediaCodec
import android.media.MediaExtractor
import android.media.MediaFormat
import com.example.sphere.nativebridge.SphereNativeEngine
import java.io.ByteArrayOutputStream
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.util.Locale
import java.util.concurrent.atomic.AtomicBoolean

internal data class DecodedMonoAudio(
    val monoPcm16: ShortArray,
    val sampleRate: Int
)

/**
 * Loads optional real recorded audio files from `assets/audio/` (supporting `.wav`, `.mp3`,
 * `.ogg`, `.m4a`, `.aac`, `.flac`), decodes them to mono PCM16, and uploads them into the native
 * C++ EngineAudioSystem / WeatherAudioSystem RAM sample cache.
 *
 * Supported filenames (case-insensitive stem):
 * - Vehicle: `truck_engine`, `air_brake`, `truck_horn`, `bus_door`, `matatu_horn`, `gear_shift`
 * - Weather: `rain_loop` (or `rain`), `wind_loop` (or `wind`), `thunder_close` (or `thunder`), `thunder_distant`
 */
object SphereAudioAssetLoader {
    private val hasLoadedOnce = AtomicBoolean(false)
    private const val ASSETS_AUDIO_DIR = "audio"
    private const val MAX_MONO_SAMPLES = 48000 * 60 // Up to 60 seconds per asset

    private val stemToSlot = mapOf(
        "air_brake" to SphereNativeEngine.AudioSampleSlots.AIR_BRAKE,
        "truck_horn" to SphereNativeEngine.AudioSampleSlots.TRUCK_HORN,
        "bus_door" to SphereNativeEngine.AudioSampleSlots.BUS_DOOR,
        "matatu_horn" to SphereNativeEngine.AudioSampleSlots.MATATU_HORN,
        "gear_shift" to SphereNativeEngine.AudioSampleSlots.GEAR_SHIFT,
        "truck_engine" to SphereNativeEngine.AudioSampleSlots.TRUCK_ENGINE,
        "rain_loop" to SphereNativeEngine.AudioSampleSlots.RAIN_LOOP,
        "rain" to SphereNativeEngine.AudioSampleSlots.RAIN_LOOP,
        "wind_loop" to SphereNativeEngine.AudioSampleSlots.WIND_LOOP,
        "wind" to SphereNativeEngine.AudioSampleSlots.WIND_LOOP,
        "thunder_close" to SphereNativeEngine.AudioSampleSlots.THUNDER_CLOSE,
        "thunder" to SphereNativeEngine.AudioSampleSlots.THUNDER_CLOSE,
        "thunder_distant" to SphereNativeEngine.AudioSampleSlots.THUNDER_DISTANT
    )

    fun loadFromAssetsIfPresent(context: Context?, forceReload: Boolean = false): Int {
        if (context == null) return 0
        if (!forceReload && !hasLoadedOnce.compareAndSet(false, true)) {
            return 0
        }

        val assets = try {
            context.assets
        } catch (_: Throwable) {
            return 0
        } ?: return 0

        val files = try {
            assets.list(ASSETS_AUDIO_DIR)
        } catch (_: Throwable) {
            null
        } ?: return 0

        if (files.isEmpty()) return 0

        var uploadedCount = 0
        for (fileName in files) {
            val dotIdx = fileName.lastIndexOf('.')
            if (dotIdx <= 0) continue
            val stem = fileName.substring(0, dotIdx).lowercase(Locale.US)
            val ext = fileName.substring(dotIdx + 1).lowercase(Locale.US)
            val slotId = stemToSlot[stem] ?: continue
            val assetPath = "$ASSETS_AUDIO_DIR/$fileName"

            val decoded: DecodedMonoAudio? = if (ext == "wav") {
                val wavBytes = try {
                    assets.open(assetPath).use { it.readBytes() }
                } catch (_: Throwable) {
                    null
                }
                val parsed = wavBytes?.let { parsePcmWav(it) }
                parsed ?: try {
                    assets.openFd(assetPath).use { afd -> decodeWithMediaCodec(afd) }
                } catch (_: Throwable) {
                    null
                }
            } else if (ext in setOf("mp3", "ogg", "m4a", "aac", "flac", "opus")) {
                try {
                    assets.openFd(assetPath).use { afd -> decodeWithMediaCodec(afd) }
                } catch (_: Throwable) {
                    null
                }
            } else {
                null
            }

            if (decoded != null && decoded.monoPcm16.isNotEmpty() && decoded.sampleRate > 0) {
                SphereNativeEngine.uploadCustomAudioSample(
                    slotId = slotId,
                    pcm16 = decoded.monoPcm16,
                    sampleRate = decoded.sampleRate
                )
                uploadedCount++
            }
        }
        return uploadedCount
    }

    internal fun resolveSlotForFilename(fileName: String): Int? {
        val dotIdx = fileName.lastIndexOf('.')
        if (dotIdx <= 0) return null
        val stem = fileName.substring(0, dotIdx).lowercase(Locale.US)
        return stemToSlot[stem]
    }

    /**
     * Pure-Kotlin RIFF/WAVE parser supporting uncompressed PCM (format = 1) at 8-bit or 16-bit,
     * any channel count (downmixed to mono).
     */
    internal fun parsePcmWav(bytes: ByteArray): DecodedMonoAudio? {
        if (bytes.size < 44) return null
        if (bytes[0] != 'R'.code.toByte() || bytes[1] != 'I'.code.toByte() ||
            bytes[2] != 'F'.code.toByte() || bytes[3] != 'F'.code.toByte()
        ) return null
        if (bytes[8] != 'W'.code.toByte() || bytes[9] != 'A'.code.toByte() ||
            bytes[10] != 'V'.code.toByte() || bytes[11] != 'E'.code.toByte()
        ) return null

        val buf = ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN)
        var offset = 12
        var audioFormat = -1
        var channels = 0
        var sampleRate = 0
        var bitsPerSample = 0
        var dataOffset = -1
        var dataSize = 0

        while (offset + 8 <= bytes.size) {
            val chunkId = String(bytes, offset, 4, Charsets.US_ASCII)
            val chunkSize = buf.getInt(offset + 4)
            val payloadOffset = offset + 8
            if (chunkSize < 0 || payloadOffset > bytes.size) break

            when (chunkId) {
                "fmt " -> {
                    if (chunkSize >= 16 && payloadOffset + 16 <= bytes.size) {
                        audioFormat = buf.getShort(payloadOffset).toInt() and 0xFFFF
                        channels = buf.getShort(payloadOffset + 2).toInt() and 0xFFFF
                        sampleRate = buf.getInt(payloadOffset + 4)
                        bitsPerSample = buf.getShort(payloadOffset + 14).toInt() and 0xFFFF
                    }
                }
                "data" -> {
                    dataOffset = payloadOffset
                    dataSize = minOf(chunkSize, bytes.size - payloadOffset)
                }
            }

            // RIFF chunks are padded to even byte boundaries
            val paddedSize = chunkSize + (chunkSize and 1)
            val nextOffset = payloadOffset + paddedSize
            if (nextOffset <= offset) break
            offset = nextOffset
        }

        if (audioFormat != 1 || channels <= 0 || sampleRate <= 0 || dataOffset < 0 || dataSize <= 0) {
            return null
        }

        val monoSamples: ShortArray = when (bitsPerSample) {
            16 -> {
                val frameBytes = channels * 2
                val frameCount = minOf(dataSize / frameBytes, MAX_MONO_SAMPLES)
                if (frameCount <= 0) return null
                ShortArray(frameCount) { f ->
                    val base = dataOffset + f * frameBytes
                    if (channels == 1) {
                        buf.getShort(base)
                    } else {
                        var sum = 0
                        for (c in 0 until channels) {
                            sum += buf.getShort(base + c * 2).toInt()
                        }
                        (sum / channels).coerceIn(Short.MIN_VALUE.toInt(), Short.MAX_VALUE.toInt()).toShort()
                    }
                }
            }
            8 -> {
                val frameBytes = channels
                val frameCount = minOf(dataSize / frameBytes, MAX_MONO_SAMPLES)
                if (frameCount <= 0) return null
                ShortArray(frameCount) { f ->
                    val base = dataOffset + f * frameBytes
                    var sum = 0
                    for (c in 0 until channels) {
                        val unsigned8 = bytes[base + c].toInt() and 0xFF
                        sum += (unsigned8 - 128) shl 8
                    }
                    (sum / channels).coerceIn(Short.MIN_VALUE.toInt(), Short.MAX_VALUE.toInt()).toShort()
                }
            }
            else -> return null
        }

        return DecodedMonoAudio(monoPcm16 = monoSamples, sampleRate = sampleRate)
    }

    private fun decodeWithMediaCodec(afd: AssetFileDescriptor): DecodedMonoAudio? {
        val extractor = MediaExtractor()
        var codec: MediaCodec? = null
        return try {
            extractor.setDataSource(afd.fileDescriptor, afd.startOffset, afd.length)
            var audioTrackIndex = -1
            var trackFormat: MediaFormat? = null
            for (i in 0 until extractor.trackCount) {
                val fmt = extractor.getTrackFormat(i)
                val mime = fmt.getString(MediaFormat.KEY_MIME) ?: ""
                if (mime.startsWith("audio/")) {
                    audioTrackIndex = i
                    trackFormat = fmt
                    break
                }
            }
            if (audioTrackIndex < 0 || trackFormat == null) return null

            extractor.selectTrack(audioTrackIndex)
            val mime = trackFormat.getString(MediaFormat.KEY_MIME) ?: return null
            var sampleRate = trackFormat.getInteger(MediaFormat.KEY_SAMPLE_RATE)
            var channelCount = trackFormat.getInteger(MediaFormat.KEY_CHANNEL_COUNT)

            codec = MediaCodec.createDecoderByType(mime)
            codec.configure(trackFormat, null, null, 0)
            codec.start()

            val pcmBytesOut = ByteArrayOutputStream(65536)
            val bufferInfo = MediaCodec.BufferInfo()
            var sawInputEos = false
            var sawOutputEos = false
            val maxRawBytes = MAX_MONO_SAMPLES * 2 * maxOf(1, channelCount)

            while (!sawOutputEos && pcmBytesOut.size() < maxRawBytes) {
                if (!sawInputEos) {
                    val inIndex = codec.dequeueInputBuffer(10_000L)
                    if (inIndex >= 0) {
                        val inBuf = codec.getInputBuffer(inIndex)
                        if (inBuf != null) {
                            val sampleSize = extractor.readSampleData(inBuf, 0)
                            if (sampleSize < 0) {
                                codec.queueInputBuffer(
                                    inIndex,
                                    0,
                                    0,
                                    0L,
                                    MediaCodec.BUFFER_FLAG_END_OF_STREAM
                                )
                                sawInputEos = true
                            } else {
                                codec.queueInputBuffer(inIndex, 0, sampleSize, extractor.sampleTime, 0)
                                extractor.advance()
                            }
                        }
                    }
                }

                val outIndex = codec.dequeueOutputBuffer(bufferInfo, 10_000L)
                if (outIndex >= 0) {
                    val outBuf = codec.getOutputBuffer(outIndex)
                    if (outBuf != null && bufferInfo.size > 0) {
                        outBuf.position(bufferInfo.offset)
                        outBuf.limit(bufferInfo.offset + bufferInfo.size)
                        val chunk = ByteArray(bufferInfo.size)
                        outBuf.get(chunk)
                        pcmBytesOut.write(chunk)
                    }
                    codec.releaseOutputBuffer(outIndex, false)
                    if ((bufferInfo.flags and MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                        sawOutputEos = true
                    }
                } else if (outIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    val newFmt = codec.outputFormat
                    if (newFmt.containsKey(MediaFormat.KEY_SAMPLE_RATE)) {
                        sampleRate = newFmt.getInteger(MediaFormat.KEY_SAMPLE_RATE)
                    }
                    if (newFmt.containsKey(MediaFormat.KEY_CHANNEL_COUNT)) {
                        channelCount = newFmt.getInteger(MediaFormat.KEY_CHANNEL_COUNT)
                    }
                }
            }

            val raw = pcmBytesOut.toByteArray()
            val safeChannels = maxOf(1, channelCount)
            val frameBytes = safeChannels * 2
            val frameCount = minOf(raw.size / frameBytes, MAX_MONO_SAMPLES)
            if (frameCount <= 0 || sampleRate <= 0) return null

            val bb = ByteBuffer.wrap(raw).order(ByteOrder.LITTLE_ENDIAN)
            val mono = ShortArray(frameCount) { f ->
                val base = f * frameBytes
                if (safeChannels == 1) {
                    bb.getShort(base)
                } else {
                    var sum = 0
                    for (c in 0 until safeChannels) {
                        sum += bb.getShort(base + c * 2).toInt()
                    }
                    (sum / safeChannels).coerceIn(Short.MIN_VALUE.toInt(), Short.MAX_VALUE.toInt()).toShort()
                }
            }
            DecodedMonoAudio(monoPcm16 = mono, sampleRate = sampleRate)
        } catch (_: Throwable) {
            null
        } finally {
            try {
                codec?.stop()
            } catch (_: Throwable) {
            }
            try {
                codec?.release()
            } catch (_: Throwable) {
            }
            try {
                extractor.release()
            } catch (_: Throwable) {
            }
        }
    }
}
