#include "SurfaceRegistry.h"

namespace sphere {

SurfaceRegistry& SurfaceRegistry::Get() {
    static SurfaceRegistry instance;
    return instance;
}

SurfaceRegistry::SurfaceRegistry() {
    RegisterDefaultSurfaces();
}

void SurfaceRegistry::Initialize() {
    std::lock_guard<std::mutex> lock(m_Mutex);
    RegisterDefaultSurfaces();
}

const SurfaceDef& SurfaceRegistry::GetSurface(const std::string& id) const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    auto it = m_Surfaces.find(id);
    if (it != m_Surfaces.end()) {
        return it->second;
    }
    return m_Fallback;
}

void SurfaceRegistry::RegisterSurface(const SurfaceDef& surface) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Surfaces[surface.id] = surface;
}

std::vector<SurfaceDef> SurfaceRegistry::GetAllSurfaces() const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    std::vector<SurfaceDef> list;
    list.reserve(m_Surfaces.size());
    for (const auto& pair : m_Surfaces) {
        list.push_back(pair.second);
    }
    return list;
}

std::vector<SurfaceDef> SurfaceRegistry::GetSurfacesByCategory(const std::string& category) const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    std::vector<SurfaceDef> list;
    for (const auto& pair : m_Surfaces) {
        if (pair.second.category == category) {
            list.push_back(pair.second);
        }
    }
    return list;
}

