#pragma once

#include <cstdint>

namespace procedural::props {

struct CommunicationParams {
    float mastHeight = 36.0f;
    float mastBaseWidth = 3.6f;
    float dishDiameter = 2.8f;
    float dishElevationDeg = 48.0f;
    float dishAzimuthDeg = 135.0f;
    bool hasGuyWires = false;
};

struct RadioMastParams {
    float height = 32.0f;
    float baseWidth = 3.2f;
    float topWidth = 0.8f;
    int sectionCount = 5;
    int guyWireLevelCount = 2;          // 0 = self-supporting, >0 = guyed tower
    bool hasAviationLight = true;
};

struct SatelliteDishParams {
    float diameter = 2.4f;
    float focalDepth = 0.45f;
    float elevationDeg = 45.0f;
    float elevationAngleDeg = 45.0f;
    float azimuthDeg = 120.0f;
    int feedSupportLegs = 3;            // Tripod feedhorn support struts
    bool hasHeaterDeicer = false;
    bool hasDeicerHeater = false;
};

} // namespace procedural::props
