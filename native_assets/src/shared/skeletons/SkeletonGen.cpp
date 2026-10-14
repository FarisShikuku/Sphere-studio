#include "SkeletonGen.h"
#include <cmath>

namespace sphere {

void SkeletonGen::AddBone(const std::string& name, int32_t parentIndex, const Transform& localTransform) {
    Bone bone;
    bone.name = name;
    bone.parentIndex = parentIndex;
    bone.localTransform = localTransform;
    m_Bones.push_back(bone);
}

int32_t SkeletonGen::FindBoneIndex(const std::string& name) const {
    for (size_t i = 0; i < m_Bones.size(); ++i) {
        if (m_Bones[i].name == name) {
            return static_cast<int32_t>(i);
        }
    }
    return -1;
}

Mat4 SkeletonGen::Multiply(const Mat4& a, const Mat4& b) {
    Mat4 res{};
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k) {
                sum += a.m[k * 4 + row] * b.m[col * 4 + k];
            }
            res.m[col * 4 + row] = sum;
        }
    }
    return res;
}

Mat4 SkeletonGen::Inverse(const Mat4& m) {
    Mat4 inv{};
    const float* a = m.m.data();

    float s0 = a[0] * a[5] - a[4] * a[1];
    float s1 = a[0] * a[6] - a[4] * a[2];
    float s2 = a[0] * a[7] - a[4] * a[3];
    float s3 = a[1] * a[6] - a[5] * a[2];
    float s4 = a[1] * a[7] - a[5] * a[3];
    float s5 = a[2] * a[7] - a[6] * a[3];

    float c5 = a[10] * a[15] - a[14] * a[11];
    float c4 = a[9] * a[15] - a[13] * a[11];
    float c3 = a[9] * a[14] - a[13] * a[10];
    float c2 = a[8] * a[15] - a[12] * a[11];
    float c1 = a[8] * a[14] - a[12] * a[10];
    float c0 = a[8] * a[13] - a[12] * a[9];

    float det = s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
    if (std::abs(det) < 1e-8f) {
        return Mat4{}; // Identity on degenerate
    }
    float invDet = 1.0f / det;

    inv.m[0] = ( a[5] * c5 - a[6] * c4 + a[7] * c3) * invDet;
    inv.m[1] = (-a[1] * c5 + a[2] * c4 - a[3] * c3) * invDet;
    inv.m[2] = ( a[13] * s5 - a[14] * s4 + a[15] * s3) * invDet;
    inv.m[3] = (-a[9] * s5 + a[10] * s4 - a[11] * s3) * invDet;

    inv.m[4] = (-a[4] * c5 + a[6] * c2 - a[7] * c1) * invDet;
    inv.m[5] = ( a[0] * c5 - a[2] * c2 + a[3] * c1) * invDet;
    inv.m[6] = (-a[12] * s5 + a[14] * s2 - a[15] * s1) * invDet;
    inv.m[7] = ( a[8] * s5 - a[10] * s2 + a[11] * s1) * invDet;

    inv.m[8] = ( a[4] * c4 - a[5] * c2 + a[7] * c0) * invDet;
    inv.m[9] = (-a[0] * c4 + a[1] * c2 - a[3] * c0) * invDet;
    inv.m[10] = ( a[12] * s4 - a[13] * s2 + a[15] * s0) * invDet;
    inv.m[11] = (-a[8] * s4 + a[9] * s2 - a[11] * s0) * invDet;

    inv.m[12] = (-a[4] * c3 + a[5] * c1 - a[6] * c0) * invDet;
    inv.m[13] = ( a[0] * c3 - a[1] * c1 + a[2] * c0) * invDet;
    inv.m[14] = (-a[12] * s3 + a[13] * s1 - a[14] * s0) * invDet;
    inv.m[15] = ( a[8] * s3 - a[9] * s1 + a[10] * s0) * invDet;

    return inv;
}

void SkeletonGen::ComputeBindPose() {
    std::vector<Mat4> globals;
    UpdateGlobalTransforms(globals);
    for (size_t i = 0; i < m_Bones.size(); ++i) {
        m_Bones[i].inverseBindMatrix = Inverse(globals[i]);
    }
}

void SkeletonGen::UpdateGlobalTransforms(std::vector<Mat4>& outTransforms) const {
    outTransforms.resize(m_Bones.size());
    for (size_t i = 0; i < m_Bones.size(); ++i) {
        Mat4 localM = m_Bones[i].localTransform.GetMatrix();
        int32_t pIdx = m_Bones[i].parentIndex;
        if (pIdx >= 0 && static_cast<size_t>(pIdx) < i) {
            outTransforms[i] = Multiply(outTransforms[pIdx], localM);
        } else {
            outTransforms[i] = localM;
        }
    }
}

} // namespace sphere
