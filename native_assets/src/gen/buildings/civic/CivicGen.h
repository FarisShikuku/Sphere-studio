#pragma once

#include "../../IGenerator.h"
#include "../BuildingTypes.h"
#include <vector>

namespace procedural::buildings {

enum class CivicType {
    School,
    Hospital,
    TownHall,
    BusTerminal = 3,
    Stadium = 4,
    AirportTerminal = 5,
    TrainStation = 6,
    DomedStadium = 7,
    BaseballBallpark = 8,
    OlympicVelodrome = 9,
    FireStation = 10,
    BotanicalGreenhouse = 11,
    Observatory = 12,
    PublicLibraryMuseum = 13,
    GothicCathedral = 14,
    ParishChapel = 15,
    ModernSanctuary = 16,
    CommunityAmphitheater = 17,
    ParliamentTower = 18,
    KiccTower = 19
};

struct CivicParams {
    CivicType subType = CivicType::TownHall;
    int32_t stories = 3;
    float storyHeight = 3.6f;
    float mainWidth = 28.0f;
    float mainDepth = 18.0f;
    float wingLength = 12.0f;
    bool hasClockTower = true;
    bool hasHelipad = false;
    bool hasPortico = true;
    int32_t columnsCount = 6;
    int32_t busBays = 4;
    bool hasCanopy = true;
    bool hasFloodlights = true;
    bool hasControlTower = true;

    bool useExtendedParams = false;
    int32_t windowsPerFacade = 4;
    WindowType windowType = WindowType::ArchedHistoric;
    float windowWidth = 1.6f;
    float windowHeight = 1.8f;
    float windowSillHeight = 0.8f;
    GlassType glassType = GlassType::ClearFloat;
    DoorType doorType = DoorType::DoubleGlassCommercial;
    float doorWidth = 2.6f;
    float doorHeight = 2.8f;
    bool hasEntranceCanopy = true;
    RoofType roofStyle = RoofType::Mansard;
    float roofHeight = 3.2f;
    float roofOverhang = 0.6f;
    bool showRooftopClutter = false;
    WallMaterialType wallMaterial = WallMaterialType::ConcreteBrutalist;
    bool hasBalconies = false;
    bool hasCornice = true;
    bool hasWindowShutters = false;
    bool hasSunLouvres = false;
    float balconyDepth = 1.0f;
};

class CivicGen : public IGenerator {
public:
    CivicGen();
    ~CivicGen() override = default;

    const char* getName() const override { return "CivicGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Buildings; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const CivicParams& params) { params_ = params; }
    const CivicParams& getParameters() const { return params_; }

    void generateSchool(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateHospital(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateTownHall(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateBusTerminal(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateStadium(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateAirportTerminal(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateTrainStation(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateDomedStadium(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateBaseballBallpark(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateOlympicVelodrome(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateFireStation(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateBotanicalGreenhouse(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateObservatory(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generatePublicLibraryMuseum(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateGothicCathedral(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateParishChapel(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateModernSanctuary(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateCommunityAmphitheater(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateParliamentTower(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateKiccTower(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);

private:
    CivicParams params_;
};

} // namespace procedural::buildings
