package com.example.sphere.model

import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontFamily

enum class TextRenderMode(val label: String, val description: String) {
    SDF_DISTANCE_FIELD("SDF Vector Font", "Sascha Willems signed distance field shader with subpixel anti-aliasing"),
    TEXT_OVERLAY_2D("2D HUD Overlay", "Dynamic orthographic text overlay with host-mapped vertex buffer updates"),
    BILLBOARD_3D_WORLD("3D World Billboard", "Spherical billboard text hovering in 3D world space facing camera"),
    BILLBOARD_3D_AXIAL("3D Axial Billboard", "Cylindrical billboard constrained to vertical Y-axis rotation"),
    SURFACE_DECAL_3D("Surface Decal", "Conformal planar text decal projected along surface normals")
}

enum class FontTypeface(val label: String, val family: FontFamily, val styleDesc: String) {
    ROBOTO_SDF("Roboto Bold SDF", FontFamily.Default, "Balanced geometric sans-serif for UI, HUDs and labels"),
    JETBRAINS_MONO("JetBrains Mono SDF", FontFamily.Monospace, "Developer monospace with fixed glyph advances for telemetry"),
    ORBITRON_SCI_FI("Orbitron HUD SDF", FontFamily.SansSerif, "Futuristic sci-fi angular glyphs for tactical avionics"),
    CINZEL_SERIF("Cinzel Serif SDF", FontFamily.Serif, "Classic high-contrast roman serif for cinematic titles")
}

enum class TextAlignment(val label: String) {
    LEFT("Left"),
    CENTER("Center"),
    RIGHT("Right")
}

enum class TextBadgeStyle(val label: String) {
    NONE("None (Text Only)"),
    GLASS_DARK("Frosted Dark Glass"),
    NEON_CYBER("Cyber Neon Border"),
    SOLID_SLATE("Solid Dark Slate"),
    TACTICAL_BRACKETS("Avionics Corner Brackets")
}

enum class TextMountTarget(val label: String, val shortName: String, val description: String) {
    WALL("🧱 Architectural Wall", "Wall", "Text applied onto a brick, concrete, stone, or timber architectural wall"),
    POST("📍 Street / Sign Post", "Post", "Text mounted on a 3D street pole or signpost with mounting brackets"),
    FLOATING("☁️ Floating Billboard", "Billboard", "3D world-space text hovering above the ground platform"),
    HUD("🖥️ 2D HUD Overlay", "HUD", "Screen-space dynamic text overlay (Sascha Willems textoverlay)")
}

enum class WallMaterialType(val label: String, val primaryColor: Color, val mortarColor: Color, val accentColor: Color) {
    BRICK_RED("Red Stretcher Brick", Color(0xFF9E3A26), Color(0xFFD4C8B8), Color(0xFFB84530)),
    CONCRETE_MODERN("Cast Formwork Concrete", Color(0xFF5A6372), Color(0xFF38404E), Color(0xFF6B7688)),
    DARK_STONE("Dark Ashlar Stone", Color(0xFF2E3540), Color(0xFF1E222A), Color(0xFF3C4554)),
    TIMBER_PLANK("Cedar Timber Siding", Color(0xFF6B482A), Color(0xFF452C16), Color(0xFF7E5632))
}

enum class PostStyleType(val label: String, val poleColor: Color, val bracketColor: Color, val plateColor: Color) {
    STEEL_CYLINDER("Round Galvanized Steel", Color(0xFF8896A6), Color(0xFF425062), Color(0xFF1B3B2B)),
    TIMBER_SQUARE("Square Cedar Timber", Color(0xFF6E4A2C), Color(0xFF382414), Color(0xFF2C241E)),
    CAST_IRON_BLACK("Victorian Cast Iron", Color(0xFF22262E), Color(0xFF12151A), Color(0xFF1E2B38)),
    HIGHWAY_GANTRY("Industrial Steel / Yellow", Color(0xFF5A6270), Color(0xFF2A303A), Color(0xFFD97706))
}

enum class TextApplicationStyle(val label: String, val description: String) {
    MOUNTED_PLAQUE("Mounted Signboard Plaque", "Raised metal/acrylic plate with bevelled rim & corner bolts"),
    DIRECT_DECAL("Surface Stencil Decal", "Painted directly onto the wall/post conforming to surface"),
    CHANNEL_3D_LETTERS("3D Raised Channel Letters", "Extruded letters casting realistic drop shadows onto surface"),
    NEON_SIGN("Neon Emissive Sign", "Glowing light tubes casting radial illumination onto wall/post")
}

