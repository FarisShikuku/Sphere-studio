// ============================================================================
// HeliosCanopyEngine.cpp - Native C++17 Plant Canopy Radiation & Microclimate Engine
// Based on PlantSimulationLab/Helios (UC Davis / Bailey et al.)
// https://github.com/PlantSimulationLab/Helios
// ============================================================================

#include "HeliosCanopyEngine.h"
#include <cmath>
#include <algorithm>

namespace vegetation {
namespace helios {

static constexpr float PI_F = 3.14159265358979323846f;
static constexpr float DEG_TO_RAD = PI_F / 180.0f;

HeliosCanopyEngine::HeliosCanopyEngine() {
    m_radParams = HeliosRadiationParams();
}

void HeliosCanopyEngine::setRadiationParams(const HeliosRadiationParams& params) {
    m_radParams = params;
}

float HeliosCanopyEngine::evaluateGFunction(float zenithRad, LeafAngleDistribution lad) {
    float theta = std::clamp(zenithRad, 0.01f, 1.55f);
    float cosTheta = std::cos(theta);
    float sinTheta = std::sin(theta);

    // Campbell (1986) ellipsoidal leaf angle distribution parameter chi (ratio of horizontal to vertical semi-axes)
    float chi = 1.0f;
    switch (lad) {
        case LeafAngleDistribution::SPHERICAL:    chi = 1.0f; break;
        case LeafAngleDistribution::PLANOPHILE:   chi = 2.5f; break;
        case LeafAngleDistribution::ERECTOPHILE:  chi = 0.4f; break;
        case LeafAngleDistribution::PLAGIOPHILE:  chi = 1.6f; break;
        case LeafAngleDistribution::EXTREMOPHILE: chi = 0.15f; break;
    }

    if (std::abs(chi - 1.0f) < 0.05f) {
        // Spherical distribution has constant projection G = 0.5
        return 0.5f;
    }

    // Ellipsoidal G approximation: G = sqrt(chi^2 + tan(theta)^2) / (chi + 1.774 * (chi + 1.182)^(-0.733)) * cos(theta)
    float tanTheta = sinTheta / cosTheta;
    float numerator = std::sqrt(chi * chi + tanTheta * tanTheta);
    float denominator = chi + 1.774f * std::pow(chi + 1.182f, -0.733f);
    float g = (numerator / denominator) * cosTheta;
    return std::clamp(g, 0.1f, 1.0f);
}

float HeliosCanopyEngine::evaluateExtinctionK(float zenithRad, LeafAngleDistribution lad) {
    float theta = std::clamp(zenithRad, 0.01f, 1.55f);
    float cosTheta = std::max(std::cos(theta), 0.05f);
    float g = evaluateGFunction(zenithRad, lad);
    return std::clamp(g / cosTheta, 0.2f, 3.5f);
}

float HeliosCanopyEngine::evaluatePhotosynthesisFvCB(
    float absorbedPAR,
    float leafTempC,
    float co2Ppm,
    float& outStomatalConductance
) {
    // Farquhar, von Caemmerer and Berry (1980) C3 photosynthesis model
    // 1. Rubisco carboxylation capacity Vcmax adjusted for temperature
    float tk = leafTempC + 273.15f;
    float vcmax25 = 95.0f; // micromol / m^2 / s
    float jmax25 = 165.0f; // micromol / m^2 / s
    float rd25 = 1.2f;    // Dark respiration

    // Arrhenius temperature activation
    float q10 = 2.0f;
    float tempFactor = std::pow(q10, (leafTempC - 25.0f) / 10.0f);
    float vcmax = vcmax25 * tempFactor;
    float jmax = jmax25 * tempFactor;
    float rd = rd25 * tempFactor;

    // Michaelis-Menten constants for CO2 (Kc) and O2 (Ko)
    float kc = 404.9f;
    float ko = 278.4f;
    float o2Conc = 210.0f; // mmol/mol
    float km = kc * (1.0f + o2Conc / ko);

    // CO2 compensation point Gamma*
    float gammaStar = 42.75f * tempFactor;
    float ci = co2Ppm * 0.72f; // Internal substomatal CO2 (approx 72% ambient)

    // Rubisco-limited carboxylation Ac
    float ac = vcmax * (ci - gammaStar) / (ci + km);

    // Electron transport rate J driven by absorbed PAR
    float alpha = 0.85f;
    float thetaJ = 0.90f;
    float parMicro = absorbedPAR * 4.6f; // W/m^2 to micromol photons / m^2 / s
    float i2 = parMicro * (1.0f - 0.15f) / 2.0f;
    float term = i2 + jmax;
    float j = (term - std::sqrt(std::max(0.0f, term * term - 4.0f * thetaJ * i2 * jmax))) / (2.0f * thetaJ);

    // RuBP-regeneration-limited rate Aj
    float aj = j * (ci - gammaStar) / (4.0f * (ci + 2.0f * gammaStar));

    // Net assimilation An = min(Ac, Aj) - Rd
    float an = std::max(0.0f, std::min(ac, aj) - rd);

    // Ball-Berry stomatal conductance: gs = g0 + g1 * (An * hs / Cs)
    float g0 = 0.02f; // mol / m^2 / s (minimum residual conductance)
    float g1 = 9.5f;  // slope coefficient
    float hs = 0.65f; // relative humidity at leaf surface
    outStomatalConductance = g0 + g1 * (an * hs / std::max(co2Ppm, 100.0f));
    outStomatalConductance = std::clamp(outStomatalConductance, 0.02f, 0.65f);

    return an;
}

HeliosCanopyState HeliosCanopyEngine::simulateCanopy(
    float totalLAI,
    float canopyHeightMeters,
    LeafAngleDistribution lad,
    int layerSubdivisions
) {
    HeliosCanopyState state;
    state.totalLAI = std::max(totalLAI, 0.1f);
    state.canopyHeightMeters = std::max(canopyHeightMeters, 0.2f);
    state.lad = lad;

    int numLayers = std::clamp(layerSubdivisions, 4, 32);
    float dLAI = state.totalLAI / numLayers;
    float dZ = state.canopyHeightMeters / numLayers;

    float zenithRad = m_radParams.sunZenithAngleDeg * DEG_TO_RAD;
    float kExt = evaluateExtinctionK(zenithRad, lad);
    state.extinctionCoefficientK = kExt;

    float cumLAI = 0.0f;
    float totalAn = 0.0f;
    float totalTrans = 0.0f;
    float topPAR = m_radParams.directSolarFlux + m_radParams.diffuseSolarFlux;

    state.verticalLayers.resize(numLayers);

    for (int i = 0; i < numLayers; ++i) {
        // Layer ordered from canopy top down to ground
        int layerIdx = numLayers - 1 - i;
        float zTop = (layerIdx + 1) * dZ;
        float zBot = layerIdx * dZ;

        cumLAI += dLAI;

        // Beer-Lambert light profile: I(cumLAI) = I_0 * exp(-k * cumLAI)
        float fluxAtTop = topPAR * std::exp(-kExt * (cumLAI - dLAI));
        float fluxAtBot = topPAR * std::exp(-kExt * cumLAI);
        float absorbedFlux = (fluxAtTop - fluxAtBot) * m_radParams.leafAbsorptance;

        float gs = 0.0f;
        float an = evaluatePhotosynthesisFvCB(
            absorbedFlux / std::max(dLAI, 0.01f),
            m_radParams.airTemperatureC + 0.8f * (1.0f - static_cast<float>(i) / numLayers),
            m_radParams.ambientCO2Ppm,
            gs
        );

        // Transpiration flux E = gs * VPD (vapor pressure deficit)
        float vpdKPa = 1.45f * (1.0f - m_radParams.relativeHumidityFraction);
        float transpiration = gs * vpdKPa * 18.0f * 3600.0f * 1e-3f; // mm / h

        HeliosCanopyLayer& layer = state.verticalLayers[layerIdx];
        layer.layerHeightBottomMeters = zBot;
        layer.layerHeightTopMeters = zTop;
        layer.layerLAI = dLAI;
        layer.cumulativeLAI = cumLAI;
        layer.absorbedPAR = absorbedFlux;
        layer.netPhotosynthesis = an;
        layer.stomatalConductance = gs;
        layer.transpirationRate = transpiration;
        layer.leafTemperatureC = m_radParams.airTemperatureC + 0.5f;

        totalAn += an * dLAI;
        totalTrans += transpiration * dLAI;
    }

    state.canopyAbsorbedFraction = 1.0f - std::exp(-kExt * state.totalLAI);
    state.totalCanopyPhotosynthesis = totalAn;
    state.totalCanopyTranspiration = totalTrans;

    return state;
}

} // namespace helios
} // namespace vegetation
