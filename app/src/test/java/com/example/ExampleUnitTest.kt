package com.example

import org.junit.Assert.*
import org.junit.Test

/**
 * Example local unit test, which will execute on the development machine (host).
 *
 * See [testing documentation](http://d.android.com/tools/testing).
 */
class ExampleUnitTest {
  @Test
  fun addition_isCorrect() {
    assertEquals(4, 2 + 2)
  }

  @Test
  fun `available asset categories contains sky as independent option`() {
    val skyCategory = com.example.sphere.availableAssetCategories.find { it.id == "sky" }
    assertNotNull(skyCategory)
    assertEquals("Atmosphere & Sky", skyCategory?.title)
  }

  @Test
  fun `sky presets list contains all diurnal stages`() {
    val presets = com.example.sphere.skyPresetsList
    assertTrue(presets.size >= 5)
    assertTrue(presets.any { it.name == "High Noon" })
    assertTrue(presets.any { it.name == "Sunset" })
    assertTrue(presets.any { it.name == "Midnight" })
  }

  @Test
  fun `available asset categories contains text as independent GlyphCore Studio option`() {
    val textCategory = com.example.sphere.availableAssetCategories.find { it.id == "text" }
    assertNotNull(textCategory)
    assertEquals("GlyphCore Studio", textCategory?.title)
    assertTrue(textCategory?.subtitle?.contains("Vulkan") == true)
  }

  @Test
  fun `default text presets contain telemetry HUD and 3D billboard presets`() {
    val presets = com.example.sphere.model.defaultTextPresets
    assertTrue(presets.isNotEmpty())
    assertTrue(presets.any { it.id == "vulkan_telemetry_hud" })
    assertTrue(presets.any { it.id == "billboard_stage_anchor" })
    assertTrue(presets.any { it.id == "scifi_cyber_terminal" })
  }

  @Test
  fun `TextureManager rejects corrupted or truncated KTX2 headers`() {
    val invalidBytes = byteArrayOf(0x00, 0x01, 0x02, 0x03)
    val parsed = com.example.sphere.viewport.TextureManager.parseKtx2(invalidBytes)
    assertNull(parsed)
  }

  @Test
  fun `available asset categories contains lighting studio as independent option`() {
    val lightingCategory = com.example.sphere.availableAssetCategories.find { it.id == "lighting" }
    assertNotNull(lightingCategory)
    assertEquals("Lighting Studio", lightingCategory?.title)
    assertTrue(lightingCategory?.description?.contains("flashlight", ignoreCase = true) == true)
  }

  @Test
  fun `default lighting scenarios contain flashlight, bulb and area light`() {
    val scenarios = com.example.sphere.model.DEFAULT_LIGHTING_SCENARIOS
    assertTrue(scenarios.isNotEmpty())
    assertTrue(scenarios.any { it.params.type == com.example.sphere.model.LightSourceType.SPOT_FLASHLIGHT })
    assertTrue(scenarios.any { it.params.type == com.example.sphere.model.LightSourceType.POINT_BULB })
    assertTrue(scenarios.any { it.params.type == com.example.sphere.model.LightSourceType.AREA_SOFTBOX })
  }

  @Test
  fun `kelvinToColor outputs warm reddish-amber for candle and cool bluish-white for sky`() {
    val candle = com.example.sphere.model.kelvinToColor(2000f)
    val daylight = com.example.sphere.model.kelvinToColor(7500f)

    // Candle should have high red and lower blue
    assertTrue(candle.red > candle.blue)
    // 7500K overcast daylight should have substantial blue
    assertTrue(daylight.blue > 0.8f)
  }
}
