#pragma once
#include <string>
#include <memory>
#include "GeneratorContext.h"
#include "scene/SceneNode.h"

namespace sphere {

enum class GeneratorType {
    Water,
    Road,
    Vegetation,
    Surface,
    Building,
    Weather,
    Terrain,
    Vehicle,
    Prop,
    Sky,
    Particle,
    Audio
};

class IGenerator {
public:
    virtual ~IGenerator() = default;

    virtual const std::string& GetName() const = 0;
    virtual GeneratorType GetType() const = 0;
    virtual bool Generate(GeneratorContext& context, std::shared_ptr<SceneNode> outNode) = 0;
};
}\n