#pragma once
#include "scene/Transform.h"
#include "scene/AABB.h"
#include "CollisionGroups.h"
#include <vector>
#include <cstdint>

namespace sphere {

struct RaycastHit {
    bool hasHit = false;
    Vec3 point{0,0,0};
    Vec3 normal{0,1,0};
    float distance = 0.0f;
    uint32_t colliderId = 0;
    uint32_t layer = CollisionGroup_Default;
};

struct SphereCollider {
    uint32_t id = 0;
    Vec3 center{0, 0, 0};
    float radius = 1.0f;
    uint32_t layer = CollisionGroup_Static;
    uint32_t mask = CollisionGroup_All;
};

struct BoxCollider {
    uint32_t id = 0;
    AABB aabb;
    Transform transform;
    uint32_t layer = CollisionGroup_Static;
    uint32_t mask = CollisionGroup_All;
};

struct MeshCollider {
    uint32_t id = 0;
    std::vector<Vec3> vertices;
    std::vector<uint32_t> indices;
    AABB bounds;
    Transform transform;
    uint32_t layer = CollisionGroup_Static;
    uint32_t mask = CollisionGroup_All;
};

struct RigidBody {
    uint32_t id = 0;
    Vec3 position{0, 0, 0};
    Vec3 velocity{0, 0, 0};
    Vec3 acceleration{0, 0, 0};
    float mass = 1.0f;
    float restitution = 0.25f;
    float friction = 0.4f;
    bool isStatic = false;
    uint32_t layer = CollisionGroup_Default;
    uint32_t mask = CollisionGroup_All;
};

class PhysicsWorld {
public:
    static PhysicsWorld& Get();

    void Step(float dt);

    // Collider management
    uint32_t AddSphereCollider(const Vec3& center, float radius, uint32_t layer = CollisionGroup_Static, uint32_t mask = CollisionGroup_All);
    uint32_t AddBoxCollider(const AABB& aabb, const Transform& transform, uint32_t layer = CollisionGroup_Static, uint32_t mask = CollisionGroup_All);
    uint32_t AddMeshCollider(const std::vector<Vec3>& vertices, const std::vector<uint32_t>& indices, const Transform& transform, uint32_t layer = CollisionGroup_Static, uint32_t mask = CollisionGroup_All);
    uint32_t AddRigidBody(const Vec3& position, float mass = 1.0f, bool isStatic = false, uint32_t layer = CollisionGroup_Default);

    void Clear();

    // Raycast query with Möller–Trumbore, ray-sphere, and slab tests
    RaycastHit Raycast(const Vec3& origin, const Vec3& direction, float maxDistance, uint32_t mask = CollisionGroup_All);

    // Getters for debug drawing
    const std::vector<SphereCollider>& GetSpheres() const { return m_Spheres; }
    const std::vector<BoxCollider>& GetBoxes() const { return m_Boxes; }
    const std::vector<MeshCollider>& GetMeshes() const { return m_Meshes; }
    const std::vector<RigidBody>& GetRigidBodies() const { return m_Bodies; }

    void SetGravity(float g) { m_Gravity = g; }
    float GetGravity() const { return m_Gravity; }

private:
    float m_Gravity = -9.81f;
    uint32_t m_NextId = 1;

    std::vector<SphereCollider> m_Spheres;
    std::vector<BoxCollider> m_Boxes;
    std::vector<MeshCollider> m_Meshes;
    std::vector<RigidBody> m_Bodies;

    static bool RayIntersectsAABB(const Vec3& ro, const Vec3& invDir, const AABB& aabb, float maxDist, float& outT);
    static bool RayIntersectsTriangle(const Vec3& ro, const Vec3& rd, const Vec3& v0, const Vec3& v1, const Vec3& v2, float maxDist, float& outT, Vec3& outNormal);
};

} // namespace sphere
