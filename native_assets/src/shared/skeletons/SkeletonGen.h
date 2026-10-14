#pragma once
#include "scene/Transform.h"
#include <string>
#include <vector>
#include <array>
#include <cstdint>

namespace sphere {

struct Bone {
    std::string name;
    int32_t parentIndex = -1;
    Transform localTransform;
    Mat4 inverseBindMatrix;
};

struct VertexSkinWeight {
    std::array<uint32_t, 4> boneIndices{0, 0, 0, 0};
    std::array<float, 4> weights{1.0f, 0.0f, 0.0f, 0.0f};
};

class SkeletonGen {
public:
    void AddBone(const std::string& name, int32_t parentIndex, const Transform& localTransform);
    int32_t FindBoneIndex(const std::string& name) const;
    void ComputeBindPose();
    void UpdateGlobalTransforms(std::vector<Mat4>& outTransforms) const;

    const std::vector<Bone>& GetBones() const { return m_Bones; }
    std::vector<Bone>& GetBones() { return m_Bones; }

    static Mat4 Multiply(const Mat4& a, const Mat4& b);
    static Mat4 Inverse(const Mat4& m);

    // Procedural smooth distance skinning generator (distance-decay bone heat)
    static std::vector<VertexSkinWeight> CalculateSmoothWeights(
        const std::vector<Vec3>& vertices,
        const std::vector<Bone>& bones,
        const std::vector<Mat4>& globalTransforms,
        float falloffPower = 2.0f
    );

private:
    std::vector<Bone> m_Bones;
};

} // namespace sphere
