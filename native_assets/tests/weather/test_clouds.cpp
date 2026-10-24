#include <cassert>
#include <iostream>
#include "weather/clouds/CloudGen.h"
#include "weather/clouds/CloudRaymarch.h"

int main() {
    sphere::weather::CloudGen cloudGen;
    sphere::weather::CloudParams params;
    params.coverage = 0.60f;
    params.type = sphere::weather::CloudType::Cumulus;
    cloudGen.SetParams(params);

    // 1. Verify height fraction in atmosphere shell
    sphere::Vec3 cloudBasePos{0.0f, 5000.0f, 0.0f};
    float hFracBase = cloudGen.GetHeightFraction(cloudBasePos);
    assert(hFracBase >= 0.0f && hFracBase <= 0.05f);

    sphere::Vec3 cloudMidPos{0.0f, 13500.0f, 0.0f};
    float hFracMid = cloudGen.GetHeightFraction(cloudMidPos);
    assert(hFracMid >= 0.45f && hFracMid <= 0.55f);

    // 2. Verify Nubis cloud height gradients
    float stratusDensity = cloudGen.GetDensityForCloud(0.15f, sphere::weather::CloudType::Stratus);
    float cumulusDensity = cloudGen.GetDensityForCloud(0.50f, sphere::weather::CloudType::Cumulus);
    assert(stratusDensity > 0.0f);
    assert(cumulusDensity > 0.0f);

    // 3. Verify optical radiative transfer (Beer-Lambert & Powder sugar effect)
    float beerVal = cloudGen.Beer(1.0f);
    float powderVal = cloudGen.Powder(1.0f);
    assert(beerVal < 1.0f && beerVal > 0.0f);
    assert(powderVal > 0.0f && powderVal <= 1.0f);

    // 4. Verify Henyey-Greenstein silver lining peak (forward scattering > backward scattering)
    float forwardScattering = cloudGen.HenyeyGreenstein(0.95f, 0.8f);
    float sideScattering = cloudGen.HenyeyGreenstein(0.0f, 0.8f);
    assert(forwardScattering > sideScattering);

    // 5. Verify volumetric raymarching execution
    sphere::weather::CloudRaymarch raymarch;
    auto result = raymarch.March(
        sphere::Vec3{0, 100, 0},
        sphere::Vec3{0, 0.6f, 0.8f},
        sphere::Vec3{0.5f, 0.5f, 0.0f},
        sphere::Vec3{1.0f, 0.95f, 0.9f},
        cloudGen,
        0.0f
    );

    std::cout << "[PASS] test_clouds Beckebans Nubis raymarch validated. Alpha: " 
              << result.cloudAlpha << ", Transmittance: " << result.transmittance << std::endl;
    return 0;
}
