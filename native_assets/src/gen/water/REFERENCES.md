# References (`gen/water/`)

1. **Primary Reference Repository**:
   - `talvinckb/OpenGL-Water-Simulation` — Real-time 3D SPH fluid simulation and Screen-Space Fluid Rendering (SSFR) in C++20.
2. **Smoothed Particle Hydrodynamics (SPH) Kernels & Equations**:
   - Müller, M., Charypar, D., & Gross, M. (2003). *Particle-Based Fluid Simulation for Interactive Applications*. SCA 2003 (Poly6, Spiky, Viscosity Laplacian kernels).
   - Solenthaler, B., & Pajarola, R. (2008). *Density Contrast SPH Interfaces* / Double-density relaxation (Density + Near-Density pressure terms).
   - Sebastian Lague (2023). *Coding Adventure: Simulating Fluids* — Spatial hashing with prime hash multipliers, GPU bitonic merge sort, cell offset table, and predicted positions.
3. **Screen-Space Fluid Rendering (SSFR)**:
   - van der Laan, W. J., Green, S., & Sainz, M. (2009). *Screen Space Fluid Rendering with Curvature Flow*. I3D 2009.
   - Green, S. (2010). *Screen Space Fluid Rendering for Games*. GDC / NVIDIA (Sphere impostor depth & thickness, bilateral depth-aware Gaussian filtering, view-space normal reconstruction, Beer-Lambert optical absorption, Schlick Fresnel, refraction & SSR).
4. **Vulkan 1.3 Framework**:
   - Sascha Willems Vulkan Compute Particles & Descriptor Indexing patterns (`native_assets/base/`).
