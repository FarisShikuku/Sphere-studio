package com.example.sphere

import android.graphics.Bitmap
import androidx.compose.animation.core.LinearEasing
import androidx.compose.animation.core.RepeatMode
import androidx.compose.animation.core.animateFloat
import androidx.compose.animation.core.infiniteRepeatable
import androidx.compose.animation.core.rememberInfiniteTransition
import androidx.compose.animation.core.tween
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.gestures.detectDragGestures
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.AcUnit
import androidx.compose.material.icons.filled.Cloud
import androidx.compose.material.icons.filled.FilterDrama
import androidx.compose.material.icons.filled.FlashOn
import androidx.compose.material.icons.filled.Grain
import androidx.compose.material.icons.filled.Pause
import androidx.compose.material.icons.filled.PlayArrow
import androidx.compose.material.icons.filled.Refresh
import androidx.compose.material.icons.filled.WaterDrop
import androidx.compose.material.icons.filled.WbSunny
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ScrollableTabRow
import androidx.compose.material3.Slider
import androidx.compose.material3.SliderDefaults
import androidx.compose.material3.Surface
import androidx.compose.material3.Switch
import androidx.compose.material3.SwitchDefaults
import androidx.compose.material3.Tab
import androidx.compose.material3.TabRowDefaults
import androidx.compose.material3.TabRowDefaults.tabIndicatorOffset
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.FilterQuality
import androidx.compose.ui.graphics.ImageBitmap
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.graphics.drawscope.DrawScope
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.graphics.drawscope.clipRect
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.IntOffset
import androidx.compose.ui.unit.IntSize
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import kotlinx.coroutines.delay
import kotlin.math.PI
import kotlin.math.abs
import kotlin.math.cos
import kotlin.math.exp
import kotlin.math.max
import kotlin.math.min
import kotlin.math.sin
import kotlin.math.sqrt
import kotlin.math.tan
import kotlin.random.Random

data class WeatherPreset(
    val name: String,
    val icon: String,
    val rainIntensity: Float,
    val snowIntensity: Float,
    val cloudCoverage: Float,
    val fogDensity: Float,
    val windSpeed: Float,
    val windAzimuth: Float,
    val isStormy: Boolean,
    val cloudType: Int = 2 // 0=Stratus, 1=Stratocumulus, 2=Cumulus
)

val weatherPresetsList = listOf(
    WeatherPreset("Clear", "☀", 0.0f, 0.0f, 0.18f, 0.01f, 6.0f, 45f, false, cloudType = 2),
    WeatherPreset("Overcast", "☁", 0.0f, 0.0f, 0.88f, 0.08f, 12.0f, 60f, false, cloudType = 1),
    WeatherPreset("Rain", "🌧", 0.55f, 0.0f, 0.90f, 0.15f, 18.0f, 225f, false, cloudType = 1),
    WeatherPreset("Downpour", "⛈", 0.95f, 0.0f, 0.98f, 0.25f, 36.0f, 240f, true, cloudType = 2),
    WeatherPreset("Snow", "❄", 0.0f, 0.50f, 0.75f, 0.18f, 10.0f, 90f, false, cloudType = 1),
    WeatherPreset("Blizzard", "🌨", 0.0f, 1.0f, 1.0f, 0.55f, 55.0f, 315f, true, cloudType = 0),
    WeatherPreset("Dense Fog", "🌫", 0.05f, 0.0f, 0.70f, 0.85f, 3.0f, 0f, false, cloudType = 0),
    WeatherPreset("Thunderstorm", "⚡", 0.88f, 0.0f, 0.98f, 0.30f, 44.0f, 270f, true, cloudType = 2)
)

// =====================================================================
// PROCEDURAL 3D VOLUMETRIC CLOUD RAYMARCHING ENGINE
// True perspective projection through 3D world space (Nubis-Decima model)
// Converted from Robert Beckebans's OpenGL-VolumetricClouds
// =====================================================================
object ProceduralCloudSynthesizer {
    private const val SHAPE_SIZE = 256
    private const val EROSION_SIZE = 128

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
     */
    fun sampleDensity(
        x: Float,
        z: Float,
        h: Float, // 0.0 at base, 1.0 at top
        cloudType: Int, // 0=Stratus, 1=Stratocumulus, 2=Cumulus
        coverage: Float,
        density: Float,
        detailErosion: Float
    ): Float {
        // Nubis height profile curves (Lifted Condensation Level base, convective body, anvil top):
        val nubisGrad = when (cloudType) {
            0 -> {
                // Stratus: Smooth flat low sheet
                val b = smoothstep(0.04f, 0.14f, h)
                val t = 1.0f - smoothstep(0.24f, 0.44f, h)
                b * t
            }
            1 -> {
                // Stratocumulus: Rolling puffy blanket
                val b = smoothstep(0.06f, 0.18f, h)
                val t = 1.0f - smoothstep(0.55f, 0.78f, h)
                b * t
            }
            else -> {
                // Cumulus Congestus: Flat lifted condensation base, massive towering cauliflower heads
                val b = smoothstep(0.04f, 0.15f, h)
                val t = 1.0f - smoothstep(0.72f, 0.96f, h)
                b * t
            }
        }
        if (nubisGrad <= 0.005f) return 0.0f

        // Sample Perlin-Worley shape at world coordinate scale
        val sX = x * 0.045f
        val sZ = z * 0.045f
        val shape = sampleShape(sX * SHAPE_SIZE, sZ * SHAPE_SIZE)

        // Weather coverage thresholding
        val threshold = (1.0f - coverage * 0.88f).coerceIn(0.05f, 0.95f)
        if (shape < threshold - 0.10f) return 0.0f

        val base = ((shape - threshold) / (1.0f - threshold)).coerceIn(0f, 1f)

        // Billow mounds and cauliflower vertical expansion
        val bX = x * 0.12f + h * 0.35f
        val bZ = z * 0.12f + h * 0.35f
        val billow = sampleBillow(bX * SHAPE_SIZE, bZ * SHAPE_SIZE)
        val combined = base * 0.65f + billow * 0.35f

        val eX = x * 0.30f
        val eZ = z * 0.30f
        val erosion = sampleErosion(eX * EROSION_SIZE, eZ * EROSION_SIZE)

        val eroded = (combined - (1.0f - erosion) * (detailErosion * 0.35f)).coerceAtLeast(0f)
        return (eroded * nubisGrad * density * 2.2f).coerceIn(0f, 1f)
    }

