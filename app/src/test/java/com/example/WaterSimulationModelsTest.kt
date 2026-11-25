package com.example

import com.example.sphere.availableAssetCategories
import com.example.sphere.model.*
import org.junit.Assert.*
import org.junit.Test

class WaterSimulationModelsTest {

    @Test
    fun waterCategoryIsRegisteredInAssetCategories() {
        val waterCategory = availableAssetCategories.firstOrNull { it.id == "water" }
        assertNotNull("Water category must exist in availableAssetCategories", waterCategory)
        assertTrue(waterCategory!!.subtitle.contains("SPH", ignoreCase = true))
        assertTrue(waterCategory.description.contains("Beer-Lambert", ignoreCase = true))
    }

    @Test
    fun spawnScenarioOrdinalsMatchCppFluidParamsEnum() {
        assertEquals(0, WaterSpawnScenarioType.DAM_BREAK.ordinalId)
        assertEquals(1, WaterSpawnScenarioType.DUAL_WAVE.ordinalId)
        assertEquals(2, WaterSpawnScenarioType.FOUNTAIN_DROP.ordinalId)
        assertEquals(3, WaterSpawnScenarioType.SLOSH_TANK.ordinalId)
        assertEquals(4, WaterSpawnScenarioType.ZERO_G_ORB.ordinalId)
        assertEquals(5, WaterSpawnScenarioType.WATERFALL_CLIFF.ordinalId)
    }

    @Test
    fun debugViewOrdinalsMatchCppFluidParamsEnum() {
        assertEquals(0, FluidDebugViewType.COMPOSITE.ordinalId)
        assertEquals(1, FluidDebugViewType.RAW_DEPTH.ordinalId)
        assertEquals(2, FluidDebugViewType.SMOOTHED_DEPTH.ordinalId)
        assertEquals(3, FluidDebugViewType.NORMALS.ordinalId)
        assertEquals(4, FluidDebugViewType.THICKNESS.ordinalId)
        assertEquals(5, FluidDebugViewType.PARTICLE_SPHERES.ordinalId)
    }

    @Test
    fun waterPresetsHaveValidPhysicalAndOpticalBounds() {
        assertTrue("Expected at least 6 water presets", waterPresetsList.size >= 6)
        waterPresetsList.forEach { preset ->
            assertTrue("${preset.name} particleCount out of bounds", preset.physics.particleCount in 128..4096)
            assertTrue("${preset.name} smoothingRadius out of bounds", preset.physics.smoothingRadius in 0.10f..0.60f)
            assertTrue("${preset.name} substeps out of bounds", preset.physics.substeps in 1..4)
            assertTrue("${preset.name} IOR out of bounds", preset.optics.indexOfRefraction in 1.0f..2.0f)
            assertTrue("${preset.name} bounds positive", preset.tank.boundsWidth > 1.0f && preset.tank.boundsHeight > 1.0f)
        }
    }

    @Test
    fun waterfallPresetsAndConstraintsHaveValidBounds() {
        assertTrue("Expected at least 5 waterfall cascade presets", waterfallCascadePresetsList.size >= 5)
        waterfallCascadePresetsList.forEach { preset ->
            val w = preset.waterfall
            assertTrue("${preset.name} should enable waterfall", w.enabled)
            assertTrue("${preset.name} height bounds", w.cliffHeight in 0.55f..2.30f)
            assertTrue("${preset.name} width bounds", w.cliffWidth in 0.40f..2.60f)
            assertTrue("${preset.name} depth bounds", w.cliffDepth in 0.35f..1.80f)
            assertTrue("${preset.name} volume bounds", w.flowVolume in 0.15f..2.80f)
            assertTrue("${preset.name} slope bounds", w.lipSlopeDeg in 0.0f..45.0f)
            assertTrue("${preset.name} undercut bounds", w.undercut in 0.0f..0.50f)
            assertTrue("${preset.name} ledges bounds", w.steppedLedges in 0..3)
            assertTrue("${preset.name} roughness bounds", w.roughness in 0.0f..1.0f)
            assertTrue("${preset.name} channeling bounds", w.rockChanneling in 0.0f..1.0f)
            assertTrue("${preset.name} mist bounds", w.airDragAndMist in 0.0f..1.0f)
            assertTrue("${preset.name} pool depth bounds", w.plungePoolDepth in 0.05f..0.90f)
        }
    }

    @Test
    fun waterSurfacePresetsAndDebugModesMatchNativeBounds() {
        assertEquals(0, WaterSurfaceDebugViewType.LIT_OPTICS.ordinalId)
        assertEquals(1, WaterSurfaceDebugViewType.DISPLACEMENT_MAP.ordinalId)
        assertEquals(2, WaterSurfaceDebugViewType.NORMAL_MAP.ordinalId)
        assertEquals(3, WaterSurfaceDebugViewType.SEABED_DEPTH.ordinalId)
        assertEquals(4, WaterSurfaceDebugViewType.FRESNEL_AND_FOAM.ordinalId)
        assertEquals(5, WaterSurfaceDebugViewType.WIREFRAME_GRID.ordinalId)

        assertTrue("Expected at least 6 water surface presets", waterSurfacePresetsList.size >= 6)
        val cutPreset = waterSurfacePresetsList.firstOrNull { it.state.cutoutEnabled }
        assertNotNull("Expected 10m Ocean Cut preset in waterSurfacePresetsList", cutPreset)
        assertEquals(10f, cutPreset!!.state.cutoutWidthM, 0.01f)
        assertEquals(10f, cutPreset.state.cutoutHeightM, 0.01f)
        assertEquals(10f, cutPreset.state.cutoutDepthM, 0.01f)

        val defaultState = WaterSurfaceState()
        assertFalse(defaultState.cutoutEnabled)
        assertEquals(10f, defaultState.cutoutWidthM, 0.01f)
        assertEquals(10f, defaultState.cutoutHeightM, 0.01f)
        assertEquals(10f, defaultState.cutoutDepthM, 0.01f)
        assertTrue(waterCutoutSizePresets.any { it.widthM == 10f && it.heightM == 10f && it.depthM == 10f })

        waterSurfacePresetsList.forEach { preset ->
            val s = preset.state
            assertTrue("${preset.name} should enable surface mode", s.enabled)
            assertTrue("${preset.name} gridResolution power of 2", s.gridResolution in listOf(32, 64, 128))
            assertTrue("${preset.name} patchSizeM bounds", s.patchSizeM in 8f..256f)
            assertTrue("${preset.name} windSpeedMs bounds", s.windSpeedMs in 1f..36f)
            assertTrue("${preset.name} choppinessLambda bounds", s.choppinessLambda in 0f..2.5f)
            assertTrue("${preset.name} waterDepthM bounds", s.waterDepthM in 1f..24f)
            assertTrue("${preset.name} indexOfRefraction bounds", s.indexOfRefraction in 1.0f..1.75f)
            assertTrue("${preset.name} cutout dims bounds", s.cutoutWidthM in 3f..32f && s.cutoutHeightM in 3f..24f && s.cutoutDepthM in 3f..32f)
        }
    }
}
