#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "../IGenerator.h"
#include "../GeneratorRegistry.h"

#include "street/LampPostGen.h"
#include "street/TrafficLightGen.h"
#include "street/BenchGen.h"
#include "street/TrashBinGen.h"
#include "street/ExpandableBarricadeGen.h"
#include "street/FlagPostGen.h"

#include "industrial/CrateGen.h"
#include "industrial/BarrelGen.h"
#include "industrial/PalletGen.h"
#include "industrial/PipeGen.h"
#include "industrial/SecurityFenceGen.h"

#include "urban/HydrantGen.h"
#include "urban/MailboxGen.h"
#include "urban/BusStopGen.h"
#include "urban/UrbanFenceGen.h"
#include "urban/BollardGen.h"
#include "urban/JerseyBarrierGen.h"
#include "urban/SecurityGateGen.h"
#include "urban/PrivacyFenceGen.h"

#include "rural/FenceGen.h"
#include "rural/HayBaleGen.h"
#include "rural/WaterTroughGen.h"
#include "rural/GrainSiloGen.h"
#include "rural/FarmWindmillGen.h"
#include "rural/FarmWagonGen.h"
#include "rural/FarmGateGen.h"

#include "interior/DeskGen.h"
#include "interior/ChairGen.h"
#include "interior/ShelfGen.h"

#include "signage_street/InfoBoardGen.h"
#include "signage_street/KilometreMarkerGen.h"

#include "utility/ElectricPostGen.h"
#include "utility/TransformerGen.h"
#include "utility/PowerMeterGen.h"
#include "utility/StreetLightGen.h"
#include "utility/WaterTowerGen.h"
#include "utility/WaterTankGen.h"
#include "utility/TelecomTowerGen.h"

#include "energy/WindTurbineGen.h"
#include "energy/SolarPanelGen.h"
#include "energy/SolarFarmGen.h"
#include "energy/BatteryBankGen.h"
#include "energy/SubstationGen.h"

#include "communication/RadioMastGen.h"
#include "communication/SatelliteDishGen.h"

#include "gates/EstateGateGen.h"
#include "gates/TollGateGen.h"
#include "gates/RailwayGateGen.h"
#include "gates/GardenGateGen.h"
#include "gates/ModernGatesGen.h"
#include "fences/ModernFencesGen.h"

namespace procedural::props {

class PropSystem {
public:
    static PropSystem& instance();

    void initialize(const GeneratorContext& ctx);
    void registerAllToRegistry();

    std::shared_ptr<IGenerator> getPropGenerator(const std::string& name);
    GeneratedMeshPayload generateMesh(const std::string& propName, const GeneratorContext& ctx);

    std::vector<std::string> getCategories() const;
    std::vector<std::string> getPropsForCategory(const std::string& category) const;
    std::string getDiagnostics() const;

    // Direct typed generator getters
    std::shared_ptr<LampPostGen> getLampPostGen() const { return lampPostGen_; }
    std::shared_ptr<TrafficLightGen> getTrafficLightGen() const { return trafficLightGen_; }
    std::shared_ptr<BenchGen> getBenchGen() const { return benchGen_; }
    std::shared_ptr<TrashBinGen> getTrashBinGen() const { return trashBinGen_; }
    std::shared_ptr<FlagPostGen> getFlagPostGen() const { return flagPostGen_; }

    std::shared_ptr<CrateGen> getCrateGen() const { return crateGen_; }
    std::shared_ptr<BarrelGen> getBarrelGen() const { return barrelGen_; }
    std::shared_ptr<PalletGen> getPalletGen() const { return palletGen_; }
    std::shared_ptr<PipeGen> getPipeGen() const { return pipeGen_; }

