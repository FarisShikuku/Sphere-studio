#pragma once

#include "../../IGenerator.h"
#include "../BuildingTypes.h"
#include <vector>

namespace procedural::buildings {

enum class CommercialType {
    OfficeTower = 0,
    ShoppingMall = 1,
    SmallShop = 2,
    PetrolStation = 3,
    CarWash = 4,
    MegaMall = 5,
    Supermarket = 6,
    AutoDealership = 7,
    FastFood = 8,
    Hotel = 9,
    EvChargingStation = 10,
    ServiceGarage = 11,
    ParkingGarage = 12,
    SelfStorage = 13,
    ContainerShop = 14,
    CoffeeKiosk = 15,
    BoutiqueHotel = 16,
    MarketHall = 17,
    OpenFarmersMarket = 18,
    NightMarketPlaza = 19,
    SweepingRibbonCampus = 20,
    CorporateGlassPark = 21,
    EcoTerraceSpire = 22,
    GlassPilasterHQ = 23,
    DiagonalStrutPavilion = 24,
    // 11 Iconic Nairobi Commercial Skyscrapers (>100m)
    BritamTower = 25,
    GtcOfficeTower = 26,
    UapOldMutualTower = 27,
    TimesTower = 28,
    PrismTower = 29,
    LemacTower = 30,
    TelepostaHouse = 31,
    CbkPensionTower = 32,
    KcbPlaza = 33,
    UpperHillChambers = 34,
    NssfBuilding = 35
};

struct CommercialParams {
    CommercialType subType = CommercialType::OfficeTower;
    int32_t stories = 12;
    float storyHeight = 3.5f;
    float footprintWidth = 24.0f;
    float footprintDepth = 18.0f;
    bool hasSetbacks = true;
    bool hasEntranceAtrium = true;
    bool curtainWallGlazing = true;

    bool useExtendedParams = false;
    int32_t windowsPerFacade = 5;
    WindowType windowType = WindowType::CurtainWall;
    float windowWidth = 1.6f;
    float windowHeight = 1.8f;
    float windowSillHeight = 0.8f;
    GlassType glassType = GlassType::ClearFloat;
    DoorType doorType = DoorType::DoubleGlassCommercial;
    float doorWidth = 2.4f;
    float doorHeight = 2.6f;
    bool hasEntranceCanopy = true;
    RoofType roofStyle = RoofType::FlatParapet;
    float roofHeight = 2.5f;
    float roofOverhang = 0.6f;
    bool showRooftopClutter = true;
    WallMaterialType wallMaterial = WallMaterialType::ConcreteBrutalist;
    bool hasBalconies = false;
    bool hasCornice = true;
    bool hasWindowShutters = false;
    bool hasSunLouvres = false;
    float balconyDepth = 1.0f;
    bool showInside = false;
    int32_t interiorDesign = 0;
};

class TowerGen : public IGenerator {
public:
    TowerGen();
    ~TowerGen() override = default;

    const char* getName() const override { return "TowerGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Buildings; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const CommercialParams& params) { params_ = params; }
    const CommercialParams& getParameters() const { return params_; }

    void generateOfficeTower(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateMall(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateSmallShop(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generatePetrolStation(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateCarWash(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateMegaMall(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateSupermarket(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateAutoDealership(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateFastFood(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateHotel(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateEvChargingStation(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateServiceGarage(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateParkingGarage(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateSelfStorage(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateContainerShop(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateCoffeeKiosk(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateBoutiqueHotel(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateMarketHall(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateOpenFarmersMarket(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateNightMarketPlaza(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateSweepingRibbonCampus(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateCorporateGlassPark(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateEcoTerraceSpire(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateGlassPilasterHQ(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateDiagonalStrutPavilion(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateBritamTower(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateGtcOfficeTower(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateUapOldMutualTower(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateTimesTower(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generatePrismTower(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateLemacTower(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateTelepostaHouse(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateCbkPensionTower(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateKcbPlaza(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateUpperHillChambers(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateNssfBuilding(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);

private:
    CommercialParams params_;
};

} // namespace procedural::buildings
