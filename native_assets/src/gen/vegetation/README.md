# Vegetation & Flora Botanical Synthesis Domain

> **Master Architecture Document:** `native-assets/src/vegetation/README.md` (Mirrored in `app/src/main/assets/vegetation/README.md`)  
> **Conforming Specification:** `Structureassets.md` Section 3.7 (`src/vegetation/`)  
> **Target Framework:** Vulkan 1.2+ Compute Shaders, Native C++17 Android NDK, Kotlin Multiplatform  
> **Theoretical Core:** Prusinkiewicz & Lindenmayer (*The Algorithmic Beauty of Plants*), Christophe Pradal & Frédéric Boudon (*PlantGL Asymmetric Crown Hulls*), Leonardo da Vinci's Pipe Model, Whittaker Biome Classification.

---

## 1. Domain Architecture & Ecological Synthesis

The `vegetation/` domain provides an end-to-end, botanically grounded procedural synthesis and rendering engine for natural landscapes. It bridges individual hero plant geometry (internodes, petioles, leaf blades) with massive forest stands (thousands of asymmetric crown envelopes and millions of volumetric foliage platelets).

### Unified Submodule Hierarchy (`Structureassets.md` Section 3.7)

```
native-assets/src/vegetation/
├── CMakeLists.txt                                 # Master vegetation CMake build script
├── VegetationSystem.h / .cpp                      # Ecosystem coordinator & spatial dispatch
├── README.md                                      # This master architecture & formulation index
│
├── shaders/                                       # Cross-vegetation compute shaders
│   ├── include/vegetation_common.glsl             # Shared lighting, wind, and PBR helpers
│   ├── vegetation_scatter.comp                    # Poisson disk ecological scatter
│   └── vegetation_lod.comp                        # Continuous distance-based LOD & frustum cull
│
├── trees/                                         # Stochastic L-System trees & pipe model
│   ├── README.md                                  # Detailed grammar, turtle, and leaf math
│   ├── LSystemTree.h / .cpp                       # Stochastic turtle evaluator & cylinder generator
│   ├── TreeSpecies.h                              # Botanical species archetypes (Oak, Pine, Birch, Palm)
│   └── shaders/tree_growth.comp                   # GPU-driven branch internode synthesis
│
├── forest/                                        # Forest stand canopy & crown envelopes
│   ├── README.md                                  # PlantGL 6-extremum hull & volumetric foliage math
│   ├── ForestStandManager.h / .cpp                # Stand-level forest allocation & tree competition
│   └── PlantGLCrownHull.h                         # Analytical 6-extremum parametric surface evaluator
│
├── grass/                                         # Procedural grass & prairie meadows
│   ├── README.md                                  # Quadratic De Casteljau Bezier & wind force math
│   ├── GrassBladeGen.h / .cpp                     # GPU blade patch generator
│   └── shaders/grass_blade.comp                   # Vulkan tessellation control & evaluation
│
├── bushes/                                        # Procedural shrubs & multi-cluster foliage
│   ├── README.md                                  # Volumetric ellipsoidal shrub hulls
│   └── BushGenerator.h / .cpp                     # Stochastic branching shrub synthesis
│
├── flowers/                                       # Wildflower meadows & floral anatomy
│   ├── README.md                                  # Petal rotation, nectary, and golden angle math
│   └── FlowerGen.h / .cpp                         # Parametric corolla and calyx generator
│
├── crops/                                         # Agricultural furrows & cultivated rows
│   ├── README.md                                  # Parallel furrow splines & stalk density
│   └── CropRowGen.h / .cpp                        # Agricultural field procedural instancer
│
├── rocks/                                         # Weathered boulders & scree
│   ├── README.md                                  # Voronoi cellular fractures & hydraulic erosion
│   └── RockGenerator.h / .cpp                     # Perturbed superquadric rock generator
│
├── debris/                                        # Forest floor organic litter
│   ├── README.md                                  # Twig fragmentation & leaf mulch decay
│   └── DebrisGen.h / .cpp                         # Stochastic ground scatter generator
│
├── biomes/                                        # Whittaker ecological classification
│   ├── README.md                                  # Temperature-precipitation Whittaker diagram
│   └── BiomeRules.h / .cpp                        # Species distribution rules per climate zone
│
└── wind/                                          # Macro & micro atmospheric wind fields
    ├── README.md                                  # Directional gusting, turbulence, and canopy drag
    └── WindField.h / .cpp                         # Global 3D vector wind field evaluator
```

---

## 2. Core Mathematical Formulations Summary

### 2.1 Hero Trees & Vascular Branching (`trees/`)
- **3D Turtle Matrix Basis:** Orthonormal orientation $[\vec{H}, \vec{L}, \vec{U}]$ updated via yaw $R_U(\theta)$, pitch $R_L(\alpha)$, and roll $R_H(\phi)$ matrices.
- **Leonardo da Vinci Pipe Model:** Area-preserving vascular cross-section:
  $$r_p^\eta = \sum_{i=1}^{N} r_i^\eta, \qquad \eta \in [1.8, 2.4]$$
