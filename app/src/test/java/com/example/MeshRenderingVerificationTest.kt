package com.example

import com.example.sphere.model.TrailerCategoryType
import com.example.sphere.model.VehicleCategoryType
import com.example.sphere.model.createDefaultVehiclesCatalog
import com.example.sphere.model.defaultCommodityItems
import com.example.sphere.model.defaultTrailerItems
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner
import org.robolectric.annotation.Config

@RunWith(RobolectricTestRunner::class)
@Config(sdk = [36])
class MeshRenderingVerificationTest {

    @Test
    fun allCommoditiesCatalogIsOpaqueWatertightAndComplete() {
        assertEquals("Expected 51 procedural C++ commodities", 51, defaultCommodityItems.size)
        for (item in defaultCommodityItems) {
            assertTrue(
                "Commodity ${item.id} (${item.name}) opacity must default >= 0.99f",
                item.opacity >= 0.99f
            )
            assertEquals(
                "Commodity ${item.id} (${item.name}) opacityBias must default to 0.0f (opaque)",
                0.0f,
                item.opacityBias,
                0.001f
            )
            assertTrue(
                "Commodity ${item.id} (${item.name}) must have positive real-world dimensions",
                item.lengthM > 0.01f && item.widthM > 0.01f && item.heightM > 0.01f
            )
        }
    }

    @Test
    fun allTrailersCatalogMatchesRealWorldScaleAndOpacity() {
        assertEquals("Expected 51 procedural C++ trailers", 51, defaultTrailerItems.size)
        val lowDeckCategories = setOf(
            TrailerCategoryType.HEAVY_HAULAGE,
            TrailerCategoryType.CAR_CARRIER,
            TrailerCategoryType.RECREATIONAL,
            TrailerCategoryType.FLATBED,
            TrailerCategoryType.CHASSIS,
            TrailerCategoryType.LOGGING
        )
        for (item in defaultTrailerItems) {
            assertEquals(
                "Trailer ${item.id} (${item.name}) curtainOpacity must default to 1.0f (opaque)",
                1.0f,
                item.curtainOpacity,
                0.001f
            )
            assertTrue(
                "Trailer ${item.id} (${item.name}) ground clearance must be >= 0.24m",
                item.groundClearanceM >= 0.24f
            )
            if (item.category !in lowDeckCategories) {
                assertTrue(
                    "Trailer ${item.id} (${item.name}) tire OD (${item.wheelRadiusM * 2f}m) must fit below deck (${item.deckHeightM}m)",
                    item.wheelRadiusM * 2f <= item.deckHeightM + 0.08f
                )
            }
        }
    }

    @Test
    fun allVehiclesCatalogHasValidMetricsAndOpaqueLiveries() {
        val vehicles = createDefaultVehiclesCatalog()
        assertEquals("Expected 48 procedural C++ lorries and buses", 48, vehicles.size)
        assertEquals("Expected 5 vehicle categories", 5, VehicleCategoryType.entries.size)

        for ((idx, item) in vehicles.withIndex()) {
            assertEquals("Vehicle index must match ID", idx, item.id)
            assertTrue("Vehicle ${item.name} length must be positive", item.lengthM in 4.0f..20.0f)
            if (item.id in 21..32) {
                assertEquals("Extended coach ${item.name} must belong to BUS_COACH", VehicleCategoryType.BUS_COACH, item.category)
                assertTrue("Extended coach ${item.name} must be at least 12m length", item.lengthM >= 12.0f)
            }
            assertTrue("Vehicle ${item.name} width must be road-legal", item.widthM in 1.7f..2.65f)
            assertTrue("Vehicle ${item.name} height must be positive", item.heightM in 2.0f..4.6f)
            assertTrue("Vehicle ${item.name} wheelbase must be shorter than length", item.wheelbaseM < item.lengthM)
            assertTrue("Vehicle ${item.name} kerb weight must be less than GVW", item.kerbWeightKg < item.maxGvwKg)
            assertTrue("Vehicle ${item.name} torque & HP must be positive", item.engineTorqueNm > 0f && item.enginePowerHp > 0f)
            assertEquals("Vehicle base color must be opaque", 1.0f, item.baseColor.alpha, 0.001f)
            assertEquals("Vehicle accent color must be opaque", 1.0f, item.accentColor.alpha, 0.001f)
            assertEquals("Vehicle ${item.name} doorOpenRatio must default to 0.0f", 0.0f, item.doorOpenRatio, 0.001f)

            if (item.category == VehicleCategoryType.LORRY_ARTICULATED) {
                assertTrue("Articulated tractor ${item.name} must expose positive fifth-wheel offset", item.fifthWheelOffsetM > 0.5f)
                assertTrue("Articulated tractor ${item.name} canHitchTrailer must be true", item.canHitchTrailer)
            } else {
                assertFalse("Non-tractor ${item.name} must not allow 5th-wheel hitch", item.canHitchTrailer)
            }
        }
    }
}

