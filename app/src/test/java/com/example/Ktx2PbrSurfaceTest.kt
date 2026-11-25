package com.example

import android.content.Context
import androidx.test.core.app.ApplicationProvider
import com.example.sphere.model.MaterialChannelMode
import com.example.sphere.model.SurfacePatternType
import com.example.sphere.model.defaultCompositeSurfaces
import com.example.sphere.model.defaultPbrMaterials
import com.example.sphere.model.getCompositeSurfaceForMaterial
import com.example.sphere.nativebridge.SphereNativeEngine
import com.example.sphere.texture.Ktx2TextureLoader
import com.example.sphere.viewport.computePhysicalUvRepeat
import org.junit.Assert.assertEquals
import org.junit.Assert.assertNotNull
import org.junit.Assert.assertTrue
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner
import org.robolectric.annotation.Config

@RunWith(RobolectricTestRunner::class)
@Config(sdk = [36])
class Ktx2PbrSurfaceTest {

    @Test
    fun `converted KTX2 materials exist in PBR library and composite surfaces with filename scale`() {
        val expectedScales = mapOf(
            "grass_meadow_1000" to 1.000f,
            "coast_sand_800" to 0.800f,
            "brown_mud_leaves" to 1.300f
        )

        expectedScales.forEach { (id, scaleMeters) ->
            val mat = defaultPbrMaterials.firstOrNull { it.id == id }
            assertNotNull("Missing PbrMaterial for $id", mat)
            assertEquals(scaleMeters, mat!!.tileScaleMeters, 0.0001f)
            assertEquals("file:///android_asset/$id.ktx2", mat.assetPath)

            val surf = defaultCompositeSurfaces.firstOrNull { it.id == id }
            assertNotNull("Missing CompositeSurface for $id", surf)
            assertEquals(scaleMeters, surf!!.macroTileMeters, 0.0001f)
            assertEquals(surf, getCompositeSurfaceForMaterial(mat))
        }

        val sandSurf = defaultCompositeSurfaces.first { it.id == "coast_sand_800" }
        assertEquals(SurfacePatternType.SAND_COASTAL_AGGREGATE, sandSurf.patternType)
        assertTrue(SphereNativeEngine.getRegisteredTextureCount() >= 71)
    }

    @Test
    fun `road KTX2 materials exist in PBR library and composite surfaces with road metadata`() {
        val expectedRoads = mapOf(
            "road_highway_two_lane" to 7.400f,
            "road_two_lane_white_dash" to 7.200f,
            "road_double_yellow_line" to 6.800f,
            "road_highway_yellow_edge" to 7.500f,
            "road_cracked_urban_unmarked" to 4.000f
        )

        expectedRoads.forEach { (id, widthMeters) ->
            val mat = defaultPbrMaterials.firstOrNull { it.id == id }
            assertNotNull("Missing road PbrMaterial for $id", mat)
            assertEquals("Roads", mat!!.category)
            assertEquals(widthMeters, mat.tileScaleMeters, 0.0001f)
            assertEquals("file:///android_asset/$id.ktx2", mat.assetPath)
            assertNotNull("Missing roadType on $id", mat.roadType)
            assertNotNull("Missing laneCount on $id", mat.laneCount)
            assertNotNull("Missing roadMarkings on $id", mat.roadMarkings)

            val surf = defaultCompositeSurfaces.firstOrNull { it.id == id }
            assertNotNull("Missing road CompositeSurface for $id", surf)
            assertEquals(widthMeters, surf!!.macroTileMeters, 0.0001f)
            val expectedPattern = if (id == "road_cracked_urban_unmarked") {
                SurfacePatternType.ASPHALT_AGGREGATE_MACADAM
            } else {
                SurfacePatternType.ROAD_HIGHWAY_LANES
            }
            assertEquals(expectedPattern, surf.patternType)
            assertEquals(surf, getCompositeSurfaceForMaterial(mat))
        }
    }

    @Test
    fun `Ktx2TextureLoader decodes all 3 bundled KTX2 assets and parses filename KVD metadata`() {
        val context = ApplicationProvider.getApplicationContext<Context>()
        val cases = listOf(
            Triple("grass_meadow_1000", "1000", "grass-1000-mm-architextures__1__cfb377f7.jpg"),
            Triple("coast_sand_800", "800", "coast-sand-800-mm-architextures_e08dc7f1.jpg"),
            Triple("brown_mud_leaves", "1300", "brown-mud-leaves-1300-mm-architextures__1__331d74d3.jpg")
        )

        cases.forEach { (id, expectedScaleMm, expectedSrcFile) ->
            val decoded = Ktx2TextureLoader.loadSyncFromAssets(context, "file:///android_asset/$id.ktx2")
            assertNotNull("Failed to decode $id.ktx2 from Android assets", decoded)
            val info = decoded!!.info
            assertEquals(43, info.vkFormat) // VK_FORMAT_R8G8B8A8_SRGB
            assertEquals(512, info.pixelWidth)
            assertEquals(512, info.pixelHeight)
            assertEquals(10, info.levelCount)
            assertEquals(512, decoded.albedoBitmap.width)
            assertEquals(512, decoded.albedoBitmap.height)

            assertEquals(expectedScaleMm, info.kvdMetadata["KTXtileScaleMm"])
            assertEquals(expectedSrcFile, info.kvdMetadata["KTXsourceFile"])
            assertEquals("Architextures", info.kvdMetadata["KTXmaterialSource"])
            assertEquals(id, info.kvdMetadata["KTXmaterialId"])

            val normalBmp = decoded.bitmapForChannel(MaterialChannelMode.NORMAL_MAP, 0.8f, 0f, 1f)
            val ormBmp = decoded.bitmapForChannel(MaterialChannelMode.ORM_MAP, 0.8f, 0f, 1f)
            val heightBmp = decoded.bitmapForChannel(MaterialChannelMode.HEIGHT_MAP, 0.8f, 0f, 1f)
            assertEquals(512, normalBmp.width)
            assertEquals(512, ormBmp.width)
            assertEquals(512, heightBmp.width)
        }
    }

