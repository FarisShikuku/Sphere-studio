#include "CityBusVariants.h"

namespace sphere::vehicles {

VehicleDef CreateCityBusDefaultDef(VehicleID id) {
    VehicleDef def;
    def.id = id;
    def.name = GetVehicleName(id);
    def.category = GetVehicleCategory(id);
    def.fifth_wheel_offset_m = 0.0f;
    def.wheel_radius_m = 0.48f;
    def.tire_width_m = 0.295f;
    def.ground_clearance_m = 0.24f;

    switch (id) {
        case VehicleID::BUS_CITY_STANDARD:
            def.drive_config = VehicleDriveConfig::DRIVE_4X2;
            def.livery = LiveryID::TRANSIT_MUNICIPAL;
            def.kerb_weight_kg = 11400.0f;
            def.max_gvw_kg = 19000.0f;
            def.wheelbase_m = 5.90f;
            def.cab_height_m = 3.12f;
            def.engine_torque_nm = 1250.0f;
            def.engine_power_hp = 310.0f;
            def.passenger_capacity = 78;
            def.dimensions = Vec3(12.00f, 2.55f, 3.12f);
            def.base_color = Vec4(0.10f, 0.48f, 0.36f, 1.0f);
            def.accent_color = Vec4(0.95f, 0.78f, 0.12f, 1.0f);
            def.roughness = 0.34f;
            def.metallic = 0.25f;
            break;

        case VehicleID::BUS_CITY_DOUBLE_DECKER:
            def.drive_config = VehicleDriveConfig::DRIVE_6X2_TAG;
            def.livery = LiveryID::HIGHWAY_HAULAGE;
            def.kerb_weight_kg = 14800.0f;
            def.max_gvw_kg = 24000.0f;
            def.wheelbase_m = 5.45f;
            def.cab_height_m = 4.38f;
            def.engine_torque_nm = 1480.0f;
            def.engine_power_hp = 360.0f;
            def.passenger_capacity = 104;
            def.dimensions = Vec3(10.90f, 2.55f, 4.38f);
            def.base_color = Vec4(0.78f, 0.12f, 0.14f, 1.0f);
            def.accent_color = Vec4(0.94f, 0.94f, 0.96f, 1.0f);
            def.roughness = 0.28f;
            def.metallic = 0.30f;
            break;

        case VehicleID::BUS_CITY_ARTICULATED_BENDY:
            def.drive_config = VehicleDriveConfig::DRIVE_6X2_TAG;
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 17600.0f;
            def.max_gvw_kg = 28000.0f;
            def.wheelbase_m = 5.85f;
            def.cab_height_m = 3.18f;
            def.engine_torque_nm = 1650.0f;
            def.engine_power_hp = 390.0f;
            def.passenger_capacity = 145;
            def.dimensions = Vec3(18.00f, 2.55f, 3.18f);
            def.base_color = Vec4(0.14f, 0.36f, 0.68f, 1.0f);
            def.accent_color = Vec4(0.96f, 0.62f, 0.10f, 1.0f);
            def.roughness = 0.32f;
            def.metallic = 0.28f;
            break;

        case VehicleID::BUS_CITY_LOW_FLOOR:
        default:
            def.drive_config = VehicleDriveConfig::DRIVE_4X2;
            def.livery = LiveryID::TRANSIT_MUNICIPAL;
            def.kerb_weight_kg = 12200.0f;
            def.max_gvw_kg = 19500.0f;
            def.wheelbase_m = 6.05f;
            def.cab_height_m = 3.28f;
            def.engine_torque_nm = 2400.0f; // High instant electric traction torque
            def.engine_power_hp = 400.0f;
            def.passenger_capacity = 86;
            def.ground_clearance_m = 0.21f;
            def.dimensions = Vec3(12.20f, 2.55f, 3.28f);
            def.base_color = Vec4(0.90f, 0.93f, 0.96f, 1.0f);
            def.accent_color = Vec4(0.08f, 0.68f, 0.56f, 1.0f);
            def.roughness = 0.24f;
            def.metallic = 0.34f;
            break;
    }
    return def;
}

} // namespace sphere::vehicles
