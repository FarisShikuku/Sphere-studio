#include "BuildingSystem.h"

namespace procedural::buildings {

BuildingSystem& BuildingSystem::instance() {
    static BuildingSystem sInstance;
    return sInstance;
}

BuildingSystem::BuildingSystem() {
    proceduralBuildingGen_ = std::make_shared<ProceduralBuildingGenerator>();
    houseGen_ = std::make_shared<HouseGen>();
    towerGen_ = std::make_shared<TowerGen>();
    petrolStationGen_ = std::make_shared<PetrolStationGen>();
    carWashGen_ = std::make_shared<CarWashGen>();
    warehouseGen_ = std::make_shared<WarehouseGen>();
    civicGen_ = std::make_shared<CivicGen>();
    roomGen_ = std::make_shared<RoomGen>();
    propAttach_ = std::make_shared<PropAttach>();
    neonSignGen_ = std::make_shared<NeonSignGen>();
    damageGen_ = std::make_shared<DamageGen>();
    impostorGen_ = std::make_shared<ImpostorGen>();

    allGenerators_[proceduralBuildingGen_->getName()] = proceduralBuildingGen_;
    allGenerators_[houseGen_->getName()] = houseGen_;
    allGenerators_[towerGen_->getName()] = towerGen_;
    allGenerators_[petrolStationGen_->getName()] = petrolStationGen_;
    allGenerators_[carWashGen_->getName()] = carWashGen_;
    allGenerators_[warehouseGen_->getName()] = warehouseGen_;
    allGenerators_[civicGen_->getName()] = civicGen_;
    allGenerators_[roomGen_->getName()] = roomGen_;
    allGenerators_[propAttach_->getName()] = propAttach_;
    allGenerators_[neonSignGen_->getName()] = neonSignGen_;
    allGenerators_[damageGen_->getName()] = damageGen_;
    allGenerators_[impostorGen_->getName()] = impostorGen_;
}

void BuildingSystem::initialize(const GeneratorContext& ctx) {
    for (auto& [name, gen] : allGenerators_) {
        if (gen) gen->initialize(ctx);
    }
}

void BuildingSystem::registerAllToRegistry() {
    auto& reg = GeneratorRegistry::instance();
    for (auto& [name, gen] : allGenerators_) {
        if (gen) reg.registerGenerator(gen);
    }
}

std::shared_ptr<IGenerator> BuildingSystem::getBuildingGenerator(const std::string& name) {
    auto it = allGenerators_.find(name);
    if (it != allGenerators_.end()) {
        return it->second;
    }
    return nullptr;
}

GeneratedMeshPayload BuildingSystem::generateMesh(const std::string& generatorName, const GeneratorContext& ctx) {
    auto gen = getBuildingGenerator(generatorName);
    if (gen) {
        return gen->generateMesh(ctx);
    }
    return GeneratedMeshPayload{};
}

std::vector<std::string> BuildingSystem::getCategories() const {
    return {
        "Massing & Shapes",
        "Residential",
        "Commercial",
        "Industrial",
        "Civic",
        "Interiors",
        "Rooftop Attachments",
        "Signage & Billboards",
        "Structural Damage",
        "LOD & Distant Blocks"
    };
}

std::vector<std::string> BuildingSystem::getGeneratorsForCategory(const std::string& category) const {
    if (category == "Massing & Shapes") return {"ProceduralBuildingGenerator"};
    if (category == "Residential") return {"HouseGen"};
    if (category == "Commercial") return {"TowerGen", "PetrolStationGen", "CarWashGen"};
    if (category == "Industrial") return {"WarehouseGen"};
    if (category == "Civic") return {"CivicGen"};
    if (category == "Interiors") return {"RoomGen"};
    if (category == "Rooftop Attachments") return {"PropAttach"};
    if (category == "Signage & Billboards") return {"NeonSignGen"};
    if (category == "Structural Damage") return {"DamageGen"};
    if (category == "LOD & Distant Blocks") return {"ImpostorGen"};
    return {};
}

std::string BuildingSystem::getDiagnostics() const {
    std::string diag = "BuildingSystem: 12 generators registered\n"
                       "  - ProceduralBuildingGenerator (Subdivision & Shapes)\n"
                       "  - HouseGen (38 Residential Types: 10 Classic + 10 Heritage + 10 Modern Flat-Roof + 8 Signature Mansions/Apartments/Villas)\n"
                       "  - TowerGen (Office, Mall, Small Shop, Petrol, CarWash, MegaMall, Supermarket, AutoDealership, FastFood, Hotel, EvChargingStation, ServiceGarage, ParkingGarage, SelfStorage, ContainerShop, CoffeeKiosk, BoutiqueHotel, MarketHall, OpenFarmersMarket, NightMarketPlaza, SweepingRibbonCampus, CorporateGlassPark, EcoTerraceSpire, GlassPilasterHQ, DiagonalStrutPavilion)\n"
                       "  - PetrolStationGen (Canopy, Multi-Product Dispensers, Totem, Express Mart)\n"
                       "  - CarWashGen (Drive-Through Wash Tunnel, Roller Brushes, Dryer Blower, Vacuums)\n"
                       "  - WarehouseGen (Warehouse, Factory, Silo, PowerPlant, Refinery, ContainerTerminal, SteelMill, CleanroomFab, PowerLines, WaterTreatment, DataCenter, AerospaceHangar, WasteIncinerator, BiopharmPlant)\n"
                       "  - CivicGen (School, Hospital, Town Hall, BusTerminal, Stadium, AirportTerminal, TrainStation, DomedStadium, BaseballBallpark, OlympicVelodrome, FireStation, BotanicalGreenhouse, Observatory, PublicLibraryMuseum, GothicCathedral, ParishChapel, ModernSanctuary, CommunityAmphitheater)\n"
                       "  - RoomGen (Living Room, Office Floor, Floorplan, Warehouse Interior & Loading Docks, Vehicle Repair Workshop)\n"
                       "  - PropAttach (AC Units, Chimneys, Antennas)\n"
                       "  - NeonSignGen (Billboards, Neon Glow Fascia)\n"
                       "  - DamageGen (Cracked Walls, Blast Craters, Rubble)\n"
                       "  - ImpostorGen (City Blocks, Billboards, Cards)";
    return diag;
}

} // namespace procedural::buildings