- **Leaf Blade Geometry:** Parametric boundary half-width $W(s)$ for normalized midrib arc-length $s \in [0, 1]$:
  - Simple Ovate: $W(s) = 4 W_{max} s (1 - s)$
  - Lanceolate: $W(s) = W_{max} \sin(\pi s)^{0.65} (1 - 0.35 s)$
  - Maple Palmate: $W(s) = W_{max} \left[ \sin(\pi s) + 0.45\sin(3\pi s) + 0.25\sin(5\pi s) \right]$
  - Acicular Needle: $W(s) = W_{max} (1 - s^{12})$
- **Petiole Stem Anatomy:** Cantilever deflection $\Delta z = \frac{m g L_p^3}{3 E I}$, insertion angle $\theta_p$, and central midrib camber $\kappa$.

### 2.2 Stand-Level Canopy Envelopes (`forest/`)
- **PlantGL Asymmetric Crown Hulls:** 6 extreme 3D points $(P_0, P_1, P_2, P_3, P_b, P_t)$ interpolated across azimuth $\theta \in [0, 2\pi)$ and elevation $\varphi \in [-\frac{\pi}{2}, +\frac{\pi}{2}]$:
  $$R(\theta) = \frac{R_X(\theta) R_Y(\theta)}{\sqrt{(R_Y\cos\theta)^2 + (R_X\sin\theta)^2}}, \qquad Hz(\theta) = Hz_X\cos^2\theta + Hz_Y\sin^2\theta$$
  $$S_{top}(\theta, \varphi) = \begin{pmatrix} R(\theta)\cos\theta (\cos\varphi)^{2/C_{top}} \\ R(\theta)\sin\theta (\cos\varphi)^{2/C_{top}} \\ Hz(\theta) + (H_c - Hz(\theta))(\sin\varphi)^{2/C_{top}} \end{pmatrix}$$
- **Beer-Lambert Light Interception:** $I(z) = I_0 \exp\left(-k_{ext} \int_z^{H_{top}} \text{LAD}(z') dz'\right)$.

### 2.3 Grass Blade Splines (`grass/`)
- **Quadratic De Casteljau Spline:**
  $$\vec{B}(u) = (1 - u)^2 \vec{P}_0 + 2(1 - u)u \vec{P}_1 + u^2 \vec{P}_2, \qquad u \in [0, 1]$$
- **Dynamic Wind Displacement:** Sinusoidal traveling wave $\vec{D}_{wind}(x, z, t) = A \sin(\vec{k} \cdot \vec{x} - \omega t) \vec{W}_{dir}$.
- **Hardware Tessellation LOD:** Distance-based continuous factor clamp.

### 2.4 Whittaker Ecological Biomes (`biomes/`)
Ecosystem distribution follows mean annual temperature $T \in [-15^\circ\text{C}, +30^\circ\text{C}]$ and annual precipitation $P \in [0, 450\,\text{cm}]$:
- **Tundra / Subarctic:** $T < 0^\circ\text{C}$ (Dwarf willow, lichen, moss, scree).
- **Boreal Taiga:** $0^\circ\text{C} \le T < 5^\circ\text{C}$, $P > 40\,\text{cm}$ (Scots pine, Norway spruce).
- **Temperate Deciduous Forest:** $5^\circ\text{C} \le T < 20^\circ\text{C}$, $P \ge 75\,\text{cm}$ (English oak, silver birch, beech).
- **Prairie Grassland:** $5^\circ\text{C} \le T < 22^\circ\text{C}$, $P \in [25, 75\,\text{cm}]$ (Big bluestem, fescue, wildflowers).
- **Subtropical / Desert:** $T \ge 20^\circ\text{C}$, $P < 25\,\text{cm}$ (Date palm, drought-tolerant shrubs, exposed boulders).

---

## 3. Submodule Documentation Cross-References

For exhaustive derivation proofs, code listings, and GPU shader pipelines, consult each submodule's dedicated documentation:
- **Trees & L-Systems:** [`trees/README.md`](trees/README.md)
- **Canopy Hulls & Forest Stand:** [`forest/README.md`](forest/README.md)
- **Tessellated Grass:** [`grass/README.md`](grass/README.md)
- **Shrubs & Bushes:** [`bushes/README.md`](bushes/README.md)
- **Wildflowers:** [`flowers/README.md`](flowers/README.md)
- **Agricultural Crops:** [`crops/README.md`](crops/README.md)
- **Boulders & Rocks:** [`rocks/README.md`](rocks/README.md)
- **Ground Debris:** [`debris/README.md`](debris/README.md)
- **Biomes & Ecology:** [`biomes/README.md`](biomes/README.md)
- **Wind Dynamics:** [`wind/README.md`](wind/README.md)
