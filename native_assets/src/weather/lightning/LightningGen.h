#pragma once
#include <vector>
#include "LightningParams.h"
#include "scene/Transform.h"

namespace sphere::weather {

struct LightningSegment {
    Vec3 start;
    Vec3 end;
    float intensity;
};

class LightningGen {
public:
    LightningGen() = default;
    void SetParams(const LightningParams& params) { m_params = params; }
    const LightningParams& GetParams() const { return m_params; }

    std::vector<LightningSegment> GenerateBolt(const Vec3& start, const Vec3& target);

private:
    LightningParams m_params{};
};

} // namespace sphere::weather
