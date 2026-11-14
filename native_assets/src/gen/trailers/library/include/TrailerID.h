#pragma once
#include <cstdint>

namespace sphere::trailers {

enum class TrailerCategory : uint32_t {
    FLATBED = 0,
    TANKER,
    BOX_VAN,
    CHASSIS,
    LOGGING,
    CURTAINSIDER,
    BULK_HOPPER,
    CAR_CARRIER,
    HEAVY_HAULAGE,
    DOLLY,
    RECREATIONAL,
    SPECIALTY,
    COUNT
};

enum class TrailerID : uint32_t {
    // 1. Flatbeds (5)
    FLATBED_STD = 0,
    FLATBED_EXTENDABLE,
    FLATBED_STEP_DECK,
    FLATBED_DROP_DECK,
    FLATBED_SIDE_LOADER,

    // 2. Tankers (6)
    TANKER_WATER,
    TANKER_FUEL_MULTI_COMPARTMENT,
    TANKER_MILK,
    TANKER_CHEMICAL,
    TANKER_LPG_GAS,
    TANKER_POWDER,

    // 3. Box Vans (5)
    DRY_VAN_53FT,
    BOX_REEFER,
    BOX_PARCEL_VAN,
    BOX_MOVING_VAN,
    BOX_DOUBLE_DECK,

    // 4. Container Chassis (5)
    CHASSIS_20FT,
    CHASSIS_40FT,
    CHASSIS_GOOSENECK,
    CHASSIS_SLIDER,
    CHASSIS_TIPPING,

    // 5. Logging (3)
    LOGGING_POLE_TRAILER,
    LOGGING_BUNK_TRAILER,
    LOGGING_EXTENDABLE_LOG,

    // 6. Curtainsiders (4)
    CURTAINSIDER_STANDARD,
    CURTAINSIDER_XL,
    CURTAINSIDER_DOUBLE_DECK,
    CURTAINSIDER_SIDE_LOADING,

    // 7. Bulk Hoppers (4)
    HOPPER_GRAIN,
    HOPPER_CEMENT_POWDER,
    HOPPER_AGGREGATE_TIPPER,
    HOPPER_SIDE_TIPPER,

    // 8. Car Carriers (4)
    CARCARRIER_SINGLE_DECK,
    CARCARRIER_DOUBLE_DECK,
    CARCARRIER_OPEN_TRANSPORTER,
    CARCARRIER_ENCLOSED,

    // 9. Heavy Haulage (4)
    HEAVY_LOWBOY,
    HEAVY_MULTI_AXLE_MODULAR,
    HEAVY_BLADE_TRANSPORTER,
    HEAVY_TUNNEL_TRANSPORTER,

    // 10. Dollies (3)
    DOLLY_CONVERTER,
    DOLLY_A_DOLLY,
    DOLLY_JEEP,

    // 11. Recreational (4)
    RECREATION_BOAT_TRAILER,
    RECREATION_ATV_TRAILER,
    RECREATION_CAMPER_TRAILER,
    RECREATION_UTILITY_TRAILER,

    // 12. Specialty (4)
    SPECIALTY_LIVESTOCK,
    SPECIALTY_WASTE_TANKER,
    SPECIALTY_MILITARY_TRANSPORT,
    SPECIALTY_ARMORED_CAR,

    COUNT
};

const char* GetTrailerName(TrailerID id);
const char* GetTrailerCategoryName(TrailerCategory cat);
const char* GetTrailerCategory(TrailerID id);
TrailerCategory GetTrailerCategoryEnum(TrailerID id);

} // namespace sphere::trailers
