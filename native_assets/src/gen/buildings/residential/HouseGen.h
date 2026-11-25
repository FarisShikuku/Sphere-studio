#pragma once

#include "../../IGenerator.h"
#include "../BuildingTypes.h"
#include "../parts/socket.h"
#include <vector>

namespace procedural::buildings {

enum class ResidentialType {
    SuburbanHouse = 0,
    ApartmentBlock = 1,
    Villa = 2,
    RowhouseBrownstone = 3,
    CraftsmanTudor = 4,
    MidCenturyRanch = 5,
    ResidentialSkyscraper = 6,
    ModernDuplex = 7,
    CountryFarmhouse = 8,
    ResidentialGarage = 9,
    // 10 New Residential Architectural Designs
    VictorianPaintedLady = 10,
    MediterraneanVilla = 11,
    AFrameChalet = 12,
    TinyHouseModular = 13,
    CapeCodCottage = 14,
    GeodesicEcoDome = 15,
    CourtyardAtriumHouse = 16,
    JapaneseMachiya = 17,
    CantileverGlassHouse = 18,
    ColonialFourSquare = 19,
    // 10 Modern Flat-Roof Architectural Designs
    MinimalistCubicVilla = 20,
    BauhausResidence = 21,
    DesertModernistPavilion = 22,
    BrutalistConcreteHome = 23,
    JapaneseZenFlatHouse = 24,
    ShippingContainerHome = 25,
    GreenRoofEcoVilla = 26,
    TropicalLouveredHouse = 27,
    ClerestoryModernHome = 28,
    PenthouseTerraceHouse = 29,
    // 4 Signature Uploaded Residential Designs
    TropicalGableManor = 30,
    RooftopLoungeCube = 31,
    ParapetConcealedBungalow = 32,
    HipArchedBungalow = 33,
    ObsidianLouverVilla = 34,
    CantileverVStrutMansion = 35,
    StaggeredTerraceApartments = 36,
    HaloCanopyVilla = 37,
    ModernDualBoxCarportVilla = 38,
    TaupeInterlockingFrameVilla = 39,
    TwinWingSkybridgeApartments = 40,
    CorinthianPorticoModernVilla = 41,
    StoneAtriumPergolaVilla = 42,
    DiagridPanelCornerApartments = 43,
    SlantedProwModernVilla = 44,
    TimberBandPodiumApartments = 45,
    TriangularVoidGeometricVilla = 46,
    BrickPergolaPenthouseVilla = 47,
    TwinStairCornerApartments = 48,
    MinimalistRibbonBungalow = 49,
    // 3 Iconic Nairobi Residential Skyscrapers (>100m)
    Condominium88Nairobi = 50,
    GtcResidential1 = 51,
    AlturaTower = 52
};

struct ResidentialParams {
    ResidentialType subType = ResidentialType::SuburbanHouse;
    int32_t stories = 2;
    float storyHeight = 3.2f;
    float footprintWidth = 14.0f;
    float footprintDepth = 10.0f;
    RoofType roofStyle = RoofType::Gable;
    bool hasGarage = true;
    bool hasPorchOrBalcony = true;
    uint32_t wallColor = 0xFFE2E8F0;

    // Extended architectural controls (active when useExtendedParams == true)
    bool useExtendedParams = false;
    int32_t windowsPerFacade = 3;
    WindowType windowType = WindowType::DoubleHungSash;
    float windowWidth = 1.4f;
    float windowHeight = 1.6f;
    float windowSillHeight = 0.8f;
    GlassType glassType = GlassType::ClearFloat;
    DoorType doorType = DoorType::SingleModern;
    float doorWidth = 1.2f;
    float doorHeight = 2.2f;
    bool hasEntranceCanopy = true;
    float roofHeight = 2.8f;
    float roofOverhang = 0.6f;
    bool showRooftopClutter = false;
    WallMaterialType wallMaterial = WallMaterialType::ConcreteBrutalist;
    bool hasCornice = true;
    bool hasWindowShutters = false;
    bool hasSunLouvres = false;
    float balconyDepth = 1.2f;
};

class HouseGen : public IGenerator {
public:
    HouseGen();
    ~HouseGen() override = default;

    const char* getName() const override { return "HouseGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Buildings; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const ResidentialParams& params) { params_ = params; }
    const ResidentialParams& getParameters() const { return params_; }

    // 10 Original Residential Generators
    void generateSuburbanHouse(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateApartmentBlock(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateVilla(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateRowhouseBrownstone(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateCraftsmanTudor(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateMidCenturyRanch(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateResidentialSkyscraper(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateModernDuplex(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateCountryFarmhouse(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateResidentialGarage(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);

    // 10 New Residential Generators
    void generateVictorianPaintedLady(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateMediterraneanVilla(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateAFrameChalet(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateTinyHouseModular(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateCapeCodCottage(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateGeodesicEcoDome(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateCourtyardAtriumHouse(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateJapaneseMachiya(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateCantileverGlassHouse(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateColonialFourSquare(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);

    // 10 Modern Flat-Roof Residential Generators
    void generateMinimalistCubicVilla(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateBauhausResidence(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateDesertModernistPavilion(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateBrutalistConcreteHome(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateJapaneseZenFlatHouse(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateShippingContainerHome(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateGreenRoofEcoVilla(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateTropicalLouveredHouse(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateClerestoryModernHome(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generatePenthouseTerraceHouse(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);

    // 4 Signature Uploaded Residential Designs
    void generateTropicalGableManor(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateRooftopLoungeCube(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateParapetConcealedBungalow(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateHipArchedBungalow(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateObsidianLouverVilla(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateCantileverVStrutMansion(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateStaggeredTerraceApartments(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateHaloCanopyVilla(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateModernDualBoxCarportVilla(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateTaupeInterlockingFrameVilla(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateTwinWingSkybridgeApartments(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateCorinthianPorticoModernVilla(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateStoneAtriumPergolaVilla(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateDiagridPanelCornerApartments(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateSlantedProwModernVilla(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateTimberBandPodiumApartments(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateTriangularVoidGeometricVilla(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateBrickPergolaPenthouseVilla(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateTwinStairCornerApartments(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateMinimalistRibbonBungalow(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateCondominium88Nairobi(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateGtcResidential1(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateAlturaTower(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);

private:
    ResidentialParams params_;
};

} // namespace procedural::buildings
