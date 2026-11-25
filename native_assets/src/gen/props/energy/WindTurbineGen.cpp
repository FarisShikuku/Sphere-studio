#include "WindTurbineGen.h"

namespace procedural::props {

WindTurbineGen::WindTurbineGen() {
    params_ = WindTurbineParams{};
}

bool WindTurbineGen::initialize(const GeneratorContext& /*ctx*/) {
    return true;
}

void WindTurbineGen::update(float /*timeSeconds*/, float /*dt*/) {
}

GeneratedMeshPayload WindTurbineGen::generateMesh(const GeneratorContext& /*ctx*/) {
    GeneratedMeshPayload payload;
    WindTurbineTower::generate(payload, params_);
    WindTurbineBlades::generate(payload, params_);
    return payload;
}

} // namespace procedural::props
