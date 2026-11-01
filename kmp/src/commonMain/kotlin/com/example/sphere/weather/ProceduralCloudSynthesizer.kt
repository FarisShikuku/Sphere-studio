package com.example.sphere

import android.graphics.Bitmap
import androidx.compose.ui.geometry.Offset
import kotlin.math.PI
import kotlin.math.cos
import kotlin.math.exp
import kotlin.math.max
import kotlin.math.sin
import kotlin.math.sqrt
import kotlin.math.tan
import kotlin.random.Random

// =====================================================================
// PROCEDURAL 3D VOLUMETRIC CLOUD RAYMARCHING ENGINE
// Converted & adapted from AmanSachan1/Meteoros (Aman Sachan & Meghana Seshadri)
// Based on Guerrilla Games' Decima Engine (Nubis-HZD Architecture)
// =====================================================================
object ProceduralCloudSynthesizer {
    private const val SHAPE_SIZE = 256
    private const val EROSION_SIZE = 128

    // Meteoros 4x4 Bayer matrix for ray offset jittering (eliminates woodgrain slicing artifacts)
    private val BAYER_4X4 = floatArrayOf(
        0.0f / 16.0f,  8.0f / 16.0f,  2.0f / 16.0f, 10.0f / 16.0f,
        12.0f / 16.0f,  4.0f / 16.0f, 14.0f / 16.0f,  6.0f / 16.0f,
        3.0f / 16.0f, 11.0f / 16.0f,  1.0f / 16.0f,  9.0f / 16.0f,
        15.0f / 16.0f,  7.0f / 16.0f, 13.0f / 16.0f,  5.0f / 16.0f
    )

    // Precomputed seamless periodic noise arrays for ultra-fast raymarching
    private val shapeNoise = FloatArray(SHAPE_SIZE * SHAPE_SIZE)
    private val billowNoise = FloatArray(SHAPE_SIZE * SHAPE_SIZE)
    private val erosionNoise = FloatArray(EROSION_SIZE * EROSION_SIZE)

