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
            def.base_color = Vec4(0.14f, 0.28f, 0.52f, 1.0f);
            def.accent_color = Vec4(0.92f, 0.72f, 0.16f, 1.0f);
            def.roughness = 0.22f;
            def.metallic = 0.56f;
            break;

        case VehicleID::COACH_SLEEPER:
            def.drive_config = VehicleDriveConfig::DRIVE_6X2_TAG;
            def.livery = LiveryID::HIGHWAY_HAULAGE;
            def.kerb_weight_kg = 15800.0f;
            def.max_gvw_kg = 25000.0f;
            def.wheelbase_m = 6.50f;
            def.cab_height_m = 3.85f;
            def.engine_torque_nm = 2150.0f;
            def.engine_power_hp = 460.0f;
            def.passenger_capacity = 40;
            def.dimensions = Vec3(13.80f, 2.55f, 3.85f);
            def.base_color = Vec4(0.68f, 0.14f, 0.18f, 1.0f);
            def.accent_color = Vec4(0.94f, 0.94f, 0.96f, 1.0f);
            def.roughness = 0.24f;
            def.metallic = 0.48f;
            break;

        case VehicleID::COACH_TOURING:
            def.drive_config = VehicleDriveConfig::DRIVE_4X2;
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 12900.0f;
            def.max_gvw_kg = 19000.0f;
            def.wheelbase_m = 6.05f;
            def.cab_height_m = 3.58f;
            def.engine_torque_nm = 1780.0f;
            def.engine_power_hp = 400.0f;
            def.passenger_capacity = 50;
            def.dimensions = Vec3(12.80f, 2.55f, 3.58f);
            def.base_color = Vec4(0.88f, 0.91f, 0.95f, 1.0f);
            def.accent_color = Vec4(0.10f, 0.52f, 0.82f, 1.0f);
            def.roughness = 0.20f;
            def.metallic = 0.40f;
            break;

        case VehicleID::COACH_LUXURY:
        default:
            def.drive_config = VehicleDriveConfig::DRIVE_6X2_TAG;
            def.livery = LiveryID::LUXURY_METALLIC;
            def.kerb_weight_kg = 16400.0f;
            def.max_gvw_kg = 26000.0f;
            def.wheelbase_m = 6.75f;
            def.cab_height_m = 3.88f;
            def.engine_torque_nm = 2350.0f;
            def.engine_power_hp = 510.0f;
            def.passenger_capacity = 36;
            def.dimensions = Vec3(14.20f, 2.55f, 3.88f);
            def.base_color = Vec4(0.14f, 0.16f, 0.21f, 1.0f);
            def.accent_color = Vec4(0.86f, 0.72f, 0.42f, 1.0f);
            def.roughness = 0.16f;
            def.metallic = 0.78f;
            break;
    }
    return def;
}

} // namespace sphere::vehicles
