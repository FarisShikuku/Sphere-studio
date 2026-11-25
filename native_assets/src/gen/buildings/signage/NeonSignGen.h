#pragma once

#include "../../IGenerator.h"
#include "../BuildingTypes.h"
#include <vector>

namespace procedural::buildings {

enum class SignType {
    NeonGlow,
    Billboard,
    FasciaStorefront
};

struct NeonSignParams {
    SignType subType = SignType::Billboard;
    Vec3 position{0.0f, 0.0f, 0.0f};
    Vec3 normal{0.0f, 0.0f, 1.0f};
    float width = 8.0f;
    float height = 4.0f;
    float columnHeight = 6.0f;
    bool hasSpotlights = true;
    bool hasCatwalk = true;
    uint32_t glowColor = 0xFFFF0055; // Neon magenta
};

class NeonSignGen : public IGenerator {
public:
    NeonSignGen();
    ~NeonSignGen() override = default;

    const char* getName() const override { return "NeonSignGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Buildings; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const NeonSignParams& params) { params_ = params; }
    const NeonSignParams& getParameters() const { return params_; }

    void generateBillboard(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);
    void generateNeonSign(std::vector<BuildingVertex>& outVerts, std::vector<uint32_t>& outIndices);

private:
    NeonSignParams params_;
};

} // namespace procedural::buildings
