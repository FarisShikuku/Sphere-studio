#include "MinibusVariants.h"

namespace sphere::vehicles {

VehicleDef CreateMinibusDefaultDef(VehicleID id) {
    VehicleDef def;
    def.id = id;
    def.name = GetVehicleName(id);
    def.category = GetVehicleCategory(id);
    def.fifth_wheel_offset_m = 0.0f;
    def.drive_config = VehicleDriveConfig::DRIVE_4X2;
    def.ground_clearance_m = 0.22f;

    switch (id) {
        case VehicleID::MATATU_14_SEATER:
            def.livery = LiveryID::NAIROBI_NGANYA;
            def.kerb_weight_kg = 2250.0f;
            def.max_gvw_kg = 3500.0f;
            def.wheelbase_m = 3.11f;
            def.cab_height_m = 2.28f;
            def.engine_torque_nm = 380.0f;
            def.engine_power_hp = 152.0f;
            def.passenger_capacity = 14;
            def.wheel_radius_m = 0.36f;
            def.tire_width_m = 0.215f;
            def.has_roof_rack = true;
            def.dimensions = Vec3(5.38f, 1.88f, 2.34f);
            def.base_color = Vec4(0.11f, 0.13f, 0.20f, 1.0f);
            def.accent_color = Vec4(0.96f, 0.18f, 0.56f, 1.0f);
            def.roughness = 0.24f;
            def.metallic = 0.48f;
            break;

        case VehicleID::SPRINTER_VAN:
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 2780.0f;
            def.max_gvw_kg = 5000.0f;
            def.wheelbase_m = 4.32f;
            def.cab_height_m = 2.74f;
            def.engine_torque_nm = 440.0f;
            def.engine_power_hp = 190.0f;
            def.passenger_capacity = 20;
            def.wheel_radius_m = 0.38f;
            def.tire_width_m = 0.235f;
            def.has_roof_rack = false;
            def.dimensions = Vec3(6.95f, 2.02f, 2.74f);
            def.base_color = Vec4(0.90f, 0.92f, 0.95f, 1.0f);
            def.accent_color = Vec4(0.14f, 0.38f, 0.72f, 1.0f);
            def.roughness = 0.30f;
            def.metallic = 0.28f;
            break;

        case VehicleID::SHUTTLE_BUS:
            def.livery = LiveryID::TRANSIT_MUNICIPAL;
            def.kerb_weight_kg = 4600.0f;
            def.max_gvw_kg = 7500.0f;
            def.wheelbase_m = 4.55f;
            def.cab_height_m = 2.95f;
            def.engine_torque_nm = 620.0f;
            def.engine_power_hp = 215.0f;
            def.passenger_capacity = 28;
            def.wheel_radius_m = 0.41f;
            def.tire_width_m = 0.245f;
            def.has_roof_rack = false;
            def.dimensions = Vec3(7.80f, 2.34f, 2.95f);
            def.base_color = Vec4(0.12f, 0.48f, 0.44f, 1.0f);
            def.accent_color = Vec4(0.96f, 0.74f, 0.12f, 1.0f);
            def.roughness = 0.32f;
            def.metallic = 0.30f;
            break;

        case VehicleID::SCHOOL_VAN:
        default:
            def.livery = LiveryID::INDUSTRIAL_WORKHORSE;
            def.kerb_weight_kg = 3450.0f;
            def.max_gvw_kg = 6200.0f;
            def.wheelbase_m = 3.95f;
            def.cab_height_m = 2.68f;
            def.engine_torque_nm = 510.0f;
            def.engine_power_hp = 185.0f;
            def.passenger_capacity = 24;
            def.wheel_radius_m = 0.39f;
            def.tire_width_m = 0.235f;
            def.has_roof_rack = false;
            def.dimensions = Vec3(6.60f, 2.15f, 2.68f);
            def.base_color = Vec4(0.96f, 0.70f, 0.06f, 1.0f);
            def.accent_color = Vec4(0.12f, 0.13f, 0.16f, 1.0f);
            def.roughness = 0.36f;
            def.metallic = 0.20f;
            break;
    }
    return def;
}

} // namespace sphere::vehicles
