#pragma once

#include <cstdint>

namespace procedural::props {

struct SignageStreetParams {
    float boardWidth = 2.4f;
    float boardHeight = 1.6f;
    float mountingHeight = 2.2f;
    int postCount = 2;
    bool hasIlluminationHood = true;
    float markerDistanceKm = 42.0f;
};

struct InfoBoardParams {
    float boardWidth = 2.4f;
    float boardHeight = 1.6f;
    float postHeight = 2.4f;
    int postCount = 2;                  // 1 = cantilever unistrut, 2 = H-frame dual post
    int boardColor = 0;                 // 0: Highway Green, 1: Directional Blue, 2: Tourist Brown
    bool hasRoofCanopy = true;
};

struct KilometreMarkerParams {
    float height = 0.85f;
    float width = 0.35f;
    float depth = 0.25f;
    int markerStyle = 0;                // 0: Arched Stone Milestone, 1: Modern Plastic Reflector Delineator
    int distanceValue = 42;
    bool hasReflectorBadge = true;
};

} // namespace procedural::props
