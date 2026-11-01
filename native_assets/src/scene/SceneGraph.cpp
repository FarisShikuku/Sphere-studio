#include "SceneGraph.h"
#include <queue>

namespace sphere {

SceneGraph::SceneGraph() {
    m_Root = std::make_shared<SceneGraphNode>("RootNode");
}

void SceneGraph::Update() {
    if (m_Root) {
        m_Root->UpdateTransforms(Mat4::Identity());
    }
}

void SceneGraph::Clear() {
    if (m_Root) {
        m_Root->children.clear();
    }
}

std::shared_ptr<SceneGraphNode> SceneGraph::FindNode(const std::string& name) {
    if (!m_Root) return nullptr;

    std::queue<std::shared_ptr<SceneGraphNode>> q;
    q.push(m_Root);

    while (!q.empty()) {
        auto cur = q.front();
        q.pop();

        if (cur->name == name) {
            return cur;
        }

        for (const auto& child : cur->children) {
            q.push(child);
        }
    }
    return nullptr;
}

void SceneGraph::ForEachVisibleNode(const std::function<void(SceneGraphNode&)>& callback) {
    if (m_Root) {
        m_Root->Traverse(callback);
    }
}

size_t SceneGraph::GetTotalNodeCount() const {
    if (!m_Root) return 0;
    size_t count = 0;
    std::queue<std::shared_ptr<SceneGraphNode>> q;
    q.push(m_Root);

    while (!q.empty()) {
        auto cur = q.front();
        q.pop();
        count++;

        for (const auto& child : cur->children) {
            q.push(child);
        }
    }
    return count;
}

} // namespace sphere
