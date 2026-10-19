#pragma once

namespace sphere::weather {

struct LightningParams {
    float strikeIntensity = 1.0f;
    float branchProbability = 0.35f;
    float jitterAmount = 1.8f;
    float duration = 0.22f; // seconds
};

} // namespace sphere::weather
