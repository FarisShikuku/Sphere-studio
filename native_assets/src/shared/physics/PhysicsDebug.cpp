#include "PhysicsWorld.h"
#include "shared/debug/DebugDraw.h"

namespace sphere {

class PhysicsDebug {
public:
    static void DrawColliders(const PhysicsWorld& world) {
        auto& debug = DebugDraw::Get();

        // Spheres: Green wireframes
        const Vec4 sphereColor = { 0.2f, 0.9f, 0.3f, 1.0f };
        for (const auto& sphere : world.GetSpheres()) {
            debug.DrawSphere(sphere.center, sphere.radius, sphereColor, 24);
        }

        // Boxes: Cyan wireframes
        const Vec4 boxColor = { 0.1f, 0.7f, 0.9f, 1.0f };
        for (const auto& box : world.GetBoxes()) {
            AABB worldAABB;
            worldAABB.min = { box.aabb.min.x + box.transform.position.x,
                              box.aabb.min.y + box.transform.position.y,
                              box.aabb.min.z + box.transform.position.z };
            worldAABB.max = { box.aabb.max.x + box.transform.position.x,
                              box.aabb.max.y + box.transform.position.y,
                              box.aabb.max.z + box.transform.position.z };
            debug.DrawAABB(worldAABB, boxColor);
        }

        // Meshes: Yellow bounding boxes
        const Vec4 meshColor = { 0.95f, 0.8f, 0.1f, 1.0f };
        for (const auto& mesh : world.GetMeshes()) {
            AABB worldBounds;
            worldBounds.min = { mesh.bounds.min.x + mesh.transform.position.x,
                                mesh.bounds.min.y + mesh.transform.position.y,
                                mesh.bounds.min.z + mesh.transform.position.z };
            worldBounds.max = { mesh.bounds.max.x + mesh.transform.position.x,
                                mesh.bounds.max.y + mesh.transform.position.y,
                                mesh.bounds.max.z + mesh.transform.position.z };
            debug.DrawAABB(worldBounds, meshColor);
        }

        // Rigid body centroids: Red small markers
        const Vec4 bodyColor = { 1.0f, 0.2f, 0.2f, 1.0f };
        for (const auto& body : world.GetRigidBodies()) {
            debug.DrawSphere(body.position, 0.15f, bodyColor, 12);
        }
    }
};

} // namespace sphere
