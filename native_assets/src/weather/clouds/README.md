# Volumetric Clouds Subsystem (Nubis-Decima Engine)

Converted from [Robert Beckebans's OpenGL-VolumetricClouds](https://github.com/RobertBeckebans/OpenGL-VolumetricClouds) into Sphere Studio's Vulkan 1.3 / C++20 compute pipeline.

## Architectural Improvements & Features
- **Spherical Atmosphere Shell**: Dual-boundary planetary geometry (`SPHERE_INNER_RADIUS = 705,000m`, `SPHERE_OUTER_RADIUS = 722,000m`).
- **Nubis Cloud Types & Density Gradients**:
  - `STRATUS_GRADIENT` $(0.0, 0.10, 0.20, 0.30)$ — thin horizontal low-altitude fog veils.
  - `STRATOCUMULUS_GRADIENT` $(0.02, 0.20, 0.48, 0.625)$ — rolling fluffy blanket sheets.
  - `CUMULUS_GRADIENT` $(0.00, 0.1625, 0.88, 0.98)$ — towering 3D cauliflower heaps with flat dark bases.
- **Physically-Based Optical Radiative Transfer**:
  - **Beer's Law**: $T = \exp(-d)$ exponential extinction along sightline and sun shadow rays.
  - **Beer-Powder Sugar Effect**: $P = (1.0 - \exp(-2d))$ representing multiple forward scattering in dense droplet clusters.
  - **Dual-Lobe Henyey-Greenstein**: Forward peak ($g_1 = 0.8$) producing vibrant solar "Silver Lining" halos, blended with backscattering ($g_2 = -0.2$).
- **Cone-Stepped Light Sampling**: 6-vector jittered cone sampling towards the sun for soft volumetric self-shadowing without expensive multi-pass shadowmaps.
- **Multi-Octave 3D Perlin-Worley Noise**: Low-frequency billowy shapes eroded by high-frequency 3D cellular Worley noise with altitude-dependent wind shear deflection.
