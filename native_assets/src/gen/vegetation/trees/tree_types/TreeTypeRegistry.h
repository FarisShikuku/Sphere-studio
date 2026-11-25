#pragma once
// ============================================================================
// TreeTypeRegistry.h - Unified Tree Types Factory & Manager
// ============================================================================

#include <memory>
#include <string>
#include <vector>
#include "TreeTypeBase.h"
#include "OakTreeType.h"
#include "PineTreeType.h"
#include "BirchTreeType.h"
#include "WillowTreeType.h"
#include "MapleTreeType.h"
#include "AcaciaTreeType.h"
#include "SpruceTreeType.h"
#include "BotanicalSpeciesTreeType.h"

namespace vegetation {

class TreeTypeRegistry {
public:
    static std::unique_ptr<TreeTypeBase> createTreeType(TreeSpeciesId id);
    static std::unique_ptr<TreeTypeBase> createTreeTypeByName(const std::string& name);

    static std::vector<std::string> getAvailableTreeTypeNames();
    static std::vector<TreeSpeciesId> getAvailableTreeTypeIds();
};

} // namespace vegetation
