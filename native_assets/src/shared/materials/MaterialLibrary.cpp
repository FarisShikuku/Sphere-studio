#include "MaterialLibrary.h"

namespace sphere {

void MaterialLibrary::Boot() {
    MaterialRegistry::Get().Initialize();
}

const MaterialDef& MaterialLibrary::Find(MaterialID id) {
    return MaterialRegistry::Get().GetMaterial(id);
}

const MaterialDef& MaterialLibrary::FindByName(const std::string& name) {
    auto allIds = MaterialRegistry::Get().GetAllMaterialIDs();
    for (auto id : allIds) {
        const auto& mat = MaterialRegistry::Get().GetMaterial(id);
        if (mat.name == name) {
            return mat;
        }
    }
    return MaterialRegistry::Get().GetMaterial(MaterialID::BRICK_RED);
}

std::vector<std::string> MaterialLibrary::GetCategories() {
    return {
        "Masonry & Architecture",
        "Nature & Ground Surfaces",
        "Metals & Polymers",
        "Optical & Refractive",
        "Specialized & Organics"
    };
}

size_t MaterialLibrary::GetMaterialCount() {
    return MaterialRegistry::Get().GetAllMaterialIDs().size();
}

} // namespace sphere