    init {
        val perm = intArrayOf(
            151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
            8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
            35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
            134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
            55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,
            18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
            250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
            189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
            172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,
            228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,
            107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
            138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
        )
        val p = IntArray(512)
        for (i in 0 until 256) {
            p[i] = perm[i]
            p[256 + i] = perm[i]
        }

        fun grad(hash: Int, x: Float, y: Float): Float {
            val h = hash and 7
            val u = if (h < 4) x else y
            val v = if (h < 4) y else x
            return (if ((h and 1) == 0) u else -u) + (if ((h and 2) == 0) 2.0f * v else -2.0f * v)
        }

        fun perlin(x: Float, y: Float, period: Int): Float {
            val xi = (x.toInt().let { if (x < 0) it - 1 else it })
            val yi = (y.toInt().let { if (y < 0) it - 1 else it })
            val fx = x - xi
            val fy = y - yi

            val x0 = (xi % period + period) % period
            val y0 = (yi % period + period) % period
            val x1 = (x0 + 1) % period
            val y1 = (y0 + 1) % period

            val u = fx * fx * fx * (fx * (fx * 6f - 15f) + 10f)
            val v = fy * fy * fy * (fy * (fy * 6f - 15f) + 10f)

            val s1 = grad(p[p[x0] + y0], fx, fy)
            val s2 = grad(p[p[x1] + y0], fx - 1f, fy)
            val s3 = grad(p[p[x0] + y1], fx, fy - 1f)
            val s4 = grad(p[p[x1] + y1], fx - 1f, fy - 1f)

            val a = s1 + u * (s2 - s1)
            val b = s3 + u * (s4 - s3)
            return (a + v * (b - a)) * 0.5f + 0.5f
        }

        // Voronoi cell centers for Worley cauliflower billows (periodic 8x8 cells)
        val cellCount = 8
        val featureX = FloatArray(cellCount * cellCount)
        val featureY = FloatArray(cellCount * cellCount)
        val rng = Random(1337)
        for (cy in 0 until cellCount) {
            for (cx in 0 until cellCount) {
                featureX[cy * cellCount + cx] = (cx + 0.15f + rng.nextFloat() * 0.70f) / cellCount
                featureY[cy * cellCount + cx] = (cy + 0.15f + rng.nextFloat() * 0.70f) / cellCount
            }
        }

        fun worley(u: Float, v: Float): Float {
            val cu = (u % 1.0f + 1.0f) % 1.0f
            val cv = (v % 1.0f + 1.0f) % 1.0f
            val cellX = (cu * cellCount).toInt()
            val cellY = (cv * cellCount).toInt()
            var minDistSq = 999.0f

            for (dy in -1..1) {
                for (dx in -1..1) {
                    val nx = (cellX + dx + cellCount) % cellCount
                    val ny = (cellY + dy + cellCount) % cellCount
                    val fx = featureX[ny * cellCount + nx] + (if (cellX + dx < 0) -1.0f else if (cellX + dx >= cellCount) 1.0f else 0.0f)
                    val fy = featureY[ny * cellCount + nx] + (if (cellY + dy < 0) -1.0f else if (cellY + dy >= cellCount) 1.0f else 0.0f)
                    val distSq = (cu - fx) * (cu - fx) + (cv - fy) * (cv - fy)
                    if (distSq < minDistSq) minDistSq = distSq
                }
            }
            return (1.0f - sqrt(minDistSq) * (cellCount * 0.95f)).coerceIn(0f, 1f)
        }

        // Populate shapeNoise (256x256 seamless Perlin-Worley macro shapes)
        for (y in 0 until SHAPE_SIZE) {
            val fy = y.toFloat() / SHAPE_SIZE
            for (x in 0 until SHAPE_SIZE) {
                val fx = x.toFloat() / SHAPE_SIZE
                val p1 = perlin(fx * 4f, fy * 4f, 4)
                val p2 = perlin(fx * 8f + 1.2f, fy * 8f + 1.7f, 8) * 0.5f
                val w1 = worley(fx, fy)
                val w2 = worley(fx * 2f, fy * 2f) * 0.5f
                shapeNoise[y * SHAPE_SIZE + x] = ((p1 + p2) * 0.55f + (w1 + w2) * 0.45f).coerceIn(0f, 1f)
            }
        }

        // Populate billowNoise (256x256 Worley cauliflower billows)
        for (y in 0 until SHAPE_SIZE) {
            val fy = y.toFloat() / SHAPE_SIZE
            for (x in 0 until SHAPE_SIZE) {
                val fx = x.toFloat() / SHAPE_SIZE
                val w1 = worley(fx * 1.5f + 0.3f, fy * 1.5f + 0.5f)
                val w2 = worley(fx * 3.0f + 0.8f, fy * 3.0f + 0.2f)
                billowNoise[y * SHAPE_SIZE + x] = (w1 * 0.65f + w2 * 0.35f).coerceIn(0f, 1f)
            }
        }

        // Populate erosionNoise (128x128 fine edge turbulence)
        for (y in 0 until EROSION_SIZE) {
            val fy = y.toFloat() / EROSION_SIZE
            for (x in 0 until EROSION_SIZE) {
                val fx = x.toFloat() / EROSION_SIZE
                val p = perlin(fx * 8f, fy * 8f, 8)
                val w = worley(fx * 4f, fy * 4f)
                erosionNoise[y * EROSION_SIZE + x] = (p * 0.5f + w * 0.5f).coerceIn(0f, 1f)
            }
        }
    }

    private fun smoothstep(e0: Float, e1: Float, x: Float): Float {
        val t = ((x - e0) / (e1 - e0)).coerceIn(0f, 1f)
        return t * t * (3f - 2f * t)
    }

    private fun lerp(t: Float, a: Float, b: Float) = a + t * (b - a)

    // ACES filmic tonemapping curve for high dynamic range solar halos & deep cloud shadows
    private fun acesTonemap(color: Float): Float {
        val x = max(0.0f, color)
        return ((x * (2.51f * x + 0.03f)) / (x * (2.43f * x + 0.59f) + 0.14f)).coerceIn(0f, 1f)
    }

    private fun sampleShape(x: Float, z: Float): Float {
        val u = (x.toInt().let { if (x < 0) it - 1 else it } and (SHAPE_SIZE - 1))
        val v = (z.toInt().let { if (z < 0) it - 1 else it } and (SHAPE_SIZE - 1))
        return shapeNoise[v * SHAPE_SIZE + u]
    }

    private fun sampleBillow(x: Float, z: Float): Float {
        val u = (x.toInt().let { if (x < 0) it - 1 else it } and (SHAPE_SIZE - 1))
        val v = (z.toInt().let { if (z < 0) it - 1 else it } and (SHAPE_SIZE - 1))
        return billowNoise[v * SHAPE_SIZE + u]
    }

    private fun sampleErosion(x: Float, z: Float): Float {
        val u = (x.toInt().let { if (x < 0) it - 1 else it } and (EROSION_SIZE - 1))
        val v = (z.toInt().let { if (z < 0) it - 1 else it } and (EROSION_SIZE - 1))
        return erosionNoise[v * EROSION_SIZE + u]
    }