    std::shared_ptr<HydrantGen> getHydrantGen() const { return hydrantGen_; }
    std::shared_ptr<MailboxGen> getMailboxGen() const { return mailboxGen_; }
    std::shared_ptr<BusStopGen> getBusStopGen() const { return busStopGen_; }
    std::shared_ptr<UrbanFenceGen> getUrbanFenceGen() const { return urbanFenceGen_; }
    std::shared_ptr<BollardGen> getBollardGen() const { return bollardGen_; }
    std::shared_ptr<JerseyBarrierGen> getJerseyBarrierGen() const { return jerseyBarrierGen_; }

    std::shared_ptr<FenceGen> getFenceGen() const { return fenceGen_; }
    std::shared_ptr<HayBaleGen> getHayBaleGen() const { return hayBaleGen_; }
    std::shared_ptr<WaterTroughGen> getWaterTroughGen() const { return waterTroughGen_; }
    std::shared_ptr<GrainSiloGen> getGrainSiloGen() const { return grainSiloGen_; }
    std::shared_ptr<FarmWindmillGen> getFarmWindmillGen() const { return farmWindmillGen_; }
    std::shared_ptr<FarmWagonGen> getFarmWagonGen() const { return farmWagonGen_; }

    std::shared_ptr<DeskGen> getDeskGen() const { return deskGen_; }
    std::shared_ptr<ChairGen> getChairGen() const { return chairGen_; }
    std::shared_ptr<ShelfGen> getShelfGen() const { return shelfGen_; }

    std::shared_ptr<InfoBoardGen> getInfoBoardGen() const { return infoBoardGen_; }
    std::shared_ptr<KilometreMarkerGen> getKilometreMarkerGen() const { return kilometreMarkerGen_; }

    std::shared_ptr<ElectricPostGen> getElectricPostGen() const { return electricPostGen_; }
    std::shared_ptr<TransformerGen> getTransformerGen() const { return transformerGen_; }
    std::shared_ptr<PowerMeterGen> getPowerMeterGen() const { return powerMeterGen_; }
    std::shared_ptr<StreetLightGen> getStreetLightGen() const { return streetLightGen_; }
    std::shared_ptr<WaterTowerGen> getWaterTowerGen() const { return waterTowerGen_; }
    std::shared_ptr<WaterTankGen> getWaterTankGen() const { return waterTankGen_; }
    std::shared_ptr<TelecomTowerGen> getTelecomTowerGen() const { return telecomTowerGen_; }

    std::shared_ptr<WindTurbineGen> getWindTurbineGen() const { return windTurbineGen_; }
    std::shared_ptr<SolarPanelGen> getSolarPanelGen() const { return solarPanelGen_; }
    std::shared_ptr<SolarFarmGen> getSolarFarmGen() const { return solarFarmGen_; }
    std::shared_ptr<BatteryBankGen> getBatteryBankGen() const { return batteryBankGen_; }
    std::shared_ptr<SubstationGen> getSubstationGen() const { return substationGen_; }

    std::shared_ptr<RadioMastGen> getRadioMastGen() const { return radioMastGen_; }
    std::shared_ptr<SatelliteDishGen> getSatelliteDishGen() const { return satelliteDishGen_; }

    std::shared_ptr<ExpandableBarricadeGen> getExpandableBarricadeGen() const { return expandableBarricadeGen_; }
    std::shared_ptr<SecurityFenceGen> getSecurityFenceGen() const { return securityFenceGen_; }
    std::shared_ptr<SecurityGateGen> getSecurityGateGen() const { return securityGateGen_; }
    std::shared_ptr<PrivacyFenceGen> getPrivacyFenceGen() const { return privacyFenceGen_; }
    std::shared_ptr<FarmGateGen> getFarmGateGen() const { return farmGateGen_; }
    std::shared_ptr<EstateGateGen> getEstateGateGen() const { return estateGateGen_; }
    std::shared_ptr<TollGateGen> getTollGateGen() const { return tollGateGen_; }
    std::shared_ptr<RailwayGateGen> getRailwayGateGen() const { return railwayGateGen_; }
    std::shared_ptr<GardenGateGen> getGardenGateGen() const { return gardenGateGen_; }

