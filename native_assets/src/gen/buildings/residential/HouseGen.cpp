#include "HouseGen.h"
#include "../BuildingComponents.h"
#include <cmath>

namespace procedural::buildings {

HouseGen::HouseGen() = default;

bool HouseGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void HouseGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload HouseGen::generateMesh(const GeneratorContext& /*ctx*/) {
    std::vector<BuildingVertex> verts;
    std::vector<uint32_t> indices;

    switch (params_.subType) {
        case ResidentialType::SuburbanHouse:
            generateSuburbanHouse(verts, indices);
            break;
        case ResidentialType::ApartmentBlock:
            generateApartmentBlock(verts, indices);
            break;
        case ResidentialType::Villa:
            generateVilla(verts, indices);
            break;
        case ResidentialType::RowhouseBrownstone:
            generateRowhouseBrownstone(verts, indices);
            break;
        case ResidentialType::CraftsmanTudor:
            generateCraftsmanTudor(verts, indices);
            break;
        case ResidentialType::MidCenturyRanch:
            generateMidCenturyRanch(verts, indices);
            break;
        case ResidentialType::ResidentialSkyscraper:
            generateResidentialSkyscraper(verts, indices);
            break;
        case ResidentialType::ModernDuplex:
            generateModernDuplex(verts, indices);
            break;
        case ResidentialType::CountryFarmhouse:
            generateCountryFarmhouse(verts, indices);
            break;
        case ResidentialType::ResidentialGarage:
            generateResidentialGarage(verts, indices);
            break;
        case ResidentialType::VictorianPaintedLady:
            generateVictorianPaintedLady(verts, indices);
            break;
        case ResidentialType::MediterraneanVilla:
            generateMediterraneanVilla(verts, indices);
            break;
        case ResidentialType::AFrameChalet:
            generateAFrameChalet(verts, indices);
            break;
        case ResidentialType::TinyHouseModular:
            generateTinyHouseModular(verts, indices);
            break;
        case ResidentialType::CapeCodCottage:
            generateCapeCodCottage(verts, indices);
            break;
        case ResidentialType::GeodesicEcoDome:
            generateGeodesicEcoDome(verts, indices);
            break;
        case ResidentialType::CourtyardAtriumHouse:
            generateCourtyardAtriumHouse(verts, indices);
            break;
        case ResidentialType::JapaneseMachiya:
            generateJapaneseMachiya(verts, indices);
            break;
        case ResidentialType::CantileverGlassHouse:
            generateCantileverGlassHouse(verts, indices);
            break;
        case ResidentialType::ColonialFourSquare:
            generateColonialFourSquare(verts, indices);
            break;
        case ResidentialType::MinimalistCubicVilla:
            generateMinimalistCubicVilla(verts, indices);
            break;
        case ResidentialType::BauhausResidence:
            generateBauhausResidence(verts, indices);
            break;
        case ResidentialType::DesertModernistPavilion:
            generateDesertModernistPavilion(verts, indices);
            break;
        case ResidentialType::BrutalistConcreteHome:
            generateBrutalistConcreteHome(verts, indices);
            break;
        case ResidentialType::JapaneseZenFlatHouse:
            generateJapaneseZenFlatHouse(verts, indices);
            break;
        case ResidentialType::ShippingContainerHome:
            generateShippingContainerHome(verts, indices);
            break;
        case ResidentialType::GreenRoofEcoVilla:
            generateGreenRoofEcoVilla(verts, indices);
            break;
        case ResidentialType::TropicalLouveredHouse:
            generateTropicalLouveredHouse(verts, indices);
            break;
        case ResidentialType::ClerestoryModernHome:
            generateClerestoryModernHome(verts, indices);
            break;
        case ResidentialType::PenthouseTerraceHouse:
            generatePenthouseTerraceHouse(verts, indices);
            break;
        case ResidentialType::TropicalGableManor:
            generateTropicalGableManor(verts, indices);
            break;
        case ResidentialType::RooftopLoungeCube:
            generateRooftopLoungeCube(verts, indices);
            break;
        case ResidentialType::ParapetConcealedBungalow:
            generateParapetConcealedBungalow(verts, indices);
            break;
        case ResidentialType::HipArchedBungalow:
            generateHipArchedBungalow(verts, indices);
            break;
        case ResidentialType::ObsidianLouverVilla:
            generateObsidianLouverVilla(verts, indices);
            break;
        case ResidentialType::CantileverVStrutMansion:
            generateCantileverVStrutMansion(verts, indices);
            break;
        case ResidentialType::StaggeredTerraceApartments:
            generateStaggeredTerraceApartments(verts, indices);
            break;
        case ResidentialType::HaloCanopyVilla:
            generateHaloCanopyVilla(verts, indices);
            break;
        case ResidentialType::ModernDualBoxCarportVilla:
            generateModernDualBoxCarportVilla(verts, indices);
            break;
        case ResidentialType::TaupeInterlockingFrameVilla:
            generateTaupeInterlockingFrameVilla(verts, indices);
            break;
        case ResidentialType::TwinWingSkybridgeApartments:
            generateTwinWingSkybridgeApartments(verts, indices);
            break;
        case ResidentialType::CorinthianPorticoModernVilla:
            generateCorinthianPorticoModernVilla(verts, indices);
            break;
        case ResidentialType::StoneAtriumPergolaVilla:
            generateStoneAtriumPergolaVilla(verts, indices);
            break;
        case ResidentialType::DiagridPanelCornerApartments:
            generateDiagridPanelCornerApartments(verts, indices);
            break;
        case ResidentialType::SlantedProwModernVilla:
            generateSlantedProwModernVilla(verts, indices);
            break;
        case ResidentialType::TimberBandPodiumApartments:
            generateTimberBandPodiumApartments(verts, indices);
            break;
        case ResidentialType::TriangularVoidGeometricVilla:
            generateTriangularVoidGeometricVilla(verts, indices);
            break;
        case ResidentialType::BrickPergolaPenthouseVilla:
            generateBrickPergolaPenthouseVilla(verts, indices);
            break;
        case ResidentialType::TwinStairCornerApartments:
            generateTwinStairCornerApartments(verts, indices);
            break;
        case ResidentialType::MinimalistRibbonBungalow:
            generateMinimalistRibbonBungalow(verts, indices);
            break;
        case ResidentialType::Condominium88Nairobi:
            generateCondominium88Nairobi(verts, indices);
            break;
        case ResidentialType::GtcResidential1:
            generateGtcResidential1(verts, indices);
            break;
        case ResidentialType::AlturaTower:
            generateAlturaTower(verts, indices);
            break;
    }

    GeneratedMeshPayload payload;
    payload.vertexStride = 9 * sizeof(float); // pos(3), norm(3), uv(2), mat(1)
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
