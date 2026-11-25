#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "../shared/math/MathTypes.h"

namespace procedural::buildings {

enum class BuildingVolumeShape : int32_t {
    Square = 0,
    Rectangle = 1,
    Cube = 2,
    Oval = 3,
    SemiOval = 4,
    Cylinder = 5,
    TriangleSemiCircleHybrid = 6,
    TriangularFlatiron = 7,
    LShaped = 8,
    Octagon = 9,
    Hexagon = 10,
    SteppedZiggurat = 11,
    // Star Designs
    StarTriPointY = 12,
    StarFourPointCross = 13,
    StarFivePointApex = 14,
    // General Architectural Designs
    TwistedHelix = 15,
    TorusRingPavilion = 16,
    CantileverOverhang = 17,
    ConicalPyramid = 18,
    // Complex Architectural Building Shapes
    UShapedCourtyard = 19,
    HShapedTwinWing = 20,
    CurvedWaveRibbon = 21,
    DiagridExoskeletonTower = 22,
    MobiusArchPortal = 23,
    SteppedSailMarineTower = 24,
    InterlockingJengaCantilevers = 25,
    FacetedCrystallineGem = 26
};

enum class BuildingCornerType : int32_t {
    Sharp90 = 0,
    Chamfer45 = 1,
    RoundedFillet = 2,
    InsetNotch = 3,
    SteppedDoubleNotch = 4,
    CornerColumn = 5,
    GlazedSlotReveal = 6
};

enum class BuildingStepMode : int32_t {
    UniformStraight = 0,
    SteppedBlocks = 1,
    ContinuousTaper = 2,
    ConcaveCurvedTaper = 3,
    ExpandingCantilever = 4
};

enum class WindowType : int32_t {
    ModernCasement = 0,
    DoubleHungSash = 1,
    PictureGlass = 2,
    RibbonStrip = 3,
    ArchedHistoric = 4,
    CurtainWall = 5,
    BayWindow = 6,
    CornerWrap = 7,
    ClerestoryTransom = 8,
    RoundOculus = 9,
    LouveredJalousie = 10,
    ArchedGothicLancet = 11,
    MultiPaneFactory = 12,
    // 10 New Architectural Window Designs
    DiamondLeadGlazing = 13,
    PivotingHorizontalHopper = 14,
    PivotingVerticalSash = 15,
    OctagonalBullseye = 16,
    PalladianTripleArched = 17,
    TriangularApexTransom = 18,
    HexagonalHoneycombAperture = 19,
    BriseSoleilDeepFinned = 20,
    SlidingPocketSash = 21,
    FrenchCasementFullHeight = 22,
    // Aliases
    IndustrialMultiPane = 12,
    LouvredShutters = 10,
    TransomArch = 4,
    BayWindowExtruded = 6,
    ClerestoryHigh = 8,
    BullseyeCircular = 9
};

enum class GlassType : int32_t {
    ClearFloat = 0,
    SolarTintedSmoke = 1,
    ReflectiveChromeMirror = 2,
    LowEEmerald = 3,
    BronzeArchitectural = 4,
    FrostedSatinPrivacy = 5,
    GoldMirroredLuxury = 6,
    StainedGlassArtisan = 7,
    // Aliases
    TintedSmoke = 1,
    ReflectiveMirror = 2,
    FrostedObscure = 5,
    EmeraldGreen = 3,
    GoldSolar = 6,
    WiredSafety = 8
};

enum class WallMaterialType : int32_t {
    ConcreteBrutalist = 0,
    BrickRedMasonry = 1,
    LimestonePortland = 2,
    GlassCurtainWall = 3,
    WhiteStuccoModern = 4,
    WeatheredCortenSteel = 5,
    DarkBasaltSlate = 6,
    NaturalTimberSiding = 7,
    BlackAluminumGlassGrid = 8,
    StructuralSiliconeGlazing = 9,
    ChampagneAluminumGlass = 10,
    DoubleSkinAerodynamicGlass = 11,
    BronzeAluminumSolarFins = 12,
    GunmetalAluminumCassette = 13,
    PerforatedAluminumBriseSoleil = 14,
    SpiderPointFixedGlass = 15
};

enum class DoorType : int32_t {
    None = 0,
    SingleModern = 1,
    DoubleGlassCommercial = 2,
    ArchedWooden = 3,
    IndustrialRollUp = 4,
    RevolvingDoor = 5,
    AutomaticSliding = 6,
    FrenchDoublePatio = 7,
    CanopiedGrandPortal = 8,
    SecurityGateGrille = 9,
    // Aliases
    ResidentialPanel = 7,
    EmergencyExitPanic = 8,
    HeavySecurityVault = 9
};

enum class RoofType : int32_t {
    FlatParapet = 0,
    Gable = 1,
    Hip = 2,
    Mansard = 3,
    Dome = 4,
    NoneFlat = 5,
    ShedMonopitch = 6,
    SawtoothIndustrial = 7,
    PyramidalApex = 8,
    BarrelVault = 9,
    GambrelDutch = 10,
    GreenRoofTerrace = 11,
    // 10 Complex Architectural Roof Designs
    Butterfly = 12,
    CrossGableMultiDormer = 13,
    DutchHipWithCupola = 14,
    PagodaCurvedEaves = 15,
    GeodesicSkylightCrown = 16,
    SplitClerestoryTwinPitch = 17,
    HyperbolicParaboloidShell = 18,
    ConicalSpireLantern = 19,
    DiagridSailCanopy = 20,
    TieredSteppedZigguratCrown = 21,
    // Aliases
    Gambrel = 10,
    PyramidHip = 8
};

enum class BalconyType : int32_t {
    None = 0,
    CantileveredGlass = 1,
    WroughtIron = 2,
    ConcreteLedge = 3
};

enum class MaterialSlot : uint32_t {
    WallConcrete = 0,
    Glass = 1,
    FrameMetal = 2,
    DoorWoodMetal = 3,
    RoofSurface = 4,
    Foundation = 5,
    HvacMetal = 6,
    GlassTinted = 7,
    GlassReflective = 8,
    GlassFrosted = 9,
    GlassEmerald = 10,
    GlassGold = 11,
    GlassWired = 12,
    GlassBronze = 13,
    GlassStained = 14,
    BalconyMetal = 15,
    BalconyGlass = 16,
    WoodShutters = 17,
    CorniceStone = 18,
    BrassHardware = 19,
    WallBrick = 20,
    WallLimestone = 21,
    WallStucco = 22,
    WallCorten = 23,
    WallBasalt = 24,
    WallTimber = 25,
    PergolaWood = 26,
    GreenTurfDeck = 27,
    PlanterBox = 28,
    WallBlackAluminum = 29,
    WallChampagneAluminum = 30,
    WallGunmetalAluminum = 31,
    WallBronzeAluminum = 32,
    WallSilverAluminum = 33,
    WallSpandrelGlass = 34,
    StadiumSeatBlue = 35,
    TrackRed = 36,
    SafetyYellow = 37,
    TarmacDark = 38,
    CommercialAccentRed = 39,
    CommercialSignOrange = 40,
    TireRubber = 41,
    ShopFloorEpoxy = 42,
    CardboardTan = 43,
    VehicleBlue = 44,
    VehicleWhite = 45,
    HydraulicGreen = 46,
    LightBulbWarm = 47,
    LightBulbDaylight = 48,
    LightFixtureMetal = 49
};

struct BuildingParameters {
    BuildingVolumeShape shape = BuildingVolumeShape::Rectangle;
    int32_t floorCount = 5;
    float floorHeight = 3.0f; // e.g. 5 floors * 3m = 15m
    float width = 20.0f;
    float depth = 14.0f;
    float cornerRadius = 3.0f; // For oval / semi-oval / beveled corners

