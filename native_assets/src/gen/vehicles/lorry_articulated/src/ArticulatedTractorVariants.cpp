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

        case VehicleID::TRACTOR_VOLVO_FH_ELECTRIC:
            def.drive_config = VehicleDriveConfig::DRIVE_4X2;
            def.livery = LiveryID::TRANSIT_MUNICIPAL;
            def.kerb_weight_kg = 8650.0f;
            def.max_gvw_kg = 44000.0f;
            def.wheelbase_m = 3.80f;
            def.fifth_wheel_offset_m = 1.55f;
            def.cab_height_m = 3.90f;
            def.engine_torque_nm = 2400.0f;
            def.engine_power_hp = 666.0f;
            def.dimensions = Vec3(6.08f, 2.54f, 3.96f);
            def.has_exhaust_stacks = false;
            def.base_color = Vec4(0.23f, 0.43f, 0.40f, 1.0f);
            def.accent_color = Vec4(0.14f, 0.80f, 0.85f, 1.0f);
            def.roughness = 0.20f;
            def.metallic = 0.52f;
            break;

        case VehicleID::TRACTOR_MERCEDES_EACTROS_600:
            def.drive_config = VehicleDriveConfig::DRIVE_4X2;
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 11400.0f;
            def.max_gvw_kg = 44000.0f;
            def.wheelbase_m = 4.00f;
            def.fifth_wheel_offset_m = 1.60f;
            def.cab_height_m = 3.95f;
            def.engine_torque_nm = 2800.0f;
            def.engine_power_hp = 816.0f;
            def.dimensions = Vec3(6.25f, 2.54f, 4.00f);
            def.has_exhaust_stacks = false;
            def.base_color = Vec4(0.11f, 0.29f, 0.48f, 1.0f);
            def.accent_color = Vec4(0.55f, 0.88f, 0.14f, 1.0f);
            def.roughness = 0.18f;
            def.metallic = 0.56f;
            break;

        case VehicleID::TRACTOR_MERCEDES_GENH2:
            def.drive_config = VehicleDriveConfig::DRIVE_4X2;
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 9850.0f;
            def.max_gvw_kg = 40000.0f;
            def.wheelbase_m = 4.25f;
            def.fifth_wheel_offset_m = 1.75f;
            def.cab_height_m = 3.92f;
            def.engine_torque_nm = 2500.0f;
            def.engine_power_hp = 625.0f;
            def.dimensions = Vec3(6.48f, 2.54f, 3.98f);
            def.has_exhaust_stacks = false;
            def.base_color = Vec4(0.81f, 0.84f, 0.88f, 1.0f);
            def.accent_color = Vec4(0.07f, 0.61f, 0.88f, 1.0f);
            def.roughness = 0.20f;
            def.metallic = 0.62f;
            break;

        case VehicleID::TRACTOR_SCANIA_45R_ELECTRIC:
            def.drive_config = VehicleDriveConfig::DRIVE_4X2;
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 9750.0f;
            def.max_gvw_kg = 44000.0f;
            def.wheelbase_m = 3.95f;
            def.fifth_wheel_offset_m = 1.62f;
            def.cab_height_m = 3.82f;
            def.engine_torque_nm = 2800.0f;
            def.engine_power_hp = 610.0f;
            def.dimensions = Vec3(6.15f, 2.54f, 3.90f);
            def.has_exhaust_stacks = false;
            def.base_color = Vec4(0.93f, 0.95f, 0.97f, 1.0f);
            def.accent_color = Vec4(0.11f, 0.49f, 0.88f, 1.0f);
            def.roughness = 0.20f;
            def.metallic = 0.34f;
            break;

        case VehicleID::TRACTOR_SCANIA_R560_SUPER:
            def.drive_config = VehicleDriveConfig::DRIVE_4X2;
            def.livery = LiveryID::LUXURY_METALLIC;
            def.kerb_weight_kg = 7900.0f;
            def.max_gvw_kg = 40000.0f;
            def.wheelbase_m = 3.75f;
            def.fifth_wheel_offset_m = 1.54f;
            def.cab_height_m = 3.85f;
            def.engine_torque_nm = 2800.0f;
            def.engine_power_hp = 560.0f;
            def.dimensions = Vec3(6.00f, 2.54f, 3.92f);
            def.has_exhaust_stacks = false;
            def.base_color = Vec4(0.28f, 0.31f, 0.34f, 1.0f);
            def.accent_color = Vec4(0.79f, 0.19f, 0.17f, 1.0f);
            def.roughness = 0.20f;
            def.metallic = 0.64f;
            break;

        case VehicleID::TRACTOR_SCANIA_S770_LONGLINE:
            def.drive_config = VehicleDriveConfig::DRIVE_6X4;
            def.livery = LiveryID::HIGHWAY_HAULAGE;
            def.kerb_weight_kg = 10400.0f;
            def.max_gvw_kg = 50000.0f;
            def.wheelbase_m = 4.70f;
            def.fifth_wheel_offset_m = 1.98f;
            def.cab_height_m = 3.96f;
            def.engine_torque_nm = 3700.0f;
            def.engine_power_hp = 770.0f;
            def.dimensions = Vec3(7.35f, 2.55f, 4.02f);
            def.has_exhaust_stacks = false;
            def.base_color = Vec4(0.62f, 0.09f, 0.13f, 1.0f);
            def.accent_color = Vec4(0.13f, 0.15f, 0.18f, 1.0f);
            def.roughness = 0.18f;
            def.metallic = 0.54f;
            break;

        case VehicleID::TRACTOR_SCANIA_40R_XT_ELECTRIC:
            def.drive_config = VehicleDriveConfig::DRIVE_4X2;
            def.livery = LiveryID::INDUSTRIAL_WORKHORSE;
            def.kerb_weight_kg = 9450.0f;
            def.max_gvw_kg = 42000.0f;
            def.wheelbase_m = 3.95f;
            def.fifth_wheel_offset_m = 1.62f;
            def.cab_height_m = 3.78f;
            def.ground_clearance_m = 0.35f;
            def.wheel_radius_m = 0.54f;
            def.engine_torque_nm = 2500.0f;
            def.engine_power_hp = 540.0f;
            def.dimensions = Vec3(6.20f, 2.54f, 3.86f);
            def.has_exhaust_stacks = false;
            def.base_color = Vec4(0.94f, 0.95f, 0.96f, 1.0f);
            def.accent_color = Vec4(0.12f, 0.50f, 0.90f, 1.0f);
            def.roughness = 0.24f;
            def.metallic = 0.36f;
            break;

        case VehicleID::TRACTOR_FORD_FMAX_500:
            def.drive_config = VehicleDriveConfig::DRIVE_4X2;
            def.livery = LiveryID::HIGHWAY_HAULAGE;
            def.kerb_weight_kg = 7850.0f;
            def.max_gvw_kg = 40000.0f;
            def.wheelbase_m = 3.60f;
            def.fifth_wheel_offset_m = 1.50f;
            def.cab_height_m = 3.92f;
            def.engine_torque_nm = 2500.0f;
            def.engine_power_hp = 500.0f;
            def.dimensions = Vec3(5.96f, 2.54f, 3.98f);
            def.has_exhaust_stacks = false;
            def.base_color = Vec4(0.74f, 0.14f, 0.13f, 1.0f);
            def.accent_color = Vec4(0.11f, 0.30f, 0.62f, 1.0f);
            def.roughness = 0.20f;
            def.metallic = 0.48f;
            break;

        case VehicleID::TRACTOR_IVECO_SEWAY_ELECTRIC:
            def.drive_config = VehicleDriveConfig::DRIVE_4X2;
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 9550.0f;
            def.max_gvw_kg = 44000.0f;
            def.wheelbase_m = 3.85f;
            def.fifth_wheel_offset_m = 1.58f;
            def.cab_height_m = 3.88f;
            def.engine_torque_nm = 2400.0f;
            def.engine_power_hp = 645.0f;
            def.dimensions = Vec3(6.14f, 2.54f, 3.94f);
            def.has_exhaust_stacks = false;
            def.base_color = Vec4(0.77f, 0.80f, 0.85f, 1.0f);
            def.accent_color = Vec4(0.08f, 0.47f, 0.83f, 1.0f);
            def.roughness = 0.20f;
            def.metallic = 0.56f;
            break;

        case VehicleID::TRACTOR_VW_CONSTELLATION_25390:
            def.drive_config = VehicleDriveConfig::DRIVE_6X2_TAG;
            def.livery = LiveryID::FLEET_CORPORATE;
            def.kerb_weight_kg = 8650.0f;
            def.max_gvw_kg = 45000.0f;
            def.wheelbase_m = 4.30f;
            def.fifth_wheel_offset_m = 1.65f;
            def.cab_height_m = 3.45f;
            def.engine_torque_nm = 1750.0f;
            def.engine_power_hp = 390.0f;
            def.dimensions = Vec3(6.65f, 2.50f, 3.55f);
            def.has_exhaust_stacks = false;
            def.base_color = Vec4(0.95f, 0.96f, 0.97f, 1.0f);
            def.accent_color = Vec4(0.13f, 0.15f, 0.18f, 1.0f);
            def.roughness = 0.24f;
            def.metallic = 0.26f;
            break;

        case VehicleID::TRACTOR_VW_METEOR_28460:
            def.drive_config = VehicleDriveConfig::DRIVE_6X4;
            def.livery = LiveryID::LUXURY_METALLIC;
            def.kerb_weight_kg = 9450.0f;
            def.max_gvw_kg = 48500.0f;
            def.wheelbase_m = 4.55f;
            def.fifth_wheel_offset_m = 1.85f;
            def.cab_height_m = 3.86f;
            def.engine_torque_nm = 2300.0f;
            def.engine_power_hp = 460.0f;
            def.dimensions = Vec3(6.85f, 2.54f, 3.92f);
            def.has_exhaust_stacks = false;
            def.base_color = Vec4(0.71f, 0.74f, 0.78f, 1.0f);
            def.accent_color = Vec4(0.13f, 0.15f, 0.18f, 1.0f);
            def.roughness = 0.22f;
            def.metallic = 0.62f;
            break;

        case VehicleID::TRACTOR_MAN_TGX_18510:
            def.drive_config = VehicleDriveConfig::DRIVE_4X2;
            def.livery = LiveryID::HIGHWAY_HAULAGE;
            def.kerb_weight_kg = 7750.0f;
            def.max_gvw_kg = 40000.0f;
            def.wheelbase_m = 3.60f;
            def.fifth_wheel_offset_m = 1.52f;
            def.cab_height_m = 3.88f;
            def.engine_torque_nm = 2600.0f;
            def.engine_power_hp = 510.0f;
            def.dimensions = Vec3(5.98f, 2.54f, 3.94f);
            def.has_exhaust_stacks = false;
            def.base_color = Vec4(0.92f, 0.42f, 0.08f, 1.0f);
            def.accent_color = Vec4(0.10f, 0.12f, 0.14f, 1.0f);
            def.roughness = 0.22f;
            def.metallic = 0.36f;
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
