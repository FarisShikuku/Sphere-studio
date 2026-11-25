# Dynamic Vegetation Wind Field Subsystem

> **Subsystem Path:** `native-assets/src/vegetation/wind/` (Mirrored in `app/src/main/assets/vegetation/wind/`)  
> **Domain:** Atmospheric Boundary Layer, Aerodynamic Canopy Drag, Turbulent Vortices, Gust Waveforms  
> **Conforming Specification:** `Structureassets.md` Section 3.7 (`wind/`)

---

## 1. Wind Field Vector Equations

The global instantaneous 3D wind velocity vector $\vec{W}(\vec{x}, t) \in \mathbb{R}^3$ at position $\vec{x} = (x, y, z)$ and time $t$ is composed of three superposition terms:

$$\vec{W}(\vec{x}, t) = \vec{W}_{mean}(y) + \vec{W}_{gust}(\vec{x}, t) + \vec{W}_{turb}(\vec{x}, t)$$

### 1.1 Vertical Logarithmic Wind Profile (Free Atmosphere)
Above vegetation canopy height $H_c$:
$$\|\vec{W}_{mean}(y)\| = \frac{u_*}{\kappa} \ln\left( \frac{y - d_0}{z_0} \right)$$
where $u_*$ is friction velocity, $\kappa \approx 0.40$ is the Von Kármán constant, $d_0 \approx 0.7 H_c$ is the zero-plane displacement height, and $z_0 \approx 0.1 H_c$ is the aerodynamic roughness length.

### 1.2 Canopy In-Stand Attenuation Profile
Inside the dense forest or crop canopy ($y \le H_c$):
$$\|\vec{W}_{canopy}(y)\| = \|\vec{W}(H_c)\| \cdot \exp\left( -\alpha_{att} \left( 1 - \frac{y}{H_c} \right) \right)$$
where $\alpha_{att} \approx 1.8 - 2.8$ is the canopy extinction attenuation coefficient.

### 1.3 Gust Wavefront & Turbulent Vortices
Gust waves travel across the terrain with wavevector $\vec{k}_{gust}$:
$$\vec{W}_{gust}(\vec{x}, t) = A_{gust} \cdot \sin\left( k_x x + k_z z - \omega_{gust} t \right) \cdot \hat{W}_{dir}$$
Turbulence is evaluated using divergence-free curl noise:
$$\vec{W}_{turb}(\vec{x}, t) = \nabla \times \vec{\Psi}_{perlin}(\vec{x} \cdot s_{turb} - \vec{v}_{drift} t)$$
guaranteeing volume conservation and swirling eddies around branch structures.
