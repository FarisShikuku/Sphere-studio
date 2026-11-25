#pragma once

#include "../../IGenerator.h"
#include "../BuildingTypes.h"
#include "../parts/socket.h"
#include <vector>

namespace procedural::buildings {

enum class AttachmentType {
    AcUnit,
    Chimney,
    Antenna
};

struct PropAttachParams {
    AttachmentType subType = AttachmentType::AcUnit;
    Vec3 attachPosition{0.0f, 0.0f, 0.0f};
    Vec3 normal{0.0f, 1.0f, 0.0f};
    float scale = 1.0f;
    bool clusterUnits = false;
    int32_t clusterCount = 3;
};

class PropAttach : public IGenerator {
public:
    PropAttach();
    ~PropAttach() override = default;

    const char* getName() const override { return "PropAttach"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Buildings; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const PropAttachParams& params) { params_ = params; }
    const PropAttachParams& getParameters() const { return params_; }

    void generateAcUnit(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateChimney(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateAntenna(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);

private:
    PropAttachParams params_;
};

} // namespace procedural::buildings
