#pragma once

#include <cstdint>

namespace procedural::props {

struct UtilityParams {
    float poleSpacing = 35.0f;          // Distance between utility poles in meters
    float wireSag = 0.85f;              // Catenary sag depth
    float poleHeight = 10.5f;           // Standard utility post height
    float crossarmWidth = 2.4f;         // Width of insulator crossarm
    int numCircuits = 3;                // 3-phase AC distribution
    bool hasTransformer = true;         // Pole-mounted transformer drum
    float wireTension = 1200.0f;        // Cable tension in Newtons
};

struct ElectricPostParams {
    float height = 10.5f;
    float baseRadius = 0.18f;
    float topRadius = 0.12f;
    int materialType = 0;               // 0: Creosote Wood, 1: Spun Concrete, 2: Galvanized Steel
    int crossarmCount = 2;              // 1 to 3 crossarms
    float crossarmWidth = 2.2f;
    bool hasInsulators = true;
    bool hasTransformer = true;
    float spanLength = 20.0f;           // Distance between poles for power lines
    float wireSag = 0.85f;              // Catenary sag depth
    int poleCount = 2;                  // Number of power posts connected by lines (1 to 3)
    bool hasPowerLines = true;          // Enable continuous bent cylinder catenary power lines
};

struct TransformerParams {
    float height = 1.1f;
    float radius = 0.35f;
    int mountType = 0;                  // 0: Pole-Mounted Can, 1: Pad-Mounted Green Enclosure
    int kVA = 50;                       // Power rating
    int finCount = 12;                  // Cooling fin ribs
    bool hasBushingInsulators = true;
};

struct PowerMeterParams {
    float height = 0.45f;
    float width = 0.30f;
    float depth = 0.20f;
    int meterType = 0;                  // 0: Smart Digital, 1: Analog Dial
    bool hasConduit = true;
};

struct StreetLightParams {
    float height = 8.0f;
    float armReach = 2.2f;
    float armCurveRadius = 1.2f;
    int fixtureStyle = 0;               // 0: Cobra-head LED, 1: High-Pressure Sodium, 2: Teardrop
    bool hasPhotocell = true;
};

struct WaterTowerParams {
    float totalHeight = 32.0f;
    float tankDiameter = 12.0f;
    float tankHeight = 8.0f;
    int legCount = 6;                   // 4, 6, or 8 structural lattice legs
    int style = 0;                      // 0: Multi-leg Steel, 1: Spheroid Hydro-pillar
    bool hasAccessLadder = true;
};

struct TelecomTowerParams {
    float height = 45.0f;
    float baseWidth = 5.0f;
    float topWidth = 1.2f;
    int sectionCount = 6;
    int antennaTierCount = 3;           // Microwave drums + cellular panels
    bool hasAviationBeacon = true;
};

} // namespace procedural::props
