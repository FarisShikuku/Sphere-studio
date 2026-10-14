#include "Mesh.h"

namespace sphere {
void Mesh::CalculateBounds() {
    if (vertices.empty()) return;
    bounds.min = vertices[0].position;
    bounds.max = vertices[0].position;
    for (const auto& v : vertices) {
        bounds.min.x = std::min(bounds.min.x, v.position.x);
        bounds.min.y = std::min(bounds.min.y, v.position.y);
        bounds.min.z = std::min(bounds.min.z, v.position.z);
        bounds.max.x = std::max(bounds.max.x, v.position.x);
        bounds.max.y = std::max(bounds.max.y, v.position.y);
        bounds.max.z = std::max(bounds.max.z, v.position.z);
    }
}
}\n