# Energy Props Engine

Procedural renewable energy generation, energy storage systems, and power substation infrastructure.

## Components
- **WindTurbineGen**: Mega-watt scale horizontal-axis wind turbine with tapered tubular tower, streamlined nacelle, and aerodynamic NACA airfoil blades.
- **SolarPanelGen**: Photovoltaic single module with extruded aluminum perimeter frame, glass laminate, and silver conductive busbars.
- **SolarFarmGen**: Utility-scale solar farm layout with single-axis tracking torque tubes, pile-driven piers, and central inverter skids.
- **BatteryBankGen**: Grid-scale Battery Energy Storage System (BESS) based on 40ft modular containers with HVAC chillers and fire suppression systems.
- **SubstationGen**: High-voltage step-up/step-down substation with oil containment bunds, radiator banks, HV porcelain bushings, lattice gantries, and overhead busbars.

## Shaders & Compute
- `wind_turbine_tower.comp`: Tubular steel shell lofting with diameter taper.
- `wind_turbine_blade.comp`: Airfoil spline lofting across span.
- `wind_turbine_rotation.comp`: Rotor spin compute kernel.
- `solar_panel.comp`: Panel framing and cell atlas synthesis.
- `solar_farm_layout.comp`: Row pitch, solar tracker gimbal, and sun angle alignment.
- `solar_tracker.comp`: Actuator and slewing drive dynamics.
- `battery_container.comp`: ISO container corrugations and HVAC louvers.
- `substation.comp`: Transformer yard layout, lattice gantry beams, and catenary busbar sag.
