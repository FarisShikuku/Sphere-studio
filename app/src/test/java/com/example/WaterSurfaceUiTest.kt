package com.example

import androidx.compose.material3.MaterialTheme
import androidx.compose.ui.test.assertIsDisplayed
import androidx.compose.ui.test.junit4.createComposeRule
import androidx.compose.ui.test.onNodeWithTag
import androidx.compose.ui.test.performClick
import androidx.compose.ui.test.performScrollTo
import com.example.sphere.WaterStudioScreen
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner
import org.robolectric.annotation.Config

@RunWith(RobolectricTestRunner::class)
@Config(sdk = [36], qualifiers = "w412dp-h915dp")
class WaterSurfaceUiTest {

    @get:Rule
    val composeTestRule = createComposeRule()

    @Test
    fun waterSurfaceTabAndControlsRenderInWaterStudio() {
        composeTestRule.setContent {
            MaterialTheme {
                WaterStudioScreen(onNavigateBack = {})
            }
        }
        composeTestRule.waitForIdle()

        composeTestRule.onNodeWithTag("water_3d_viewport").assertIsDisplayed()
        composeTestRule.onNodeWithTag("water_tab_2").performScrollTo().performClick()
        composeTestRule.waitForIdle()

        composeTestRule.onNodeWithTag("water_surface_tab").assertIsDisplayed()
        composeTestRule.onNodeWithTag("water_surface_enable_switch")
            .performScrollTo()
            .assertIsDisplayed()
        composeTestRule.onNodeWithTag("water_surface_cutout_switch")
            .performScrollTo()
            .assertIsDisplayed()
        composeTestRule.onNodeWithTag("water_surface_cutout_chip_10x10x10")
            .performScrollTo()
            .performClick()
        composeTestRule.onNodeWithTag("water_surface_cutout_width_slider")
            .performScrollTo()
            .assertIsDisplayed()
        composeTestRule.onNodeWithTag("water_surface_cutout_height_slider")
            .performScrollTo()
            .assertIsDisplayed()
        composeTestRule.onNodeWithTag("water_surface_cutout_depth_slider")
            .performScrollTo()
            .assertIsDisplayed()
        composeTestRule.onNodeWithTag("water_surface_debug_wireframe_grid")
            .performScrollTo()
            .performClick()
        composeTestRule.onNodeWithTag("water_surface_wind_speed_slider")
            .performScrollTo()
            .assertIsDisplayed()
        composeTestRule.onNodeWithTag("water_surface_choppiness_slider")
            .performScrollTo()
            .assertIsDisplayed()
        composeTestRule.onNodeWithTag("water_surface_depth_slider")
            .performScrollTo()
            .assertIsDisplayed()
    }
}
