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
}