void SurfaceRegistry::RegisterDefaultSurfaces() {
    m_Fallback.id = "fallback_surface";
    m_Fallback.name = "Default Composite Surface";
    m_Fallback.category = "Generic";

    // 1. Rusticated Red Brick Wall Surface (Clay Brick + Mortar)
    SurfaceDef brickWall;
    brickWall.id = "brick_wall_surface";
    brickWall.name = "Rusticated Red Brick Wall";
    brickWall.category = "Architectural";
    brickWall.description = "Composite masonry: fired clay brick blocks bound with recessed cementitious mortar joints.";
    brickWall.primaryMaterial = MaterialID::BRICK_RUSTICATED;
    brickWall.secondaryMaterial = MaterialID::CONCRETE_RAW; // Cement mortar
    brickWall.tertiaryMaterial = MaterialID::DIRT_DRY;      // Weathering dust
    brickWall.pattern = SurfacePatternType::BRICK_BOND_MASONRY;
    brickWall.surfaceMode = SurfaceMode::SURFACE_FLAT_TEXTURE;
    brickWall.primaryRatio = 0.88f;
    brickWall.secondaryRatio = 0.12f; // 12% mortar
    brickWall.macroTileSizeMeters = 0.90f;
    brickWall.displacementScale = 0.025f;
    brickWall.assetPath = "textures/rusticated-red-brick.jpg";
    m_Surfaces[brickWall.id] = brickWall;

    // 2. Mondo Grass Lawn Turf Surface (Living Grass + Thatch + Soil)
    SurfaceDef grassTurf;
    grassTurf.id = "grass_turf_surface";
    grassTurf.name = "Mondo Grass Lawn Surface";
    grassTurf.category = "Nature";
    grassTurf.description = "Composite ground surface: high-density chlorophyll blades blended with root thatch and dark organic loam.";
    grassTurf.primaryMaterial = MaterialID::GRASS_SURFACE;
    grassTurf.secondaryMaterial = MaterialID::DIRT_DRY;
    grassTurf.tertiaryMaterial = MaterialID::FOLIAGE_LEAF;
    grassTurf.pattern = SurfacePatternType::TURF_BLADE_ORGANIC;
    grassTurf.surfaceMode = SurfaceMode::SURFACE_FLAT_TEXTURE;
    grassTurf.primaryRatio = 0.75f;
    grassTurf.secondaryRatio = 0.25f;
    grassTurf.macroTileSizeMeters = 0.30f;
    grassTurf.displacementScale = 0.015f;
    grassTurf.assetPath = "textures/mondo-grass.jpg";
    m_Surfaces[grassTurf.id] = grassTurf;

    // 3. Earth with Foliage Surface (Topsoil + Ivy/Clover + Pebbles)
    SurfaceDef earthFoliage;
    earthFoliage.id = "earth_foliage_surface";
    earthFoliage.name = "Earth with Foliage Surface";
    earthFoliage.category = "Nature";
    earthFoliage.description = "Composite terrain: nutrient-rich topsoil substrate supporting creeping broadleaf foliage and basalt pebbles.";
    earthFoliage.primaryMaterial = MaterialID::EARTH_FOLIAGE;
    earthFoliage.secondaryMaterial = MaterialID::GRAVEL_CRUSHED;
    earthFoliage.tertiaryMaterial = MaterialID::FOLIAGE_LEAF;
    earthFoliage.pattern = SurfacePatternType::FOLIAGE_SOIL_SPLATMAP;
    earthFoliage.surfaceMode = SurfaceMode::SURFACE_FLAT_TEXTURE;
    earthFoliage.primaryRatio = 0.60f;
    earthFoliage.secondaryRatio = 0.40f;
    earthFoliage.macroTileSizeMeters = 0.70f;
    earthFoliage.displacementScale = 0.030f;
    earthFoliage.assetPath = "textures/earth-foliage.jpg";
    m_Surfaces[earthFoliage.id] = earthFoliage;

    // 4. Automotive Paint Surface (User example: metallic black/white glossy surface)
    SurfaceDef carPaint;
    carPaint.id = "car_paint_surface";
    carPaint.name = "Automotive Gloss Clearcoat Surface";
    carPaint.category = "Transportation";
    carPaint.description = "Multi-layer vehicle surface: structural steel body + base pigment coat + high-index polyurethane clearcoat.";
    carPaint.primaryMaterial = MaterialID::PAINT_GLOSS;
    carPaint.secondaryMaterial = MaterialID::METAL_STEEL;
    carPaint.tertiaryMaterial = MaterialID::GLASS_CLEAR; // Clearcoat reflection layer
    carPaint.pattern = SurfacePatternType::DUAL_LAYER_CLEARCOAT;
    carPaint.surfaceMode = SurfaceMode::SURFACE_FLAT_TEXTURE;
    carPaint.primaryRatio = 0.90f;
    carPaint.secondaryRatio = 0.10f;
    carPaint.macroTileSizeMeters = 2.0f;
    carPaint.displacementScale = 0.001f; // Mirror smooth
    m_Surfaces[carPaint.id] = carPaint;

    // 5. Wood Plank Floor Surface (User example: wood floor)
    SurfaceDef woodFloor;
    woodFloor.id = "wood_floor_surface";
    woodFloor.name = "Varnished Oak Wood Floor Surface";
    woodFloor.category = "Interior";
    woodFloor.description = "Composite architectural floor: natural oak grain planks joined with bevel tongue-and-groove shadows and satin lacquer finish.";
    woodFloor.primaryMaterial = MaterialID::BARK_OAK;
    woodFloor.secondaryMaterial = MaterialID::PAINT_GLOSS; // Polyurethane finish
    woodFloor.tertiaryMaterial = MaterialID::RUBBER_SMOOTH; // Gap shadow
    woodFloor.pattern = SurfacePatternType::PLANK_GRAIN_FLOOR;
    woodFloor.surfaceMode = SurfaceMode::SURFACE_FLAT_TEXTURE;
    woodFloor.primaryRatio = 0.92f;
    woodFloor.secondaryRatio = 0.08f;
    woodFloor.macroTileSizeMeters = 1.20f;
    woodFloor.displacementScale = 0.005f;
    m_Surfaces[woodFloor.id] = woodFloor;

    // 6. Ceramic Tile Surface (User example: tiles house)
    SurfaceDef ceramicTile;
    ceramicTile.id = "ceramic_tile_surface";
    ceramicTile.name = "Glazed Ceramic Tile Surface";
    ceramicTile.category = "Architectural";
    ceramicTile.description = "Interior/exterior architectural surface: vitrified ceramic glazed tiles surrounded by anti-microbial cementitious grout lines.";
    ceramicTile.primaryMaterial = MaterialID::TILE_CERAMIC;
    ceramicTile.secondaryMaterial = MaterialID::CONCRETE_POLISHED; // Grout
    ceramicTile.tertiaryMaterial = MaterialID::GLASS_CLEAR;
    ceramicTile.pattern = SurfacePatternType::TILED_GROUT_GRID;
    ceramicTile.surfaceMode = SurfaceMode::SURFACE_FLAT_TEXTURE;
    ceramicTile.primaryRatio = 0.93f;
    ceramicTile.secondaryRatio = 0.07f;
    ceramicTile.macroTileSizeMeters = 0.60f;
    ceramicTile.displacementScale = 0.008f;
    m_Surfaces[ceramicTile.id] = ceramicTile;

    // 7. Pathway / Road Surface (User example: pathways)
    SurfaceDef pathway;
    pathway.id = "pathway_road_surface";
    pathway.name = "Urban Pathway & Asphalt Surface";
    pathway.category = "Infrastructure";
    pathway.description = "Composite exterior ground: bitumen aggregate asphalt base marked with reflective road stripes and wet rainwater sheen.";
    pathway.primaryMaterial = MaterialID::ASPHALT_WET;
    pathway.secondaryMaterial = MaterialID::PAINT_GLOSS; // Road marking paint
    pathway.tertiaryMaterial = MaterialID::WATER_SURFACE; // Rain puddles
    pathway.pattern = SurfacePatternType::ASPHALT_ROAD_MARKING;
    pathway.surfaceMode = SurfaceMode::SURFACE_FLAT_TEXTURE;
    pathway.primaryRatio = 0.85f;
    pathway.secondaryRatio = 0.15f;
    pathway.macroTileSizeMeters = 3.0f;
    pathway.displacementScale = 0.012f;
    m_Surfaces[pathway.id] = pathway;
}

} // namespace sphere
