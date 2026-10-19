#include "LightningGen.h"
#include <cstdlib>

namespace sphere::weather {

std::vector<LightningSegment> LightningGen::GenerateBolt(const Vec3& start, const Vec3& target) {
    std::vector<LightningSegment> segments;
    Vec3 cur = start;
    int steps = 16;
    float stepY = (target.y - start.y) / static_cast<float>(steps);

    for (int i = 0; i < steps; ++i) {
        float jx = static_cast<float>(rand() % 200 - 100) / 100.0f * m_params.jitterAmount;
        float jz = static_cast<float>(rand() % 200 - 100) / 100.0f * m_params.jitterAmount;
        Vec3 next{cur.x + jx, cur.y + stepY, cur.z + jz};
        if (i == steps - 1) next = target;

        segments.push_back({cur, next, m_params.strikeIntensity});
        cur = next;
    }
    return segments;
}

} // namespace sphere::weather
