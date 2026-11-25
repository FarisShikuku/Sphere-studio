#include "PropSystem.h"
#include <sstream>

namespace procedural::props {

PropSystem& PropSystem::instance() {
    static PropSystem instance;
    return instance;
}

PropSystem::PropSystem() {
    lampPostGen_ = std::make_shared<LampPostGen>();
    trafficLightGen_ = std::make_shared<TrafficLightGen>();
    benchGen_ = std::make_shared<BenchGen>();
    trashBinGen_ = std::make_shared<TrashBinGen>();
    expandableBarricadeGen_ = std::make_shared<ExpandableBarricadeGen>();
    flagPostGen_ = std::make_shared<FlagPostGen>();

    crateGen_ = std::make_shared<CrateGen>();
    barrelGen_ = std::make_shared<BarrelGen>();
    palletGen_ = std::make_shared<PalletGen>();
    pipeGen_ = std::make_shared<PipeGen>();
    securityFenceGen_ = std::make_shared<SecurityFenceGen>();

    hydrantGen_ = std::make_shared<HydrantGen>();
    mailboxGen_ = std::make_shared<MailboxGen>();
    busStopGen_ = std::make_shared<BusStopGen>();
    urbanFenceGen_ = std::make_shared<UrbanFenceGen>();
    bollardGen_ = std::make_shared<BollardGen>();
    jerseyBarrierGen_ = std::make_shared<JerseyBarrierGen>();
    securityGateGen_ = std::make_shared<SecurityGateGen>();
    privacyFenceGen_ = std::make_shared<PrivacyFenceGen>();

    fenceGen_ = std::make_shared<FenceGen>();
    hayBaleGen_ = std::make_shared<HayBaleGen>();
    waterTroughGen_ = std::make_shared<WaterTroughGen>();
    grainSiloGen_ = std::make_shared<GrainSiloGen>();
    farmWindmillGen_ = std::make_shared<FarmWindmillGen>();
    farmWagonGen_ = std::make_shared<FarmWagonGen>();
    farmGateGen_ = std::make_shared<FarmGateGen>();

    deskGen_ = std::make_shared<DeskGen>();
    chairGen_ = std::make_shared<ChairGen>();
    shelfGen_ = std::make_shared<ShelfGen>();

    infoBoardGen_ = std::make_shared<InfoBoardGen>();
    kilometreMarkerGen_ = std::make_shared<KilometreMarkerGen>();

    electricPostGen_ = std::make_shared<ElectricPostGen>();
    transformerGen_ = std::make_shared<TransformerGen>();
    powerMeterGen_ = std::make_shared<PowerMeterGen>();
    streetLightGen_ = std::make_shared<StreetLightGen>();
    waterTowerGen_ = std::make_shared<WaterTowerGen>();
    waterTankGen_ = std::make_shared<WaterTankGen>();
    telecomTowerGen_ = std::make_shared<TelecomTowerGen>();

    windTurbineGen_ = std::make_shared<WindTurbineGen>();
    solarPanelGen_ = std::make_shared<SolarPanelGen>();
    solarFarmGen_ = std::make_shared<SolarFarmGen>();
    batteryBankGen_ = std::make_shared<BatteryBankGen>();
    substationGen_ = std::make_shared<SubstationGen>();

    radioMastGen_ = std::make_shared<RadioMastGen>();
    satelliteDishGen_ = std::make_shared<SatelliteDishGen>();

    estateGateGen_ = std::make_shared<EstateGateGen>();
    tollGateGen_ = std::make_shared<TollGateGen>();
    railwayGateGen_ = std::make_shared<RailwayGateGen>();
    gardenGateGen_ = std::make_shared<GardenGateGen>();

    modernGlassFenceGen_ = std::make_shared<ModernGlassFenceGen>();
    aluminumFinFenceGen_ = std::make_shared<AluminumFinFenceGen>();
    laserCutFenceGen_ = std::make_shared<LaserCutFenceGen>();
    gabionStoneFenceGen_ = std::make_shared<GabionStoneFenceGen>();
    concreteRibFenceGen_ = std::make_shared<ConcreteRibFenceGen>();
    louverBladeFenceGen_ = std::make_shared<LouverBladeFenceGen>();
    livingWallFenceGen_ = std::make_shared<LivingWallFenceGen>();
    bambooSteelFenceGen_ = std::make_shared<BambooSteelFenceGen>();
    cableRailFenceGen_ = std::make_shared<CableRailFenceGen>();
    corrugatedCortenFenceGen_ = std::make_shared<CorrugatedCortenFenceGen>();

    modernSlatGateGen_ = std::make_shared<ModernSlatGateGen>();
    glassPivotGateGen_ = std::make_shared<GlassPivotGateGen>();
    laserArtGateGen_ = std::make_shared<LaserArtGateGen>();
    bifoldSpeedGateGen_ = std::make_shared<BifoldSpeedGateGen>();
    aluminumLouverGateGen_ = std::make_shared<AluminumLouverGateGen>();
    brutalistPortalGateGen_ = std::make_shared<BrutalistPortalGateGen>();
    glassSpeedGateGen_ = std::make_shared<GlassSpeedGateGen>();
    japaneseToriGateGen_ = std::make_shared<JapaneseToriGateGen>();
    gabionTimberGateGen_ = std::make_shared<GabionTimberGateGen>();
    solarSmartGateGen_ = std::make_shared<SolarSmartGateGen>();

    allProps_["LampPostGen"] = lampPostGen_;
    allProps_["TrafficLightGen"] = trafficLightGen_;
    allProps_["BenchGen"] = benchGen_;
    allProps_["TrashBinGen"] = trashBinGen_;
    allProps_["ExpandableBarricadeGen"] = expandableBarricadeGen_;
    allProps_["FlagPostGen"] = flagPostGen_;

    allProps_["CrateGen"] = crateGen_;
    allProps_["BarrelGen"] = barrelGen_;
    allProps_["PalletGen"] = palletGen_;
    allProps_["PipeGen"] = pipeGen_;
    allProps_["SecurityFenceGen"] = securityFenceGen_;

    allProps_["HydrantGen"] = hydrantGen_;
    allProps_["MailboxGen"] = mailboxGen_;
    allProps_["BusStopGen"] = busStopGen_;
    allProps_["UrbanFenceGen"] = urbanFenceGen_;
    allProps_["BollardGen"] = bollardGen_;
    allProps_["JerseyBarrierGen"] = jerseyBarrierGen_;
    allProps_["SecurityGateGen"] = securityGateGen_;
    allProps_["PrivacyFenceGen"] = privacyFenceGen_;

    allProps_["FenceGen"] = fenceGen_;
    allProps_["HayBaleGen"] = hayBaleGen_;
    allProps_["WaterTroughGen"] = waterTroughGen_;
    allProps_["GrainSiloGen"] = grainSiloGen_;
    allProps_["FarmWindmillGen"] = farmWindmillGen_;
    allProps_["FarmWagonGen"] = farmWagonGen_;
    allProps_["FarmGateGen"] = farmGateGen_;

    allProps_["DeskGen"] = deskGen_;
    allProps_["ChairGen"] = chairGen_;
    allProps_["ShelfGen"] = shelfGen_;

    allProps_["InfoBoardGen"] = infoBoardGen_;
    allProps_["KilometreMarkerGen"] = kilometreMarkerGen_;

    allProps_["ElectricPostGen"] = electricPostGen_;
    allProps_["TransformerGen"] = transformerGen_;
    allProps_["PowerMeterGen"] = powerMeterGen_;
    allProps_["StreetLightGen"] = streetLightGen_;
    allProps_["WaterTowerGen"] = waterTowerGen_;
    allProps_["WaterTankGen"] = waterTankGen_;
    allProps_["TelecomTowerGen"] = telecomTowerGen_;

    allProps_["WindTurbineGen"] = windTurbineGen_;
    allProps_["SolarPanelGen"] = solarPanelGen_;
    allProps_["SolarFarmGen"] = solarFarmGen_;
    allProps_["BatteryBankGen"] = batteryBankGen_;
    allProps_["SubstationGen"] = substationGen_;

    allProps_["RadioMastGen"] = radioMastGen_;
    allProps_["SatelliteDishGen"] = satelliteDishGen_;

    allProps_["EstateGateGen"] = estateGateGen_;
    allProps_["TollGateGen"] = tollGateGen_;
    allProps_["RailwayGateGen"] = railwayGateGen_;
    allProps_["GardenGateGen"] = gardenGateGen_;

    allProps_["ModernGlassFenceGen"] = modernGlassFenceGen_;
    allProps_["AluminumFinFenceGen"] = aluminumFinFenceGen_;
    allProps_["LaserCutFenceGen"] = laserCutFenceGen_;
    allProps_["GabionStoneFenceGen"] = gabionStoneFenceGen_;
    allProps_["ConcreteRibFenceGen"] = concreteRibFenceGen_;
    allProps_["LouverBladeFenceGen"] = louverBladeFenceGen_;
    allProps_["LivingWallFenceGen"] = livingWallFenceGen_;
    allProps_["BambooSteelFenceGen"] = bambooSteelFenceGen_;
    allProps_["CableRailFenceGen"] = cableRailFenceGen_;
    allProps_["CorrugatedCortenFenceGen"] = corrugatedCortenFenceGen_;

    allProps_["ModernSlatGateGen"] = modernSlatGateGen_;
    allProps_["GlassPivotGateGen"] = glassPivotGateGen_;
    allProps_["LaserArtGateGen"] = laserArtGateGen_;
    allProps_["BifoldSpeedGateGen"] = bifoldSpeedGateGen_;
    allProps_["AluminumLouverGateGen"] = aluminumLouverGateGen_;
    allProps_["BrutalistPortalGateGen"] = brutalistPortalGateGen_;
    allProps_["GlassSpeedGateGen"] = glassSpeedGateGen_;
    allProps_["JapaneseToriGateGen"] = japaneseToriGateGen_;
    allProps_["GabionTimberGateGen"] = gabionTimberGateGen_;
    allProps_["SolarSmartGateGen"] = solarSmartGateGen_;

    registerAllToRegistry();
}

void PropSystem::initialize(const GeneratorContext& ctx) {
    for (auto& [name, gen] : allProps_) {
        if (gen) gen->initialize(ctx);
    }
}

void PropSystem::registerAllToRegistry() {
    auto& reg = GeneratorRegistry::instance();
    for (auto& [name, gen] : allProps_) {
        if (gen) {
            reg.registerGenerator(gen);
        }
    }
}

std::shared_ptr<IGenerator> PropSystem::getPropGenerator(const std::string& name) {
    auto it = allProps_.find(name);
    return (it != allProps_.end()) ? it->second : nullptr;
}

GeneratedMeshPayload PropSystem::generateMesh(const std::string& propName, const GeneratorContext& ctx) {
    auto gen = getPropGenerator(propName);
    if (gen) {
        return gen->generateMesh(ctx);
    }
    return GeneratedMeshPayload{};
}

std::vector<std::string> PropSystem::getCategories() const {
    return {"street", "signage_street", "industrial", "urban", "rural", "fences", "gates", "interior", "utility", "energy", "communication"};
}

std::vector<std::string> PropSystem::getPropsForCategory(const std::string& category) const {
    if (category == "street") {
        return {"LampPostGen", "TrafficLightGen", "BenchGen", "TrashBinGen", "ExpandableBarricadeGen", "FlagPostGen"};
    } else if (category == "signage_street") {
        return {"InfoBoardGen", "KilometreMarkerGen"};
    } else if (category == "industrial") {
        return {"CrateGen", "BarrelGen", "PalletGen", "PipeGen"};
    } else if (category == "urban") {
        return {"HydrantGen", "MailboxGen", "BusStopGen", "BollardGen", "JerseyBarrierGen"};
    } else if (category == "rural") {
        return {"HayBaleGen", "WaterTroughGen", "GrainSiloGen", "FarmWindmillGen", "FarmWagonGen"};
    } else if (category == "fences") {
        return {
            "SecurityFenceGen", "UrbanFenceGen", "PrivacyFenceGen", "FenceGen",
            "ModernGlassFenceGen", "AluminumFinFenceGen", "LaserCutFenceGen",
            "GabionStoneFenceGen", "ConcreteRibFenceGen", "LouverBladeFenceGen",
            "LivingWallFenceGen", "BambooSteelFenceGen", "CableRailFenceGen",
            "CorrugatedCortenFenceGen"
        };
    } else if (category == "gates") {
        return {
            "SecurityGateGen", "EstateGateGen", "FarmGateGen",
            "TollGateGen", "RailwayGateGen", "GardenGateGen",
            "ModernSlatGateGen", "GlassPivotGateGen", "LaserArtGateGen",
            "BifoldSpeedGateGen", "AluminumLouverGateGen", "BrutalistPortalGateGen",
            "GlassSpeedGateGen", "JapaneseToriGateGen", "GabionTimberGateGen",
            "SolarSmartGateGen"
        };
    } else if (category == "interior") {
        return {"DeskGen", "ChairGen", "ShelfGen"};
    } else if (category == "utility") {
        return {"ElectricPostGen", "TransformerGen", "PowerMeterGen", "StreetLightGen", "WaterTowerGen", "WaterTankGen", "TelecomTowerGen"};
    } else if (category == "energy") {
        return {"ElectricPostGen", "WindTurbineGen", "SolarPanelGen", "SolarFarmGen", "BatteryBankGen", "SubstationGen"};
    } else if (category == "communication") {
        return {"RadioMastGen", "SatelliteDishGen"};
    }
    return {};
}

std::string PropSystem::getDiagnostics() const {
    std::ostringstream ss;
    ss << "=== Procedural Prop System Diagnostics ===\n";
    ss << "Categories: 11 (street, signage_street, industrial, urban, rural, fences, gates, interior, utility, energy, communication)\n";
    ss << "Total Registered Props: " << allProps_.size() << "\n";
    ss << "Geometry Dispatch: Dynamic procedural C++ vertex/index synthesis\n";
    for (const auto& [name, gen] : allProps_) {
        GeneratorContext ctx;
        auto payload = gen->generateMesh(ctx);
        ss << " • " << name << " -> " << payload.vertexCount << " verts, "
           << (payload.indexCount / 3) << " tris\n";
    }
    return ss.str();
}

} // namespace procedural::props