    /**
     * Renders a true 3D perspective volumetric cloudscape raymarched across the sky dome.
     * Evaluates 3D world rays for each pixel, casting through the atmosphere layer.
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
        horizonYFrac: Float
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

        // Wind drift in world coordinates
        val windRad = windAzimuth * (PI.toFloat() / 180f)
        val windDrift = timeSeconds * (windSpeed * 0.035f)
        val windX = sin(windRad) * windDrift
        val windZ = cos(windRad) * windDrift

        // Sun direction in 3D world space (Azimuth 225°, Elevation 38°)
        val sunAzRad = 225.0f * (PI.toFloat() / 180f)
        val sunElRad = 38.0f * (PI.toFloat() / 180f)
        val sunX = cos(sunElRad) * sin(sunAzRad)
        val sunY = sin(sunElRad)
        val sunZ = cos(sunElRad) * cos(sunAzRad)

        // Palette configuration
        val baseShadowR = if (isStorm) 30 else 50
        val baseShadowG = if (isStorm) 40 else 64
        val baseShadowB = if (isStorm) 55 else 84

        val sunlitR = if (isStorm) 180 else 255
        val sunlitG = if (isStorm) 185 else 250
        val sunlitB = if (isStorm) 198 else 240

        val goldRimR = if (isStorm) 130 else 255
        val goldRimG = if (isStorm) 140 else 244
        val goldRimB = if (isStorm) 150 else 205

        val horizonSkyR = if (isStorm) 35 else 145
        val horizonSkyG = if (isStorm) 45 else 190
        val horizonSkyB = if (isStorm) 60 else 235

        val yEnd = (height * (horizonYFrac + 0.04f)).toInt().coerceIn(0, height)

        val g1 = silverLiningG.coerceIn(0.5f, 0.95f)
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

                // Ray must point upwards into the sky dome
                if (dirY <= 0.012f) {
                    pixels[rowOffset + x] = 0
                    continue
                }

                // Ray intersections with cloud layer
                val tEnter = yBase / dirY
                val tExit = yTop / dirY
                val rayLength = tExit - tEnter
                val dt = rayLength * 0.20f // 5 steps

                // Henyey-Greenstein phase function for forward silver lining
                val cosAlpha = (dirX * sunX + dirY * sunY + dirZ * sunZ).coerceIn(-1f, 1f)
                val term1 = 1.0f + g1Sq - 2.0f * g1 * cosAlpha
                val term2 = 1.0f + g2Sq - 2.0f * g2 * cosAlpha
                val p1 = (1.0f - g1Sq) / (term1 * sqrt(max(0.001f, term1)))
                val p2 = (1.0f - g2Sq) / (term2 * sqrt(max(0.001f, term2)))
                val phase = (0.75f * p1 + 0.25f * p2) * 0.22f

                var accumR = 0f
                var accumG = 0f
                var accumB = 0f
                var transmittance = 1.0f

                // 5 Volumetric height samples
                for (step in 0 until 5) {
                    val hFrac = (step + 0.5f) * 0.20f // 0.1, 0.3, 0.5, 0.7, 0.9
                    val t = tEnter + hFrac * rayLength
                    val posX = t * dirX + windX
                    val posZ = t * dirZ + windZ

                    val d = sampleDensity(posX, posZ, hFrac, cloudType, coverage, density, detailErosion)
                    if (d > 0.015f) {
                        // Light march step towards sun for self-shadowing
                        val sunD = sampleDensity(
                            posX + sunX * 0.35f,
                            posZ + sunZ * 0.35f,
                            (hFrac + sunY * 0.22f).coerceIn(0f, 1f),
                            cloudType, coverage, density, detailErosion
                        )
                        val sunVisibility = exp(-sunD * sunAbsorption * 3.6f)

                        // Beer-Lambert extinction & Powder sugar effect
                        val tau = d * dt.coerceAtMost(3.0f) * 0.50f
                        val stepTrans = exp(-tau)
                        val powder = 1.0f - exp(-2.2f * tau)
                        val stepAlpha = ((1.0f - stepTrans) * lerp(powder, 1.0f, 1.0f - powderEffect * 0.4f)).coerceIn(0f, 1f)

                        // Direct illumination + ambient sky occlusion + silver lining
                        val ambFactor = 0.55f + 0.45f * hFrac
                        val ambR = baseShadowR * ambFactor
                        val ambG = baseShadowG * ambFactor
                        val ambB = baseShadowB * ambFactor

                        val dirR = sunlitR * sunVisibility
                        val dirG = sunlitG * sunVisibility
                        val dirB = sunlitB * sunVisibility

                        val silver = phase * (1.0f - sunVisibility * 0.4f)
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

                // Atmospheric aerial perspective (haze fading clouds near horizon)
                val haze = (1.0f - exp(-tEnter * 0.038f)).coerceIn(0f, 0.85f)
                val finalR = (lerp(haze, accumR, horizonSkyR.toFloat())).toInt().coerceIn(0, 255)
                val finalG = (lerp(haze, accumG, horizonSkyG.toFloat())).toInt().coerceIn(0, 255)
                val finalB = (lerp(haze, accumB, horizonSkyB.toFloat())).toInt().coerceIn(0, 255)
                val finalA = (totalAlpha * (1.0f - haze * 0.25f) * 255f).toInt().coerceIn(0, 255)

                pixels[rowOffset + x] = (finalA shl 24) or (finalR shl 16) or (finalG shl 8) or finalB
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

        // Sun world unit vector
        val sunAzRad = sunAzimuth * (PI.toFloat() / 180f)
        val sunElRad = sunElevation * (PI.toFloat() / 180f)
        val sx = cos(sunElRad) * sin(sunAzRad)
        val sy = sin(sunElRad)
        val sz = cos(sunElRad) * cos(sunAzRad)

        // Rotate by -yaw
        val cosY = cos(yawRad)
        val sinY = sin(yawRad)
        val sx1 = sx * cosY - sz * sinY
        val sy1 = sy
        val sz1 = sx * sinY + sz * cosY

        // Rotate by -pitch
        val cosP = cos(pitchRad)
        val sinP = sin(pitchRad)
        val sx2 = sx1
        val sy2 = sy1 * cosP - sz1 * sinP
        val sz2 = sy1 * sinP + sz1 * cosP

        if (sz2 <= 0.05f) return null // Sun is behind camera

        val normX = sx2 / (sz2 * tanH)
        val normY = sy2 / (sz2 * tanV)

        val screenX = (normX * 0.5f + 0.5f) * w
        val screenY = (0.5f - normY * 0.5f) * h
        return Offset(screenX, screenY)
    }
}

@Composable
fun WeatherStudioScreen(
    modifier: Modifier = Modifier
) {
    // -----------------------------------------------------------------
    // Simulation Parameters
    // -----------------------------------------------------------------
    var currentPresetName by remember { mutableStateOf("Clear") }
    var isSimPlaying by remember { mutableStateOf(true) }

    // Rain params
    var rainIntensity by remember { mutableFloatStateOf(0.0f) }
    var dropSpeed by remember { mutableFloatStateOf(18.0f) }
    var dropletSize by remember { mutableFloatStateOf(1.2f) }
    var splashDensity by remember { mutableFloatStateOf(0.70f) }
    var puddleAccumulation by remember { mutableFloatStateOf(0.0f) }

    // Snow params
    var snowIntensity by remember { mutableFloatStateOf(0.0f) }
    var flakeFlutter by remember { mutableFloatStateOf(0.60f) }
    var frostGlaze by remember { mutableFloatStateOf(0.20f) }
    var groundSnowCover by remember { mutableFloatStateOf(0.0f) }

    // Beckebans Nubis Volumetric Clouds Parameters
    var cloudType by remember { mutableIntStateOf(2) } // 0=Stratus, 1=Stratocumulus, 2=Cumulus (Default to Cumulus)
    var cloudCoverage by remember { mutableFloatStateOf(0.35f) }
    var cloudDensity by remember { mutableFloatStateOf(0.85f) }
    var silverLiningG by remember { mutableFloatStateOf(0.82f) } // Forward Henyey-Greenstein g1
    var powderEffect by remember { mutableFloatStateOf(0.85f) } // Beer-Powder sugar approximation
    var sunAbsorption by remember { mutableFloatStateOf(0.65f) } // Extinction coefficient along sun march
    var detailErosion by remember { mutableFloatStateOf(0.45f) } // 3D Worley high-frequency erosion
    var cloudBaseAltitude by remember { mutableFloatStateOf(5000f) } // meters
    var raymarchSteps by remember { mutableIntStateOf(48) } // step count

    // Advanced Cinematic Volumetrics
    var enableGodRays by remember { mutableStateOf(true) }
    var godRaysIntensity by remember { mutableFloatStateOf(0.75f) }
    var enableCirrus by remember { mutableStateOf(true) }
    var cirrusCoverage by remember { mutableFloatStateOf(0.45f) }
    var billowDepth by remember { mutableFloatStateOf(1.0f) }
    var cloudSunBacklight by remember { mutableFloatStateOf(0.85f) }

    // Fog params
    var fogDensity by remember { mutableFloatStateOf(0.01f) }
    var groundMist by remember { mutableFloatStateOf(0.05f) }

    // Wind & Storm params
    var windSpeed by remember { mutableFloatStateOf(14.0f) }
    var windAzimuth by remember { mutableFloatStateOf(225.0f) }
    var lightningFrequency by remember { mutableFloatStateOf(0.0f) }
    var isStormActive by remember { mutableStateOf(false) }

    // Camera orbit controls
    var camYaw by remember { mutableFloatStateOf(145.0f) }
    var camPitch by remember { mutableFloatStateOf(18.0f) }

    // Surface Scene Mode & Stage Dimensions
    var surfaceMode by remember { mutableStateOf(SurfaceSceneMode.ENDLESS) }
    var stageWidth by remember { mutableFloatStateOf(10.0f) }
    var stageLength by remember { mutableFloatStateOf(10.0f) }

    // Active bottom tab
    var activeTab by remember { mutableIntStateOf(2) } // Open on Clouds & Fog by default

    // Animation & Lightning dynamics
    var timeSeconds by remember { mutableFloatStateOf(0.0f) }
    var lightningFlash by remember { mutableFloatStateOf(0.0f) }
    var lightningSeed by remember { mutableIntStateOf(42) }

    val horizonYFrac = (0.54f + (camPitch / 55f) * 0.38f).coerceIn(0.10f, 0.90f)

    // Procedural 3D Volumetric Perspective Cloud Raymarch (Re-evaluated on camera rotation and weather dynamics)
    val cloudImageBitmap: ImageBitmap = remember(
        cloudType,
        (cloudCoverage * 20).toInt(),
        (cloudDensity * 20).toInt(),
        (silverLiningG * 20).toInt(),
        (powderEffect * 20).toInt(),
        (sunAbsorption * 20).toInt(),
        (detailErosion * 20).toInt(),
        isStormActive,
        (camYaw / 1.5f).toInt(),
        (camPitch / 1.5f).toInt(),
        (timeSeconds * 3.0f).toInt(),
        (windSpeed * 5).toInt(),
        (horizonYFrac * 25).toInt()
    ) {
        ProceduralCloudSynthesizer.renderPerspectiveSky(
            width = 240,
            height = 135,
            camYaw = camYaw,
            camPitch = camPitch,
            timeSeconds = timeSeconds,
            windSpeed = windSpeed,
            windAzimuth = windAzimuth,
            cloudType = cloudType,
            coverage = cloudCoverage,
            density = cloudDensity,
            silverLiningG = silverLiningG,
            powderEffect = powderEffect,
            sunAbsorption = sunAbsorption,
            detailErosion = detailErosion,
            cloudBaseAltitude = cloudBaseAltitude,
            isStorm = isStormActive,
            horizonYFrac = horizonYFrac
        ).asImageBitmap()
    }

    // Precipitation particle state
    val rainParticles = remember {
        List(140) {
            floatArrayOf(
                Random.nextFloat(),
                Random.nextFloat(),
                0.4f + Random.nextFloat() * 0.6f,
                0.7f + Random.nextFloat() * 0.6f
            )
        }
    }

    val snowParticles = remember {
        List(90) {
            floatArrayOf(
                Random.nextFloat(),
                Random.nextFloat(),
                Random.nextFloat() * 6.28f,
                1.5f + Random.nextFloat() * 2.5f
            )
        }
    }

    // Simulation loop
    LaunchedEffect(isSimPlaying) {
        var lastStrike = 0f
        while (isSimPlaying) {
            delay(16) // ~60 fps
            timeSeconds += 0.016f

            if ((isStormActive || lightningFrequency > 0.6f) && timeSeconds - lastStrike > (5.0f - lightningFrequency * 4.0f)) {
                lightningFlash = 1.0f
                lightningSeed = Random.nextInt(1000)
                lastStrike = timeSeconds
            }

            if (lightningFlash > 0.0f) {
                lightningFlash = (lightningFlash - 0.05f).coerceAtLeast(0.0f)
            }
        }
    }

    val infiniteTransition = rememberInfiniteTransition(label = "weather_anim")
    val cloudWave by infiniteTransition.animateFloat(
        initialValue = 0f,
        targetValue = 6.28318f,
        animationSpec = infiniteRepeatable(
            animation = tween(durationMillis = 8000, easing = LinearEasing),
            repeatMode = RepeatMode.Restart
        ),
        label = "cloud_wave"
    )

    Column(
        modifier = modifier
            .fillMaxSize()
            .background(Color(0xFF090B10))
            .testTag("weather_studio_screen")
    ) {
        // =============================================================
        // 1. TOP VIEWPORT: 3D PROCEDURAL WEATHER SIMULATION (weight 1.08f)
        // =============================================================
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .weight(1.08f)
                .background(Color(0xFF05070B))
                .pointerInput(Unit) {
                    detectDragGestures { change, dragAmount ->
                        change.consume()
                        camYaw = (camYaw - dragAmount.x * 0.35f + 360f) % 360f
                        camPitch = (camPitch + dragAmount.y * 0.25f).coerceIn(-15f, 75f)
                    }
                }
                .testTag("weather_viewport")
        ) {
            Canvas(modifier = Modifier.fillMaxSize()) {
                val w = size.width
                val h = size.height
                val horizonY = h * horizonYFrac

                // 1. Dynamic Atmosphere Gradient based on Sun, Storm, and Fog
                drawWeatherAtmosphere(
                    w = w,
                    h = h,
                    horizonY = horizonY,
                    cloudCoverage = cloudCoverage,
                    fogDensity = fogDensity,
                    isStorm = isStormActive,
                    lightningFlash = lightningFlash,
                    rainIntensity = rainIntensity,
                    snowIntensity = snowIntensity
                )

                // 2. Horizon and Perspective Ground Plane with Shadows & Puddles
                drawGroundSurface(
                    w = w,
                    h = h,
                    horizonY = horizonY,
                    camPitch = camPitch,
                    camYaw = camYaw,
                    surfaceMode = surfaceMode,
                    stageWidth = stageWidth,
                    stageLength = stageLength,
                    puddleDepth = puddleAccumulation,
                    snowCover = groundSnowCover,
                    wetness = (rainIntensity * 0.8f + puddleAccumulation * 0.2f).coerceIn(0f, 1f),
                    cloudCoverage = cloudCoverage,
                    windSpeed = windSpeed,
                    cloudWave = cloudWave,
                    lightningFlash = lightningFlash
                )

                // 3. Robert Beckebans 3D Perspective Volumetric Cloudscape
                drawPhotorealisticCloudscape(
                    w = w,
                    h = h,
                    horizonY = horizonY,
                    cloudImage = cloudImageBitmap,
                    coverage = cloudCoverage,
                    density = cloudDensity,
                    enableGodRays = enableGodRays,
                    godRaysIntensity = godRaysIntensity,
                    enableCirrus = enableCirrus,
                    cirrusCoverage = cirrusCoverage,
                    sunBacklight = cloudSunBacklight,
                    camYaw = camYaw,
                    camPitch = camPitch,
                    windSpeed = windSpeed,
                    cloudWave = cloudWave,
                    timeSeconds = timeSeconds,
                    isStorm = isStormActive,
                    lightningFlash = lightningFlash
                )

                // 4. Procedural Lightning Bolt Arc
                if (lightningFlash > 0.15f) {
                    drawProceduralLightning(
                        w = w,
                        h = h,
                        flashIntensity = lightningFlash,
                        seed = lightningSeed,
                        horizonY = horizonY
                    )
                }

                // 5. Rain Particle Streaks and Ground Splashes
                if (rainIntensity > 0.05f) {
                    drawRainStreaks(
                        w = w,
                        h = h,
                        particles = rainParticles,
                        intensity = rainIntensity,
                        dropSpeed = dropSpeed,
                        windAngle = windAzimuth,
                        windSpeed = windSpeed,
                        time = timeSeconds,
                        horizonY = horizonY
                    )
                }

                // 6. Snow Particle Flurries
                if (snowIntensity > 0.05f) {
                    drawSnowFlurries(
                        w = w,
                        h = h,
                        particles = snowParticles,
                        intensity = snowIntensity,
                        flutter = flakeFlutter,
                        windSpeed = windSpeed,
                        time = timeSeconds,
                        horizonY = horizonY
                    )
                }

                // 7. Volumetric Froxel Fog Inscattering
                if (fogDensity > 0.05f) {
                    drawFogVolumeOverlay(
                        w = w,
                        h = h,
                        density = fogDensity,
                        mist = groundMist,
                        lightningFlash = lightningFlash,
                        horizonY = horizonY
                    )
                }
            }

            // ---------------------------------------------------------
            // Top HUD Overlay: Badges and Live Telemetry
            // ---------------------------------------------------------
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(10.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                // Weather State Badge
                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = Color(0xFF0F172A).copy(alpha = 0.88f),
                    border = CardDefaults.outlinedCardBorder().copy(
                        brush = Brush.linearGradient(listOf(Color(0xFF1E293B), Color(0xFF0284C7)))
                    )
                ) {
                    Row(
                        modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        val iconVector = when {
                            snowIntensity > 0.4f -> Icons.Default.AcUnit
                            isStormActive -> Icons.Default.FlashOn
                            rainIntensity > 0.4f -> Icons.Default.WaterDrop
                            fogDensity > 0.4f -> Icons.Default.FilterDrama
                            cloudCoverage > 0.6f -> Icons.Default.Cloud
                            else -> Icons.Default.WbSunny
                        }
                        Icon(
                            imageVector = iconVector,
                            contentDescription = currentPresetName,
                            tint = when {
                                isStormActive -> Color(0xFFFBBF24)
                                snowIntensity > 0.3f -> Color(0xFF93C5FD)
                                rainIntensity > 0.3f -> Color(0xFF38BDF8)
                                else -> Color(0xFFF59E0B)
                            },
                            modifier = Modifier.size(16.dp)
                        )
                        Spacer(modifier = Modifier.width(6.dp))
                        Text(
                            text = currentPresetName.uppercase(),
                            fontSize = 11.sp,
                            fontWeight = FontWeight.Bold,
                            color = Color.White
                        )
                        val typeLabel = when (cloudType) {
                            0 -> "Stratus"
                            1 -> "Stratocumulus"
                            else -> "Cumulus Congestus"
                        }
                        Text(
                            text = " • $typeLabel • ${(cloudCoverage * 100).toInt()}% Cover",
                            fontSize = 10.sp,
                            color = Color(0xFF94A3B8)
                        )
                    }
                }

                // Viewport Controls: Play/Pause, Reset View
                Row(verticalAlignment = Alignment.CenterVertically) {
                    IconButton(
                        onClick = { isSimPlaying = !isSimPlaying },
                        modifier = Modifier
                            .size(32.dp)
                            .background(
                                if (isSimPlaying) Color(0xFF0284C7).copy(alpha = 0.85f)
                                else Color(0xFF334155).copy(alpha = 0.85f),
                                CircleShape
                            )
                            .testTag("toggle_weather_sim_button")
                    ) {
                        Icon(
                            imageVector = if (isSimPlaying) Icons.Default.Pause else Icons.Default.PlayArrow,
                            contentDescription = "Play/Pause weather sim",
                            tint = Color.White,
                            modifier = Modifier.size(16.dp)
                        )
                    }

                    Spacer(modifier = Modifier.width(6.dp))

                    IconButton(
                        onClick = {
                            camPitch = 18.0f
                            camYaw = 145.0f
                        },
                        modifier = Modifier
                            .size(32.dp)
                            .background(Color(0xFF1E293B).copy(alpha = 0.85f), CircleShape)
                            .testTag("reset_weather_camera_button")
                    ) {
                        Icon(
                            imageVector = Icons.Default.Refresh,
                            contentDescription = "Center View",
                            tint = Color(0xFF94A3B8),
                            modifier = Modifier.size(16.dp)
                        )
                    }

                    Spacer(modifier = Modifier.width(6.dp))

                    Surface(
                        shape = RoundedCornerShape(8.dp),
                        color = Color(0xFF1E293B).copy(alpha = 0.85f),
                        border = CardDefaults.outlinedCardBorder().copy(
                            brush = Brush.linearGradient(listOf(Color(0xFF334155), Color(0xFF1E293B)))
                        ),
                        modifier = Modifier
                            .clickable {
                                val modes = SurfaceSceneMode.values()
                                val nextIdx = (modes.indexOf(surfaceMode) + 1) % modes.size
                                surfaceMode = modes[nextIdx]
                            }
                            .testTag("weather_surface_mode_pill")
                    ) {
                        Text(
                            text = "${surfaceMode.icon} ${surfaceMode.shortLabel}",
                            style = MaterialTheme.typography.labelSmall,
                            color = Color(0xFF38BDF8),
                            fontWeight = FontWeight.Bold,
                            modifier = Modifier.padding(horizontal = 8.dp, vertical = 6.dp)
                        )
                    }
                }
            }

            // ---------------------------------------------------------
            // Bottom Overlays: Presets Bar inside Viewport
            // ---------------------------------------------------------
            Row(
                modifier = Modifier
                    .align(Alignment.BottomCenter)
                    .fillMaxWidth()
                    .padding(horizontal = 8.dp, vertical = 6.dp)
                    .horizontalScroll(rememberScrollState()),
                horizontalArrangement = Arrangement.spacedBy(6.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                weatherPresetsList.forEach { preset ->
                    val isSelected = currentPresetName == preset.name
                    Surface(
                        shape = RoundedCornerShape(8.dp),
                        color = if (isSelected)
                            Color(0xFF0284C7).copy(alpha = 0.85f)
                        else
                            Color(0xCC10141E),
                        border = CardDefaults.outlinedCardBorder().copy(
                            brush = Brush.linearGradient(
                                if (isSelected) listOf(Color(0xFF38BDF8), Color(0xFF0284C7))
                                else listOf(Color(0xFF1E293B), Color(0xFF334155))
                            )
                        ),
                        modifier = Modifier
                            .clickable {
                                currentPresetName = preset.name
                                rainIntensity = preset.rainIntensity
                                snowIntensity = preset.snowIntensity
                                cloudCoverage = preset.cloudCoverage
                                fogDensity = preset.fogDensity
                                windSpeed = preset.windSpeed
                                windAzimuth = preset.windAzimuth
                                isStormActive = preset.isStormy
                                cloudType = preset.cloudType
                                puddleAccumulation = (preset.rainIntensity * 0.75f).coerceIn(0f, 1f)
                                groundSnowCover = (preset.snowIntensity * 0.85f).coerceIn(0f, 1f)
                            }
                            .testTag("preset_chip_${preset.name.lowercase()}")
                    ) {
                        Row(
                            modifier = Modifier.padding(horizontal = 10.dp, vertical = 5.dp),
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Text(text = preset.icon, fontSize = 13.sp)
                            Spacer(modifier = Modifier.width(5.dp))
                            Text(
                                text = preset.name,
                                fontSize = 11.sp,
                                fontWeight = if (isSelected) FontWeight.Bold else FontWeight.Normal,
                                color = if (isSelected) Color.White else Color(0xFFCBD5E1)
                            )
                        }
                    }
                }
            }
        }

        // =============================================================
        // 2. HORIZONTAL DIVIDER (1.dp)
        // =============================================================
        HorizontalDivider(
            thickness = 1.dp,
            color = Color(0xFF1E293B)
        )

        // =============================================================
        // 3. BOTTOM CONTROLS PANEL (weight 0.92f)
        // =============================================================
        Surface(
            modifier = Modifier
                .fillMaxWidth()
                .weight(0.92f),
            color = Color(0xFF0F121A)
        ) {
            Column(modifier = Modifier.fillMaxSize()) {
                ScrollableTabRow(
                    selectedTabIndex = activeTab,
                    containerColor = Color(0xFF131722),
                    contentColor = Color.White,
                    edgePadding = 12.dp,
                    indicator = { tabPositions ->
                        TabRowDefaults.SecondaryIndicator(
                            Modifier.tabIndicatorOffset(tabPositions[activeTab]),
                            color = Color(0xFF38BDF8),
                            height = 3.dp
                        )
                    },
                    modifier = Modifier.fillMaxWidth().testTag("weather_tabs_row")
                ) {
                    val tabs = listOf(
                        "Precipitation",
                        "Snow & Frost",
                        "Nubis Clouds & Fog",
                        "Wind & Storms",
                        "Native C++ Core"
                    )
                    tabs.forEachIndexed { index, title ->
                        Tab(
                            selected = activeTab == index,
                            onClick = { activeTab = index },
                            text = {
                                Text(
                                    text = title,
                                    fontSize = 12.sp,
                                    fontWeight = if (activeTab == index) FontWeight.Bold else FontWeight.Normal
                                )
                            }
                        )
                    }
                }

                // Tab Content Body
                Column(
                    modifier = Modifier
                        .fillMaxSize()
                        .verticalScroll(rememberScrollState())
                        .padding(16.dp)
                ) {
                    when (activeTab) {
                        0 -> PrecipitationTab(
                            rainIntensity = rainIntensity,
                            onRainIntensityChange = { rainIntensity = it },
                            dropSpeed = dropSpeed,
                            onDropSpeedChange = { dropSpeed = it },
                            dropletSize = dropletSize,
                            onDropletSizeChange = { dropletSize = it },
                            splashDensity = splashDensity,
                            onSplashDensityChange = { splashDensity = it },
                            puddleAccumulation = puddleAccumulation,
                            onPuddleAccumulationChange = { puddleAccumulation = it }
                        )
                        1 -> SnowFrostTab(
                            snowIntensity = snowIntensity,
                            onSnowIntensityChange = { snowIntensity = it },
                            flakeFlutter = flakeFlutter,
                            onFlakeFlutterChange = { flakeFlutter = it },
                            frostGlaze = frostGlaze,
                            onFrostGlazeChange = { frostGlaze = it },
                            groundSnowCover = groundSnowCover,
                            onGroundSnowCoverChange = { groundSnowCover = it }
                        )
                        2 -> CloudsFogTab(
                            cloudType = cloudType,
                            onCloudTypeChange = { cloudType = it },
                            cloudCoverage = cloudCoverage,
                            onCloudCoverageChange = { cloudCoverage = it },
                            cloudDensity = cloudDensity,
                            onCloudDensityChange = { cloudDensity = it },
                            silverLiningG = silverLiningG,
                            onSilverLiningGChange = { silverLiningG = it },
                            powderEffect = powderEffect,
                            onPowderEffectChange = { powderEffect = it },
                            sunAbsorption = sunAbsorption,
                            onSunAbsorptionChange = { sunAbsorption = it },
                            detailErosion = detailErosion,
                            onDetailErosionChange = { detailErosion = it },
                            cloudBaseAltitude = cloudBaseAltitude,
                            onCloudBaseAltitudeChange = { cloudBaseAltitude = it },
                            raymarchSteps = raymarchSteps,
                            onRaymarchStepsChange = { raymarchSteps = it },
                            enableGodRays = enableGodRays,
                            onEnableGodRaysChange = { enableGodRays = it },
                            godRaysIntensity = godRaysIntensity,
                            onGodRaysIntensityChange = { godRaysIntensity = it },
                            enableCirrus = enableCirrus,
                            onEnableCirrusChange = { enableCirrus = it },
                            cirrusCoverage = cirrusCoverage,
                            onCirrusCoverageChange = { cirrusCoverage = it },
                            billowDepth = billowDepth,
                            onBillowDepthChange = { billowDepth = it },
                            sunBacklight = cloudSunBacklight,
                            onSunBacklightChange = { cloudSunBacklight = it },
                            fogDensity = fogDensity,
                            onFogDensityChange = { fogDensity = it },
                            groundMist = groundMist,
                            onGroundMistChange = { groundMist = it }
                        )
                        3 -> WindStormsTab(
                            windSpeed = windSpeed,
                            onWindSpeedChange = { windSpeed = it },
                            windAzimuth = windAzimuth,
                            onWindAzimuthChange = { windAzimuth = it },
                            lightningFrequency = lightningFrequency,
                            onLightningFrequencyChange = { lightningFrequency = it },
                            isStormActive = isStormActive,
                            onStormActiveChange = { isStormActive = it },
                            onTriggerStrike = {
                                lightningFlash = 1.0f
                                lightningSeed = Random.nextInt(1000)
                            }
                        )
                        4 -> NativeEngineTab()
                    }
                }
            }
        }
    }
}

// ---------------------------------------------------------------------
// CANVAS DRAWING EXTENSIONS WITH CONTINUOUS PROCEDURAL CLOUDMAP
// ---------------------------------------------------------------------

private fun DrawScope.drawWeatherAtmosphere(
    w: Float,
    h: Float,
    horizonY: Float,
    cloudCoverage: Float,
    fogDensity: Float,
    isStorm: Boolean,
    lightningFlash: Float,
    rainIntensity: Float,
    snowIntensity: Float
) {
    val (topSky, midSky, horizonSky) = when {
        lightningFlash > 0.2f -> {
            Triple(Color(0xFFCBD5E1), Color(0xFFE2E8F0), Color(0xFFF8FAFC))
        }
        isStorm -> {
            Triple(Color(0xFF080D1A), Color(0xFF131D31), Color(0xFF1E293B))
        }
        snowIntensity > 0.3f -> {
            Triple(Color(0xFF1E293B), Color(0xFF334155), Color(0xFF64748B))
        }
        rainIntensity > 0.4f -> {
            Triple(Color(0xFF0F172A), Color(0xFF1E293B), Color(0xFF334155))
        }
        fogDensity > 0.5f -> {
            Triple(Color(0xFF334155), Color(0xFF64748B), Color(0xFF94A3B8))
        }
        cloudCoverage > 0.7f -> {
            Triple(Color(0xFF1A2639), Color(0xFF2D3E56), Color(0xFF475569))
        }
        else -> {
            Triple(Color(0xFF0D47A1), Color(0xFF0284C7), Color(0xFF90CAF9))
        }
    }

    drawRect(
        brush = Brush.verticalGradient(
            colors = listOf(topSky, midSky, horizonSky),
            startY = 0f,
            endY = horizonY
        ),
        size = Size(w, horizonY)
    )
}

private fun DrawScope.drawGroundSurface(
    w: Float,
    h: Float,
    horizonY: Float,
    camPitch: Float,
    camYaw: Float,
    surfaceMode: SurfaceSceneMode = SurfaceSceneMode.ENDLESS,
    stageWidth: Float = 10.0f,
    stageLength: Float = 10.0f,
    puddleDepth: Float,
    snowCover: Float,
    wetness: Float,
    cloudCoverage: Float,
    windSpeed: Float,
    cloudWave: Float,
    lightningFlash: Float
) {
    if (surfaceMode == SurfaceSceneMode.NONE) {
        // Void / Cloud Flight mode: Zero ground surface, pure cloud immersion!
        return
    }

    val baseGroundColor = when {
        lightningFlash > 0.2f -> Color(0xFF475569)
        snowCover > 0.4f -> Color(0xFFCBD5E1)
        wetness > 0.5f -> Color(0xFF0B0F19)
        else -> Color(0xFF131B2A)
    }

    if (surfaceMode == SurfaceSceneMode.ENDLESS) {
        // Endless Ground fill
        drawRect(
            brush = Brush.verticalGradient(
                colors = listOf(baseGroundColor.copy(alpha = 0.95f), baseGroundColor),
                startY = horizonY,
                endY = h
            ),
            topLeft = Offset(0f, horizonY),
            size = Size(w, (h - horizonY).coerceAtLeast(0f))
        )

        // Perspective terrain grid lines
        val gridColor = if (snowCover > 0.4f) Color(0xFF94A3B8).copy(alpha = 0.3f)
        else Color(0xFF1E293B).copy(alpha = 0.6f)

        for (i in 0..12) {
            val frac = i / 12f
            val py = horizonY + (h - horizonY) * (frac * frac)
            drawLine(
                color = gridColor,
                start = Offset(0f, py),
                end = Offset(w, py),
                strokeWidth = 1f
            )
        }

        // Perspective vanishing rays
        val centerX = w * 0.5f + sin((camYaw - 180f) * (PI.toFloat() / 180f)) * (w * 0.5f)
        for (i in -8..8) {
            val gx = centerX + i * (w * 0.12f)
            drawLine(
                color = gridColor.copy(alpha = 0.35f),
                start = Offset(centerX, horizonY),
                end = Offset(gx, h),
                strokeWidth = 1f
            )
        }

        // Projected Volumetric Cloud Shadows moving across the ground
        if (cloudCoverage > 0.15f) {
            val shadowShift = (cloudWave * 15f + windSpeed * 2f) % (w * 0.8f)
            val shadowAlpha = (cloudCoverage * 0.35f).coerceIn(0.08f, 0.45f)
            drawOval(
                brush = Brush.radialGradient(
                    colors = listOf(Color.Black.copy(alpha = shadowAlpha), Color.Transparent),
                    center = Offset(w * 0.35f + shadowShift * 0.6f, horizonY + (h - horizonY) * 0.45f),
                    radius = w * 0.32f
                ),
                topLeft = Offset(w * 0.15f + shadowShift * 0.6f, horizonY + (h - horizonY) * 0.25f),
                size = Size(w * 0.45f, (h - horizonY) * 0.55f)
            )
            drawOval(
                brush = Brush.radialGradient(
                    colors = listOf(Color.Black.copy(alpha = shadowAlpha * 0.85f), Color.Transparent),
                    center = Offset(w * 0.72f - shadowShift * 0.4f, horizonY + (h - horizonY) * 0.65f),
                    radius = w * 0.28f
                ),
                topLeft = Offset(w * 0.55f - shadowShift * 0.4f, horizonY + (h - horizonY) * 0.45f),
                size = Size(w * 0.40f, (h - horizonY) * 0.48f)
            )
        }

        // Puddle specular reflections on ground
        if (puddleDepth > 0.05f) {
            val puddleColor = if (lightningFlash > 0.2f) Color(0xFFE2E8F0) else Color(0xFF1E3A5F).copy(alpha = 0.65f)
            drawOval(
                color = puddleColor,
                topLeft = Offset(w * 0.22f, horizonY + 28f),
                size = Size(w * 0.26f * puddleDepth, 22f * puddleDepth)
            )
            drawOval(
                color = puddleColor,
                topLeft = Offset(w * 0.58f, horizonY + 54f),
                size = Size(w * 0.32f * puddleDepth, 32f * puddleDepth)
            )
            drawOval(
                color = puddleColor,
                topLeft = Offset(w * 0.36f, horizonY + 95f),
                size = Size(w * 0.42f * puddleDepth, 38f * puddleDepth)
            )
        }
    } else {
        // Bounded piece floating in weather atmosphere (FLAT_PIECE, WATER_BODY, or BROKEN_SLAB)
        val stageCenterY = horizonY + (h - horizonY) * 0.45f
        val stageHalfW = (w * 0.36f * (stageWidth / 10.0f)).coerceIn(w * 0.18f, w * 0.48f)
        val stageHalfH = (h * 0.18f * (stageLength / 10.0f)).coerceIn(h * 0.09f, h * 0.28f)
        val stageThickness = 28f

        val topP0 = Offset(w * 0.5f - stageHalfW, stageCenterY)
        val topP1 = Offset(w * 0.5f, stageCenterY - stageHalfH)
        val topP2 = Offset(w * 0.5f + stageHalfW, stageCenterY)
        val topP3 = Offset(w * 0.5f, stageCenterY + stageHalfH)

        val btmP0 = Offset(topP0.x, topP0.y + stageThickness)
        val btmP2 = Offset(topP2.x, topP2.y + stageThickness)
        val btmP3 = Offset(topP3.x, topP3.y + stageThickness)

        // Drop shadow on clouds/void
        drawOval(
            brush = Brush.radialGradient(
                colors = listOf(Color.Black.copy(alpha = 0.55f), Color.Transparent),
                center = Offset(w * 0.5f, stageCenterY + stageHalfH + 35f),
                radius = stageHalfW * 0.9f
            ),
            topLeft = Offset(w * 0.5f - stageHalfW * 0.9f, stageCenterY + stageHalfH + 15f),
            size = Size(stageHalfW * 1.8f, stageHalfH * 1.4f)
        )

        when (surfaceMode) {
            SurfaceSceneMode.WATER_BODY -> {
                // Submerged Basin Skirt
                val basinPath = Path().apply {
                    moveTo(topP0.x, topP0.y); lineTo(topP3.x, topP3.y); lineTo(topP2.x, topP2.y)
                    lineTo(btmP2.x, btmP2.y); lineTo(btmP3.x, btmP3.y); lineTo(btmP0.x, btmP0.y); close()
                }
                drawPath(basinPath, color = Color(0xDD075985))

                // Water Surface
                val waterPath = Path().apply {
                    moveTo(topP0.x, topP0.y); lineTo(topP1.x, topP1.y); lineTo(topP2.x, topP2.y); lineTo(topP3.x, topP3.y); close()
                }
                drawPath(
                    waterPath,
                    brush = Brush.linearGradient(
                        colors = listOf(Color(0xFA0284C7), Color(0xFA0369A1), Color(0xFA0C4A6E)),
                        start = topP1,
                        end = topP3
                    )
                )
                // Rain Ripples across water surface
                for (k in 1..4) {
                    val rippleY = stageCenterY - stageHalfH * 0.5f + (k * 18f)
                    drawOval(
                        color = Color(0xFFE0F2FE).copy(alpha = 0.45f),
                        topLeft = Offset(w * 0.5f - stageHalfW * 0.6f * (k / 4f), rippleY),
                        size = Size(stageHalfW * 1.2f * (k / 4f), 12f * (k / 4f)),
                        style = Stroke(width = 1.2f)
                    )
                }
                drawPath(waterPath, color = Color(0xFFBAE6FD).copy(alpha = 0.85f), style = Stroke(width = 2.5f))
            }
            SurfaceSceneMode.BROKEN_SLAB -> {
                // Fractured rock diorama
                val skirtPath = Path().apply {
                    moveTo(topP0.x, topP0.y); lineTo(topP3.x + 8f, topP3.y - 4f); lineTo(topP2.x, topP2.y)
                    lineTo(btmP2.x - 12f, btmP2.y + 12f); lineTo(btmP3.x, btmP3.y + 16f); lineTo(btmP0.x + 10f, btmP0.y + 14f); close()
                }
                drawPath(skirtPath, color = Color(0xFF1E293B))
                drawPath(skirtPath, color = Color(0xFF334155), style = Stroke(width = 1.5f))

                val topRock = Path().apply {
                    moveTo(topP0.x, topP0.y); lineTo(topP1.x + 12f, topP1.y + 6f); lineTo(topP2.x, topP2.y)
                    lineTo(topP3.x - 8f, topP3.y - 4f); close()
                }
                drawPath(topRock, color = if (snowCover > 0.4f) Color(0xFFCBD5E1) else Color(0xFF293548))
                drawPath(topRock, color = Color(0xFF64748B), style = Stroke(width = 1.8f))
            }
            else -> {
                // FLAT_PIECE (10x10m default or 3x3m piece)
                val skirtPath = Path().apply {
                    moveTo(topP0.x, topP0.y); lineTo(topP3.x, topP3.y); lineTo(topP2.x, topP2.y)
                    lineTo(btmP2.x, btmP2.y); lineTo(btmP3.x, btmP3.y); lineTo(btmP0.x, btmP0.y); close()
                }
                drawPath(skirtPath, color = Color(0xFF131B2A))
                drawPath(skirtPath, color = Color(0xFF1E293B), style = Stroke(width = 1f))

                val topSlab = Path().apply {
                    moveTo(topP0.x, topP0.y); lineTo(topP1.x, topP1.y); lineTo(topP2.x, topP2.y); lineTo(topP3.x, topP3.y); close()
                }
                val slabFillColor = when {
                    snowCover > 0.4f -> Color(0xFFCBD5E1)
                    wetness > 0.5f -> Color(0xFF111726)
                    else -> Color(0xFF1E2638)
                }
                drawPath(topSlab, color = slabFillColor)

                // 1m Grid Lines on Platform
                for (g in 1..4) {
                    val f = g / 5f
                    val pA = Offset(topP0.x + (topP1.x - topP0.x) * f, topP0.y + (topP1.y - topP0.y) * f)
                    val pB = Offset(topP3.x + (topP2.x - topP3.x) * f, topP3.y + (topP2.y - topP3.y) * f)
                    drawLine(Color(0xFF38BDF8).copy(alpha = 0.25f), pA, pB, strokeWidth = 1f)

                    val pC = Offset(topP1.x + (topP2.x - topP1.x) * f, topP1.y + (topP2.y - topP1.y) * f)
                    val pD = Offset(topP0.x + (topP3.x - topP0.x) * f, topP0.y + (topP3.y - topP0.y) * f)
                    drawLine(Color(0xFF38BDF8).copy(alpha = 0.25f), pC, pD, strokeWidth = 1f)
                }

                drawPath(topSlab, color = Color(0xFF38BDF8).copy(alpha = 0.65f), style = Stroke(width = 2f))

                // Wet puddle on platform
                if (puddleDepth > 0.05f) {
                    val pColor = if (lightningFlash > 0.2f) Color(0xFFE2E8F0) else Color(0xFF1E3A5F).copy(alpha = 0.75f)
                    drawOval(
                        color = pColor,
                        topLeft = Offset(w * 0.5f - stageHalfW * 0.35f, stageCenterY - 10f),
                        size = Size(stageHalfW * 0.7f * puddleDepth, 24f * puddleDepth)
                    )
                }
            }
        }
    }
}

/**
 * 3D Perspective Volumetric Cloudscape Renderer:
 * - True 3D perspective projection across the sky dome with camera pitch and yaw
 * - Solar disc & corona bloom with Henyey-Greenstein silver lining
 * - Volumetric crepuscular light shafts (god rays) streaming through cloud gaps
 * - High-altitude wispy cirrus ice crystals layer
 * - Zero circle primitives: 100% continuous, billowy, soft, organic cloudmass!
 */
private fun DrawScope.drawPhotorealisticCloudscape(
    w: Float,
    h: Float,
    horizonY: Float,
    cloudImage: ImageBitmap,
    coverage: Float,
    density: Float,
    enableGodRays: Boolean,
    godRaysIntensity: Float,
    enableCirrus: Boolean,
    cirrusCoverage: Float,
    sunBacklight: Float,
    camYaw: Float,
    camPitch: Float,
    windSpeed: Float,
    cloudWave: Float,
    timeSeconds: Float,
    isStorm: Boolean,
    lightningFlash: Float
) {
    if (coverage < 0.02f) return

    // 1. Calculate Solar Position in 3D Celestial Hemisphere
    val sunCenter = ProceduralCloudSynthesizer.projectSunToScreen(
        w = w,
        h = h,
        camYaw = camYaw,
        camPitch = camPitch
    )
    val isSunVisible = sunCenter != null && sunCenter.y < horizonY + 80f && sunCenter.x in (-w * 0.35f)..(w * 1.35f)

    clipRect(left = 0f, top = 0f, right = w, bottom = horizonY) {

        // 2. CELESTIAL SUN DISC & CORONA BLOOM (Behind Clouds)
        if (isSunVisible && sunCenter != null && !isStorm) {
            val coronaRadius = w * 0.28f
            drawCircle(
                brush = Brush.radialGradient(
                    colors = listOf(
                        Color(0xFFFFFBEB).copy(alpha = 0.60f * sunBacklight),
                        Color(0xFFFFEDD5).copy(alpha = 0.28f * sunBacklight),
                        Color(0xFFBAE6FD).copy(alpha = 0.08f * sunBacklight),
                        Color.Transparent
                    ),
                    center = sunCenter,
                    radius = coronaRadius
                ),
                radius = coronaRadius,
                center = sunCenter
            )

            // Radiant golden solar disc
            drawCircle(
                brush = Brush.radialGradient(
                    colors = listOf(
                        Color(0xFFFFFFFF),
                        Color(0xFFFEF08A),
                        Color(0xFFF59E0B)
                    ),
                    center = sunCenter,
                    radius = 24f
                ),
                radius = 24f,
                center = sunCenter
            )
        }

        // 3. VOLUMETRIC CREPUSCULAR GOD RAYS (Light Shafts)
        if (enableGodRays && isSunVisible && sunCenter != null && godRaysIntensity > 0.05f) {
            val rayCount = 14
            val baseAlpha = (godRaysIntensity * (1.0f - abs(coverage - 0.60f) * 0.7f) * 0.40f).coerceIn(0.06f, 0.38f)

            for (r in 0 until rayCount) {
                val rayAngle = (r.toFloat() / rayCount) * 160f - 80f + sin(cloudWave + r) * 4f
                val rad = (rayAngle + 90f) * PI.toFloat() / 180f
                val rayLength = h * 0.90f

                val endX = sunCenter.x + cos(rad) * rayLength
                val endY = sunCenter.y + sin(rad) * rayLength
                val beamWidth = 28f + (r % 3) * 16f

                val rayPath = Path().apply {
                    moveTo(sunCenter.x - beamWidth * 0.2f, sunCenter.y)
                    lineTo(sunCenter.x + beamWidth * 0.2f, sunCenter.y)
                    lineTo(endX + beamWidth * 1.5f, endY)
                    lineTo(endX - beamWidth * 1.5f, endY)
                    close()
                }

                val beamShimmer = 0.75f + 0.25f * sin(timeSeconds * 1.5f + r * 1.8f)
                drawPath(
                    path = rayPath,
                    brush = Brush.linearGradient(
                        colors = listOf(
                            Color(0xFFFFFBEB).copy(alpha = baseAlpha * beamShimmer),
                            Color(0xFFFEF3C7).copy(alpha = baseAlpha * 0.5f * beamShimmer),
                            Color.Transparent
                        ),
                        start = sunCenter,
                        end = Offset(endX, endY)
                    )
                )
            }
        }

        // 4. HIGH-ALTITUDE CIRRUS FILAMENT LAYER (10,000m+)
        if (enableCirrus && cirrusCoverage > 0.05f) {
            val cirrusAlpha = (cirrusCoverage * 0.50f).coerceIn(0.08f, 0.65f)
            val cirrusDrift = (timeSeconds * 8.0f) % (w * 1.5f)

            for (c in 0..7) {
                val baseY = horizonY * (0.08f + c * 0.05f)
                val curvePath = Path().apply {
                    val startX = -w * 0.2f + (c * w * 0.25f + cirrusDrift) % (w * 1.4f)
                    moveTo(startX, baseY)
                    quadraticTo(
                        startX + w * 0.35f,
                        baseY - 20f + sin(cloudWave + c) * 6f,
                        startX + w * 0.70f,
                        baseY + 12f
                    )
                }

                drawPath(
                    path = curvePath,
                    color = Color.White.copy(alpha = cirrusAlpha * (0.4f + (c % 3) * 0.2f)),
                    style = Stroke(width = 12f + (c % 4) * 8f, cap = StrokeCap.Round)
                )
            }
        }

        // 5. TRUE 3D PERSPECTIVE VOLUMETRIC CLOUDMAP
        // Drawn directly across the celestial sky dome to the horizon
        val cloudAlpha = if (lightningFlash > 0.2f) 1.0f else (0.95f * density).coerceIn(0.1f, 1.0f)

        drawImage(
            image = cloudImage,
            dstOffset = IntOffset(0, 0),
            dstSize = IntSize(w.toInt(), horizonY.toInt().coerceAtLeast(1)),
            alpha = cloudAlpha,
            filterQuality = FilterQuality.Medium
        )
    }
}

private fun DrawScope.drawProceduralLightning(
    w: Float,
    h: Float,
    flashIntensity: Float,
    seed: Int,
    horizonY: Float
) {
    val rng = Random(seed)
    val startX = w * (0.3f + rng.nextFloat() * 0.4f)
    var curX = startX
    var curY = horizonY * 0.12f
    val targetY = horizonY

    val boltColor = Color(0xFFF8FAFC).copy(alpha = flashIntensity)
    val glowColor = Color(0xFF60A5FA).copy(alpha = flashIntensity * 0.6f)

    val path = Path()
    path.moveTo(curX, curY)

    val steps = 14
    val stepY = (targetY - curY) / steps

    for (s in 0 until steps) {
        curX += (rng.nextFloat() - 0.5f) * 45f
        curY += stepY
        path.lineTo(curX, curY)
    }

    drawPath(
        path = path,
        color = glowColor,
        style = Stroke(width = 7f, cap = StrokeCap.Round)
    )
    drawPath(
        path = path,
        color = boltColor,
        style = Stroke(width = 2.5f, cap = StrokeCap.Round)
    )
}

private fun DrawScope.drawRainStreaks(
    w: Float,
    h: Float,
    particles: List<FloatArray>,
    intensity: Float,
    dropSpeed: Float,
    windAngle: Float,
    windSpeed: Float,
    time: Float,
    horizonY: Float
) {
    val rad = windAngle * PI.toFloat() / 180f
    val windDX = cos(rad) * (windSpeed / 12f)
    val count = (particles.size * intensity).toInt().coerceAtLeast(10)
    val rainColor = Color(0xFF93C5FD).copy(alpha = 0.65f)

    for (i in 0 until count) {
        val p = particles[i]
        val speed = p[3] * (dropSpeed / 16f)
        val y = ((p[1] + time * speed) % 1.0f) * h
        val x = ((p[0] + (y / h) * windDX * 0.05f) % 1.0f) * w
        val len = 12f * p[2]

        drawLine(
            color = rainColor,
            start = Offset(x, y),
            end = Offset(x + windDX * 1.5f, y + len),
            strokeWidth = 1.2f,
            cap = StrokeCap.Round
        )

        if (y > horizonY && y < horizonY + 20f && (i % 3 == 0)) {
            drawCircle(
                color = Color(0xFFBFDBFE).copy(alpha = 0.5f),
                radius = 3.5f,
                center = Offset(x, horizonY + 6f),
                style = Stroke(width = 1f)
            )
        }
    }
}

private fun DrawScope.drawSnowFlurries(
    w: Float,
    h: Float,
    particles: List<FloatArray>,
    intensity: Float,
    flutter: Float,
    windSpeed: Float,
    time: Float,
    horizonY: Float
) {
    val count = (particles.size * intensity).toInt().coerceAtLeast(10)
    val flakeColor = Color(0xFFF8FAFC).copy(alpha = 0.85f)

    for (i in 0 until count) {
        val p = particles[i]
        val phase = p[2] + time * 3f
        val flutterX = sin(phase) * flutter * 8f
        val y = ((p[1] + time * 0.08f) % 1.0f) * h
        val x = ((p[0] + flutterX / w + (windSpeed / 100f) * (y / h)) % 1.0f) * w

        drawCircle(
            color = flakeColor,
            radius = p[3],
            center = Offset(x, y)
        )
    }
}

private fun DrawScope.drawFogVolumeOverlay(
    w: Float,
    h: Float,
    density: Float,
    mist: Float,
    lightningFlash: Float,
    horizonY: Float
) {
    val fogColor = if (lightningFlash > 0.2f) Color(0xFFE2E8F0) else Color(0xFF64748B)
    val fogTop = (horizonY - 20f).coerceAtLeast(0f)
    drawRect(
        brush = Brush.verticalGradient(
            colors = listOf(
                fogColor.copy(alpha = 0.05f * density),
                fogColor.copy(alpha = 0.55f * density + 0.25f * mist)
            ),
            startY = fogTop,
            endY = h
        ),
        topLeft = Offset(0f, fogTop),
        size = Size(w, h - fogTop)
    )
}

// ---------------------------------------------------------------------
// TAB CONTENTS
// ---------------------------------------------------------------------

@Composable
private fun PrecipitationTab(
    rainIntensity: Float,
    onRainIntensityChange: (Float) -> Unit,
    dropSpeed: Float,
    onDropSpeedChange: (Float) -> Unit,
    dropletSize: Float,
    onDropletSizeChange: (Float) -> Unit,
    splashDensity: Float,
    onSplashDensityChange: (Float) -> Unit,
    puddleAccumulation: Float,
    onPuddleAccumulationChange: (Float) -> Unit
) {
    Card(
        colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
        shape = RoundedCornerShape(12.dp),
        modifier = Modifier.padding(bottom = 12.dp)
    ) {
        Column(modifier = Modifier.padding(14.dp)) {
            Text(
                text = "Precipitation Dynamics",
                style = MaterialTheme.typography.titleMedium,
                color = Color(0xFF38BDF8),
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(10.dp))

            SliderRow(
                label = "Rain Intensity",
                value = rainIntensity,
                onValueChange = onRainIntensityChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(rainIntensity * 100).toInt()}%"
            )
            SliderRow(
                label = "Terminal Drop Velocity",
                value = dropSpeed,
                onValueChange = onDropSpeedChange,
                valueRange = 8.0f..32.0f,
                displayValue = "${dropSpeed.toInt()} m/s"
            )
            SliderRow(
                label = "Droplet Size (Volume)",
                value = dropletSize,
                onValueChange = onDropletSizeChange,
                valueRange = 0.5f..3.0f,
                displayValue = "${(dropletSize * 10).toInt() / 10f} mm"
            )
            SliderRow(
                label = "Ground Splash Impact",
                value = splashDensity,
                onValueChange = onSplashDensityChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(splashDensity * 100).toInt()}%"
            )
            SliderRow(
                label = "Puddle Water Accumulation",
                value = puddleAccumulation,
                onValueChange = onPuddleAccumulationChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(puddleAccumulation * 100).toInt()}%"
            )
        }
    }
}

