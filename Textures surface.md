

This folder acts as a single, global repository for all surface definitions. It is completely independent of the src/ categories (buildings, props, road, etc.). Any geometry generator can simply request MaterialID::BRICK_RED or MaterialID::WOOD_OAK from this library.

```text
native-assets/
└── materials/
    ├── CMakeLists.txt                             # Builds the material shared library (libmaterials.a)
    ├── README.md                                  # Documents the Material Registry and how to add new materials
    │
    ├── library/                                   # Central Registry & Interfaces
    │   ├── include/
    │   │   ├── MaterialID.h                       # enum class MaterialID { BRICK_RED, WOOD_OAK, ASPHALT_WET, ... }
    │   │   ├── MaterialDef.h                      # struct MaterialDef { VkFormat formats, float roughness_bias, ... }
    │   │   ├── MaterialRegistry.h                 # Singleton: Register(), Get(), RequestImage()
    │   │   └── IMaterial.h                        # Base interface for all material generators
    │   └── src/
    │       ├── MaterialID.cpp
    │       ├── MaterialDef.cpp
    │       ├── MaterialRegistry.cpp               # Handles lazy generation & caching of VkImages
    │       └── IMaterial.cpp
    │
    ├── brick/                                     # BRICK (Used by: House, Fence, Studio Wall, Ruins)
    │   ├── include/
    │   │   ├── BrickMaterial.h                    # Derives from IMaterial
    │   │   └── BrickVariants.h                    # constexpr: RED, GREY, WHITE, WEATHERED, BLACK
    │   ├── src/
    │   │   ├── BrickMaterial.cpp                  # Orchestrates compute dispatch
    │   │   └── BrickVariants.cpp
    │   ├── shaders/
    │   │   ├── include/
    │   │   │   └── brick_common.glsl              # Brick-specific math & layout
    │   │   ├── brick_albedo.comp                  # Output: BC7_UNORM_SRGB
    │   │   ├── brick_normal.comp                  # Output: BC5_UNORM
    │   │   ├── brick_orm.comp                     # Output: BC7_UNORM (AO/Rough/Metal)
    │   │   └── brick_height.comp                  # Output: BC4_UNORM (Displacement)
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── brick_seed.bin                 # uint32 noise seeds for brick color variation
    │   │   └── spec/
    │   │       └── brick_spec.h                   # Resolution, tile size, VkFormat declarations
    │   └── README.md
    │
    ├── wood/                                      # WOOD (Used by: Floor, Fence, Shelf, Deck, Studio Floor)
    │   ├── include/
    │   │   ├── WoodMaterial.h
    │   │   └── WoodVariants.h                     # OAK, PINE, PLYWOOD, MDF, WEATHERED, BAMBOO
    │   ├── src/
    │   │   ├── WoodMaterial.cpp
    │   │   └── WoodVariants.cpp
    │   ├── shaders/
    │   │   ├── include/
    │   │   │   └── wood_common.glsl
    │   │   ├── wood_albedo.comp
    │   │   ├── wood_normal.comp
    │   │   ├── wood_orm.comp
    │   │   └── wood_rings.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── wood_seed.bin
    │   │   └── spec/
    │   │       └── wood_spec.h
    │   └── README.md
    │
    ├── metal/                                     # METAL (Used by: Car, Fence, Appliance, Studio Equipment)
    │   ├── include/
    │   │   ├── MetalMaterial.h
    │   │   └── MetalVariants.h                    # STEEL_BRUSHED, ALUMINUM, RUSTED, GALVANIZED, CHROME
    │   ├── src/
    │   │   ├── MetalMaterial.cpp
    │   │   └── MetalVariants.cpp
    │   ├── shaders/
    │   │   ├── include/
    │   │   │   └── metal_common.glsl
    │   │   ├── metal_albedo.comp
    │   │   ├── metal_normal.comp
    │   │   ├── metal_orm.comp
    │   │   └── metal_scratch.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── metal_seed.bin
    │   │   └── spec/
    │   │       └── metal_spec.h
    │   └── README.md
    │
    ├── concrete/                                  # CONCRETE (Used by: Building, Sidewalk, Bridge, Studio Wall)
    │   ├── include/
    │   │   ├── ConcreteMaterial.h
    │   │   └── ConcreteVariants.h                 # RAW, PAINTED, REINFORCED, PRECAST, POLISHED
    │   ├── src/
    │   │   ├── ConcreteMaterial.cpp
    │   │   └── ConcreteVariants.cpp
    │   ├── shaders/
    │   │   ├── include/
    │   │   │   └── concrete_common.glsl
    │   │   ├── concrete_albedo.comp
    │   │   ├── concrete_normal.comp
    │   │   ├── concrete_orm.comp
    │   │   └── concrete_cracks.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── concrete_seed.bin
    │   │   └── spec/
    │   │       └── concrete_spec.h
    │   └── README.md
    │
    ├── stone/                                     # STONE (Used by: Foundation, Cobblestone, Monument)
    │   ├── include/
    │   │   ├── StoneMaterial.h
    │   │   └── StoneVariants.h                    # GRANITE, MARBLE, SANDSTONE, SLATE, COBBLESTONE
    │   ├── src/
    │   │   ├── StoneMaterial.cpp
    │   │   └── StoneVariants.cpp
    │   ├── shaders/
    │   │   ├── include/
    │   │   │   └── stone_common.glsl
    │   │   ├── stone_albedo.comp
    │   │   ├── stone_normal.comp
    │   │   ├── stone_orm.comp
    │   │   └── stone_veins.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── stone_seed.bin
    │   │   └── spec/
    │   │       └── stone_spec.h
    │   └── README.md
    │
    ├── glass/                                     # GLASS (Used by: Window, Car, Storefront, Studio Window)
    │   ├── include/
    │   │   ├── GlassMaterial.h
    │   │   └── GlassVariants.h                    # CLEAR, FROSTED, TINTED, STAINED, SAFETY
    │   ├── src/
    │   │   ├── GlassMaterial.cpp
    │   │   └── GlassVariants.cpp
    │   ├── shaders/
    │   │   ├── include/
    │   │   │   └── glass_common.glsl
    │   │   ├── glass_refract.comp
    │   │   └── glass_dirt.comp
    │   ├── textures/
    │   │   └── spec/
    │   │       └── glass_spec.h
    │   └── README.md
    │
    ├── tile/                                      # TILE (Used by: Bathroom, Roof, Kitchen, Studio Floor)
    │   ├── include/
    │   │   ├── TileMaterial.h
    │   │   └── TileVariants.h                     # CERAMIC_WHITE, CERAMIC_BLUE, PORCELAIN, MOSAIC
    │   ├── src/
    │   │   ├── TileMaterial.cpp
    │   │   └── TileVariants.cpp
    │   ├── shaders/
    │   │   ├── include/
    │   │   │   └── tile_common.glsl
    │   │   ├── tile_albedo.comp
    │   │   ├── tile_normal.comp
    │   │   ├── tile_orm.comp
    │   │   └── tile_grout.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── tile_seed.bin
    │   │   └── spec/
    │   │       └── tile_spec.h
    │   └── README.md
    │
    ├── fabric/                                    # FABRIC (Used by: Curtain, Chair, Carpet, Awning, Studio Panels)
    │   ├── include/
    │   │   ├── FabricMaterial.h
    │   │   └── FabricVariants.h                   # CANVAS, CARPET, VELVET, DENIM, LINEN, ACOUSTIC
    │   ├── src/
    │   │   ├── FabricMaterial.cpp
    │   │   └── FabricVariants.cpp
    │   ├── shaders/
    │   │   ├── include/
    │   │   │   └── fabric_common.glsl
    │   │   ├── fabric_albedo.comp
    │   │   ├── fabric_normal.comp
    │   │   ├── fabric_orm.comp
    │   │   └── fabric_weave.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── fabric_seed.bin
    │   │   └── spec/
    │   │       └── fabric_spec.h
    │   └── README.md
    │
    ├── asphalt/                                   # ASPHALT (Used by: Road, Parking Lot, Driveway, Race Track)
    │   ├── include/
    │   │   ├── AsphaltMaterial.h
    │   │   └── AsphaltVariants.h                  # DRY, WET, GRAVEL_MIX, RACE_TRACK, POTHOLED
    │   ├── src/
    │   │   ├── AsphaltMaterial.cpp
    │   │   └── AsphaltVariants.cpp
    │   ├── shaders/
    │   │   ├── include/
    │   │   │   └── asphalt_common.glsl
    │   │   ├── asphalt_albedo.comp
    │   │   ├── asphalt_normal.comp
    │   │   ├── asphalt_orm.comp
    │   │   ├── asphalt_height.comp
    │   │   └── asphalt_wetness.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── asphalt_seed.bin
    │   │   └── spec/
    │   │       └── asphalt_spec.h
    │   └── README.md
    │
    ├── dirt/                                      # DIRT (Used by: Road Shoulder, Farm Field, Construction, Off-road)
    │   ├── include/
    │   │   ├── DirtMaterial.h
    │   │   └── DirtVariants.h                     # DRY_SOIL, WET_MUD, COMPACTED, SANDY, CLAY
    │   ├── src/
    │   │   ├── DirtMaterial.cpp
    │   │   └── DirtVariants.cpp
    │   ├── shaders/
    │   │   ├── include/
    │   │   │   └── dirt_common.glsl
    │   │   ├── dirt_albedo.comp
    │   │   ├── dirt_normal.comp
    │   │   ├── dirt_orm.comp
    │   │   └── dirt_ruts.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── dirt_seed.bin
    │   │   └── spec/
    │   │       └── dirt_spec.h
    │   └── README.md
    │
    ├── gravel/                                    # GRAVEL (Used by: Rural Road, Driveway, Construction, Path)
    │   ├── include/
    │   │   ├── GravelMaterial.h
    │   │   └── GravelVariants.h                   # CRUSHED, ROUNDED, PEA, RAILROAD_BALLAST
    │   ├── src/
    │   │   ├── GravelMaterial.cpp
    │   │   └── GravelVariants.cpp
    │   ├── shaders/
    │   │   ├── include/
    │   │   │   └── gravel_common.glsl
    │   │   ├── gravel_albedo.comp
    │   │   ├── gravel_normal.comp
    │   │   └── gravel_scatter.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── gravel_seed.bin
    │   │   └── spec/
    │   │       └── gravel_spec.h
    │   └── README.md
    │
    ├── sand/                                      # SAND (Used by: Beach, Desert, Sandbox, Playground)
    │   ├── include/
    │   │   ├── SandMaterial.h
    │   │   └── SandVariants.h                     # BEACH_FINE, DESERT_DUNE, WET_SAND
    │   ├── src/
    │   │   ├── SandMaterial.cpp
    │   │   └── SandVariants.cpp
    │   ├── shaders/
    │   │   ├── include/
    │   │   │   └── sand_common.glsl
    │   │   ├── sand_albedo.comp
    │   │   ├── sand_normal.comp
    │   │   └── sand_ripple.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── sand_seed.bin
    │   │   └── spec/
    │   │       └── sand_spec.h
    │   └── README.md
    │
    ├── snow/                                      # SNOW (Used by: Winter Scene, Mountain, Studio Snow Shot)
    │   ├── include/
    │   │   ├── SnowMaterial.h
    │   │   └── SnowVariants.h                     # FRESH, PACKED, DIRTY, ICE_CRUST
    │   ├── src/
    │   │   ├── SnowMaterial.cpp
    │   │   └── SnowVariants.cpp
    │   ├── shaders/
    │   │   ├── include/
    │   │   │   └── snow_common.glsl
    │   │   ├── snow_albedo.comp
    │   │   ├── snow_normal.comp
    │   │   ├── snow_tracks.comp
    │   │   └── snow_subsurface.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── snow_seed.bin
    │   │   └── spec/
    │   │       └── snow_spec.h
    │   └── README.md
    │
    ├── ice/                                       # ICE (Used by: Winter Road, Lake, Glacier)
    │   ├── include/
    │   │   ├── IceMaterial.h
    │   │   └── IceVariants.h                      # CLEAR, FROSTY, CRACKED
    │   ├── src/
    │   │   ├── IceMaterial.cpp
    │   │   └── IceVariants.cpp
    │   ├── shaders/
    │   │   ├── ice_albedo.comp
    │   │   ├── ice_normal.comp
    │   │   └── ice_cracks.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── ice_seed.bin
    │   │   └── spec/
    │   │       └── ice_spec.h
    │   └── README.md
    │
    ├── mud/                                       # MUD (Used by: Off-road, Farm, Construction Site)
    │   ├── include/
    │   │   ├── MudMaterial.h
    │   │   └── MudVariants.h                      # WET_SLICK, DRY_CRACKED, THICK_CLAY
    │   ├── src/
    │   │   ├── MudMaterial.cpp
    │   │   └── MudVariants.cpp
    │   ├── shaders/
    │   │   ├── mud_albedo.comp
    │   │   ├── mud_normal.comp
    │   │   └── mud_wetness.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── mud_seed.bin
    │   │   └── spec/
    │   │       └── mud_spec.h
    │   └── README.md
    │
    ├── bark/                                      # BARK (Used by: Trees, Log Props, Fence Posts)
    │   ├── include/
    │   │   ├── BarkMaterial.h
    │   │   └── BarkVariants.h                     # OAK, PINE, BIRCH, PALM, BURNT
    │   ├── src/
    │   │   ├── BarkMaterial.cpp
    │   │   └── BarkVariants.cpp
    │   ├── shaders/
    │   │   ├── bark_albedo.comp
    │   │   ├── bark_normal.comp
    │   │   ├── bark_orm.comp
    │   │   └── bark_furrows.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── bark_seed.bin
    │   │   └── spec/
    │   │       └── bark_spec.h
    │   └── README.md
    │
    ├── foliage/                                   # FOLIAGE (Used by: Leaves, Bush Surface, Hedge, Vine)
    │   ├── include/
    │   │   ├── FoliageMaterial.h
    │   │   └── FoliageVariants.h                  # LEAF_OAK, LEAF_PINE, BUSH, HEDGE, VINE, FERN
    │   ├── src/
    │   │   ├── FoliageMaterial.cpp
    │   │   └── FoliageVariants.cpp
    │   ├── shaders/
    │   │   ├── foliage_albedo.comp
    │   │   ├── foliage_normal.comp
    │   │   ├── foliage_orm.comp
    │   │   └── foliage_subsurface.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── foliage_seed.bin
    │   │   └── spec/
    │   │       └── foliage_spec.h
    │   └── README.md
    │
    ├── grass_surface/                             # GRASS SURFACE (Used by: Lawn, Field, Median, Garden)
    │   ├── include/
    │   │   ├── GrassSurfaceMaterial.h
    │   │   └── GrassSurfaceVariants.h             # LAWN, WILD, MEADOW, DRY, WET
    │   ├── src/
    │   │   ├── GrassSurfaceMaterial.cpp
    │   │   └── GrassSurfaceVariants.cpp
    │   ├── shaders/
    │   │   ├── grass_surface_albedo.comp
    │   │   ├── grass_surface_normal.comp
    │   │   └── grass_surface_orm.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── grass_surface_seed.bin
    │   │   └── spec/
    │   │       └── grass_surface_spec.h
    │   └── README.md
    │
    ├── rubber/                                    # RUBBER (Used by: Tire, Mat, Bumper, Studio Floor)
    │   ├── include/
    │   │   ├── RubberMaterial.h
    │   │   └── RubberVariants.h                   # TIRE_TREAD, SMOOTH, FLOOR_MAT
    │   ├── src/
    │   │   ├── RubberMaterial.cpp
    │   │   └── RubberVariants.cpp
    │   ├── shaders/
    │   │   ├── rubber_albedo.comp
    │   │   ├── rubber_normal.comp
    │   │   └── rubber_orm.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── rubber_seed.bin
    │   │   └── spec/
    │   │       └── rubber_spec.h
    │   └── README.md
    │
    ├── plastic/                                   # PLASTIC (Used by: Dashboard, Container, Appliance, Studio)
    │   ├── include/
    │   │   ├── PlasticMaterial.h
    │   │   └── PlasticVariants.h                  # HARD_ABS, SOFT_VINYL, TRANSPARENT
    │   ├── src/
    │   │   ├── PlasticMaterial.cpp
    │   │   └── PlasticVariants.cpp
    │   ├── shaders/
    │   │   ├── plastic_albedo.comp
    │   │   ├── plastic_normal.comp
    │   │   └── plastic_orm.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── plastic_seed.bin
    │   │   └── spec/
    │   │       └── plastic_spec.h
    │   └── README.md
    │
    ├── paint/                                     # PAINT (Used by: Car Body, House Wall, Sign, Studio)
    │   ├── include/
    │   │   ├── PaintMaterial.h
    │   │   └── PaintVariants.h                    # MATTE, GLOSS, AUTO_METALLIC, PEARL
    │   ├── src/
    │   │   ├── PaintMaterial.cpp
    │   │   └── PaintVariants.cpp
    │   ├── shaders/
    │   │   ├── paint_albedo.comp
    │   │   ├── paint_normal.comp
    │   │   ├── paint_orm.comp
    │   │   └── paint_flake.comp
    │   ├── textures/
    │   │   └── spec/
    │   │       └── paint_spec.h
    │   └── README.md
    │
    ├── leather/                                   # LEATHER (Used by: Car Seat, Saddle, Bag, Studio Chair)
    │   ├── include/
    │   │   ├── LeatherMaterial.h
    │   │   └── LeatherVariants.h                  # SMOOTH, GRAINED, SUEDE, WORN
    │   ├── src/
    │   │   ├── LeatherMaterial.cpp
    │   │   └── LeatherVariants.cpp
    │   ├── shaders/
    │   │   ├── leather_albedo.comp
    │   │   ├── leather_normal.comp
    │   │   └── leather_orm.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── leather_seed.bin
    │   │   └── spec/
    │   │       └── leather_spec.h
    │   └── README.md
    │
    ├── skin/                                      # SKIN (Used by: Characters, Pedestrians, Animals)
    │   ├── include/
    │   │   ├── SkinMaterial.h
    │   │   └── SkinVariants.h                     # HUMAN_LIGHT, HUMAN_MEDIUM, HUMAN_DARK, ANIMAL
    │   ├── src/
    │   │   ├── SkinMaterial.cpp
    │   │   └── SkinVariants.cpp
    │   ├── shaders/
    │   │   ├── skin_albedo.comp
    │   │   ├── skin_normal.comp
    │   │   ├── skin_orm.comp
    │   │   └── skin_subsurface.comp
    │   ├── textures/
    │   │   └── spec/
    │   │       └── skin_spec.h
    │   └── README.md
    │
    ├── emissive/                                  # EMISSIVE (Used by: Neon Sign, Tail Light, LED, Screen)
    │   ├── include/
    │   │   ├── EmissiveMaterial.h
    │   │   └── EmissiveVariants.h                 # NEON_PINK, NEON_BLUE, LED_WHITE, TAIL_RED, HEADLIGHT
    │   ├── src/
    │   │   ├── EmissiveMaterial.cpp
    │   │   └── EmissiveVariants.cpp
    │   ├── shaders/
    │   │   ├── emissive_pattern.comp
    │   │   ├── emissive_albedo.comp
    │   │   └── emissive_glow.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── emissive_seed.bin
    │   │   └── spec/
    │   │       └── emissive_spec.h
    │   └── README.md
    │
    ├── terracotta/                                # TERRACOTTA (Used by: Roof Tile, Pot, Ornament)
    │   ├── include/
    │   │   ├── TerracottaMaterial.h
    │   │   └── TerracottaVariants.h               # ROOF_TILE, GLAZED, UNGLAZED
    │   ├── src/
    │   │   ├── TerracottaMaterial.cpp
    │   │   └── TerracottaVariants.cpp
    │   ├── shaders/
    │   │   ├── terracotta_albedo.comp
    │   │   ├── terracotta_normal.comp
    │   │   └── terracotta_orm.comp
    │   ├── textures/
    │   │   └── spec/
    │   │       └── terracotta_spec.h
    │   └── README.md
    │
    ├── rust/                                      # RUST (Used by: Old Car, Gate, Pipe, Corrugated Roof, Ruins)
    │   ├── include/
    │   │   ├── RustMaterial.h
    │   │   └── RustVariants.h                     # LIGHT, HEAVY, FLASH, BURNT
    │   ├── src/
    │   │   ├── RustMaterial.cpp
    │   │   └── RustVariants.cpp
    │   ├── shaders/
    │   │   ├── rust_albedo.comp
    │   │   ├── rust_normal.comp
    │   │   ├── rust_orm.comp
    │   │   └── rust_flake.comp
    │   ├── textures/
    │   │   ├── seeds/
    │   │   │   └── rust_seed.bin
    │   │   └── spec/
    │   │       └── rust_spec.h
    │   └── README.md
    │
    └── water_surface/                             # WATER SURFACE (Used by: Ocean, Lake, River, Pool, Puddle)
        ├── include/
        │   ├── WaterSurfaceMaterial.h
        │   └── WaterSurfaceVariants.h             # OCEAN, LAKE, RIVER, POOL, PUDDLE
        ├── src/
        │   ├── WaterSurfaceMaterial.cpp
        │   └── WaterSurfaceVariants.cpp
        ├── shaders/
        │   ├── water_surface_albedo.comp
        │   ├── water_surface_normal.comp
        │   ├── water_surface_orm.comp
        │   ├── water_surface_caustics.comp
        │   └── water_surface_foam.comp
        ├── textures/
        │   └── spec/
        │       └── water_surface_spec.h
        └── README.md
```

How This Works (The Shared Flow)

1. Geometry Generator (e.g., FenceGen) requests a material:
   ```cpp
   auto material = MaterialRegistry::Get(MaterialID::BRICK_RED);
   ```
2. MaterialRegistry checks its LRU cache. If the textures haven't been generated yet, it dispatches the brick_albedo.comp, brick_normal.comp, etc., inside materials/brick/shaders/.
3. The Shaders read textures/seeds/brick_seed.bin and use the VkFormat rules defined in textures/spec/brick_spec.h.
4. The VkImages are stored in the global BindlessHeap.
5. The Fence receives the bindless indices and renders it using the global pbr.frag.
6. Later, the House Generator asks for BRICK_RED and receives the exact same cached VkImage handles instantly — no recompilation, no disk I/O, no duplicate VRAM.