    std::shared_ptr<ModernGlassFenceGen> getModernGlassFenceGen() const { return modernGlassFenceGen_; }
    std::shared_ptr<AluminumFinFenceGen> getAluminumFinFenceGen() const { return aluminumFinFenceGen_; }
    std::shared_ptr<LaserCutFenceGen> getLaserCutFenceGen() const { return laserCutFenceGen_; }
    std::shared_ptr<GabionStoneFenceGen> getGabionStoneFenceGen() const { return gabionStoneFenceGen_; }
    std::shared_ptr<ConcreteRibFenceGen> getConcreteRibFenceGen() const { return concreteRibFenceGen_; }
    std::shared_ptr<LouverBladeFenceGen> getLouverBladeFenceGen() const { return louverBladeFenceGen_; }
    std::shared_ptr<LivingWallFenceGen> getLivingWallFenceGen() const { return livingWallFenceGen_; }
    std::shared_ptr<BambooSteelFenceGen> getBambooSteelFenceGen() const { return bambooSteelFenceGen_; }
    std::shared_ptr<CableRailFenceGen> getCableRailFenceGen() const { return cableRailFenceGen_; }
    std::shared_ptr<CorrugatedCortenFenceGen> getCorrugatedCortenFenceGen() const { return corrugatedCortenFenceGen_; }

    std::shared_ptr<ModernSlatGateGen> getModernSlatGateGen() const { return modernSlatGateGen_; }
    std::shared_ptr<GlassPivotGateGen> getGlassPivotGateGen() const { return glassPivotGateGen_; }
    std::shared_ptr<LaserArtGateGen> getLaserArtGateGen() const { return laserArtGateGen_; }
    std::shared_ptr<BifoldSpeedGateGen> getBifoldSpeedGateGen() const { return bifoldSpeedGateGen_; }
    std::shared_ptr<AluminumLouverGateGen> getAluminumLouverGateGen() const { return aluminumLouverGateGen_; }
    std::shared_ptr<BrutalistPortalGateGen> getBrutalistPortalGateGen() const { return brutalistPortalGateGen_; }
    std::shared_ptr<GlassSpeedGateGen> getGlassSpeedGateGen() const { return glassSpeedGateGen_; }
    std::shared_ptr<JapaneseToriGateGen> getJapaneseToriGateGen() const { return japaneseToriGateGen_; }
    std::shared_ptr<GabionTimberGateGen> getGabionTimberGateGen() const { return gabionTimberGateGen_; }
    std::shared_ptr<SolarSmartGateGen> getSolarSmartGateGen() const { return solarSmartGateGen_; }

private:
    PropSystem();

    std::shared_ptr<LampPostGen> lampPostGen_;
    std::shared_ptr<TrafficLightGen> trafficLightGen_;
    std::shared_ptr<BenchGen> benchGen_;
    std::shared_ptr<TrashBinGen> trashBinGen_;
    std::shared_ptr<ExpandableBarricadeGen> expandableBarricadeGen_;
    std::shared_ptr<FlagPostGen> flagPostGen_;

    std::shared_ptr<CrateGen> crateGen_;
    std::shared_ptr<BarrelGen> barrelGen_;
    std::shared_ptr<PalletGen> palletGen_;
    std::shared_ptr<PipeGen> pipeGen_;
    std::shared_ptr<SecurityFenceGen> securityFenceGen_;

    std::shared_ptr<HydrantGen> hydrantGen_;
    std::shared_ptr<MailboxGen> mailboxGen_;
    std::shared_ptr<BusStopGen> busStopGen_;
    std::shared_ptr<UrbanFenceGen> urbanFenceGen_;
    std::shared_ptr<BollardGen> bollardGen_;
    std::shared_ptr<JerseyBarrierGen> jerseyBarrierGen_;
    std::shared_ptr<SecurityGateGen> securityGateGen_;
    std::shared_ptr<PrivacyFenceGen> privacyFenceGen_;

