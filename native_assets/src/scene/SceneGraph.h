#pragma once
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "AABB.h"
#include <vector>
#include <memory>
#include <string>
#include <functional>
namespace sphere {
class SceneGraphNode : public std::enable_shared_from_this<SceneGraphNode> {
public:
    std::string name = "SceneNode";
    Transform transform;
    Mat4 worldMatrix = Mat4::Identity();
    bool isDirty = true;
    bool isVisible = true;
    std::shared_ptr<Mesh> mesh = nullptr;
    std::shared_ptr<Material> material = nullptr;
    AABB worldBounds;
    std::weak_ptr<SceneGraphNode> parent;
    std::vector<std::shared_ptr<SceneGraphNode>> children;
    SceneGraphNode(const std::string& nodeName = "SceneNode") : name(nodeName) {}
    void AddChild(std::shared_ptr<SceneGraphNode> child) {
        child->parent = shared_from_this();
        child->SetDirty();
        children.push_back(child);
    }
    void RemoveChild(const std::string& childName) {
        children.erase(
            std::remove_if(children.begin(), children.end(),
                [&childName](const std::shared_ptr<SceneGraphNode>& node) {
                    return node->name == childName;
                }),
            children.end());
    }
    void SetDirty() {
        isDirty = true;
        for (auto& child : children) {
            child->SetDirty();
        }
    }
    void UpdateTransforms(const Mat4& parentMatrix = Mat4::Identity()) {
        Mat4 localMat = transform.GetMatrix();
        worldMatrix = Mat4::Multiply(parentMatrix, localMat);
        isDirty = false;
        if (mesh) {
            // Update world bounds
            worldBounds.min = worldMatrix.TransformPoint(mesh->bounds.min);
            worldBounds.max = worldMatrix.TransformPoint(mesh->bounds.max);
        }
        for (auto& child : children) {
            child->UpdateTransforms(worldMatrix);
        }
    }
    void Traverse(const std::function<void(SceneGraphNode&)>& visitor) {
        if (!isVisible) return;
        visitor(*this);
        for (auto& child : children) {
            child->Traverse(visitor);
        }
    }
};
class SceneGraph {
public:
    SceneGraph();
    ~SceneGraph() = default;
    std::shared_ptr<SceneGraphNode> GetRoot() { return m_Root; }
    void Update();
    void Clear();
    std::shared_ptr<SceneGraphNode> FindNode(const std::string& name);
    void ForEachVisibleNode(const std::function<void(SceneGraphNode&)>& callback);
    size_t GetTotalNodeCount() const;
private:
    std::shared_ptr<SceneGraphNode> m_Root;
};
} // namespace spher
