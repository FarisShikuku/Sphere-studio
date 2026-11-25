#include "TowerGen.h"
#include "../BuildingComponents.h"

namespace procedural::buildings {

TowerGen::TowerGen() = default;

bool TowerGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void TowerGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload TowerGen::generateMesh(const GeneratorContext& /*ctx*/) {
    std::vector<BuildingVertex> verts;
    std::vector<uint32_t> indices;

    switch (params_.subType) {
        case CommercialType::OfficeTower:
            generateOfficeTower(verts, indices);
            break;
        case CommercialType::ShoppingMall:
            generateMall(verts, indices);
            break;
        case CommercialType::SmallShop:
            generateSmallShop(verts, indices);
            break;
        case CommercialType::PetrolStation:
            generatePetrolStation(verts, indices);
            break;
        case CommercialType::CarWash:
            generateCarWash(verts, indices);
            break;
        case CommercialType::MegaMall:
            generateMegaMall(verts, indices);
            break;
        case CommercialType::Supermarket:
            generateSupermarket(verts, indices);
            break;
        case CommercialType::AutoDealership:
            generateAutoDealership(verts, indices);
            break;
        case CommercialType::FastFood:
            generateFastFood(verts, indices);
            break;
        case CommercialType::Hotel:
            generateHotel(verts, indices);
            break;
        case CommercialType::EvChargingStation:
            generateEvChargingStation(verts, indices);
            break;
        case CommercialType::ServiceGarage:
            generateServiceGarage(verts, indices);
            break;
        case CommercialType::ParkingGarage:
            generateParkingGarage(verts, indices);
            break;
        case CommercialType::SelfStorage:
            generateSelfStorage(verts, indices);
            break;
        case CommercialType::ContainerShop:
            generateContainerShop(verts, indices);
            break;
        case CommercialType::CoffeeKiosk:
            generateCoffeeKiosk(verts, indices);
            break;
        case CommercialType::BoutiqueHotel:
            generateBoutiqueHotel(verts, indices);
            break;
        case CommercialType::MarketHall:
            generateMarketHall(verts, indices);
            break;
        case CommercialType::OpenFarmersMarket:
            generateOpenFarmersMarket(verts, indices);
            break;
        case CommercialType::NightMarketPlaza:
            generateNightMarketPlaza(verts, indices);
            break;
        case CommercialType::SweepingRibbonCampus:
            generateSweepingRibbonCampus(verts, indices);
            break;
        case CommercialType::CorporateGlassPark:
            generateCorporateGlassPark(verts, indices);
            break;
        case CommercialType::EcoTerraceSpire:
            generateEcoTerraceSpire(verts, indices);
            break;
        case CommercialType::GlassPilasterHQ:
            generateGlassPilasterHQ(verts, indices);
            break;
        case CommercialType::DiagonalStrutPavilion:
            generateDiagonalStrutPavilion(verts, indices);
            break;
        case CommercialType::BritamTower:
            generateBritamTower(verts, indices);
            break;
        case CommercialType::GtcOfficeTower:
            generateGtcOfficeTower(verts, indices);
            break;
        case CommercialType::UapOldMutualTower:
            generateUapOldMutualTower(verts, indices);
            break;
        case CommercialType::TimesTower:
            generateTimesTower(verts, indices);
            break;
        case CommercialType::PrismTower:
            generatePrismTower(verts, indices);
            break;
        case CommercialType::LemacTower:
            generateLemacTower(verts, indices);
            break;
        case CommercialType::TelepostaHouse:
            generateTelepostaHouse(verts, indices);
            break;
        case CommercialType::CbkPensionTower:
            generateCbkPensionTower(verts, indices);
            break;
        case CommercialType::KcbPlaza:
            generateKcbPlaza(verts, indices);
            break;
        case CommercialType::UpperHillChambers:
            generateUpperHillChambers(verts, indices);
            break;
        case CommercialType::NssfBuilding:
            generateNssfBuilding(verts, indices);
            break;
    }

    GeneratedMeshPayload payload;
    payload.vertexStride = 9 * sizeof(float);
    payload.vertexCount = static_cast<uint32_t>(verts.size());
    payload.indexCount = static_cast<uint32_t>(indices.size());
    payload.indexBuffer = indices;

    payload.vertexBuffer.reserve(verts.size() * 9);
    for (const auto& v : verts) {
        payload.vertexBuffer.push_back(v.position.x);
        payload.vertexBuffer.push_back(v.position.y);
        payload.vertexBuffer.push_back(v.position.z);
        payload.vertexBuffer.push_back(v.normal.x);
        payload.vertexBuffer.push_back(v.normal.y);
        payload.vertexBuffer.push_back(v.normal.z);
        payload.vertexBuffer.push_back(v.uv.x);
        payload.vertexBuffer.push_back(v.uv.y);
        payload.vertexBuffer.push_back(static_cast<float>(v.materialId));
    }

    return payload;
}

} // namespace procedural::buildings