@Composable
private fun SnowFrostTab(
    snowIntensity: Float,
    onSnowIntensityChange: (Float) -> Unit,
    flakeFlutter: Float,
    onFlakeFlutterChange: (Float) -> Unit,
    frostGlaze: Float,
    onFrostGlazeChange: (Float) -> Unit,
    groundSnowCover: Float,
    onGroundSnowCoverChange: (Float) -> Unit
) {
    Card(
        colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
        shape = RoundedCornerShape(12.dp),
        modifier = Modifier.padding(bottom = 12.dp)
    ) {
        Column(modifier = Modifier.padding(14.dp)) {
            Text(
                text = "Snow & Cryosphere",
                style = MaterialTheme.typography.titleMedium,
                color = Color(0xFF93C5FD),
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(10.dp))

            SliderRow(
                label = "Snowfall Precipitation",
                value = snowIntensity,
                onValueChange = onSnowIntensityChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(snowIntensity * 100).toInt()}%"
            )
            SliderRow(
                label = "Flake Flutter Turbulence",
                value = flakeFlutter,
                onValueChange = onFlakeFlutterChange,
                valueRange = 0.1f..1.0f,
                displayValue = "${(flakeFlutter * 100).toInt()}%"
            )
            SliderRow(
                label = "Surface Frost Glaze",
                value = frostGlaze,
                onValueChange = onFrostGlazeChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(frostGlaze * 100).toInt()}%"
            )
            SliderRow(
                label = "Ground Snowpack Accumulation",
                value = groundSnowCover,
                onValueChange = onGroundSnowCoverChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(groundSnowCover * 100).toInt()}%"
            )
        }
    }
}

