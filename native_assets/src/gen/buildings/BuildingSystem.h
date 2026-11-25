#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "../IGenerator.h"
#include "../GeneratorRegistry.h"

#include "BuildingTypes.h"
#include "ProceduralBuildingGenerator.h"
#include "residential/HouseGen.h"
#include "commercial/TowerGen.h"
#include "commercial/PetrolStationGen.h"
#include "commercial/CarWashGen.h"
#include "industrial/WarehouseGen.h"
#include "civic/CivicGen.h"
#include "interiors/RoomGen.h"
#include "props/PropAttach.h"
#include "signage/NeonSignGen.h"
#include "damage/DamageGen.h"
#include "lod/ImpostorGen.h"

namespace procedural::buildings {

class BuildingSystem {
public:
    static BuildingSystem& instance();

    void initialize(const GeneratorContext& ctx);
    void registerAllToRegistry();

    std::shared_ptr<IGenerator> getBuildingGenerator(const std::string& name);
    GeneratedMeshPayload generateMesh(const std::string& generatorName, const GeneratorContext& ctx);

    std::vector<std::string> getCategories() const;
    std::vector<std::string> getGeneratorsForCategory(const std::string& category) const;
    std::string getDiagnostics() const;

    // Direct typed generator getters
    std::shared_ptr<ProceduralBuildingGenerator> getProceduralBuildingGen() const { return proceduralBuildingGen_; }
    std::shared_ptr<HouseGen> getHouseGen() const { return houseGen_; }
    std::shared_ptr<TowerGen> getTowerGen() const { return towerGen_; }
    std::shared_ptr<PetrolStationGen> getPetrolStationGen() const { return petrolStationGen_; }
    std::shared_ptr<CarWashGen> getCarWashGen() const { return carWashGen_; }
    std::shared_ptr<WarehouseGen> getWarehouseGen() const { return warehouseGen_; }
    std::shared_ptr<CivicGen> getCivicGen() const { return civicGen_; }
    std::shared_ptr<RoomGen> getRoomGen() const { return roomGen_; }
    std::shared_ptr<PropAttach> getPropAttach() const { return propAttach_; }
    std::shared_ptr<NeonSignGen> getNeonSignGen() const { return neonSignGen_; }
    std::shared_ptr<DamageGen> getDamageGen() const { return damageGen_; }
    std::shared_ptr<ImpostorGen> getImpostorGen() const { return impostorGen_; }

private:
    BuildingSystem();

    std::shared_ptr<ProceduralBuildingGenerator> proceduralBuildingGen_;
    std::shared_ptr<HouseGen> houseGen_;
    std::shared_ptr<TowerGen> towerGen_;
    std::shared_ptr<PetrolStationGen> petrolStationGen_;
    std::shared_ptr<CarWashGen> carWashGen_;
    std::shared_ptr<WarehouseGen> warehouseGen_;
    std::shared_ptr<CivicGen> civicGen_;
    std::shared_ptr<RoomGen> roomGen_;
    std::shared_ptr<PropAttach> propAttach_;
    std::shared_ptr<NeonSignGen> neonSignGen_;
    std::shared_ptr<DamageGen> damageGen_;
    std::shared_ptr<ImpostorGen> impostorGen_;

    std::unordered_map<std::string, std::shared_ptr<IGenerator>> allGenerators_;
};

} // namespace procedural::buildings
