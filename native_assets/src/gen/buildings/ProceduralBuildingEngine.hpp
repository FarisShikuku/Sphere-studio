#pragma once

// ============================================================================
// ProceduralBuildingEngine.hpp
// Native C++17 Procedural Architectural Building Engine, 3D Mesh Synthesizer,
// Optical Material System & Unit Invariant Assert Suite
//
// 100% of procedural building generation, parametric volume extrusion,
// architectural aperture placement (13 window types, 12 roof geometries,
// 10 door portals, 8 optical glass materials, 8 wall cladding materials),
// 3D camera projection, and geometric integrity assert validation
// is implemented in C++17. Kotlin (.kt) is strictly reserved for Jetpack Compose UI.
// ============================================================================

#include <cstdint>
#include <vector>
#include <string>
#include <cmath>
#include <cassert>
#include <algorithm>

namespace procedural_building {

constexpr float kPi = 3.14159265358979323846f;

// ----------------------------------------------------------------------------
// 3D Vector Math & Validation Asserts
// ----------------------------------------------------------------------------
struct BuildingPoint3D {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    BuildingPoint3D() = default;
    BuildingPoint3D(float inX, float inY, float inZ) : x(inX), y(inY), z(inZ) {
        assert(!std::isnan(x) && !std::isinf(x));
        assert(!std::isnan(y) && !std::isinf(y));
        assert(!std::isnan(z) && !std::isinf(z));
    }

    BuildingPoint3D operator+(const BuildingPoint3D& o) const { return BuildingPoint3D(x + o.x, y + o.y, z + o.z); }
    BuildingPoint3D operator-(const BuildingPoint3D& o) const { return BuildingPoint3D(x - o.x, y - o.y, z - o.z); }
    BuildingPoint3D operator*(float s) const { return BuildingPoint3D(x * s, y * s, z * s); }
    BuildingPoint3D operator-() const { return BuildingPoint3D(-x, -y, -z); }

    float lengthSq() const { return x * x + y * y + z * z; }
    float length() const { return std::sqrt(lengthSq()); }

    BuildingPoint3D normalized() const {
        float l = length();
        assert(!std::isnan(l));
        return (l > 1e-5f) ? BuildingPoint3D(x / l, y / l, z / l) : BuildingPoint3D(0.0f, 1.0f, 0.0f);
    }

    float dot(const BuildingPoint3D& o) const { return x * o.x + y * o.y + z * o.z; }

    BuildingPoint3D cross(const BuildingPoint3D& o) const {
        return BuildingPoint3D(
            y * o.z - z * o.y,
            z * o.x - x * o.z,
            x * o.y - y * o.x
        );
    }
};

struct ScreenPoint2D {
    float x = 0.0f;
    float y = 0.0f;
};

// ----------------------------------------------------------------------------
// Architectural Enumerations
// ----------------------------------------------------------------------------
enum class VolumeShape : int {
    RECTANGLE = 0,
    ROUNDED_RECTANGLE = 1,
    CYLINDER = 2,
    TRIANGLE_SEMICIRCLE = 3,
    L_SHAPED = 4,
    POLYGON_PRISM = 5,
    TRIANGULAR_FLATIRON = 6,
    STEPPED_ZIGGURAT = 7,
    SEMI_OVAL = 8
};

enum class BuildingArchetype : int {
    PARAMETRIC_BUILDING = 0,
    OFFICE_TOWER = 1,
    SHOPPING_MALL = 2,
    SMALL_SHOP = 3,
    PETROL_STATION = 4,
    CAR_WASH = 5,
    WAREHOUSE = 6,
    FACTORY = 7,
    GRAIN_SILO = 8,
    SUBURBAN_HOUSE = 9,
    APARTMENT_BLOCK = 10,
    MODERN_VILLA = 11,
    SCHOOL = 12,
    HOSPITAL = 13,
    TOWN_HALL = 14
};

enum class WindowType : int {
    MODERN_CASEMENT = 0,
    DOUBLE_HUNG_SASH = 1,
    PICTURE_GLASS = 2,
    RIBBON_STRIP = 3,
    ARCHED_HISTORIC = 4,
    CURTAIN_WALL = 5,
    BAY_WINDOW = 6,
    CORNER_WRAP = 7,
    CLERESTORY_TRANSOM = 8,
    ROUND_OCULUS = 9,
    LOUVERED_JALOUSIE = 10,
    ARCHED_GOTHIC_LANCET = 11,
    MULTI_PANE_FACTORY = 12
};

enum class DoorType : int {
    NONE = 0,
    SINGLE_MODERN = 1,
    DOUBLE_GLASS_COMMERCIAL = 2,
    ARCHED_WOODEN = 3,
    INDUSTRIAL_ROLL_UP = 4,
    REVOLVING_DOOR = 5,
    AUTOMATIC_SLIDING = 6,
    FRENCH_DOUBLE_PATIO = 7,
    CANOPIED_GRAND_PORTAL = 8,
    SECURITY_GATE_GRILLE = 9
};

enum class RoofType : int {
    FLAT_PARAPET = 0,
    GABLE = 1,
    HIP = 2,
    MANSARD = 3,
    DOME = 4,
    NONE_FLAT = 5,
    SHED_MONOPITCH = 6,
    SAWTOOTH_INDUSTRIAL = 7,
    PYRAMIDAL_APEX = 8,
    BARREL_VAULT = 9,
    GAMBREL_DUTCH = 10,
    GREEN_ROOF_TERRACE = 11
};

enum class GlassMaterial : int {
    CLEAR_FLOAT = 0,
    SOLAR_CHARCOAL = 1,
    REFLECTIVE_CHROME = 2,
    LOW_E_EMERALD = 3,
    BRONZE_ARCHITECTURAL = 4,
    FROSTED_SATIN = 5,
    GOLD_MIRRORED = 6,
    CATHEDRAL_STAINED = 7
};

enum class WallMaterial : int {
    CONCRETE_BRUTALIST = 0,
    BRICK_TERRACOTTA = 1,
    LIMESTONE_PORTLAND = 2,
    GLASS_CURTAIN = 3,
    WHITE_STUCCO = 4,
    CORTEN_STEEL = 5,
    DARK_BASALT = 6,
    TIMBER_SIDING = 7
};

// ----------------------------------------------------------------------------
// Architectural Building Parameters Struct
// ----------------------------------------------------------------------------
struct NativeBuildingConfig {
    VolumeShape shape = VolumeShape::RECTANGLE;
    int floorCount = 5;
    float floorHeight = 3.0f;
    float width = 20.0f;
    float depth = 14.0f;
    float cornerRadius = 2.5f;