@Composable
private fun CloudsFogTab(
    cloudType: Int,
    onCloudTypeChange: (Int) -> Unit,
    cloudCoverage: Float,
    onCloudCoverageChange: (Float) -> Unit,
    cloudDensity: Float,
    onCloudDensityChange: (Float) -> Unit,
    silverLiningG: Float,
    onSilverLiningGChange: (Float) -> Unit,
    powderEffect: Float,
    onPowderEffectChange: (Float) -> Unit,
    sunAbsorption: Float,
    onSunAbsorptionChange: (Float) -> Unit,
    detailErosion: Float,
    onDetailErosionChange: (Float) -> Unit,
    cloudBaseAltitude: Float,
    onCloudBaseAltitudeChange: (Float) -> Unit,
    raymarchSteps: Int,
    onRaymarchStepsChange: (Int) -> Unit,
    enableGodRays: Boolean,
    onEnableGodRaysChange: (Boolean) -> Unit,
    godRaysIntensity: Float,
    onGodRaysIntensityChange: (Float) -> Unit,
    enableCirrus: Boolean,
    onEnableCirrusChange: (Boolean) -> Unit,
    cirrusCoverage: Float,
    onCirrusCoverageChange: (Float) -> Unit,
    billowDepth: Float,
    onBillowDepthChange: (Float) -> Unit,
    sunBacklight: Float,
    onSunBacklightChange: (Float) -> Unit,
    fogDensity: Float,
    onFogDensityChange: (Float) -> Unit,
    groundMist: Float,
    onGroundMistChange: (Float) -> Unit
) {
    // Card 1: Cloud Species Selection (Robert Beckebans Nubis Types)
    Card(
        colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
        shape = RoundedCornerShape(12.dp),
        modifier = Modifier.padding(bottom = 12.dp)
    ) {
        Column(modifier = Modifier.padding(14.dp)) {
            Text(
                text = "Cloud Type & Species (Nubis Gradients)",
                style = MaterialTheme.typography.titleMedium,
                color = Color(0xFFA78BFA),
                fontWeight = FontWeight.Bold
            )
            Text(
                text = "Height density profiles converted from Robert Beckebans's Nubis model",
                style = MaterialTheme.typography.bodySmall,
                color = Color(0xFF94A3B8)
            )
            Spacer(modifier = Modifier.height(10.dp))

            val types = listOf(
                Triple(0, "Stratus", "Thin flat sheets (overcast)"),
                Triple(1, "Stratocumulus", "Rolling waves of clouds"),
                Triple(2, "Cumulus", "Towering cauliflower giants")
            )

            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                types.forEach { (typeId, title, desc) ->
                    val isSelected = cloudType == typeId
                    Surface(
                        shape = RoundedCornerShape(8.dp),
                        color = if (isSelected) Color(0xFF7C3AED).copy(alpha = 0.45f) else Color(0xFF1E293B),
                        border = CardDefaults.outlinedCardBorder().copy(
                            brush = Brush.linearGradient(
                                if (isSelected) listOf(Color(0xFFA78BFA), Color(0xFF7C3AED))
                                else listOf(Color(0xFF334155), Color(0xFF1E293B))
                            )
                        ),
                        modifier = Modifier
                            .weight(1f)
                            .clickable { onCloudTypeChange(typeId) }
                            .testTag("cloud_type_button_$typeId")
                    ) {
                        Column(modifier = Modifier.padding(8.dp)) {
                            Text(
                                text = title,
                                fontSize = 11.sp,
                                fontWeight = FontWeight.Bold,
                                color = if (isSelected) Color.White else Color(0xFFCBD5E1)
                            )
                            Spacer(modifier = Modifier.height(2.dp))
                            Text(
                                text = desc,
                                fontSize = 9.sp,
                                color = Color(0xFF94A3B8),
                                maxLines = 2
                            )
                        }
                    }
                }
            }
        }
    }

    // Card 2: Cinematic Atmospheric Volumetrics (God Rays, Cirrus, Billow Scale)
    Card(
        colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
        shape = RoundedCornerShape(12.dp),
        modifier = Modifier.padding(bottom = 12.dp)
    ) {
        Column(modifier = Modifier.padding(14.dp)) {
            Text(
                text = "Cinematic Volumetrics & Atmospheric Rays",
                style = MaterialTheme.typography.titleMedium,
                color = Color(0xFFFBBF24),
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(8.dp))

            Row(
                modifier = Modifier.fillMaxWidth().padding(vertical = 4.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(text = "Crepuscular God Rays (Light Shafts)", style = MaterialTheme.typography.bodyMedium, color = Color.White)
                Switch(checked = enableGodRays, onCheckedChange = onEnableGodRaysChange)
            }

            if (enableGodRays) {
                SliderRow(
                    label = "God Ray Shaft Beam Intensity",
                    value = godRaysIntensity,
                    onValueChange = onGodRaysIntensityChange,
                    valueRange = 0.0f..1.0f,
                    displayValue = "${(godRaysIntensity * 100).toInt()}%"
                )
            }

            Row(
                modifier = Modifier.fillMaxWidth().padding(vertical = 4.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(text = "High-Altitude Cirrus Veil (10km)", style = MaterialTheme.typography.bodyMedium, color = Color.White)
                Switch(checked = enableCirrus, onCheckedChange = onEnableCirrusChange)
            }

            if (enableCirrus) {
                SliderRow(
                    label = "Cirrus Ice Plume Density",
                    value = cirrusCoverage,
                    onValueChange = onCirrusCoverageChange,
                    valueRange = 0.0f..1.0f,
                    displayValue = "${(cirrusCoverage * 100).toInt()}%"
                )
            }

            SliderRow(
                label = "Solar Disc Backlight & Corona",
                value = sunBacklight,
                onValueChange = onSunBacklightChange,
                valueRange = 0.2f..1.2f,
                displayValue = "${(sunBacklight * 100).toInt()}%"
            )
        }
    }

    // Card 3: Optical Radiative Transfer (Beer's Law, Powder Effect, Henyey-Greenstein Silver Lining)
    Card(
        colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
        shape = RoundedCornerShape(12.dp),
        modifier = Modifier.padding(bottom = 12.dp)
    ) {
        Column(modifier = Modifier.padding(14.dp)) {
            Text(
                text = "Photometric Scattering & Optical Depth",
                style = MaterialTheme.typography.titleMedium,
                color = Color(0xFF38BDF8),
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(8.dp))

            SliderRow(
                label = "Weather Map Fractional Coverage",
                value = cloudCoverage,
                onValueChange = onCloudCoverageChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(cloudCoverage * 100).toInt()}%"
            )
            SliderRow(
                label = "Volumetric Optical Density",
                value = cloudDensity,
                onValueChange = onCloudDensityChange,
                valueRange = 0.1f..1.0f,
                displayValue = "${(cloudDensity * 100).toInt()}%"
            )
            SliderRow(
                label = "Silver Lining (Henyey-Greenstein g1)",
                value = silverLiningG,
                onValueChange = onSilverLiningGChange,
                valueRange = 0.40f..0.98f,
                displayValue = "${(silverLiningG * 100).toInt() / 100f}"
            )
            SliderRow(
                label = "Beer-Powder Sugar Approximation",
                value = powderEffect,
                onValueChange = onPowderEffectChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(powderEffect * 100).toInt()}%"
            )
            SliderRow(
                label = "Sun Cone March Extinction",
                value = sunAbsorption,
                onValueChange = onSunAbsorptionChange,
                valueRange = 0.20f..1.50f,
                displayValue = "${(sunAbsorption * 100).toInt() / 100f}"
            )
            SliderRow(
                label = "High-Freq 3D Worley Erosion",
                value = detailErosion,
                onValueChange = onDetailErosionChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(detailErosion * 100).toInt()}%"
            )
            SliderRow(
                label = "Planetary Shell Base Altitude",
                value = cloudBaseAltitude,
                onValueChange = onCloudBaseAltitudeChange,
                valueRange = 3000f..8000f,
                displayValue = "${cloudBaseAltitude.toInt()} m"
            )
            SliderRow(
                label = "Raymarch Steps Quality",
                value = raymarchSteps.toFloat(),
                onValueChange = { onRaymarchStepsChange(it.toInt()) },
                valueRange = 16f..64f,
                displayValue = "$raymarchSteps steps"
            )
            SliderRow(
                label = "Ground Froxel Fog Density",
                value = fogDensity,
                onValueChange = onFogDensityChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(fogDensity * 100).toInt()}%"
            )
            SliderRow(
                label = "Ground Mist Layer",
                value = groundMist,
                onValueChange = onGroundMistChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(groundMist * 100).toInt()}%"
            )
        }
    }

    // Card 4: Nubis Engine Architecture Telemetry Card
    Card(
        colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
        shape = RoundedCornerShape(12.dp),
        modifier = Modifier.padding(bottom = 12.dp)
    ) {
        Column(modifier = Modifier.padding(14.dp)) {
            Text(
                text = "Beckebans Nubis Pipeline Telemetry",
                style = MaterialTheme.typography.titleSmall.copy(fontWeight = FontWeight.Bold),
                color = Color(0xFF4ADE80)
            )
            Spacer(modifier = Modifier.height(6.dp))

            val stats = listOf(
                "Synthesis Pipeline" to "Continuous Toroidal Perlin-Worley FBM Map (384x192)",
                "Light Scattering" to "Dual-Lobe HG (Forward g1=0.82, Back g2=-0.2)",
                "Crepuscular God Rays" to if (enableGodRays) "Active (14 Atmospheric Beams)" else "Disabled",
                "Cirrus Ice Layer" to if (enableCirrus) "Active (10,000m Jetstream Drift)" else "Disabled",
                "Absorption Law" to "Beer's Law + Powder Sugar (1 - exp(-2d))",
                "Planetary Atmosphere" to "R_inner = 705,000m, R_outer = 722,000m"
            )

            stats.forEach { (label, value) ->
                Row(
                    modifier = Modifier.fillMaxWidth().padding(vertical = 2.dp),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text(text = label, fontSize = 10.sp, color = Color(0xFF94A3B8))
                    Text(text = value, fontSize = 10.sp, fontWeight = FontWeight.Bold, color = Color(0xFFE2E8F0))
                }
            }
        }
    }
}

