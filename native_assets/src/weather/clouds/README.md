# Meteoros Volumetric Cloudscape Subsystem

Vulkan 1.3 / C++20 real-time volumetric cloudscape renderer converted and adapted from [AmanSachan1/Meteoros](https://github.com/AmanSachan1/Meteoros) (Aman Sachan & Meghana Seshadri, CIS 565 GPU Programming and Architecture, University of Pennsylvania), based on Guerrilla Games' Decima Engine ('Nubis' SIGGRAPH 2015 & 2017 papers).

## Key Innovations & Rendering Architecture

### 1. Multi-Scattering Optical Radiative Transfer
- **Dual Beer-Lambert Law Approximation**:
  $$E_{\text{multi}} = \exp(-d) + \exp(-0.25 d) \cdot 0.5 \cdot k_{\text{multi}}$$
  Unlike single-scattering Beer's Law ($e^{-d}$), which plunges cloud interiors into unnatural pitch-black darkness, the secondary Beer-Lambert octave pushes scattered light deep into thick cloud cauliflower heaps, simulating forward multiple scattering.
- **Beer-Powder Sugar Effect**:
  $$P(d) = 1.0 - \exp(-2.0 \cdot d \cdot k_{\text{powder}})$$
  Simulates in-scattering near droplet boundaries and prevents thick cloud centers from collapsing into dark silhouettes.
- **Dual-Lobe Henyey-Greenstein (HG) Directional Phase**:
  $$P(\theta) = (1 - \alpha) \cdot HG(\cos\theta, g_1) + \alpha \cdot HG(\cos\theta, g_2)$$
  With $g_1 = 0.82$ (forward peak creating radiant solar "Silver Lining" halos) and $g_2 = -0.22$ (backscattering ambient bounce).

### 2. Planetary Atmosphere Shell & Species Gradients
- Dual-boundary concentric planetary geometry (`SPHERE_INNER_RADIUS = 705,000m`, `SPHERE_OUTER_RADIUS = 722,000m`).
- Decima/Meteoros height density profiles:
  - `STRATUS_GRADIENT`: $(0.0, 0.10, 0.20, 0.30)$ — flat overcast sheet veil.
  - `STRATOCUMULUS_GRADIENT`: $(0.02, 0.20, 0.48, 0.625)$ — undulating rolling waves.
  - `CUMULUS_GRADIENT`: $(0.00, 0.1625, 0.88, 0.98)$ — towering cauliflower piles with flat dark bases.
  - `CUMULONIMBUS_GRADIENT`: $(0.01, 0.12, 0.92, 1.00)$ — monolithic anvil thunderclouds spanning full atmospheric depth.

### 3. Meteoros Compute Pipeline & Post-Processing
- **`cloud_raymarch.comp`**: Primary Vulkan 1.3 compute shader dispatching 16x16 workgroups with Bayer interleaved jitter to eliminate woodgrain slicing artifacts. Outputs 32-bit HDR color and a sun-occlusion channel.
- **`cloud_godrays.comp`**: Meteoros volumetric crepuscular ray generator performing radial screen-space illumination marching from the sun coordinate with exponential decay, density weighting, and exposure scaling.
- **`cloud_reproject.comp`**: Temporal reprojection and neighborhood clamping compute shader for stable, high-performance rendering.
