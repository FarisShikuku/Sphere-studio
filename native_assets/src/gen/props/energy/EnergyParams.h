#pragma once

#include <cstdint>

namespace procedural::props {

struct EnergyParams {
    float hubHeight = 85.0f;
    float rotorDiameter = 90.0f;
    float bladePitchDeg = 4.0f;
    float panelTiltDeg = 28.0f;
    float farmRowSpacing = 6.0f;
    int panelsPerRow = 12;
    int rows = 6;
    bool enableSolarTracking = true;
};

struct WindTurbineParams {
    float hubHeight = 65.0f;
    float rotorDiameter = 54.0f;
    float towerBaseRadius = 2.4f;
    float towerTopRadius = 1.3f;
    int bladeCount = 3;
    float rotationSpeedRpm = 14.0f;
    float yawAngleDeg = 25.0f;
    float bladePitchDeg = 4.0f;
    bool hasAviationBeacon = true;
};

struct SolarPanelParams {
    float length = 2.0f;
    float width = 1.0f;
    float frameThickness = 0.04f;
    float tiltAngleDeg = 30.0f;
    int cellGridRows = 10;
    int cellGridCols = 6;
    bool hasDualAxisTracker = false;
    bool isSunTracking = false;
};

struct SolarFarmParams {
    int rows = 4;
    int panelsPerRow = 8;
    float rowPitch = 4.5f;
    float tiltAngleDeg = 28.0f;
    float groundClearance = 0.8f;
    bool hasInverterStation = true;
};

struct BatteryBankParams {
    float containerLength = 12.2f;      // 40ft ISO shipping container form factor
    float containerWidth = 2.44f;
    float containerHeight = 2.90f;
    int unitCount = 2;                  // Parallel battery cabinets
    int hvacUnitCount = 2;              // Roof-mounted HVAC thermal management
    bool hasFireSuppressionPiping = true;
};

struct SubstationParams {
    float yardLength = 24.0f;
    float yardWidth = 18.0f;
    int mainTransformerCount = 2;
    float gantryHeight = 8.5f;
    int busbarCount = 3;
    bool hasSecurityFence = true;
};

} // namespace procedural::props
