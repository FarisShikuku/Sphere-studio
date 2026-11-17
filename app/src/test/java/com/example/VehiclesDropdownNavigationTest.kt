package com.example

import androidx.compose.material3.MaterialTheme
import androidx.compose.ui.test.assertIsDisplayed
import androidx.compose.ui.test.junit4.createComposeRule
import androidx.compose.ui.test.onNodeWithTag
import androidx.compose.ui.test.onNodeWithText
import androidx.compose.ui.test.performClick
import androidx.compose.ui.test.performScrollTo
import com.example.sphere.VehiclesStudioScreen
import com.example.sphere.model.VehicleCategoryType
import com.example.sphere.model.createDefaultCommoditiesCatalog
import com.example.sphere.model.createDefaultTrailersCatalog
import com.example.sphere.model.createDefaultVehiclesCatalog
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner
import org.robolectric.annotation.Config

@RunWith(RobolectricTestRunner::class)
@Config(sdk = [36])
class VehiclesDropdownNavigationTest {

    @get:Rule
    val composeTestRule = createComposeRule()

    @Test
    fun navigatingVehicleCategoryChipsAndBottomSheetsDoesNotHang() {
        val vehicles = createDefaultVehiclesCatalog()
        val compatibleTrailers = createDefaultTrailersCatalog().filter {
            it.hitchType.name.contains("FIFTH") || it.hitchType.name.contains("GOOSENECK")
        }
        val cargoOptions = createDefaultCommoditiesCatalog().take(24)

        composeTestRule.setContent {
            MaterialTheme {
                VehiclesStudioScreen(onNavigateBack = {})
            }
        }
        composeTestRule.waitForIdle()

        // 1. Open variant sheet in default category (LORRY_RIGID) and select second variant
        composeTestRule.onNodeWithTag("dropdown_vehicle_selector").performClick()
        composeTestRule.waitForIdle()
        composeTestRule.onNodeWithTag("vehicle_sheet_close_button").assertIsDisplayed()
        composeTestRule.onNodeWithTag("vehicle_menu_item_1").performClick()
        composeTestRule.waitForIdle()
        composeTestRule.onNodeWithText(vehicles[1].name).assertIsDisplayed()

        // 2. Navigate through remaining categories and open/select in each category sheet
        val otherCategories = listOf(
            VehicleCategoryType.BUS_CITY,
            VehicleCategoryType.BUS_COACH,
            VehicleCategoryType.BUS_MINIBUS,
            VehicleCategoryType.LORRY_ARTICULATED
        )
        for (category in otherCategories) {
            composeTestRule.onNodeWithTag("vehicle_filter_chip_${category.name}")
                .performScrollTo()
                .performClick()
            composeTestRule.waitForIdle()

            val categoryVehicles = vehicles.filter { it.category == category }
            val secondVariant = categoryVehicles[1]

            composeTestRule.onNodeWithTag("dropdown_vehicle_selector").performClick()
            composeTestRule.waitForIdle()
            composeTestRule.onNodeWithTag("vehicle_menu_item_${secondVariant.id}").performClick()
            composeTestRule.waitForIdle()
            composeTestRule.onNodeWithText(secondVariant.name).assertIsDisplayed()
        }

        // 3. With Articulated Tractor selected, verify fifth-wheel trailer & cargo sheets open and select
        val targetTrailer = compatibleTrailers[1]
        composeTestRule.onNodeWithTag("dropdown_hitched_trailer")
            .performScrollTo()
            .performClick()
        composeTestRule.waitForIdle()
        composeTestRule.onNodeWithTag("hitch_trailer_option_${targetTrailer.id}").performClick()
        composeTestRule.waitForIdle()
        composeTestRule.onNodeWithText(targetTrailer.name).assertIsDisplayed()

        composeTestRule.onNodeWithTag("switch_vehicle_trailer_cargo")
            .performScrollTo()
            .performClick()
        composeTestRule.waitForIdle()

        val targetCargo = cargoOptions[2]
        composeTestRule.onNodeWithTag("dropdown_vehicle_cargo")
            .performScrollTo()
            .performClick()
        composeTestRule.waitForIdle()
        composeTestRule.onNodeWithTag("hitch_cargo_option_${targetCargo.id}").performClick()
        composeTestRule.waitForIdle()
    }
}
