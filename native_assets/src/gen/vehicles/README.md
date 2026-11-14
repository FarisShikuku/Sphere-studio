# Sphere Procedural Vehicles Library (`gen/vehicles/`)

Procedural C++20 / Vulkan 1.3 vehicle synthesis module dedicated to **Lorries (Rigid & Articulated Tractor Units)** and **Buses (City Transit, Intercity Coaches, Minibuses / Kenyan 14-Seater Matatus)**.

## Coordinate System & Metric Scale (1:1 Real-World Meters)
- **Y = 0.0 m**: Ground contact plane (`+Y` up; tires always rest solidly on `Y = 0`).
- **X-axis**: Vehicle longitudinal axis (`-X` Front bumper / windshield, `+X` Rear deck / fifth-wheel / taillights).
- **Z-axis**: Vehicle lateral track (`-Z` Left/Port side, `+Z` Right/Starboard side).
- **Zero-Transparency Glass**: Windshields and bus windows use opaque high-contrast dark reflective glass (`alpha = 1.0f`) to avoid hollow-interior triangle artifacts.

## Modules
1. `library/`: `VehicleID`, `VehicleDef`, `IVehicle`, `VehicleRegistry` (singleton + lazy mesh/Vulkan buffer cache + tractor-to-trailer `Hitch()` integration).
2. `lorry_rigid/`: `BOX_TRUCK`, `FLATBED_TRUCK`, `TIPPER`, `CEMENT_MIXER`, `TANKER_RIGID`.
3. `lorry_articulated/`: `CAB_OVER_ENGINE`, `CONVENTIONAL_LONG_NOSE`, `EURO_CAB`, `AMERICAN_CAB` (with exposed SAE 2" fifth-wheel hitch saddle).
4. `bus_city/`: `STANDARD_TRANSIT`, `DOUBLE_DECKER`, `ARTICULATED_BENDY` (paired body hulls joined by pleated rubber accordion bellows), `LOW_FLOOR`.
5. `bus_coach/`: `HIGH_DECKER`, `SLEEPER_COACH`, `TOURING_COACH`, `LUXURY_COACH`.
6. `bus_minibus/`: `MATATU_14_SEATER` (Nairobi Nganya algorithmic graffiti livery via `SetRandomLivery()`), `SPRINTER_VAN`, `SHUTTLE_BUS`, `SCHOOL_VAN`.
7. `cab_components/`: Shared doors, windshields, heavy chrome grilles, LED lightbars/headlamps, wing mirrors.
8. `chassis_components/`: Shared C-channel frame rails, closed cylindrical fuel tanks, vertical chrome exhaust stacks, battery boxes, engine/drivetrain blocks.

## Rigging & Physics Integration
- `VehicleDef::fifth_wheel_offset_m` exposes the longitudinal coupling point for `ITrailer::GetDef().kingpin_offset_m`.
- `IVehicle::GetTotalMassKg()` computes tractor/chassis kerb mass + passenger load + hitched trailer empty mass + loaded `CargoRegistry` freight mass.
