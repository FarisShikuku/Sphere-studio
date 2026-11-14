#include "ArticulatedTractorVariants.h"

namespace sphere::vehicles {

VehicleDef CreateArticulatedTractorDefaultDef(VehicleID id) {
    VehicleDef def;
    def.id = id;
    def.name = GetVehicleName(id);
    def.category = GetVehicleCategory(id);
    def.passenger_capacity = 2;
    def.wheel_radius_m = 0.52f;
    def.tire_width_m = 0.315f;
    def.ground_clearance_m = 0.30f;

    switch (id) {
        case VehicleID::TRACTOR_CAB_OVER:
            def.drive_config = VehicleDriveConfig::DRIVE_6X4;
            def.livery = LiveryID::HIGHWAY_HAULAGE;
            def.kerb_weight_kg = 8450.0f;
            def.max_gvw_kg = 44000.0f;
            def.wheelbase_m = 3.90f;
            def.fifth_wheel_offset_m = 1.65f;
            def.cab_height_m = 3.45f;
            def.engine_torque_nm = 2550.0f;
            def.engine_power_hp = 520.0f;
            def.dimensions = Vec3(6.30f, 2.52f, 3.60f);
            def.has_exhaust_stacks = true;
            def.base_color = Vec4(0.76f, 0.14f, 0.14f, 1.0f);
            def.accent_color = Vec4(0.92f, 0.94f, 0.96f, 1.0f);
            def.roughness = 0.28f;
            def.metallic = 0.52f;
            break;

        case VehicleID::TRACTOR_CONVENTIONAL:
            def.drive_config = VehicleDriveConfig::DRIVE_6X4;
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 9100.0f;
            def.max_gvw_kg = 48000.0f;
            def.wheelbase_m = 5.10f;
            def.fifth_wheel_offset_m = 1.95f;
            def.cab_height_m = 3.35f;
            def.engine_torque_nm = 2780.0f;
            def.engine_power_hp = 580.0f;
            def.dimensions = Vec3(7.40f, 2.55f, 3.55f);
            def.has_exhaust_stacks = true;
            def.base_color = Vec4(0.14f, 0.34f, 0.64f, 1.0f);
            def.accent_color = Vec4(0.84f, 0.86f, 0.90f, 1.0f);
            def.roughness = 0.26f;
            def.metallic = 0.58f;
            break;

        case VehicleID::TRACTOR_EURO_CAB:
            def.drive_config = VehicleDriveConfig::DRIVE_4X2;
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 7600.0f;
            def.max_gvw_kg = 40000.0f;
            def.wheelbase_m = 3.75f;
            def.fifth_wheel_offset_m = 1.55f;
            def.cab_height_m = 3.85f;
            def.engine_torque_nm = 2500.0f;
            def.engine_power_hp = 500.0f;
            def.dimensions = Vec3(6.05f, 2.52f, 3.92f);
            def.has_exhaust_stacks = false;
            def.base_color = Vec4(0.92f, 0.94f, 0.96f, 1.0f);
            def.accent_color = Vec4(0.12f, 0.42f, 0.82f, 1.0f);
            def.roughness = 0.22f;
            def.metallic = 0.44f;
            break;

        case VehicleID::TRACTOR_AMERICAN_CAB:
        default:
            def.drive_config = VehicleDriveConfig::DRIVE_6X4;
            def.livery = LiveryID::LUXURY_METALLIC;
            def.kerb_weight_kg = 9800.0f;
            def.max_gvw_kg = 52000.0f;
            def.wheelbase_m = 5.65f;
            def.fifth_wheel_offset_m = 2.15f;
            def.cab_height_m = 3.95f;
            def.engine_torque_nm = 2950.0f;
            def.engine_power_hp = 605.0f;
            def.dimensions = Vec3(7.95f, 2.55f, 4.02f);
            def.has_exhaust_stacks = true;
            def.base_color = Vec4(0.14f, 0.17f, 0.24f, 1.0f);
            def.accent_color = Vec4(0.84f, 0.70f, 0.38f, 1.0f);
            def.roughness = 0.20f;
            def.metallic = 0.72f;
            break;
    }
    return def;
}

} // namespace sphere::vehicles