    /**
     * Evaluates 3D cloud density at world horizontal coordinates (x, z) and height fraction h.
     * Incorporates Decima / Meteoros cloud species gradients (Stratus, Stratocumulus, Cumulus, Cumulonimbus).
     */
    fun sampleDensity(
        x: Float,
        z: Float,
        h: Float,
        cloudType: Int,
        coverage: Float,
        density: Float,
        detailErosion: Float
    ): Float {
        val meteorosGrad = when (cloudType) {
            0 -> {
                // Stratus: low flat overcast blanket
                val b = smoothstep(0.02f, 0.12f, h)
                val t = 1.0f - smoothstep(0.22f, 0.36f, h)
                b * t
            }
            1 -> {
                // Stratocumulus: rolling undulating fluffy waves
                val b = smoothstep(0.04f, 0.18f, h)
                val t = 1.0f - smoothstep(0.52f, 0.72f, h)
                b * t
            }
            2 -> {
                // Cumulus: towering puffy cauliflower heaps
                val b = smoothstep(0.02f, 0.15f, h)
                val t = 1.0f - smoothstep(0.75f, 0.98f, h)
                b * t
            }
            else -> {
                // Cumulonimbus: massive anvil thundercloud spanning full depth
                val b = smoothstep(0.01f, 0.10f, h)
                val t = 1.0f - smoothstep(0.88f, 1.00f, h)
                b * t
            }
        }
        if (meteorosGrad <= 0.005f) return 0.0f

        val sX = x * 0.045f
        val sZ = z * 0.045f
        val shape = sampleShape(sX * SHAPE_SIZE, sZ * SHAPE_SIZE)

        val threshold = (1.0f - coverage * 0.88f).coerceIn(0.05f, 0.95f)
        if (shape < threshold - 0.10f) return 0.0f

        val base = ((shape - threshold) / (1.0f - threshold)).coerceIn(0f, 1f)

        val bX = x * 0.12f + h * 0.35f
        val bZ = z * 0.12f + h * 0.35f
        val billow = sampleBillow(bX * SHAPE_SIZE, bZ * SHAPE_SIZE)
        val combined = base * 0.65f + billow * 0.35f

        val eX = x * 0.30f
        val eZ = z * 0.30f
        val erosion = sampleErosion(eX * EROSION_SIZE, eZ * EROSION_SIZE)

        val eroded = (combined - (1.0f - erosion) * (detailErosion * 0.38f)).coerceAtLeast(0f)
        return (eroded * meteorosGrad * density * 2.3f).coerceIn(0f, 1f)
    }

