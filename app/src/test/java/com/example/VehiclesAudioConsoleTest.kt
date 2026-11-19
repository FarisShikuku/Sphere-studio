package com.example

import androidx.compose.material3.MaterialTheme
import androidx.compose.ui.test.assertIsDisplayed
import androidx.compose.ui.test.junit4.createComposeRule
import androidx.compose.ui.test.onNodeWithTag
import androidx.compose.ui.test.performClick
import androidx.compose.ui.test.performScrollTo
import com.example.sphere.VehiclesStudioScreen
import com.example.sphere.model.createDefaultVehiclesCatalog
import com.example.sphere.vehicles.VehicleAudioController
import com.example.sphere.vehicles.computeFallbackGridCellX
import com.example.sphere.weather.WeatherAudioController
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner
import org.robolectric.annotation.Config

@RunWith(RobolectricTestRunner::class)
@Config(sdk = [36])
class VehiclesAudioConsoleTest {

    @get:Rule
    val composeTestRule = createComposeRule()

    @Test
    fun vehicleAudioControllerStateTransitionsAndGridCellsWork() {
        val controller = VehicleAudioController()
        val sampleTractor = createDefaultVehiclesCatalog().first { it.id == 5 }
        controller.configureVehicle(sampleTractor, combinedLoadKg = 14650f)

        assertFalse(controller.isEngineOn)
        assertEquals(0, controller.ignitionState)

        controller.toggleIgnition()
        assertTrue(controller.isEngineOn)

        controller.setThrottle(true)
        assertTrue(controller.throttlePressed)
        for (i in 0 until 15) {
            controller.stepJvmFallback(0.05f)
        }
        assertTrue("RPM should climb on gas pedal", controller.rpm > 800f)
        assertTrue("Speed should climb on gas pedal", controller.speedKmh > 5f)
        assertTrue("Gear should engage while driving", controller.gear >= 1)

        controller.setThrottle(false)
        controller.setBrake(true)
        assertTrue(controller.brakePressed)
        val speedBeforeBrake = controller.speedKmh
        controller.stepJvmFallback(0.25f)
        assertTrue("Braking should reduce speed", controller.speedKmh < speedBeforeBrake)
        controller.setBrake(false)

        controller.toggleCruise()
        assertTrue(controller.cruiseDrive)

        controller.toggleMute()
        assertTrue(controller.isMuted)

        controller.triggerHorn(isMinibus = false)
        controller.triggerDoorPneumatic()

        controller.setIgnition(false)
        assertFalse(controller.isEngineOn)
        assertFalse(controller.cruiseDrive)

        assertEquals(0, computeFallbackGridCellX(12f))
        assertEquals(2, computeFallbackGridCellX(130f))
    }

    @Test
    fun weatherAudioControllerTransitionsAndThunderWork() {
        val weatherController = WeatherAudioController()
        assertFalse(weatherController.isEnabled)

        weatherController.updateEnvironment(
            rainIntensity = 0.85f,
            snowIntensity = 0f,
            windSpeedKmh = 62f,
            windAzimuthDeg = 140f,
            fogDensity = 0.25f,
            stormActive = true
        )
        weatherController.toggleAudio()
        assertTrue(weatherController.isEnabled)

        weatherController.stepJvmFallback()
        assertTrue("Rain telemetry should reflect rainIntensity", weatherController.rainLevel > 0.8f)
        assertTrue("Wind telemetry should reflect windSpeedKmh", weatherController.windLevel > 0.6f)

        weatherController.triggerThunder(closeStrike = true)
        weatherController.stepJvmFallback()
        assertTrue("Thunder strike should elevate thunder telemetry", weatherController.thunderLevel > 0.5f)

        weatherController.toggleMute()
        assertTrue(weatherController.isMuted)

        weatherController.setAudioEnabled(false)
        assertFalse(weatherController.isEnabled)
    }

    @Test
    fun vehiclesScreenRendersDriveConsoleAndVulkanStreamingTabs() {
        composeTestRule.setContent {
            MaterialTheme {
                VehiclesStudioScreen(onNavigateBack = {})
            }
        }
        composeTestRule.waitForIdle()

        composeTestRule.onNodeWithTag("vehicle_drive_console_card")
            .performScrollTo()
            .assertIsDisplayed()
        composeTestRule.onNodeWithTag("vehicle_ignition_button").assertIsDisplayed()
        composeTestRule.onNodeWithTag("vehicle_horn_button").assertIsDisplayed()
        composeTestRule.onNodeWithTag("vehicle_cruise_button").assertIsDisplayed()
        composeTestRule.onNodeWithTag("vehicle_brake_pedal").assertIsDisplayed()
        composeTestRule.onNodeWithTag("vehicle_gas_pedal").assertIsDisplayed()
        composeTestRule.onNodeWithTag("vehicle_audio_mute_button").assertIsDisplayed()

        composeTestRule.onNodeWithTag("vehicles_tab_row").performScrollTo()
        composeTestRule.waitForIdle()
        composeTestRule.onNodeWithTag("vehicle_tab_3")
            .performScrollTo()
            .performClick()
        composeTestRule.waitForIdle()

        composeTestRule.onNodeWithTag("card_vehicle_spatial_meshlets")
            .performScrollTo()
            .assertIsDisplayed()
        composeTestRule.onNodeWithTag("card_vehicle_audio_compute")
            .performScrollTo()
            .assertIsDisplayed()
    }
}
