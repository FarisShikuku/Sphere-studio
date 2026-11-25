#pragma once

#include "../../IGenerator.h"
#include "../PropMeshHelper.h"

namespace procedural::props {

struct MailboxParams {
    float height = 1.25f;          // Height in meters
    int boxStyle = 0;              // 0: US Street Blue Collection Box, 1: British Red Pillar Box, 2: Suburban Post Mounted
    bool hasCollectionPlate = true;
    bool hasFlag = false;          // For suburban mailbox
    bool flapOpen = false;
};

class MailboxGen : public IGenerator {
public:
    MailboxGen();
    ~MailboxGen() override = default;

    const char* getName() const override { return "MailboxGen"; }
    GeneratorCategory getCategory() const override { return GeneratorCategory::Props; }

    bool initialize(const GeneratorContext& ctx) override;
    void update(float timeSeconds, float dt) override;
    GeneratedMeshPayload generateMesh(const GeneratorContext& ctx) override;

    void configure(const MailboxParams& params) { params_ = params; }
    const MailboxParams& getParams() const { return params_; }

private:
    MailboxParams params_;
};

} // namespace procedural::props
