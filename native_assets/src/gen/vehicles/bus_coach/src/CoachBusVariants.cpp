#include "CoachBusVariants.h"

namespace sphere::vehicles {

VehicleDef CreateCoachBusDefaultDef(VehicleID id) {
    VehicleDef def;
    def.id = id;
    def.name = GetVehicleName(id);
    def.category = GetVehicleCategory(id);
    def.fifth_wheel_offset_m = 0.0f;
    def.wheel_radius_m = 0.50f;
    def.tire_width_m = 0.305f;
    def.ground_clearance_m = 0.28f;

    switch (id) {
        case VehicleID::COACH_HIGH_DECKER:
            // Volvo UNVI High-Decker 4x2 (Pearl White + Piano Black Mask)
            def.drive_config = VehicleDriveConfig::DRIVE_4X2;
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 13400.0f;
            def.max_gvw_kg = 19500.0f;
            def.wheelbase_m = 6.20f;
            def.cab_height_m = 3.72f;
            def.engine_torque_nm = 1850.0f;
            def.engine_power_hp = 420.0f;
            def.passenger_capacity = 55;
            def.dimensions = Vec3(13.20f, 2.55f, 3.72f);
            def.base_color = Vec4(0.92f, 0.93f, 0.95f, 1.0f);
            def.accent_color = Vec4(0.86f, 0.89f, 0.93f, 1.0f);
            def.roughness = 0.18f;
            def.metallic = 0.35f;
            break;

        case VehicleID::COACH_SLEEPER:
            // Volvo 9900 Kortrijk Theater-Floor 6x2 (Pewter Anthracite + White Z-Wave)
            def.drive_config = VehicleDriveConfig::DRIVE_6X2_TAG;
            def.livery = LiveryID::LUXURY_METALLIC;
            def.kerb_weight_kg = 15800.0f;
            def.max_gvw_kg = 25000.0f;
            def.wheelbase_m = 6.50f;
            def.cab_height_m = 3.85f;
            def.engine_torque_nm = 2150.0f;
            def.engine_power_hp = 460.0f;
            def.passenger_capacity = 53;
            def.dimensions = Vec3(13.80f, 2.55f, 3.85f);
            def.base_color = Vec4(0.45f, 0.47f, 0.51f, 1.0f);
            def.accent_color = Vec4(0.95f, 0.96f, 0.98f, 1.0f);
            def.roughness = 0.20f;
            def.metallic = 0.62f;
            break;

        case VehicleID::COACH_TOURING:
            // Volvo BZR Electric Aero 6x2 (Nordic Sage-Teal Metallic + Silver Chrome Wave)
            def.drive_config = VehicleDriveConfig::DRIVE_6X2_TAG;
            def.livery = LiveryID::TRANSIT_MUNICIPAL;
            def.kerb_weight_kg = 16200.0f;
            def.max_gvw_kg = 26000.0f;
            def.wheelbase_m = 6.45f;
            def.cab_height_m = 3.78f;
            def.engine_torque_nm = 2600.0f;
            def.engine_power_hp = 540.0f;
            def.passenger_capacity = 52;
            def.dimensions = Vec3(13.60f, 2.55f, 3.78f);
            def.base_color = Vec4(0.30f, 0.49f, 0.46f, 1.0f);
            def.accent_color = Vec4(0.90f, 0.93f, 0.96f, 1.0f);
            def.roughness = 0.18f;
            def.metallic = 0.58f;
            break;

        case VehicleID::COACH_LUXURY:
            // Volvo 9700 US/CAN Aero 6x2 (Satin Silver Grey + White Brow)
            def.drive_config = VehicleDriveConfig::DRIVE_6X2_TAG;
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 16400.0f;
            def.max_gvw_kg = 26000.0f;
            def.wheelbase_m = 6.75f;
            def.cab_height_m = 3.88f;
            def.engine_torque_nm = 2350.0f;
            def.engine_power_hp = 510.0f;
            def.passenger_capacity = 56;
            def.dimensions = Vec3(14.20f, 2.55f, 3.88f);
            def.base_color = Vec4(0.64f, 0.66f, 0.70f, 1.0f);
            def.accent_color = Vec4(0.92f, 0.94f, 0.96f, 1.0f);
            def.roughness = 0.22f;
            def.metallic = 0.54f;
            break;

        case VehicleID::COACH_VOLVO_9700_DD:
            // Volvo 9700DD Double-Decker 6x2 (Champagne Bronze Metallic + Gloss Black Z-Flank)
            def.drive_config = VehicleDriveConfig::DRIVE_6X2_TAG;
            def.livery = LiveryID::LUXURY_METALLIC;
            def.kerb_weight_kg = 18600.0f;
            def.max_gvw_kg = 27000.0f;
            def.wheelbase_m = 6.95f;
            def.cab_height_m = 4.00f;
            def.engine_torque_nm = 2400.0f;
            def.engine_power_hp = 500.0f;
            def.passenger_capacity = 86;
            def.dimensions = Vec3(14.80f, 2.55f, 4.00f);
            def.base_color = Vec4(0.55f, 0.51f, 0.44f, 1.0f);
            def.accent_color = Vec4(0.08f, 0.09f, 0.11f, 1.0f);
            def.roughness = 0.18f;
            def.metallic = 0.64f;
            break;

        case VehicleID::COACH_VOLVO_9800_DD:
            // Volvo 9800 Double-Decker 6x2 (Ice Sky Cyan + Black Mid-Deck Slash)
            def.drive_config = VehicleDriveConfig::DRIVE_6X2_TAG;
            def.livery = LiveryID::LUXURY_METALLIC;
            def.kerb_weight_kg = 19100.0f;
            def.max_gvw_kg = 27500.0f;
            def.wheelbase_m = 7.10f;
            def.cab_height_m = 4.00f;
            def.engine_torque_nm = 2500.0f;
            def.engine_power_hp = 500.0f;
            def.passenger_capacity = 88;
            def.dimensions = Vec3(15.00f, 2.55f, 4.00f);
            def.base_color = Vec4(0.55f, 0.82f, 0.88f, 1.0f);
            def.accent_color = Vec4(0.10f, 0.11f, 0.13f, 1.0f);
            def.roughness = 0.18f;
            def.metallic = 0.56f;
            break;

        case VehicleID::COACH_VOLVO_9700_TEAL:
            // Volvo 9700 13.1m Modern 4x2 (Deep Ocean Teal Metallic + Seamless Brow)
            def.drive_config = VehicleDriveConfig::DRIVE_4X2;
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 13800.0f;
            def.max_gvw_kg = 19500.0f;
            def.wheelbase_m = 6.15f;
            def.cab_height_m = 3.68f;
            def.engine_torque_nm = 2200.0f;
            def.engine_power_hp = 460.0f;
            def.passenger_capacity = 53;
            def.dimensions = Vec3(13.10f, 2.55f, 3.68f);
            def.base_color = Vec4(0.11f, 0.39f, 0.47f, 1.0f);
            def.accent_color = Vec4(0.84f, 0.87f, 0.92f, 1.0f);
            def.roughness = 0.20f;
            def.metallic = 0.60f;
            break;

        case VehicleID::COACH_VOLVO_9800_AERO:
            // Volvo 9800 2026 Aero 6x2 (Periwinkle Steel-Blue + DOT Brow)
            def.drive_config = VehicleDriveConfig::DRIVE_6X2_TAG;
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 16500.0f;
            def.max_gvw_kg = 26000.0f;
            def.wheelbase_m = 6.85f;
            def.cab_height_m = 3.88f;
            def.engine_torque_nm = 2400.0f;
            def.engine_power_hp = 500.0f;
            def.passenger_capacity = 57;
            def.dimensions = Vec3(14.40f, 2.55f, 3.88f);
            def.base_color = Vec4(0.42f, 0.51f, 0.67f, 1.0f);
            def.accent_color = Vec4(0.92f, 0.94f, 0.97f, 1.0f);
            def.roughness = 0.19f;
            def.metallic = 0.55f;
            break;

        case VehicleID::COACH_YUTONG_TOURING:
            // Yutong T13 Touring 6x2 (Slate Blue-Grey Metallic + White Hockey-Stick Trim)
            def.drive_config = VehicleDriveConfig::DRIVE_6X2_TAG;
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 15400.0f;
            def.max_gvw_kg = 24500.0f;
            def.wheelbase_m = 6.50f;
            def.cab_height_m = 3.82f;
            def.engine_torque_nm = 2100.0f;
            def.engine_power_hp = 450.0f;
            def.passenger_capacity = 55;
            def.dimensions = Vec3(13.80f, 2.55f, 3.82f);
            def.base_color = Vec4(0.29f, 0.37f, 0.45f, 1.0f);
            def.accent_color = Vec4(0.92f, 0.94f, 0.96f, 1.0f);
            def.roughness = 0.21f;
            def.metallic = 0.52f;
            break;

        case VehicleID::COACH_AERO_BLADE_VIP:
            // Aero Blade VIP 6x2 Concept (Titanium Silver + Gold-Amber LED Blades)
            def.drive_config = VehicleDriveConfig::DRIVE_6X2_TAG;
            def.livery = LiveryID::LUXURY_METALLIC;
            def.kerb_weight_kg = 15600.0f;
            def.max_gvw_kg = 25000.0f;
            def.wheelbase_m = 6.45f;
            def.cab_height_m = 3.86f;
            def.engine_torque_nm = 2450.0f;
            def.engine_power_hp = 520.0f;
            def.passenger_capacity = 50;
            def.dimensions = Vec3(13.50f, 2.55f, 3.86f);
            def.base_color = Vec4(0.72f, 0.75f, 0.79f, 1.0f);
            def.accent_color = Vec4(0.98f, 0.72f, 0.24f, 1.0f);
            def.roughness = 0.16f;
            def.metallic = 0.68f;
            break;

        case VehicleID::COACH_CHAMPAGNE_SR2:
            // Champagne SR2 High-Deck 4x2 (Warm Champagne Gold + Silver Chrome Spear)
            def.drive_config = VehicleDriveConfig::DRIVE_4X2;
            def.livery = LiveryID::LUXURY_METALLIC;
            def.kerb_weight_kg = 13200.0f;
            def.max_gvw_kg = 19000.0f;
            def.wheelbase_m = 6.05f;
            def.cab_height_m = 3.78f;
            def.engine_torque_nm = 1900.0f;
            def.engine_power_hp = 410.0f;
            def.passenger_capacity = 49;
            def.dimensions = Vec3(12.80f, 2.55f, 3.78f);
            def.base_color = Vec4(0.75f, 0.70f, 0.63f, 1.0f);
            def.accent_color = Vec4(0.88f, 0.90f, 0.94f, 1.0f);
            def.roughness = 0.20f;
            def.metallic = 0.60f;
            break;

        case VehicleID::COACH_NEOPLAN_TOURLINER:
            // Neoplan Tourliner 6x2 (Pearl White + Shark-Fin B-Pillar Cut)
            def.drive_config = VehicleDriveConfig::DRIVE_6X2_TAG;
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 15900.0f;
            def.max_gvw_kg = 25500.0f;
            def.wheelbase_m = 6.60f;
            def.cab_height_m = 3.84f;
            def.engine_torque_nm = 2300.0f;
            def.engine_power_hp = 480.0f;
            def.passenger_capacity = 57;
            def.dimensions = Vec3(13.90f, 2.55f, 3.84f);
            def.base_color = Vec4(0.93f, 0.94f, 0.96f, 1.0f);
            def.accent_color = Vec4(0.12f, 0.13f, 0.16f, 1.0f);
            def.roughness = 0.17f;
            def.metallic = 0.34f;
            break;

        case VehicleID::COACH_TWIN_GLASS_DOME:
            // Twin-Dome Panoramic High-Decker 4x2 (Ceramic White + Black Upper Glass Dome)
            def.drive_config = VehicleDriveConfig::DRIVE_4X2;
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 13100.0f;
            def.max_gvw_kg = 19000.0f;
            def.wheelbase_m = 6.00f;
            def.cab_height_m = 3.82f;
            def.engine_torque_nm = 1850.0f;
            def.engine_power_hp = 400.0f;
            def.passenger_capacity = 49;
            def.dimensions = Vec3(12.60f, 2.55f, 3.82f);
            def.base_color = Vec4(0.92f, 0.93f, 0.95f, 1.0f);
            def.accent_color = Vec4(0.90f, 0.62f, 0.16f, 1.0f);
            def.roughness = 0.18f;
            def.metallic = 0.38f;
            break;

        case VehicleID::COACH_EXECUTIVE_I6:
            // Irizar i6 Executive Coach 6x2 (Metallic Slate Anthracite + Chrome V-Chevron)
            def.drive_config = VehicleDriveConfig::DRIVE_6X2_TAG;
            def.livery = LiveryID::LUXURY_METALLIC;
            def.kerb_weight_kg = 15800.0f;
            def.max_gvw_kg = 25200.0f;
            def.wheelbase_m = 6.50f;
            def.cab_height_m = 3.84f;
            def.engine_torque_nm = 2200.0f;
            def.engine_power_hp = 460.0f;
            def.passenger_capacity = 53;
            def.dimensions = Vec3(13.80f, 2.55f, 3.84f);
            def.base_color = Vec4(0.21f, 0.24f, 0.25f, 1.0f);
            def.accent_color = Vec4(0.86f, 0.89f, 0.93f, 1.0f);
            def.roughness = 0.16f;
            def.metallic = 0.62f;
            break;

        case VehicleID::COACH_LUXURY_VIP_4X2:
            // 12.0m 53-VIP-Seat Luxury Touring Coach 4x2 (Pearl White + Warm Orange-Gold Accent + Z-Boomerang)
            def.drive_config = VehicleDriveConfig::DRIVE_4X2;
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 13600.0f;
            def.max_gvw_kg = 19500.0f;
            def.wheelbase_m = 6.00f;
            def.cab_height_m = 3.82f;
            def.engine_torque_nm = 2000.0f;
            def.engine_power_hp = 430.0f;
            def.passenger_capacity = 53;
            def.dimensions = Vec3(12.00f, 2.55f, 3.82f);
            def.base_color = Vec4(0.94f, 0.94f, 0.92f, 1.0f);
            def.accent_color = Vec4(0.86f, 0.56f, 0.14f, 1.0f);
            def.roughness = 0.16f;
            def.metallic = 0.42f;
            break;

        case VehicleID::COACH_OBSIDIAN_VIP_6X2:
        default:
            // Obsidian Gloss-Black VIP 6x2 (Gloss Black + Mirror Chrome Eyebrow Trim)
            def.drive_config = VehicleDriveConfig::DRIVE_6X2_TAG;
            def.livery = LiveryID::LUXURY_METALLIC;
            def.kerb_weight_kg = 15700.0f;
            def.max_gvw_kg = 25000.0f;
            def.wheelbase_m = 6.45f;
            def.cab_height_m = 3.80f;
            def.engine_torque_nm = 2250.0f;
            def.engine_power_hp = 470.0f;
            def.passenger_capacity = 52;
            def.dimensions = Vec3(13.60f, 2.55f, 3.80f);
            def.base_color = Vec4(0.08f, 0.09f, 0.11f, 1.0f);
            def.accent_color = Vec4(0.86f, 0.89f, 0.94f, 1.0f);
            def.roughness = 0.14f;
            def.metallic = 0.72f;
            break;
    }
    return def;
}

} // namespace sphere::vehicles