data class TextPreset(
    val id: String,
    val name: String,
    val category: String,
    val description: String,
    val text: String,
    val font: FontTypeface,
    val renderMode: TextRenderMode,
    val alignment: TextAlignment,
    val fontSize: Float,
    val letterSpacing: Float,
    val lineHeight: Float,
    val textColor: Color,
    val enableOutline: Boolean,
    val outlineWidth: Float,
    val outlineColor: Color,
    val enableShadow: Boolean,
    val shadowOffsetX: Float,
    val shadowOffsetY: Float,
    val shadowSoftness: Float,
    val shadowColor: Color,
    val smoothing: Float,
    val thicknessBias: Float,
    val enableGlow: Boolean,
    val glowRadius: Float,
    val glowColor: Color,
    val posX: Float = 0.0f,
    val posY: Float = 1.35f,
    val posZ: Float = 0.0f,
    val rotX: Float = 0.0f,
    val rotY: Float = 0.0f,
    val scale: Float = 1.0f,
    val badgeStyle: TextBadgeStyle = TextBadgeStyle.GLASS_DARK,
    val mountTarget: TextMountTarget = TextMountTarget.WALL,
    val wallMaterial: WallMaterialType = WallMaterialType.BRICK_RED,
    val postStyle: PostStyleType = PostStyleType.STEEL_CYLINDER,
    val applicationStyle: TextApplicationStyle = TextApplicationStyle.MOUNTED_PLAQUE
)

