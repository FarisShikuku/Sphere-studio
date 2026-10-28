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
}
