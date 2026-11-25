#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct ShelfParams {
    float width = 1.0f;            // Width in meters (0.6 to 2.2)
    float height = 1.8f;           // Total height (1.0 to 2.4)
    float depth = 0.35f;           // Shelf depth (0.25 to 0.50)
    int tierCount = 5;             // Number of shelf tiers (3 to 7)
    bool hasBackboard = true;      // Solid back panel
    int shelfMaterial = 0;         // 0: Light Birch, 1: Industrial Wire/Steel, 2: Dark Walnut
    bool hasBooksAndObjects = true;// Procedural books, binders, and vases on shelves
};

class ShelfGen : public IGenerator {
public:
    ShelfGen();
    ~ShelfGen() override = default;

    const char* getName() const override { return "ShelfGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const ShelfParams& params) { params_ = params; }
    const ShelfParams& getParams() const { return params_; }

private:
    ShelfParams params_;
};

} // namespace procedural::props