    /**
     * Renders a true 3D perspective volumetric cloudscape raymarched across the sky dome.
     * Features AmanSachan1/Meteoros:
     *  - Multi-Scattering Dual Beer-Lambert Law approximation
     *  - Interleaved 4x4 Bayer ray jittering (temporal anti-aliasing against slicing)
     *  - Dual-Lobe Henyey-Greenstein silver lining
     *  - ACES filmic tonemapping for solar highlights
     */
    fun renderPerspectiveSky(
        width: Int = 240,
        height: Int = 135,
        camYaw: Float,
        camPitch: Float,
        timeSeconds: Float,
        windSpeed: Float,
        windAzimuth: Float,
        cloudType: Int,
        coverage: Float,
        density: Float,
        silverLiningG: Float,
        powderEffect: Float,
        sunAbsorption: Float,
        detailErosion: Float,
        cloudBaseAltitude: Float,
        isStorm: Boolean,
        horizonYFrac: Float,
        multiScatteringFactor: Float = 0.50f,
        enableBayerJitter: Boolean = true,
        sunAzimuth: Float = 225.0f,
        sunElevation: Float = 38.0f
    ): Bitmap {
        val bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
        val pixels = IntArray(width * height)

        val fovH = 75.0f * (PI.toFloat() / 180f)
        val aspect = width.toFloat() / height.toFloat()
        val tanH = tan(fovH * 0.5f)
        val tanV = tanH / aspect

        val pitchRad = camPitch * (PI.toFloat() / 180f)
        val yawRad = camYaw * (PI.toFloat() / 180f)

        val cosPitch = cos(pitchRad)
        val sinPitch = sin(pitchRad)
        val cosYaw = cos(yawRad)
        val sinYaw = sin(yawRad)

        val windRad = windAzimuth * (PI.toFloat() / 180f)
        val windDrift = timeSeconds * (windSpeed * 0.035f)
        val windX = sin(windRad) * windDrift
        val windZ = cos(windRad) * windDrift

        val sunAzRad = sunAzimuth * (PI.toFloat() / 180f)
        val sunElRad = sunElevation * (PI.toFloat() / 180f)
        val sunX = cos(sunElRad) * sin(sunAzRad)
        val sunY = sin(sunElRad)
        val sunZ = cos(sunElRad) * cos(sunAzRad)

        val baseShadowR = if (isStorm) 28 else 48
        val baseShadowG = if (isStorm) 38 else 62
        val baseShadowB = if (isStorm) 52 else 82

        val sunlitR = if (isStorm) 175 else 255
        val sunlitG = if (isStorm) 180 else 248
        val sunlitB = if (isStorm) 195 else 238

        val goldRimR = if (isStorm) 130 else 255
        val goldRimG = if (isStorm) 140 else 244
        val goldRimB = if (isStorm) 150 else 205

        val horizonSkyR = if (isStorm) 32 else 145
        val horizonSkyG = if (isStorm) 42 else 190
        val horizonSkyB = if (isStorm) 58 else 235

        val yEnd = (height * (horizonYFrac + 0.04f)).toInt().coerceIn(0, height)

        // Meteoros dual-lobe Henyey-Greenstein parameters
        val g1 = silverLiningG.coerceIn(0.5f, 0.96f)
        val g2 = -0.22f
        val g1Sq = g1 * g1
        val g2Sq = g2 * g2

        val yBase = 1.0f
        val yTop = 2.4f

        for (y in 0 until yEnd) {
            val vNorm = (1.0f - (2.0f * y) / height) * tanV
            val vy1 = vNorm * cosPitch + sinPitch
            val vz1 = -vNorm * sinPitch + cosPitch

            val rowOffset = y * width
            for (x in 0 until width) {
                val uNorm = ((2.0f * x) / width - 1.0f) * tanH
                val rx = uNorm * cosYaw + vz1 * sinYaw
                val ry = vy1
                val rz = -uNorm * sinYaw + vz1 * cosYaw

                val invLen = 1.0f / sqrt(rx * rx + ry * ry + rz * rz)
                val dirX = rx * invLen
                val dirY = ry * invLen
                val dirZ = rz * invLen

                if (dirY <= 0.012f) {
                    pixels[rowOffset + x] = 0
                    continue
                }

                val tEnter = yBase / dirY
                val tExit = yTop / dirY
                val rayLength = tExit - tEnter
                val dt = rayLength * 0.20f

                // Meteoros 4x4 Bayer interleaved ray jitter
                val bayerOffset = if (enableBayerJitter) {
                    BAYER_4X4[(y and 3) * 4 + (x and 3)] * 0.18f
                } else {
                    0.0f
                }

                val cosAlpha = (dirX * sunX + dirY * sunY + dirZ * sunZ).coerceIn(-1f, 1f)
                val term1 = 1.0f + g1Sq - 2.0f * g1 * cosAlpha
                val term2 = 1.0f + g2Sq - 2.0f * g2 * cosAlpha
                val p1 = (1.0f - g1Sq) / (term1 * sqrt(max(0.001f, term1)))
                val p2 = (1.0f - g2Sq) / (term2 * sqrt(max(0.001f, term2)))
                // Dual-lobe blend: 75% forward peak silver lining, 25% backward backscattering
                val phase = (0.75f * p1 + 0.25f * p2) * 0.24f

                var accumR = 0f
                var accumG = 0f
                var accumB = 0f
                var transmittance = 1.0f

                for (step in 0 until 5) {
                    val hFrac = ((step + 0.5f + bayerOffset) * 0.20f).coerceIn(0.05f, 0.95f)
                    val t = tEnter + hFrac * rayLength
                    val posX = t * dirX + windX
                    val posZ = t * dirZ + windZ

                    val d = sampleDensity(posX, posZ, hFrac, cloudType, coverage, density, detailErosion)
                    if (d > 0.015f) {
                        val sunD = sampleDensity(
                            posX + sunX * 0.35f,
                            posZ + sunZ * 0.35f,
                            (hFrac + sunY * 0.22f).coerceIn(0f, 1f),
                            cloudType, coverage, density, detailErosion
                        )

                        // Meteoros MultiBeer absorption along sun shadow path
                        val sunTau = sunD * sunAbsorption * 3.4f
                        val sunPrimaryBeer = exp(-sunTau)
                        val sunMultiBeer = exp(-sunTau * 0.25f) * 0.7f
                        val sunVisibility = sunPrimaryBeer + sunMultiBeer * (multiScatteringFactor * 0.5f)

                        // Meteoros Dual Beer-Lambert along view ray
                        val tau = d * dt.coerceAtMost(3.0f) * 0.50f
                        val stepPrimaryBeer = exp(-tau)
                        val stepMultiBeer = exp(-tau * 0.25f) * 0.7f
                        val stepTrans = (stepPrimaryBeer + stepMultiBeer * multiScatteringFactor).coerceIn(0f, 1f)

                        // Meteoros Powder sugar effect
                        val powder = 1.0f - exp(-2.0f * tau)
                        val stepAlpha = ((1.0f - stepPrimaryBeer) * lerp(powder, 1.0f, 1.0f - powderEffect * 0.45f)).coerceIn(0f, 1f)

                        val ambFactor = 0.55f + 0.45f * hFrac
                        val ambR = baseShadowR * ambFactor
                        val ambG = baseShadowG * ambFactor
                        val ambB = baseShadowB * ambFactor

                        val dirR = sunlitR * sunVisibility
                        val dirG = sunlitG * sunVisibility
                        val dirB = sunlitB * sunVisibility

                        val silver = phase * (1.0f - sunVisibility * 0.45f)
                        val rimR = goldRimR * silver
                        val rimG = goldRimG * silver
                        val rimB = goldRimB * silver

                        val lightR = ambR + dirR + rimR
                        val lightG = ambG + dirG + rimG
                        val lightB = ambB + dirB + rimB

                        accumR += transmittance * lightR * stepAlpha
                        accumG += transmittance * lightG * stepAlpha
                        accumB += transmittance * lightB * stepAlpha

                        transmittance *= stepTrans
                        if (transmittance < 0.02f) break
                    }
                }

                val totalAlpha = (1.0f - transmittance).coerceIn(0f, 1f)
                if (totalAlpha <= 0.015f) {
                    pixels[rowOffset + x] = 0
                    continue
                }

                val haze = (1.0f - exp(-tEnter * 0.038f)).coerceIn(0f, 0.85f)
                val rawR = lerp(haze, accumR, horizonSkyR.toFloat()) / 255.0f
                val rawG = lerp(haze, accumG, horizonSkyG.toFloat()) / 255.0f
                val rawB = lerp(haze, accumB, horizonSkyB.toFloat()) / 255.0f

                // Meteoros ACES Film Tone Mapping
                val toneR = (acesTonemap(rawR * 1.15f) * 255.0f).toInt().coerceIn(0, 255)
                val toneG = (acesTonemap(rawG * 1.15f) * 255.0f).toInt().coerceIn(0, 255)
                val toneB = (acesTonemap(rawB * 1.15f) * 255.0f).toInt().coerceIn(0, 255)
                val finalA = (totalAlpha * (1.0f - haze * 0.25f) * 255f).toInt().coerceIn(0, 255)

                pixels[rowOffset + x] = (finalA shl 24) or (toneR shl 16) or (toneG shl 8) or toneB
            }
        }

        bitmap.setPixels(pixels, 0, width, 0, 0, width, height)
        return bitmap
    }