@Composable
private fun WindStormsTab(
    windSpeed: Float,
    onWindSpeedChange: (Float) -> Unit,
    windAzimuth: Float,
    onWindAzimuthChange: (Float) -> Unit,
    lightningFrequency: Float,
    onLightningFrequencyChange: (Float) -> Unit,
    isStormActive: Boolean,
    onStormActiveChange: (Boolean) -> Unit,
    onTriggerStrike: () -> Unit
) {
    Card(
        colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
        shape = RoundedCornerShape(12.dp),
        modifier = Modifier.padding(bottom = 12.dp)
    ) {
        Column(modifier = Modifier.padding(14.dp)) {
            Text(
                text = "Wind Field & Severe Storms",
                style = MaterialTheme.typography.titleMedium,
                color = Color(0xFFFBBF24),
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(10.dp))

            SliderRow(
                label = "Wind Velocity",
                value = windSpeed,
                onValueChange = onWindSpeedChange,
                valueRange = 0.0f..90.0f,
                displayValue = "${windSpeed.toInt()} km/h"
            )
            SliderRow(
                label = "Wind Direction Angle",
                value = windAzimuth,
                onValueChange = onWindAzimuthChange,
                valueRange = 0.0f..360.0f,
                displayValue = "${windAzimuth.toInt()}°"
            )
            SliderRow(
                label = "Lightning Discharge Rate",
                value = lightningFrequency,
                onValueChange = onLightningFrequencyChange,
                valueRange = 0.0f..1.0f,
                displayValue = "${(lightningFrequency * 100).toInt()}%"
            )

            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(vertical = 8.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = "Severe Thunderstorm Mode",
                    style = MaterialTheme.typography.bodyMedium,
                    color = Color.White
                )
                Switch(
                    checked = isStormActive,
                    onCheckedChange = onStormActiveChange,
                    colors = SwitchDefaults.colors(checkedThumbColor = Color(0xFFFBBF24))
                )
            }

            Surface(
                shape = RoundedCornerShape(8.dp),
                color = Color(0xFF1E293B),
                modifier = Modifier
                    .fillMaxWidth()
                    .clickable { onTriggerStrike() }
                    .padding(top = 6.dp)
            ) {
                Row(
                    modifier = Modifier.padding(vertical = 10.dp, horizontal = 14.dp),
                    horizontalArrangement = Arrangement.Center,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Icon(
                        imageVector = Icons.Default.FlashOn,
                        contentDescription = "Trigger Strike",
                        tint = Color(0xFFFBBF24),
                        modifier = Modifier.size(16.dp)
                    )
                    Spacer(modifier = Modifier.width(8.dp))
                    Text(
                        text = "Trigger Lightning Bolt Strike",
                        fontSize = 12.sp,
                        fontWeight = FontWeight.Bold,
                        color = Color.White
                    )
                }
            }
        }
    }
}

