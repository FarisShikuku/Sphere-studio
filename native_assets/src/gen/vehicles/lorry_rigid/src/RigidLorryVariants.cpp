#include "RigidLorryVariants.h"

namespace sphere::vehicles {

VehicleDef CreateRigidLorryDefaultDef(VehicleID id) {
    VehicleDef def;
    def.id = id;
    def.name = GetVehicleName(id);
    def.category = GetVehicleCategory(id);
    def.fifth_wheel_offset_m = 0.0f;
    def.passenger_capacity = 2;
    def.wheel_radius_m = 0.50f;
    def.tire_width_m = 0.315f;
    def.ground_clearance_m = 0.32f;

    switch (id) {
        case VehicleID::LORRY_RIGID_BOX:
            def.drive_config = VehicleDriveConfig::DRIVE_4X2;
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 7250.0f;
            def.max_gvw_kg = 18000.0f;
            def.wheelbase_m = 4.80f;
            def.cab_height_m = 2.88f;
            def.engine_torque_nm = 1150.0f;
            def.engine_power_hp = 290.0f;
            def.dimensions = Vec3(8.20f, 2.50f, 3.45f);
            def.base_color = Vec4(0.16f, 0.34f, 0.62f, 1.0f);
            def.accent_color = Vec4(0.92f, 0.94f, 0.97f, 1.0f);
            def.roughness = 0.36f;
            def.metallic = 0.30f;
            break;

        case VehicleID::LORRY_RIGID_FLATBED:
            def.drive_config = VehicleDriveConfig::DRIVE_4X2;
            def.livery = LiveryID::INDUSTRIAL_WORKHORSE;
            def.kerb_weight_kg = 6650.0f;
            def.max_gvw_kg = 18000.0f;
            def.wheelbase_m = 5.00f;
            def.cab_height_m = 2.82f;
            def.engine_torque_nm = 1100.0f;
            def.engine_power_hp = 280.0f;
            def.dimensions = Vec3(8.40f, 2.50f, 2.88f);
            def.base_color = Vec4(0.86f, 0.52f, 0.08f, 1.0f);
            def.accent_color = Vec4(0.18f, 0.21f, 0.25f, 1.0f);
            def.roughness = 0.44f;
            def.metallic = 0.35f;
            break;

        case VehicleID::LORRY_RIGID_TIPPER:
            def.drive_config = VehicleDriveConfig::DRIVE_6X4;
            def.livery = LiveryID::INDUSTRIAL_WORKHORSE;
            def.kerb_weight_kg = 11200.0f;
            def.max_gvw_kg = 26000.0f;
            def.wheelbase_m = 4.30f;
            def.cab_height_m = 2.95f;
            def.engine_torque_nm = 2100.0f;
            def.engine_power_hp = 420.0f;
            def.dimensions = Vec3(8.50f, 2.55f, 3.25f);
            def.ground_clearance_m = 0.38f;
            def.wheel_radius_m = 0.54f;
            def.base_color = Vec4(0.92f, 0.66f, 0.08f, 1.0f);
            def.accent_color = Vec4(0.24f, 0.26f, 0.29f, 1.0f);
            def.roughness = 0.48f;
            def.metallic = 0.28f;
            break;

        case VehicleID::LORRY_RIGID_CEMENT_MIXER:
            def.drive_config = VehicleDriveConfig::DRIVE_8X4;
            def.livery = LiveryID::HIGHWAY_HAULAGE;
            def.kerb_weight_kg = 13800.0f;
            def.max_gvw_kg = 32000.0f;
            def.wheelbase_m = 5.10f;
            def.cab_height_m = 2.95f;
            def.engine_torque_nm = 2200.0f;
            def.engine_power_hp = 440.0f;
            def.dimensions = Vec3(9.10f, 2.55f, 3.75f);
            def.ground_clearance_m = 0.36f;
            def.wheel_radius_m = 0.53f;
            def.base_color = Vec4(0.88f, 0.90f, 0.93f, 1.0f);
            def.accent_color = Vec4(0.84f, 0.18f, 0.14f, 1.0f);
            def.roughness = 0.34f;
            def.metallic = 0.38f;
            break;

        case VehicleID::LORRY_UD_QUESTER_TIPPER:
            def.drive_config = VehicleDriveConfig::DRIVE_8X4;
            def.livery = LiveryID::INDUSTRIAL_WORKHORSE;
            def.kerb_weight_kg = 14200.0f;
            def.max_gvw_kg = 35000.0f;
            def.wheelbase_m = 5.05f;
            def.cab_height_m = 2.98f;
            def.engine_torque_nm = 2000.0f;
            def.engine_power_hp = 420.0f;
            def.dimensions = Vec3(9.10f, 2.55f, 3.38f);
            def.ground_clearance_m = 0.38f;
            def.wheel_radius_m = 0.54f;
            def.base_color = Vec4(0.11f, 0.37f, 0.41f, 1.0f);
            def.accent_color = Vec4(0.14f, 0.16f, 0.19f, 1.0f);
            def.roughness = 0.40f;
            def.metallic = 0.45f;
            def.has_exhaust_stacks = true;
            break;

        case VehicleID::LORRY_MAN_TGX_BOX:
            def.drive_config = VehicleDriveConfig::DRIVE_6X2_TAG;
            def.livery = LiveryID::HIGHWAY_HAULAGE;
            def.kerb_weight_kg = 10800.0f;
            def.max_gvw_kg = 26000.0f;
            def.wheelbase_m = 5.45f;
            def.cab_height_m = 3.22f;
            def.engine_torque_nm = 2200.0f;
            def.engine_power_hp = 440.0f;
            def.dimensions = Vec3(9.60f, 2.55f, 3.88f);
            def.ground_clearance_m = 0.30f;
            def.wheel_radius_m = 0.50f;
            def.base_color = Vec4(0.93f, 0.42f, 0.07f, 1.0f);
            def.accent_color = Vec4(0.94f, 0.95f, 0.97f, 1.0f);
            def.roughness = 0.24f;
            def.metallic = 0.34f;
            break;

        case VehicleID::LORRY_SCANIA_P320_CURTAINSIDER:
            def.drive_config = VehicleDriveConfig::DRIVE_6X2_TAG;
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 10450.0f;
            def.max_gvw_kg = 26000.0f;
            def.wheelbase_m = 5.35f;
            def.cab_height_m = 3.05f;
            def.engine_torque_nm = 1600.0f;
            def.engine_power_hp = 320.0f;
            def.dimensions = Vec3(9.45f, 2.55f, 3.78f);
            def.ground_clearance_m = 0.30f;
            def.wheel_radius_m = 0.50f;
            def.base_color = Vec4(0.94f, 0.95f, 0.97f, 1.0f);
            def.accent_color = Vec4(0.91f, 0.36f, 0.11f, 1.0f);
            def.roughness = 0.22f;
            def.metallic = 0.28f;
            break;

        case VehicleID::LORRY_RIGID_TANKER:
        default:
            def.drive_config = VehicleDriveConfig::DRIVE_6X4;
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 10400.0f;
            def.max_gvw_kg = 26000.0f;
            def.wheelbase_m = 4.70f;
            def.cab_height_m = 2.92f;
            def.engine_torque_nm = 1950.0f;
            def.engine_power_hp = 400.0f;
            def.dimensions = Vec3(8.80f, 2.52f, 3.30f);
            def.base_color = Vec4(0.84f, 0.87f, 0.91f, 1.0f);
            def.accent_color = Vec4(0.14f, 0.42f, 0.78f, 1.0f);
            def.roughness = 0.24f;
            def.metallic = 0.68f;
            break;
    }
    return def;
}

} // namespace sphere::vehicles