    /**
     * Projects 3D celestial sun vector onto the screen using camera pitch and yaw.
     */
    fun projectSunToScreen(
        w: Float,
        h: Float,
        camYaw: Float,
        camPitch: Float,
        sunAzimuth: Float = 225.0f,
        sunElevation: Float = 38.0f
    ): Offset? {
        val fovH = 75.0f * (PI.toFloat() / 180f)
        val aspect = w / h
        val tanH = tan(fovH * 0.5f)
        val tanV = tanH / aspect

        val pitchRad = camPitch * (PI.toFloat() / 180f)
        val yawRad = camYaw * (PI.toFloat() / 180f)

        val sunAzRad = sunAzimuth * (PI.toFloat() / 180f)
        val sunElRad = sunElevation * (PI.toFloat() / 180f)
        val sx = cos(sunElRad) * sin(sunAzRad)
        val sy = sin(sunElRad)
        val sz = cos(sunElRad) * cos(sunAzRad)

        val cosY = cos(yawRad)
        val sinY = sin(yawRad)
        val sx1 = sx * cosY - sz * sinY
        val sy1 = sy
        val sz1 = sx * sinY + sz * cosY

        val cosP = cos(pitchRad)
        val sinP = sin(pitchRad)
        val sx2 = sx1
        val sy2 = sy1 * cosP - sz1 * sinP
        val sz2 = sy1 * sinP + sz1 * cosP

        if (sz2 <= 0.05f) return null

        val normX = sx2 / (sz2 * tanH)
        val normY = sy2 / (sz2 * tanV)

        val screenX = (normX * 0.5f + 0.5f) * w
        val screenY = (0.5f - normY * 0.5f) * h
        return Offset(screenX, screenY)
    }
}