    int windowsPerFacade = 4;
    WindowType windowType = WindowType::MODERN_CASEMENT;
    float windowWidth = 1.6f;
    float windowHeight = 1.8f;
    float windowSillHeight = 0.8f;

    DoorType doorType = DoorType::DOUBLE_GLASS_COMMERCIAL;
    float doorWidth = 2.4f;
    float doorHeight = 2.6f;
    bool hasEntranceCanopy = true;

    RoofType roofType = RoofType::FLAT_PARAPET;
    float roofHeight = 2.5f;
    float roofOverhang = 0.6f;
    bool showRooftopClutter = true;

    GlassMaterial glassMaterial = GlassMaterial::CLEAR_FLOAT;
    WallMaterial wallMaterial = WallMaterial::GLASS_CURTAIN_WALL;
    bool hasBalconies = false;
    bool hasCornices = true;
    bool hasGroundPillars = false;
    bool hasWindowShutters = false;
    bool hasSunshades = false;
    float balconyDepth = 1.0f;
    int pillarCount = 4;
    float slabThickness = 0.35f;

    // Viewport camera parameters
    float cameraPitchDeg = 25.0f;
    float cameraYawDeg = 40.0f;
    float cameraDistance = 45.0f;
    float viewportWidth = 1000.0f;
    float viewportHeight = 1000.0f;
    float panOffsetX = 0.0f;
    float panOffsetY = 0.0f;
};

// ----------------------------------------------------------------------------
// Raw & Projected Polygons
// ----------------------------------------------------------------------------
struct NativeRawPolygon3D {
    std::vector<BuildingPoint3D> vertices;
    BuildingPoint3D normal;
    uint32_t colorArgb = 0xFFFFFFFF;
    bool doubleSided = false;
    int floorIndex = -1;
    bool isGlass = false;
    bool isRoof = false;
    int layer = 0; // 0 = host wall/slab, 1 = frame/pane, 2 = mullions/railings/decorations
};

struct NativeProjectedPolygon {
    std::vector<ScreenPoint2D> screenPoints;
    float averageDepth = 0.0f;
    BuildingPoint3D normal;
    uint32_t colorArgb = 0xFFFFFFFF;
    bool isWireframe = false;
    int floorIndex = -1;
    bool isGlass = false;
    bool isRoof = false;
    int layer = 0;
};

struct NativeBuildingMetrics {
    float totalHeightMeters = 0.0f;
    float footprintAreaSqM = 0.0f;
    float grossFloorAreaSqM = 0.0f;
    int estimatedWindows = 0;
    int vertexCount = 0;
    int polygonCount = 0;
};

// ----------------------------------------------------------------------------
// Procedural Building Engine Class Definition
// ----------------------------------------------------------------------------
class ProceduralBuildingEngine {
public:
    static void generateBuildingMesh(
        const NativeBuildingConfig& config,
        std::vector<NativeRawPolygon3D>& outPolygons,
        NativeBuildingMetrics& outMetrics
    );

    static void projectAndSortPolygons(
        const std::vector<NativeRawPolygon3D>& inPolygons,
        const NativeBuildingConfig& config,
        std::vector<NativeProjectedPolygon>& outProjected
    );

    static std::string getEngineDiagnostics();

    // Comprehensive Native Invariant Assert Suite
    static bool runBuildingEngineNativeAsserts();
};

} // namespace procedural_building
