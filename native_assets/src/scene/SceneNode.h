#pragma once
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include <vector>
#include <memory>
#include <string>

namespace sphere {
class SceneNode {
public:
    std::string name;
    Transform transform;
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
    std::vector<std::shared_ptr<SceneNode>> children;

    void AddChild(std::shared_ptr<SceneNode> child) { children.push_back(child); }
};
}\n