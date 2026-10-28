#pragma once
#include "MaterialRegistry.h"

namespace sphere {

class MaterialLibrary {
public:
    static void Boot();
    static const MaterialDef& Find(MaterialID id);
    static const MaterialDef& FindByName(const std::string& name);
    static std::vector<std::string> GetCategories();
    static size_t GetMaterialCount();
};

} // namespace sphere