@Composable
private fun NativeEngineTab() {
    Card(
        colors = CardDefaults.cardColors(containerColor = Color(0xFF131722)),
        shape = RoundedCornerShape(12.dp),
        modifier = Modifier.padding(bottom = 12.dp)
    ) {
        Column(modifier = Modifier.padding(14.dp)) {
            Text(
                text = "Converted Robert Beckebans Vulkan Modules",
                style = MaterialTheme.typography.titleMedium,
                color = Color(0xFF4ADE80),
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(10.dp))

            val modules = listOf(
                Triple("CloudRaymarch.cpp", "native_assets/src/weather/clouds/CloudRaymarch.cpp", "Beckebans planetary shell raymarcher & 6-cone light march"),
                Triple("CloudGen.cpp", "native_assets/src/weather/clouds/CloudGen.cpp", "Nubis Stratus/Cumulus gradients, Beer's law & powder effect"),
                Triple("cloud_raymarch.comp", "native_assets/src/weather/clouds/shaders/cloud_raymarch.comp", "Vulkan 1.3 compute shader converted from volumetric_clouds.frag"),
                Triple("CloudParams.h", "native_assets/src/weather/clouds/CloudParams.h", "Dual-lobe HG parameters, earth radius & optical scattering"),
                Triple("test_clouds.cpp", "native_assets/tests/weather/test_clouds.cpp", "Complete test suite validating Beckebans cloud physics (PASS)"),
                Triple("WeatherSystem.cpp", "native_assets/src/weather/WeatherSystem.cpp", "Precipitation, wetness integration & world state sync"),
                Triple("rain_particle.comp", "native_assets/src/weather/rain/shaders/rain_particle.comp", "GPU compute 65,536 rain particles with wind deflection"),
                Triple("fog_froxel.comp", "native_assets/src/weather/fog/shaders/fog_froxel.comp", "160x90x64 3D froxel volume grid with Mie inscattering")
            )

            modules.forEach { (name, path, desc) ->
                Surface(
                    shape = RoundedCornerShape(8.dp),
                    color = Color(0xFF0F172A),
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(vertical = 4.dp)
                ) {
                    Column(modifier = Modifier.padding(10.dp)) {
                        Row(
                            modifier = Modifier.fillMaxWidth(),
                            horizontalArrangement = Arrangement.SpaceBetween,
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Text(
                                text = name,
                                fontSize = 12.sp,
                                fontWeight = FontWeight.Bold,
                                color = Color(0xFF38BDF8)
                            )
                            Surface(
                                shape = RoundedCornerShape(4.dp),
                                color = Color(0xFF166534).copy(alpha = 0.5f)
                            ) {
                                Text(
                                    text = "VULKAN 1.3",
                                    fontSize = 9.sp,
                                    fontWeight = FontWeight.Bold,
                                    color = Color(0xFF4ADE80),
                                    modifier = Modifier.padding(horizontal = 4.dp, vertical = 2.dp)
                                )
                            }
                        }
                        Text(
                            text = path,
                            fontSize = 10.sp,
                            fontFamily = FontFamily.Monospace,
                            color = Color(0xFF64748B)
                        )
                        Spacer(modifier = Modifier.height(2.dp))
                        Text(
                            text = desc,
                            fontSize = 11.sp,
                            color = Color(0xFFCBD5E1)
                        )
                    }
                }
            }
        }
    }
}

@Composable
private fun SliderRow(
    label: String,
    value: Float,
    onValueChange: (Float) -> Unit,
    valueRange: ClosedFloatingPointRange<Float>,
    displayValue: String
) {
    Column(modifier = Modifier.padding(vertical = 4.dp)) {
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween
        ) {
            Text(text = label, style = MaterialTheme.typography.bodySmall, color = Color(0xFFCBD5E1))
            Text(text = displayValue, style = MaterialTheme.typography.bodySmall, fontWeight = FontWeight.Bold, color = Color(0xFF38BDF8))
        }
        Slider(
            value = value,
            onValueChange = onValueChange,
            valueRange = valueRange,
            colors = SliderDefaults.colors(
                thumbColor = Color(0xFF38BDF8),
                activeTrackColor = Color(0xFF0284C7),
                inactiveTrackColor = Color(0xFF1E293B)
            )
        )
    }
}
