#pragma once
#include "Mesh.h"
#include "Transform.h"
#include "AABB.h"
#include <vector>
#include <memory>
#include <string>
namespace sphere {
enum class StageMode {
    FlatPiece = 0,
    Boundary = 1,
    InfiniteGrid = 2,
    CalibrationCube = 3
};
class GeometryGenerator {
public:
    // Standard procedural PBR solids
    static Mesh GenerateCube(float width = 1.0f, float height = 1.0f, float depth = 1.0f, const Vec3& center = {0, 0, 0});
    static Mesh GenerateCalibrationCube(float size = 1.0f, float bevel = 0.04f);
    
    // Stages, Grounds and Reference Frames
    static Mesh GeneratePlane(float width = 10.0f, float length = 10.0f, int segmentsX = 1, int segmentsZ = 1, float uvScale = 1.0f);
    static Mesh GenerateStagePlatform(float width = 10.0f, float length = 10.0f, float height = 0.35f, float bevel = 0.05f);
    static Mesh GenerateGridLines(int lineCount = 20, float spacing = 0.5f, bool addAxes = true);
    static Mesh GenerateInfiniteGridPlane(float radius = 100.0f, float uvScale = 50.0f);
    // Parametric Spheres & Atmospheric Domes
    static Mesh GenerateSphere(float radius = 1.0f, uint32_t rings = 32, uint32_t sectors = 32);
    static Mesh GenerateSkyDome(float radius = 500.0f, uint32_t rings = 24, uint32_t sectors = 48, float hemisphereCutoff = -0.05f);
    static Mesh GenerateAtmosphereCloudHull(float innerRadius = 6371.0f + 1.5f, float outerRadius = 6371.0f + 4.0f, float angularSpanDeg = 45.0f);
    // Utility & Debug
    static Mesh GenerateBoundingBox(const AABB& aabb, const Vec4& color = {0.2f, 0.8f, 1.0f, 1.0f});
    static Mesh GenerateQuad(float width = 1.0f, float height = 1.0f, const Vec3& normal = {0, 1, 0});
    static Mesh GenerateTerrainPatch(int resX = 32, int resZ = 32, float width = 10.0f, float length = 10.0f, float maxDisplacement = 1.0f, uint32_t seed = 42);
    // Transformations & Merging
    static void TransformMesh(Mesh& mesh, const Mat4& transform);
    static void MergeMeshes(Mesh& target, const Mesh& source);
};
} // namespace spher
