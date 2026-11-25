# Procedural Grass & Prairie Meadow Synthesis

> **Subsystem Path:** `native-assets/src/vegetation/grass/` (Mirrored in `app/src/main/assets/vegetation/grass/`)  
> **Domain:** GPU Tessellated Grass Blades, De Casteljau Bezier Splines, Dynamic 2D Wind Waves, Procedural Force Fields, Distance LOD  
> **Theoretical Basis:** Real-Time Grass Rendering (Denisovich / Boulanger), Quadratic De Casteljau Bezier curves, Hardware Tessellation (Vulkan TCS/TES).

---

## 1. Architectural Scope & Overview

The `grass/` subsystem simulates lush, responsive grasslands, prairies, and turf. Rather than using static polygon clumps, the engine synthesizes procedural individual grass blades evaluated directly on the GPU via hardware tessellation shaders or compute instancing.

---

## 2. Mathematical Modeling of the Grass Blade

Each grass blade is defined by 3 control points in $\mathbb{R}^3$:
- $\vec{P}_0$: Root anchor point on the terrain surface $(x_0, y_0, z_0)$.
- $\vec{P}_1$: Midpoint control handle controlling stem curvature and stiffness.
- $\vec{P}_2$: Blade tip subjected to wind displacement, gravity droop, and collision forces.

### 2.1 Quadratic De Casteljau Bezier Spline
Along normalized blade height parameter $u \in [0, 1]$ ($u = 0$ at root, $u = 1$ at tip):
$$\vec{B}(u) = (1 - u)^2 \vec{P}_0 + 2(1 - u)u \vec{P}_1 + u^2 \vec{P}_2$$

### 2.2 Blade Width Profile & Cross-Section
The blade width narrows linearly from root width $w_0$ to a sharp tip:
$$w(u) = w_0 \cdot (1 - u)^{p_{taper}}, \qquad p_{taper} \approx 0.8 - 1.2$$

Across the transverse axis, the blade surface normal is bent into a shallow V-shaped longitudinal crease (conduplicate venation) to catch directional sunlight highlights:
$$\vec{N}_{transverse}(v) = \cos(\psi_v) \vec{N}_{flat} \pm \sin(\psi_v) \vec{B}_{tangent}, \qquad v \in [-1, +1]$$

---

## 3. Dynamic Wind & Force Field Interaction

### 3.1 Sinusoidal Traveling Wind Waves
The primary wind oscillation displaces the tip control point $\vec{P}_2$:
$$\vec{D}_{wind}(x, z, t) = A_{wind} \cdot \sin\left( \vec{k}_{wave} \cdot \begin{pmatrix} x \\ z \end{pmatrix} - \omega_{wind} t \right) \cdot \frac{\vec{W}_{dir}}{\|\vec{W}_{dir}\|}$$

### 3.2 2D Interaction Force Maps
External forces from vehicles, foot traffic, or magic shockwaves are represented as a 2D vector field $\vec{F}_{ext}(x, z)$:
- **Vortex Presets:** Rotational tangential velocity $\vec{F}_{vortex} = \frac{\Gamma}{2\pi r} \hat{\theta}$.
- **Triforce Presets:** Radial divergence away from 3 triangular impulse centers.
- **Player Trail:** Gaussian displacement footprint decaying over relaxation time $\tau_{relax}$.

The instantaneous tip position is:
$$\vec{P}_2(t) = \vec{P}_0 + h_{blade} \begin{pmatrix} 0 \\ 1 \\ 0 \end{pmatrix} + \vec{D}_{wind}(t) + \vec{F}_{ext}(t) - \frac{m_{blade} g}{2 k_{stiff}} \begin{pmatrix} 0 \\ 1 \\ 0 \end{pmatrix}$$

---

## 4. Hardware Tessellation & Distance-Based LOD

In Vulkan Tessellation Control Shaders (TCS), outer and inner tessellation factors are computed dynamically per patch:
$$\text{TessLevel}(d) = \text{clamp}\left( \left\lceil T_{max} \left( 1 - \frac{d - d_{near}}{d_{far} - d_{near}} \right) \right\rceil, \; 1, \; T_{max} \right)$$
where $d$ is the Euclidean distance from the camera to the patch center.

---

## 5. Optical Shading & Translucency

Grass blades exhibit strong forward subsurface scattering (light passing through thin chlorophyll):
$$I_{final} = I_{ambient} + I_{diffuse} \cdot \left| \vec{N} \cdot \vec{L} \right| + I_{trans} \cdot \max(0, -\vec{N} \cdot \vec{L}) \cdot C_{scatter} + I_{spec} \cdot (\vec{R} \cdot \vec{V})^\alpha$$
