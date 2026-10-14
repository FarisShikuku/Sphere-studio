#include "PhysicsWorld.h"
#include <cmath>
#include <algorithm>
#include <limits>

namespace sphere {

PhysicsWorld& PhysicsWorld::Get() {
    static PhysicsWorld instance;
    return instance;
}

uint32_t PhysicsWorld::AddSphereCollider(const Vec3& center, float radius, uint32_t layer, uint32_t mask) {
    SphereCollider col;
    col.id = m_NextId++;
    col.center = center;
    col.radius = radius;
    col.layer = layer;
    col.mask = mask;
    m_Spheres.push_back(col);
    return col.id;
}

uint32_t PhysicsWorld::AddBoxCollider(const AABB& aabb, const Transform& transform, uint32_t layer, uint32_t mask) {
    BoxCollider col;
    col.id = m_NextId++;
    col.aabb = aabb;
    col.transform = transform;
    col.layer = layer;
    col.mask = mask;
    m_Boxes.push_back(col);
    return col.id;
}

uint32_t PhysicsWorld::AddMeshCollider(const std::vector<Vec3>& vertices, const std::vector<uint32_t>& indices, const Transform& transform, uint32_t layer, uint32_t mask) {
    MeshCollider col;
    col.id = m_NextId++;
    col.vertices = vertices;
    col.indices = indices;
    col.transform = transform;
    col.layer = layer;
    col.mask = mask;

    // Compute bounding box
    if (!vertices.empty()) {
        col.bounds.min = vertices[0];
        col.bounds.max = vertices[0];
        for (const auto& v : vertices) {
            col.bounds.min.x = std::min(col.bounds.min.x, v.x);
            col.bounds.min.y = std::min(col.bounds.min.y, v.y);
            col.bounds.min.z = std::min(col.bounds.min.z, v.z);
            col.bounds.max.x = std::max(col.bounds.max.x, v.x);
            col.bounds.max.y = std::max(col.bounds.max.y, v.y);
            col.bounds.max.z = std::max(col.bounds.max.z, v.z);
        }
    }
    m_Meshes.push_back(col);
    return col.id;
}

uint32_t PhysicsWorld::AddRigidBody(const Vec3& position, float mass, bool isStatic, uint32_t layer) {
    RigidBody rb;
    rb.id = m_NextId++;
    rb.position = position;
    rb.mass = mass;
    rb.isStatic = isStatic;
    rb.layer = layer;
    m_Bodies.push_back(rb);
    return rb.id;
}

void PhysicsWorld::Clear() {
    m_Spheres.clear();
    m_Boxes.clear();
    m_Meshes.clear();
    m_Bodies.clear();
}

void PhysicsWorld::Step(float dt) {
    if (dt <= 0.0f) return;
    if (dt > 0.1f) dt = 0.1f; // Clamp to avoid large timestep tunneling

    for (auto& body : m_Bodies) {
        if (body.isStatic) continue;

        // Apply gravity acceleration
        body.velocity.y += m_Gravity * dt;

        // Linear velocity integration
        body.position.x += body.velocity.x * dt;
        body.position.y += body.velocity.y * dt;
        body.position.z += body.velocity.z * dt;

        // Ground plane collision resolution at y = 0
        if (body.position.y < 0.0f) {
            body.position.y = 0.0f;
            if (body.velocity.y < 0.0f) {
                body.velocity.y = -body.velocity.y * body.restitution;
                // Friction damping on tangent velocity
                float frictionFactor = std::max(0.0f, 1.0f - body.friction * dt * 10.0f);
                body.velocity.x *= frictionFactor;
                body.velocity.z *= frictionFactor;
            }
        }
    }
}

bool PhysicsWorld::RayIntersectsAABB(const Vec3& ro, const Vec3& invDir, const AABB& aabb, float maxDist, float& outT) {
    float t1 = (aabb.min.x - ro.x) * invDir.x;
    float t2 = (aabb.max.x - ro.x) * invDir.x;
    float tmin = std::min(t1, t2);
    float tmax = std::max(t1, t2);

    t1 = (aabb.min.y - ro.y) * invDir.y;
    t2 = (aabb.max.y - ro.y) * invDir.y;
    tmin = std::max(tmin, std::min(t1, t2));
    tmax = std::min(tmax, std::max(t1, t2));

    t1 = (aabb.min.z - ro.z) * invDir.z;
    t2 = (aabb.max.z - ro.z) * invDir.z;
    tmin = std::max(tmin, std::min(t1, t2));
    tmax = std::min(tmax, std::max(t1, t2));

    if (tmax >= std::max(0.0f, tmin) && tmin <= maxDist) {
        outT = (tmin >= 0.0f) ? tmin : tmax;
        return outT <= maxDist;
    }
    return false;
}

bool PhysicsWorld::RayIntersectsTriangle(const Vec3& ro, const Vec3& rd, const Vec3& v0, const Vec3& v1, const Vec3& v2, float maxDist, float& outT, Vec3& outNormal) {
    // Möller–Trumbore ray-triangle intersection algorithm
    const float EPSILON = 1e-7f;
    Vec3 e1 = { v1.x - v0.x, v1.y - v0.y, v1.z - v0.z };
    Vec3 e2 = { v2.x - v0.x, v2.y - v0.y, v2.z - v0.z };

    Vec3 p = {
        rd.y * e2.z - rd.z * e2.y,
        rd.z * e2.x - rd.x * e2.z,
        rd.x * e2.y - rd.y * e2.x
    };

    float det = e1.x * p.x + e1.y * p.y + e1.z * p.z;
    if (std::abs(det) < EPSILON) return false;

    float invDet = 1.0f / det;
    Vec3 s = { ro.x - v0.x, ro.y - v0.y, ro.z - v0.z };
    float u = (s.x * p.x + s.y * p.y + s.z * p.z) * invDet;
    if (u < 0.0f || u > 1.0f) return false;

    Vec3 q = {
        s.y * e1.z - s.z * e1.y,
        s.z * e1.x - s.x * e1.z,
        s.x * e1.y - s.y * e1.x
    };

    float v = (rd.x * q.x + rd.y * q.y + rd.z * q.z) * invDet;
    if (v < 0.0f || u + v > 1.0f) return false;

    float t = (e2.x * q.x + e2.y * q.y + e2.z * q.z) * invDet;
    if (t > EPSILON && t < maxDist) {
        outT = t;
        // Compute unnormalized geometric normal
        Vec3 n = {
            e1.y * e2.z - e1.z * e2.y,
            e1.z * e2.x - e1.x * e2.z,
            e1.x * e2.y - e1.y * e2.x
        };
        float len = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
        if (len > 1e-6f) {
            outNormal = { n.x / len, n.y / len, n.z / len };
        } else {
            outNormal = { 0.0f, 1.0f, 0.0f };
        }
        return true;
    }
    return false;
}

RaycastHit PhysicsWorld::Raycast(const Vec3& origin, const Vec3& direction, float maxDistance, uint32_t mask) {
    RaycastHit closestHit;
    closestHit.distance = maxDistance;

    // Normalize direction
    float dirLen = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    if (dirLen < 1e-7f) return closestHit;
    Vec3 dir = { direction.x / dirLen, direction.y / dirLen, direction.z / dirLen };

    Vec3 invDir = {
        std::abs(dir.x) > 1e-7f ? 1.0f / dir.x : 1e7f,
        std::abs(dir.y) > 1e-7f ? 1.0f / dir.y : 1e7f,
        std::abs(dir.z) > 1e-7f ? 1.0f / dir.z : 1e7f
    };

    // 1. Raycast against ground plane at y = 0
    if ((mask & CollisionGroup_Terrain) != 0 && dir.y < -1e-5f) {
        float t = -origin.y / dir.y;
        if (t >= 0.0f && t < closestHit.distance) {
            closestHit.hasHit = true;
            closestHit.distance = t;
            closestHit.point = { origin.x + dir.x * t, 0.0f, origin.z + dir.z * t };
            closestHit.normal = { 0.0f, 1.0f, 0.0f };
            closestHit.layer = CollisionGroup_Terrain;
        }
    }

    // 2. Analytical Ray-Sphere intersections
    for (const auto& sphere : m_Spheres) {
        if ((sphere.layer & mask) == 0) continue;

        Vec3 oc = { origin.x - sphere.center.x, origin.y - sphere.center.y, origin.z - sphere.center.z };
        float b = oc.x * dir.x + oc.y * dir.y + oc.z * dir.z;
        float c = (oc.x * oc.x + oc.y * oc.y + oc.z * oc.z) - (sphere.radius * sphere.radius);

        if (c > 0.0f && b > 0.0f) continue;

        float discriminant = b * b - c;
        if (discriminant < 0.0f) continue;

        float sqrtD = std::sqrt(discriminant);
        float t = -b - sqrtD;
        if (t < 0.0f) t = -b + sqrtD;

        if (t >= 0.0f && t < closestHit.distance) {
            closestHit.hasHit = true;
            closestHit.distance = t;
            closestHit.point = { origin.x + dir.x * t, origin.y + dir.y * t, origin.z + dir.z * t };
            Vec3 norm = {
                (closestHit.point.x - sphere.center.x) / sphere.radius,
                (closestHit.point.y - sphere.center.y) / sphere.radius,
                (closestHit.point.z - sphere.center.z) / sphere.radius
            };
            closestHit.normal = norm;
            closestHit.colliderId = sphere.id;
            closestHit.layer = sphere.layer;
        }
    }

    // 3. Analytical Ray-AABB intersections
    for (const auto& box : m_Boxes) {
        if ((box.layer & mask) == 0) continue;

        float tBox = 0.0f;
        // Transform origin relative to box position
        Vec3 relOrigin = {
            origin.x - box.transform.position.x,
            origin.y - box.transform.position.y,
            origin.z - box.transform.position.z
        };

        if (RayIntersectsAABB(relOrigin, invDir, box.aabb, closestHit.distance, tBox)) {
            if (tBox < closestHit.distance) {
                closestHit.hasHit = true;
                closestHit.distance = tBox;
                closestHit.point = { origin.x + dir.x * tBox, origin.y + dir.y * tBox, origin.z + dir.z * tBox };
                
                // Determine face normal
                Vec3 p = { closestHit.point.x - box.transform.position.x,
                           closestHit.point.y - box.transform.position.y,
                           closestHit.point.z - box.transform.position.z };
                const float eps = 1e-3f;
                if (std::abs(p.x - box.aabb.max.x) < eps) closestHit.normal = { 1, 0, 0};
                else if (std::abs(p.x - box.aabb.min.x) < eps) closestHit.normal = {-1, 0, 0};
                else if (std::abs(p.y - box.aabb.max.y) < eps) closestHit.normal = { 0, 1, 0};
                else if (std::abs(p.y - box.aabb.min.y) < eps) closestHit.normal = { 0,-1, 0};
                else if (std::abs(p.z - box.aabb.max.z) < eps) closestHit.normal = { 0, 0, 1};
                else closestHit.normal = {0, 0, -1};

                closestHit.colliderId = box.id;
                closestHit.layer = box.layer;
            }
        }
    }

    // 4. Möller–Trumbore Ray-Mesh intersections
    for (const auto& mesh : m_Meshes) {
        if ((mesh.layer & mask) == 0) continue;

        // First test against mesh bounding box
        float tAABB = 0.0f;
        Vec3 relOrigin = {
            origin.x - mesh.transform.position.x,
            origin.y - mesh.transform.position.y,
            origin.z - mesh.transform.position.z
        };
        if (!RayIntersectsAABB(relOrigin, invDir, mesh.bounds, closestHit.distance, tAABB)) {
            continue;
        }

        // Test each triangle
        for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3) {
            uint32_t i0 = mesh.indices[i];
            uint32_t i1 = mesh.indices[i + 1];
            uint32_t i2 = mesh.indices[i + 2];
            if (i0 >= mesh.vertices.size() || i1 >= mesh.vertices.size() || i2 >= mesh.vertices.size()) continue;

            Vec3 v0 = { mesh.vertices[i0].x + mesh.transform.position.x, mesh.vertices[i0].y + mesh.transform.position.y, mesh.vertices[i0].z + mesh.transform.position.z };
            Vec3 v1 = { mesh.vertices[i1].x + mesh.transform.position.x, mesh.vertices[i1].y + mesh.transform.position.y, mesh.vertices[i1].z + mesh.transform.position.z };
            Vec3 v2 = { mesh.vertices[i2].x + mesh.transform.position.x, mesh.vertices[i2].y + mesh.transform.position.y, mesh.vertices[i2].z + mesh.transform.position.z };

            float tTri = 0.0f;
            Vec3 triNorm;
            if (RayIntersectsTriangle(origin, dir, v0, v1, v2, closestHit.distance, tTri, triNorm)) {
                if (tTri < closestHit.distance) {
                    closestHit.hasHit = true;
                    closestHit.distance = tTri;
                    closestHit.point = { origin.x + dir.x * tTri, origin.y + dir.y * tTri, origin.z + dir.z * tTri };
                    closestHit.normal = triNorm;
                    closestHit.colliderId = mesh.id;
                    closestHit.layer = mesh.layer;
                }
            }
        }
    }

    return closestHit;
}

} // namespace sphere
