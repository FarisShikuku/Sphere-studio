#include "CloudGen.h"
#include <cmath>
#include <algorithm>

namespace sphere::weather {

float CloudGen::SampleCloudDensity(const Vec3& p, float time) const {
    if (p.y < m_params.baseAltitude || p.y > m_params.topAltitude) {
        return 0.0f;
    }
    float heightFraction = (p.y - m_params.baseAltitude) / (m_params.topAltitude - m_params.baseAltitude);
    float heightShape = std::sin(heightFraction * 3.14159f);

    float nx = std::sin(p.x * 0.001f + time * 0.02f);
    float nz = std::cos(p.z * 0.001f + time * 0.015f);
    float noise = (nx * nz * 0.5f + 0.5f);

    float density = (noise - (1.0f - m_params.coverage)) * heightShape * m_params.density;
    return std::max(0.0f, density);
}

} // namespace sphere::weather
