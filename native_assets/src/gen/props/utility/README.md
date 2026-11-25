# Utility Props Engine

Procedural utility infrastructure subsystem for power distribution, telecommunications, and municipal water networks.

## Components
- **ElectricPostGen**: Wooden, concrete, and steel utility poles with crossarms and ceramic bell insulators.
- **TransformerGen**: Pole-top oil drums and pad-mounted high-voltage step-down cabinets.
- **PowerMeterGen**: Residential and commercial smart energy meters with conduit couplings.
- **StreetLightGen**: Arterial highway cobra-head luminaires with parametric cantilever arms.
- **WaterTowerGen**: Elevated municipal multi-column structural water towers with catwalks and conical roofs.
- **TelecomTowerGen**: Triangular steel lattice cellular communications masts with microwave drums and sector panels.

## Shader & Compute Pipeline
- `electric_post.comp`: Parametric cylindrical post extrusion and crossarm mounting.
- `wire_catenary.comp`: Hyperbolic cosine catenary cable curves spanning poles.
- `transformer.comp`: Radial cooling fins and porcelain bushing synthesis.
- `street_light.comp`: Spline-based arm lofting and reflector lens positioning.
- `water_tower.comp`: Toroidal bowl, riser pipe, and perimeter bracing lattice.
- `telecom_tower.comp`: Recursive K-bracing and antenna array clustering.
