package com.example.sphere.audio

import android.content.Context
import android.media.AudioAttributes
import android.media.AudioFocusRequest
import android.media.AudioFormat
import android.media.AudioManager
import android.media.AudioTrack
import android.os.Build

/**
 * Stereo 44.1 kHz 16-bit PCM AudioTrack wrapper configured for USAGE_MEDIA / STREAM_MUSIC.
 * Using stereo USAGE_MEDIA guarantees reliable audio routing and WebRTC/bridge capture on both
 * physical Android hardware and Cloud/Streaming Android Emulators.
 */
internal class SpherePcmAudioTrack(
    context: Context?,
    private val sampleRate: Int = 44100,
    private val blockSamples: Int = 1024
) {
    private val audioManager: AudioManager? = try {
        context?.applicationContext?.getSystemService(Context.AUDIO_SERVICE) as? AudioManager
    } catch (_: Throwable) {
        null
    }

    private var focusRequest: Any? = null
    private var track: AudioTrack? = null
    private val stereoBuffer = ShortArray(blockSamples * 2)

    val isReady: Boolean
        get() = track?.state == AudioTrack.STATE_INITIALIZED

    fun start(): Boolean {
        requestFocus()
        val channelMask = AudioFormat.CHANNEL_OUT_STEREO
        val minBufBytes = try {
            val queried = AudioTrack.getMinBufferSize(
                sampleRate,
                channelMask,
                AudioFormat.ENCODING_PCM_16BIT
            )
            maxOf(queried, blockSamples * 4 * 2)
        } catch (_: Throwable) {
            blockSamples * 4 * 2
        }

        val built = try {
            AudioTrack.Builder()
                .setAudioAttributes(
                    AudioAttributes.Builder()
                        .setUsage(AudioAttributes.USAGE_MEDIA)
                        .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
                        .build()
                )
                .setAudioFormat(
                    AudioFormat.Builder()
                        .setEncoding(AudioFormat.ENCODING_PCM_16BIT)
                        .setSampleRate(sampleRate)
                        .setChannelMask(channelMask)
                        .build()
                )
                .setBufferSizeInBytes(minBufBytes)
                .setTransferMode(AudioTrack.MODE_STREAM)
                .build()
        } catch (_: Throwable) {
            null
        }

        if (built != null && built.state == AudioTrack.STATE_INITIALIZED) {
            try {
                built.setVolume(AudioTrack.getMaxVolume())
                built.play()
                track = built
                return true
            } catch (_: Throwable) {
                try {
                    built.release()
                } catch (_: Throwable) {
                }
            }
        } else {
            try {
                built?.release()
            } catch (_: Throwable) {
            }
        }
        return false
    }

    fun writeMonoBlock(monoSamples: ShortArray, count: Int) {
        val t = track ?: return
        val clamped = minOf(count, blockSamples)
        for (i in 0 until clamped) {
            val s = monoSamples[i]
            val dst = i shl 1
            stereoBuffer[dst] = s
            stereoBuffer[dst + 1] = s
        }
        try {
            t.write(stereoBuffer, 0, clamped shl 1, AudioTrack.WRITE_BLOCKING)
        } catch (_: Throwable) {
        }
    }

    fun stopAndRelease() {
        val t = track
        track = null
        if (t != null) {
            try {
                // stop() drains any remaining buffered PCM frames cleanly before release,
                // unlike pause()+flush() which discards short one-shot clips (horn/door/thunder).
                t.stop()
            } catch (_: Throwable) {
            }
            try {
                t.release()
            } catch (_: Throwable) {
            }
        }
        abandonFocus()
    }

    @Suppress("DEPRECATION")
    private fun requestFocus() {
        val am = audioManager ?: return
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                val req = AudioFocusRequest.Builder(AudioManager.AUDIOFOCUS_GAIN_TRANSIENT_MAY_DUCK)
                    .setAudioAttributes(
                        AudioAttributes.Builder()
                            .setUsage(AudioAttributes.USAGE_MEDIA)
                            .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
                            .build()
                    )
                    .build()
                am.requestAudioFocus(req)
                focusRequest = req
            } else {
                am.requestAudioFocus(null, AudioManager.STREAM_MUSIC, AudioManager.AUDIOFOCUS_GAIN_TRANSIENT_MAY_DUCK)
            }
        } catch (_: Throwable) {
        }
    }

    @Suppress("DEPRECATION")
    private fun abandonFocus() {
        val am = audioManager ?: return
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                val req = focusRequest as? AudioFocusRequest
                if (req != null) {
                    am.abandonAudioFocusRequest(req)
                }
                focusRequest = null
            } else {
                am.abandonAudioFocus(null)
            }
        } catch (_: Throwable) {
        }
    }
}
