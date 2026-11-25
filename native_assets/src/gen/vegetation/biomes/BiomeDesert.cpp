// ============================================================================
// BiomeDesert.cpp - Arid Xerophyte Flora Density & Distribution
// ============================================================================

#include "BiomeRules.h"

namespace vegetation {
// Arid domain xerophyte distribution weighting
float getDesertCactusDensity() { return 0.08f; }
float getDesertSagebrushDensity() { return 0.35f; }
float getDesertRockBoulderFrequency() { return 0.85f; }
} // namespace vegetation