    @Test
    fun `Ktx2TextureLoader decodes all 5 road KTX2 assets and parses road KVD metadata`() {
        val context = ApplicationProvider.getApplicationContext<Context>()
        data class RoadKtx2Case(
            val id: String,
            val scaleMm: String,
            val roadType: String,
            val laneCount: String,
            val roadWidthM: String,
            val srcFile: String
        )

        val roadCases = listOf(
            RoadKtx2Case("road_highway_two_lane", "7400", "Highway Two-Lane", "2", "7.40", "asphalt_texture_seamless_by_rfalworth_d6y71cv_22a8e270.jpg"),
            RoadKtx2Case("road_two_lane_white_dash", "7200", "Two-Lane Paved Highway", "2", "7.20", "road_variety_texture_tileable_by_hhh316_d5iid9j_ec512f1b.jpg"),
            RoadKtx2Case("road_double_yellow_line", "6800", "Two-Way Double Yellow", "2", "6.80", "cumdyl_by_slc_world_dan9vog_195e97ea.png"),
            RoadKtx2Case("road_highway_yellow_edge", "7500", "Multi-Lane Highway Segment", "2+", "7.50", "dbmwluryl_by_slc_world_dan9v8g_150b1859.png"),
            RoadKtx2Case("road_cracked_urban_unmarked", "4000", "Cracked Urban Asphalt", "1", "4.00", "cua_by_slc_world_dan9twd_a127362e.png")
        )

        roadCases.forEach { case ->
            val decoded = Ktx2TextureLoader.loadSyncFromAssets(context, "file:///android_asset/${case.id}.ktx2")
            assertNotNull("Failed to decode ${case.id}.ktx2 from Android assets", decoded)
            val info = decoded!!.info
            assertEquals(43, info.vkFormat)
            assertEquals(512, info.pixelWidth)
            assertEquals(512, info.pixelHeight)
            assertEquals(10, info.levelCount)

            assertEquals("Roads", info.kvdMetadata["KTXmaterialCategory"])
            assertEquals(case.id, info.kvdMetadata["KTXmaterialId"])
            assertEquals(case.scaleMm, info.kvdMetadata["KTXtileScaleMm"])
            assertEquals(case.roadType, info.kvdMetadata["KTXroadType"])
            assertEquals(case.laneCount, info.kvdMetadata["KTXlaneCount"])
            assertEquals(case.roadWidthM, info.kvdMetadata["KTXroadWidthM"])
            assertEquals(case.srcFile, info.kvdMetadata["KTXsourceFile"])
            assertNotNull(info.kvdMetadata["KTXmarkings"])
        }
    }

    @Test
    fun `fixed size PBR repeats proportionally on larger surfaces including 1km square`() {
        // 100m x 100m PBR applied to 1 km2 (1000m x 1000m) surface must repeat 10 x 10 times (100 total tiles)
        val rep100mOn1Km = computePhysicalUvRepeat(
            surfaceSpanMeters = 1000f,
            tileScaleMeters = 100f,
            useRealWorldScale = true,
            uvTilingMultiplier = 1.0f
        )
        assertEquals(10.0f, rep100mOn1Km, 0.0001f)

        // 7.40m road PBR applied on 14.80m x 29.60m highway slab repeats 2x x 4x without stretching
        val roadRepX = computePhysicalUvRepeat(14.80f, 7.40f, useRealWorldScale = true, uvTilingMultiplier = 1.0f)
        val roadRepY = computePhysicalUvRepeat(29.60f, 7.40f, useRealWorldScale = true, uvTilingMultiplier = 1.0f)
        assertEquals(2.0f, roadRepX, 0.0001f)
        assertEquals(4.0f, roadRepY, 0.0001f)

        // 7.40m road PBR applied on 1km x 1km surface keeps fixed physical width (~135.14 repeats per axis)
        val roadOn1Km = computePhysicalUvRepeat(1000f, 7.40f, useRealWorldScale = true, uvTilingMultiplier = 1.0f)
        assertEquals(1000f / 7.40f, roadOn1Km, 0.001f)
    }
}