    int32_t windowsPerFacade = 4;
    WindowType windowType = WindowType::ModernCasement;
    float windowWidth = 1.6f;
    float windowHeight = 1.8f;
    float windowSillHeight = 0.8f;

    DoorType doorType = DoorType::DoubleGlassCommercial;
    float doorWidth = 2.4f;
    float doorHeight = 2.6f;
    bool hasEntranceCanopy = true;

    RoofType roofType = RoofType::FlatParapet;
    float roofHeight = 2.5f;
    float roofOverhang = 0.6f;
    bool showRooftopClutter = true;

    GlassType glassType = GlassType::ClearFloat;
    WallMaterialType wallMaterial = WallMaterialType::GlassCurtainWall;
    BalconyType balconyType = BalconyType::None;
    bool hasBalconies = false;
    bool hasSunLouvres = false;
    bool hasFireEscape = false;
    bool hasCornice = true;
    bool hasGroundPillars = false;
    bool hasWindowShutters = false;

    float balconyDepth = 1.0f;
    int32_t pillarCount = 4;
    float slabThickness = 0.35f;

    BuildingCornerType cornerType = BuildingCornerType::Sharp90;
    float cornerSize = 2.0f; // Dimension of chamfer, fillet radius, notch depth, or corner column
    BuildingStepMode stepMode = BuildingStepMode::UniformStraight;
    int32_t stepTiers = 3; // Number of stacked blocks/tiers (2 to 6)
    float topTaperRatio = 0.55f; // Taper ratio (0.20 to 0.95: top tier scale relative to base)
    float stepShrinkFactor = 0.20f; // Reduction per tier for stepped blocks (0.05 to 0.40)
    bool stepTerraceRailing = true; // Parapet balustrades and terrace deck on setbacks

    bool showInside = false;
    int32_t interiorDesign = 0;

    float getTotalHeight() const {
        float roofH = (roofType == RoofType::NoneFlat) ? 0.0f :
                      (roofType == RoofType::FlatParapet ? 1.0f : roofHeight);
        return (floorCount * floorHeight) + roofH;
    }
};

struct BuildingVertex {
    Vec3 position;
    Vec3 normal;
    Vec2 uv;
    uint32_t materialId = 0;
};

struct BuildingStats {
    int32_t floorCount = 0;
    float totalHeightMeters = 0.0f;
    float footprintAreaSqM = 0.0f;
    float totalGrossFloorAreaSqM = 0.0f;
    int32_t totalWindowCount = 0;
    int32_t vertexCount = 0;
    int32_t triangleCount = 0;
};

} // namespace procedural::buildings
