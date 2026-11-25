#pragma once
// ============================================================================
// HeliosCanopyEngine.h - Native C++17 Plant Canopy Radiation & Microclimate Engine
// Based on PlantSimulationLab/Helios (UC Davis / Bailey et al.)
// https://github.com/PlantSimulationLab/Helios
//
// Features:
// - Multi-layer canopy radiation transport (Direct beam & diffuse skylight)
// - Turbid medium Beer-Lambert light extinction: I(z) = I_0 * exp(-k * LAI(z))
// - Campbell & Norman ellipsoidal Leaf Angle Distribution (LAD):
//   Spherical, Planophile, Erectophile, Plagiophile, Extremophile
// - G-function projection: G(theta_sun, chi)
// - Farquhar-von Caemmerer-Berry (FvCB) photosynthetic rate model (Ac, Aj, Rd)
// - Ball-Berry-Leuning stomatal conductance model (gs)
// - Canopy energy balance, sensible heat flux, and latent heat / transpiration
// ============================================================================

#include "../common/VegetationMath.h"
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

namespace vegetation {
namespace helios {

enum class LeafAngleDistribution : uint32_t {
    SPHERICAL = 0,    // chi = 1.0 (Uniform distribution across sphere)
    PLANOPHILE = 1,   // chi = 2.5 (Predominantly horizontal leaves, max overhead interception)
    ERECTOPHILE = 2,  // chi = 0.4 (Predominantly vertical leaves, desert/arid adaptation)
    PLAGIOPHILE = 3,  // chi = 1.6 (Oblique leaves, 45-degree inclination)
    EXTREMOPHILE = 4  // chi = 0.1 (Steep erect spines & succulent ribs)
};

struct HeliosRadiationParams {
    float directSolarFlux{850.0f};      // W/m^2 (Direct beam solar irradiance)
    float diffuseSolarFlux{150.0f};     // W/m^2 (Diffuse sky irradiance)
    float sunZenithAngleDeg{35.0f};     // Solar zenith angle theta_s
    float sunAzimuthAngleDeg{145.0f};   // Solar azimuth angle phi_s
    float leafReflectance{0.08f};       // Photosynthetically active radiation (PAR) reflectance
    float leafTransmittance{0.05f};     // PAR transmittance
    float leafAbsorptance{0.87f};       // PAR absorptance (1 - rho - tau)
    float airTemperatureC{28.5f};       // Ambient air temperature
    float relativeHumidityFraction{0.45f}; // Ambient relative humidity (0.0 - 1.0)
    float ambientCO2Ppm{420.0f};        // Atmospheric CO2 concentration (ppm)
};

struct HeliosCanopyLayer {
    float layerHeightBottomMeters{0.0f};
    float layerHeightTopMeters{1.0f};
    float layerLAI{0.5f};               // Incremental Leaf Area Index in this vertical layer
    float cumulativeLAI{0.5f};          // Cumulative LAI from canopy top down to this layer
    float absorbedPAR{400.0f};          // W/m^2 absorbed PAR flux
    float netPhotosynthesis{12.5f};     // micromol CO2 / m^2 / s
    float stomatalConductance{0.22f};   // mol H2O / m^2 / s
    float transpirationRate{3.2f};      // mmol H2O / m^2 / s
    float leafTemperatureC{29.2f};      // Surface leaf temperature
};

struct HeliosCanopyState {
    float totalLAI{3.2f};
    float canopyHeightMeters{1.8f};
    LeafAngleDistribution lad{LeafAngleDistribution::SPHERICAL};
    float extinctionCoefficientK{0.65f};
    float canopyAbsorbedFraction{0.82f};
    float totalCanopyPhotosynthesis{24.8f}; // micromol CO2 / m^2 ground / s
    float totalCanopyTranspiration{4.1f};   // mm H2O / hour
    std::vector<HeliosCanopyLayer> verticalLayers;
};

/**
 * HeliosCanopyEngine - Core C++17 Radiation & Ecophysiological Simulator
 */
class HeliosCanopyEngine {
public:
    HeliosCanopyEngine();
    ~HeliosCanopyEngine() = default;

    void setRadiationParams(const HeliosRadiationParams& params);
    const HeliosRadiationParams& getRadiationParams() const { return m_radParams; }

    // Evaluates G-function projection factor for given solar zenith and LAD chi parameter
    static float evaluateGFunction(float zenithRad, LeafAngleDistribution lad);

    // Evaluates Beer-Lambert extinction coefficient k = G(theta) / cos(theta)
    static float evaluateExtinctionK(float zenithRad, LeafAngleDistribution lad);

    // Simulates multi-layer canopy radiative transfer, FvCB photosynthesis, and stomatal conductance
    HeliosCanopyState simulateCanopy(
        float totalLAI,
        float canopyHeightMeters,
        LeafAngleDistribution lad,
        int layerSubdivisions = 10
    );

    // Farquhar-von Caemmerer-Berry (FvCB) C3/CAM photosynthesis model
    static float evaluatePhotosynthesisFvCB(
        float absorbedPAR,
        float leafTempC,
        float co2Ppm,
        float& outStomatalConductance
    );

private:
    HeliosRadiationParams m_radParams;
};

} // namespace helios
} // namespace vegetation