    std::shared_ptr<FenceGen> fenceGen_;
    std::shared_ptr<HayBaleGen> hayBaleGen_;
    std::shared_ptr<WaterTroughGen> waterTroughGen_;
    std::shared_ptr<GrainSiloGen> grainSiloGen_;
    std::shared_ptr<FarmWindmillGen> farmWindmillGen_;
    std::shared_ptr<FarmWagonGen> farmWagonGen_;
    std::shared_ptr<FarmGateGen> farmGateGen_;

    std::shared_ptr<DeskGen> deskGen_;
    std::shared_ptr<ChairGen> chairGen_;
    std::shared_ptr<ShelfGen> shelfGen_;

    std::shared_ptr<InfoBoardGen> infoBoardGen_;
    std::shared_ptr<KilometreMarkerGen> kilometreMarkerGen_;

    std::shared_ptr<ElectricPostGen> electricPostGen_;
    std::shared_ptr<TransformerGen> transformerGen_;
    std::shared_ptr<PowerMeterGen> powerMeterGen_;
    std::shared_ptr<StreetLightGen> streetLightGen_;
    std::shared_ptr<WaterTowerGen> waterTowerGen_;
    std::shared_ptr<WaterTankGen> waterTankGen_;
    std::shared_ptr<TelecomTowerGen> telecomTowerGen_;

    std::shared_ptr<WindTurbineGen> windTurbineGen_;
    std::shared_ptr<SolarPanelGen> solarPanelGen_;
    std::shared_ptr<SolarFarmGen> solarFarmGen_;
    std::shared_ptr<BatteryBankGen> batteryBankGen_;
    std::shared_ptr<SubstationGen> substationGen_;

    std::shared_ptr<RadioMastGen> radioMastGen_;
    std::shared_ptr<SatelliteDishGen> satelliteDishGen_;

    std::shared_ptr<EstateGateGen> estateGateGen_;
    std::shared_ptr<TollGateGen> tollGateGen_;
    std::shared_ptr<RailwayGateGen> railwayGateGen_;
    std::shared_ptr<GardenGateGen> gardenGateGen_;

    std::shared_ptr<ModernGlassFenceGen> modernGlassFenceGen_;
    std::shared_ptr<AluminumFinFenceGen> aluminumFinFenceGen_;
    std::shared_ptr<LaserCutFenceGen> laserCutFenceGen_;
    std::shared_ptr<GabionStoneFenceGen> gabionStoneFenceGen_;
    std::shared_ptr<ConcreteRibFenceGen> concreteRibFenceGen_;
    std::shared_ptr<LouverBladeFenceGen> louverBladeFenceGen_;
    std::shared_ptr<LivingWallFenceGen> livingWallFenceGen_;
    std::shared_ptr<BambooSteelFenceGen> bambooSteelFenceGen_;
    std::shared_ptr<CableRailFenceGen> cableRailFenceGen_;
    std::shared_ptr<CorrugatedCortenFenceGen> corrugatedCortenFenceGen_;

    std::shared_ptr<ModernSlatGateGen> modernSlatGateGen_;
    std::shared_ptr<GlassPivotGateGen> glassPivotGateGen_;
    std::shared_ptr<LaserArtGateGen> laserArtGateGen_;
    std::shared_ptr<BifoldSpeedGateGen> bifoldSpeedGateGen_;
    std::shared_ptr<AluminumLouverGateGen> aluminumLouverGateGen_;
    std::shared_ptr<BrutalistPortalGateGen> brutalistPortalGateGen_;
    std::shared_ptr<GlassSpeedGateGen> glassSpeedGateGen_;
    std::shared_ptr<JapaneseToriGateGen> japaneseToriGateGen_;
    std::shared_ptr<GabionTimberGateGen> gabionTimberGateGen_;
    std::shared_ptr<SolarSmartGateGen> solarSmartGateGen_;

    std::unordered_map<std::string, std::shared_ptr<IGenerator>> allProps_;
};

} // namespace procedural::props
