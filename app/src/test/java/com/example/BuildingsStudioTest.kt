package com.example

import androidx.compose.material3.MaterialTheme
import androidx.compose.ui.test.assertIsDisplayed
import androidx.compose.ui.test.junit4.createComposeRule
import androidx.compose.ui.test.onNodeWithTag
import androidx.compose.ui.test.onNodeWithText
import androidx.compose.ui.test.performClick
import androidx.compose.ui.test.performScrollTo
import com.example.sphere.BuildingsStudioScreen
import com.example.sphere.model.BuildingBalconyType
import com.example.sphere.model.BuildingCornerType
import com.example.sphere.model.BuildingDoorType
import com.example.sphere.model.BuildingGlassType
import com.example.sphere.model.BuildingRoofType
import com.example.sphere.model.BuildingStepMode
import com.example.sphere.model.BuildingStudioCategory
import com.example.sphere.model.BuildingVolumeShape
import com.example.sphere.model.BuildingWallMaterial
import com.example.sphere.model.BuildingWindowType
import com.example.sphere.model.createBuildingsCatalog
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner
import org.robolectric.annotation.Config

@RunWith(RobolectricTestRunner::class)
@Config(sdk = [36])
class BuildingsStudioTest {

    @get:Rule
    val composeTestRule = createComposeRule()

    @Test
    fun buildingEnumsAndCatalogMatchNativeCppRegistryCounts() {
        assertEquals(27, BuildingVolumeShape.entries.size)
        assertEquals(7, BuildingCornerType.entries.size)
        assertEquals(5, BuildingStepMode.entries.size)
        assertEquals(23, BuildingWindowType.entries.size)
        assertEquals(9, BuildingGlassType.entries.size)
        assertEquals(16, BuildingWallMaterial.entries.size)
        assertEquals(10, BuildingDoorType.entries.size)
        assertEquals(22, BuildingRoofType.entries.size)
        assertEquals(4, BuildingBalconyType.entries.size)

        val catalog = createBuildingsCatalog()
        assertEquals(27, catalog.count { it.category == BuildingStudioCategory.MASSING })
        assertEquals(53, catalog.count { it.category == BuildingStudioCategory.RESIDENTIAL })
        assertEquals(36, catalog.count { it.category == BuildingStudioCategory.COMMERCIAL })
        assertEquals(14, catalog.count { it.category == BuildingStudioCategory.INDUSTRIAL })
        assertEquals(20, catalog.count { it.category == BuildingStudioCategory.CIVIC })
        assertEquals(5, catalog.count { it.category == BuildingStudioCategory.INTERIORS })
        assertEquals(3, catalog.count { it.category == BuildingStudioCategory.PROPS })
        assertEquals(3, catalog.count { it.category == BuildingStudioCategory.SIGNAGE })
        assertEquals(3, catalog.count { it.category == BuildingStudioCategory.DAMAGE })
        assertEquals(3, catalog.count { it.category == BuildingStudioCategory.LOD })

        // Ensure all 12 native C++ generator domains (0..11) are represented
        val domains = catalog.map { it.domainOrdinal }.toSet()
        for (domain in 0..11) {
            assertTrue("Missing domainOrdinal=$domain in catalog", domain in domains)
        }
    }

    @Test
    fun buildingsStudioNavigatesDomainsBottomSheetAndTabs() {
        val catalog = createBuildingsCatalog()

        composeTestRule.setContent {
            MaterialTheme {
                BuildingsStudioScreen(onNavigateBack = {})
            }
        }
        composeTestRule.waitForIdle()

        // 1. Verify viewport and default massing preset
        composeTestRule.onNodeWithTag("buildings_3d_viewport").assertIsDisplayed()
        composeTestRule.onNodeWithTag("building_toggle_wireframe_button").performClick()
        composeTestRule.onNodeWithTag("building_toggle_cutaway_button").performClick()
        composeTestRule.waitForIdle()

        // 2. Switch across categories and pick variants from the ModalBottomSheet
        val targetCategories = listOf(
            BuildingStudioCategory.RESIDENTIAL,
            BuildingStudioCategory.COMMERCIAL,
            BuildingStudioCategory.INDUSTRIAL,
            BuildingStudioCategory.CIVIC,
            BuildingStudioCategory.INTERIORS
        )
        for (category in targetCategories) {
            composeTestRule.onNodeWithTag("building_category_chip_${category.name}")
                .performScrollTo()
                .performClick()
            composeTestRule.waitForIdle()

            val variants = catalog.filter { it.category == category }
            val pick = variants[1]
            composeTestRule.onNodeWithTag("dropdown_building_selector").performClick()
            composeTestRule.waitForIdle()
            composeTestRule.onNodeWithTag("building_menu_item_${pick.id}").performClick()
            composeTestRule.waitForIdle()
            composeTestRule.onNodeWithText(pick.name).assertIsDisplayed()
        }

        // 3. Scroll vertically to tab row, then switch across all 4 control tabs
        composeTestRule.onNodeWithTag("buildings_tab_row").performScrollTo()
        composeTestRule.waitForIdle()
        for (tabIndex in 0..3) {
            composeTestRule.onNodeWithTag("building_tab_$tabIndex")
                .performScrollTo()
                .performClick()
            composeTestRule.waitForIdle()
        }
        composeTestRule.onNodeWithTag("building_native_diagnostics_card")
            .performScrollTo()
            .assertIsDisplayed()
    }
}