val defaultTextPresets = listOf(
    // 0. Architectural Wall Signage (Default)
    TextPreset(
        id = "architectural_wall_sign",
        name = "Architectural Wall Signage",
        category = "Wall Mounts",
        description = "High-contrast architectural signage mounted on a weathered red brick facade with bevelled plaque and corner bolts.",
        text = "SPHERE STUDIO // 3D GRAPHICS\nBUILDING 4 - DIVISION OF VULKAN ENGINE\nARCHITECTURAL LABS & SIMULATION",
        font = FontTypeface.ROBOTO_SDF,
        renderMode = TextRenderMode.SURFACE_DECAL_3D,
        alignment = TextAlignment.CENTER,
        fontSize = 20.0f,
        letterSpacing = 1.05f,
        lineHeight = 1.30f,
        textColor = Color(0xFFF0F4F8),
        enableOutline = true,
        outlineWidth = 0.16f,
        outlineColor = Color(0xFF1A1C22),
        enableShadow = true,
        shadowOffsetX = 3.0f,
        shadowOffsetY = 4.0f,
        shadowSoftness = 0.30f,
        shadowColor = Color(0xCC000000),
        smoothing = 0.040f,
        thicknessBias = 0.02f,
        enableGlow = false,
        glowRadius = 0.0f,
        glowColor = Color.Transparent,
        badgeStyle = TextBadgeStyle.SOLID_SLATE,
        mountTarget = TextMountTarget.WALL,
        wallMaterial = WallMaterialType.BRICK_RED,
        applicationStyle = TextApplicationStyle.MOUNTED_PLAQUE
    ),

    // 0b. Street Wayfinding Signpost
    TextPreset(
        id = "street_wayfinding_post",
        name = "Street Wayfinding Signpost",
        category = "Post Mounts",
        description = "Urban street sign mounted onto a galvanized steel post with clamp brackets and reflective sign panel.",
        text = "VULKAN WAY // NDK AVE\n<-- SCENE STAGE  |  PBR LABS -->\nMAX SPEED: 144 FPS",
        font = FontTypeface.ROBOTO_SDF,
        renderMode = TextRenderMode.BILLBOARD_3D_AXIAL,
        alignment = TextAlignment.CENTER,
        fontSize = 20.0f,
        letterSpacing = 1.02f,
        lineHeight = 1.25f,
        textColor = Color(0xFFFFFFFF),
        enableOutline = true,
        outlineWidth = 0.14f,
        outlineColor = Color(0xFF0F2618),
        enableShadow = true,
        shadowOffsetX = 2.0f,
        shadowOffsetY = 3.0f,
        shadowSoftness = 0.20f,
        shadowColor = Color(0xAA000000),
        smoothing = 0.038f,
        thicknessBias = 0.0f,
        enableGlow = false,
        glowRadius = 0.0f,
        glowColor = Color.Transparent,
        badgeStyle = TextBadgeStyle.NONE,
        mountTarget = TextMountTarget.POST,
        postStyle = PostStyleType.STEEL_CYLINDER,
        applicationStyle = TextApplicationStyle.MOUNTED_PLAQUE
    ),
    // 1. Vulkan GPU Telemetry HUD (Sascha Willems classic overlay)
    TextPreset(
        id = "vulkan_telemetry_hud",
        name = "Vulkan GPU Telemetry HUD",
        category = "Engine Diagnostics",
        description = "Sascha Willems classic dynamic text overlay with frametime, memory pool, and pipeline stats.",
        text = "VK_DEVICE: Mali-G78 / Adreno 740\nFRAME: 6.94 ms (144.2 FPS)\nVRAM: 1,420 MB / 4,096 MB (34.6%)\nPIPELINES: 48 | DRAWS: 312 | PCF 5x5",
        font = FontTypeface.JETBRAINS_MONO,
        renderMode = TextRenderMode.TEXT_OVERLAY_2D,
        alignment = TextAlignment.LEFT,
        fontSize = 18.0f,
        letterSpacing = 1.05f,
        lineHeight = 1.35f,
        textColor = Color(0xFF2BF69A), // High-visibility phosphor green
        enableOutline = true,
        outlineWidth = 0.16f,
        outlineColor = Color(0xFF07140B),
        enableShadow = true,
        shadowOffsetX = 3.0f,
        shadowOffsetY = 4.0f,
        shadowSoftness = 0.25f,
        shadowColor = Color(0xCC000000),
        smoothing = 0.045f,
        thicknessBias = 0.02f,
        enableGlow = true,
        glowRadius = 0.28f,
        glowColor = Color(0x662BF69A),
        badgeStyle = TextBadgeStyle.TACTICAL_BRACKETS
    ),

    // 2. 3D Scene Anchor Billboard (Hovering in 3D world space)
    TextPreset(
        id = "billboard_stage_anchor",
        name = "3D Stage Coordinate Anchor",
        category = "3D World Labels",
        description = "Floating 3D billboard anchored over the scene calibration stage, facing camera with distance scaling.",
        text = "PBR STAGE ORIGIN [0.0, 1.20, 0.0]\nSURFACE: RUSTICATED BRICK MASONRY\nROUGHNESS: 0.88 | IOR: 1.54",
        font = FontTypeface.ROBOTO_SDF,
        renderMode = TextRenderMode.BILLBOARD_3D_WORLD,
        alignment = TextAlignment.CENTER,
        fontSize = 24.0f,
        letterSpacing = 1.0f,
        lineHeight = 1.25f,
        textColor = Color(0xFFF2F5F8),
        enableOutline = true,
        outlineWidth = 0.18f,
        outlineColor = Color(0xFF141A24),
        enableShadow = true,
        shadowOffsetX = 4.0f,
        shadowOffsetY = 5.0f,
        shadowSoftness = 0.35f,
        shadowColor = Color(0xDD000000),
        smoothing = 0.040f,
        thicknessBias = 0.0f,
        enableGlow = false,
        glowRadius = 0.0f,
        glowColor = Color.Transparent,
        posY = 1.60f,
        badgeStyle = TextBadgeStyle.GLASS_DARK
    ),

    // 3. Sci-Fi Cyberpunk Terminal
    TextPreset(
        id = "scifi_cyber_terminal",
        name = "Cyberpunk Quantum Terminal",
        category = "Sci-Fi HUDs",
        description = "Electric cyan emissive glyphs with signed distance field bloom and high-contrast dark backing.",
        text = "QUANTUM COHERENCE: OPTIMAL\nVULKAN COMPUTE DISPATCH: 1024x1024\nDISTANCE FIELD ATLAS: 16-BIT SDF",
        font = FontTypeface.ORBITRON_SCI_FI,
        renderMode = TextRenderMode.SDF_DISTANCE_FIELD,
        alignment = TextAlignment.CENTER,
        fontSize = 26.0f,
        letterSpacing = 1.15f,
        lineHeight = 1.30f,
        textColor = Color(0xFF29DCFF), // Vivid neon cyan
        enableOutline = true,
        outlineWidth = 0.20f,
        outlineColor = Color(0xFF032238),
        enableShadow = true,
        shadowOffsetX = 0.0f,
        shadowOffsetY = 3.0f,
        shadowSoftness = 0.40f,
        shadowColor = Color(0xDD020C18),
        smoothing = 0.038f,
        thicknessBias = 0.04f,
        enableGlow = true,
        glowRadius = 0.48f,
        glowColor = Color(0x8829DCFF),
        badgeStyle = TextBadgeStyle.NEON_CYBER
    ),

    // 4. Cinematic Golden Serif Title
    TextPreset(
        id = "cinematic_gold_title",
        name = "Cinematic Golden Serif Title",
        category = "Cinematics",
        description = "Deep warm gold typography with elegant drop shadow and smooth antialiased outlines.",
        text = "SPHERE ENGINE 3D\nTHE VULKAN RENDERING CHRONICLES",
        font = FontTypeface.CINZEL_SERIF,
        renderMode = TextRenderMode.SDF_DISTANCE_FIELD,
        alignment = TextAlignment.CENTER,
        fontSize = 32.0f,
        letterSpacing = 1.30f,
        lineHeight = 1.40f,
        textColor = Color(0xFFFFD56B), // Rich golden sheen
        enableOutline = true,
        outlineWidth = 0.22f,
        outlineColor = Color(0xFF321A04),
        enableShadow = true,
        shadowOffsetX = 5.0f,
        shadowOffsetY = 6.0f,
        shadowSoftness = 0.50f,
        shadowColor = Color(0xEE120802),
        smoothing = 0.035f,
        thicknessBias = 0.05f,
        enableGlow = false,
        glowRadius = 0.0f,
        glowColor = Color.Transparent,
        badgeStyle = TextBadgeStyle.NONE
    ),

    // 5. Critical Hazard Warning Banner
    TextPreset(
        id = "critical_hazard_warning",
        name = "Critical Hazard Holo-Banner",
        category = "Tactical & Alerts",
        description = "Industrial safety alert featuring high-contrast amber/crimson text with thick dark outlines.",
        text = "CAUTION: HIGH VOLTAGE RAY TRACING\nUNIFORM BUFFER OVERFLOW IMMINENT\nREDUCE SHADOW CASCADE COUNT TO 3",
        font = FontTypeface.JETBRAINS_MONO,
        renderMode = TextRenderMode.SDF_DISTANCE_FIELD,
        alignment = TextAlignment.CENTER,
        fontSize = 22.0f,
        letterSpacing = 1.05f,
        lineHeight = 1.30f,
        textColor = Color(0xFFFF4838), // Alert vermillion
        enableOutline = true,
        outlineWidth = 0.26f,
        outlineColor = Color(0xFF1E0604),
        enableShadow = true,
        shadowOffsetX = 3.0f,
        shadowOffsetY = 4.0f,
        shadowSoftness = 0.30f,
        shadowColor = Color(0xCC000000),
        smoothing = 0.040f,
        thicknessBias = 0.08f,
        enableGlow = true,
        glowRadius = 0.35f,
        glowColor = Color(0x66FF4838),
        badgeStyle = TextBadgeStyle.SOLID_SLATE
    ),

    // 6. Game Quest Objective
    TextPreset(
        id = "game_quest_objective",
        name = "Tactical Quest Objective",
        category = "Game UI",
        description = "RPG & Action game quest tracker with objective completion checkbox and crisp typography.",
        text = "OBJECTIVE [PRIMARY]:\n[x] Calibrate 4-Cascade Shadow Map\n[x] Synthesize Preetham Atmosphere\n[ ] Render Signed Distance Field Fonts",
        font = FontTypeface.ROBOTO_SDF,
        renderMode = TextRenderMode.TEXT_OVERLAY_2D,
        alignment = TextAlignment.LEFT,
        fontSize = 20.0f,
        letterSpacing = 1.0f,
        lineHeight = 1.30f,
        textColor = Color(0xFFE5ECF6),
        enableOutline = true,
        outlineWidth = 0.15f,
        outlineColor = Color(0xFF101722),
        enableShadow = true,
        shadowOffsetX = 3.0f,
        shadowOffsetY = 3.0f,
        shadowSoftness = 0.25f,
        shadowColor = Color(0xAA000000),
        smoothing = 0.042f,
        thicknessBias = 0.0f,
        enableGlow = false,
        glowRadius = 0.0f,
        glowColor = Color.Transparent,
        badgeStyle = TextBadgeStyle.GLASS_DARK
    )
)
