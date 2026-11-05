// atmosphere_common.glsl - Physically Based Atmospheric Scattering
// Referencing Gaukler/PlainRenderer and Bruneton/Preetham sky models

struct GpuAtmosphereData {
    vec4 sunDirectionAndZenith;    // xyz = sun direction, w = solar zenith angle
    vec4 sunColorAndIntensity;     // xyz = sunlight RGB after atmospheric transmission, w = illuminance in lux
    vec4 rayleighScattering;       // xyz = beta_R scattering coefficients (m^-1), w = scale height H_R (m)
    vec4 mieScattering;            // xyz = beta_M scattering coefficients (m^-1), w = scale height H_M (m)
    vec4 ozoneAbsorption;          // xyz = beta_O absorption coefficients (m^-1), w = mie asymmetry factor g
    vec4 groundAlbedoAndRadius;    // xyz = ground albedo RGB, w = planet radius / atmosphere max
    vec4 skyAmbientHemisphere;     // xyz = diffuse sky irradiance on ground, w = sun disk angular radius rad
};

// Rayleigh phase function: P_R(theta) = 3 / (16 * PI) * (1 + cos^2(theta))
float rayleighPhase(float cosTheta) {
    return (3.0 / (16.0 * 3.14159265)) * (1.0 + cosTheta * cosTheta);
}

// Cornette-Shanks phase function for Mie aerosol scattering
float cornetteShanksPhase(float cosTheta, float g) {
    float g2 = g * g;
    float num = 3.0 * (1.0 - g2) * (1.0 + cosTheta * cosTheta);
    float denom = (8.0 * 3.14159265) * (2.0 + g2) * pow(1.0 + g2 - 2.0 * g * cosTheta, 1.5);
    return num / max(denom, 1e-5);
}

// Optical air mass via Kasten-Young formula
float computeOpticalAirMass(float cosZenith) {
    float z = acos(clamp(cosZenith, 0.0, 1.0)) * 57.2957795; // In degrees
    return 1.0 / (cosZenith + 0.50572 * pow(max(96.07995 - z, 0.01), -1.6364));
}

// Transmittance of atmosphere along ray: T = exp(-tau)
vec3 computeAtmosphericTransmittance(GpuAtmosphereData atm, float cosZenith) {
    float m = computeOpticalAirMass(max(cosZenith, 0.01));
    vec3 betaR = atm.rayleighScattering.xyz * atm.rayleighScattering.w;
    vec3 betaM = atm.mieScattering.xyz * atm.mieScattering.w;
    vec3 betaO = atm.ozoneAbsorption.xyz * 4000.0;
    vec3 tau = (betaR + betaM + betaO) * m;
    return exp(-tau);
}

// Evaluates sky radiance in view direction
vec3 evaluateSkyRadiance(GpuAtmosphereData atm, vec3 viewDir) {
    vec3 sunDir = normalize(atm.sunDirectionAndZenith.xyz);
    float cosTheta = dot(viewDir, sunDir);
    float cosZenith = max(viewDir.y, 0.001);

    float pR = rayleighPhase(cosTheta);
    float pM = cornetteShanksPhase(cosTheta, atm.ozoneAbsorption.w);

    vec3 transView = computeAtmosphericTransmittance(atm, cosZenith);
    float sunCosZenith = max(sunDir.y, 0.01);
    vec3 transSun = computeAtmosphericTransmittance(atm, sunCosZenith);

    // Single-scattered in-scattered light
    vec3 rayleighScat = atm.rayleighScattering.xyz * pR;
    vec3 mieScat = atm.mieScattering.xyz * pM;
    vec3 inScattered = (rayleighScat + mieScat) * transSun * (1.0 - transView) * (atm.sunColorAndIntensity.w * 0.0001);

    // Solar disk rendering
    float sunAngularRadius = atm.skyAmbientHemisphere.w;
    float sunAngle = acos(clamp(cosTheta, -1.0, 1.0));
    if (sunAngle < sunAngularRadius) {
        float limbDarkening = clamp(1.0 - pow(sunAngle / sunAngularRadius, 2.0), 0.0, 1.0);
        vec3 sunLuminance = transSun * (atm.sunColorAndIntensity.w * 0.02) * (0.4 + 0.6 * limbDarkening);
        inScattered += sunLuminance;
    }

    return inScattered;
}
