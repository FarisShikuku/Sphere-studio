#include "CivicGen.h"
#include "../BuildingComponents.h"

namespace procedural::buildings {

CivicGen::CivicGen() = default;

bool CivicGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void CivicGen::update(float /*timeSeconds*/, float /*dt*/) {}

GeneratedMeshPayload CivicGen::generateMesh(const GeneratorContext& /*ctx*/) {
    std::vector<BuildingVertex> verts;
    std::vector<uint32_t> indices;

    switch (params_.subType) {
        case CivicType::School:
            generateSchool(verts, indices);
            break;
        case CivicType::Hospital:
            generateHospital(verts, indices);
            break;
        case CivicType::TownHall:
            generateTownHall(verts, indices);
            break;
        case CivicType::BusTerminal:
            generateBusTerminal(verts, indices);
            break;
        case CivicType::Stadium:
            generateStadium(verts, indices);
            break;
        case CivicType::AirportTerminal:
            generateAirportTerminal(verts, indices);
            break;
        case CivicType::TrainStation:
            generateTrainStation(verts, indices);
            break;
        case CivicType::DomedStadium:
            generateDomedStadium(verts, indices);
            break;
        case CivicType::BaseballBallpark:
            generateBaseballBallpark(verts, indices);
            break;
        case CivicType::OlympicVelodrome:
            generateOlympicVelodrome(verts, indices);
            break;
        case CivicType::FireStation:
            generateFireStation(verts, indices);
            break;
        case CivicType::BotanicalGreenhouse:
            generateBotanicalGreenhouse(verts, indices);
            break;
        case CivicType::Observatory:
            generateObservatory(verts, indices);
            break;
        case CivicType::PublicLibraryMuseum:
            generatePublicLibraryMuseum(verts, indices);
            break;
        case CivicType::GothicCathedral:
            generateGothicCathedral(verts, indices);
            break;
        case CivicType::ParishChapel:
            generateParishChapel(verts, indices);
            break;
        case CivicType::ModernSanctuary:
            generateModernSanctuary(verts, indices);
            break;
        case CivicType::CommunityAmphitheater:
            generateCommunityAmphitheater(verts, indices);
            break;
        case CivicType::ParliamentTower:
            generateParliamentTower(verts, indices);
            break;
        case CivicType::KiccTower:
            generateKiccTower(verts, indices);
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
