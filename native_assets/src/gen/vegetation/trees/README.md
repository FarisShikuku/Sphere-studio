# Stochastic 3D L-System Trees & Botanical Architecture

> **Subsystem Path:** `native-assets/src/vegetation/trees/` (Mirrored in `app/src/main/assets/vegetation/trees/`)  
> **Domain:** Procedural Botanical Modeling, Stochastic Parametric L-Systems, 3D Turtle Geometry, Leonardo da Vinci Pipe Model, Petiole & Leaf Blade Anatomy, Biomechanical Tropisms  
> **Theoretical Foundation:** Prusinkiewicz & Lindenmayer (*The Algorithmic Beauty of Plants*), Leonardo da Vinci's Pipe Model, Honda & Fisher branching allometry, Weber & Penn tree modeling.

---

## 1. Architectural Scope & Overview

The `trees/` subsystem generates fully realized, botanically accurate 3D tree models. In contrast to simplified bounding envelopes, hero trees and near-to-mid field forest stands require explicit 3D branching topology, bark internodes, root flare curvature, petiole connections, and parametric foliage blades.

### Core Modeling Pipeline
1. **Stochastic Parametric L-System Grammars:** Context-free, probabilistic rewriting systems with continuous stochastic perturbations.
2. **3D Turtle Frame Orientation:** Orthonormal local basis $[\vec{H}, \vec{L}, \vec{U}]$ with yaw, pitch, roll, and branch stack operations.
3. **Leonardo da Vinci Pipe Model:** Vascular area conservation ensuring realistic cross-sectional branch tapering from trunk base to terminal twigs.
4. **Petiole & Leaf Blade Morphology:** Explicit geometric modeling of the leaf stem (petiole), central midrib curvature, pinnate/palmate secondary venation, and boundary contour functions $W(s)$.
5. **Biomechanical Tropisms:** Environmental bending forces (upward phototropism, downward gravitropism, and prevailing wind load).
6. **LOD & Impostor Generation:** Seamless LOD transitions to 8-view octahedral impostors for large-scale forest stands.

---

## 2. 3D Turtle Geometry & Spatial Orientation Matrices

The 3D turtle state at any derivation step is defined as the tuple:
$$\mathcal{T} = \left( \vec{P}, \; \vec{H}, \; \vec{L}, \; \vec{U}, \; r, \; k, \; \text{flags} \right)$$

where:
- $\vec{P} \in \mathbb{R}^3$: Current 3D apex position in world space $(x, y, z)$.
- $\vec{H} \in \mathbb{R}^3$: Normalized **Heading** vector pointing in the direction of axial growth.
- $\vec{L} \in \mathbb{R}^3$: Normalized **Left** vector orthogonal to $\vec{H}$.
- $\vec{U} \in \mathbb{R}^3$: Normalized **Up** vector ($\vec{U} = \vec{H} \times \vec{L}$).
- $r > 0$: Current branch cross-sectional radius in meters.
- $k \in \{0, 1, \dots, k_{max}\}$: Branching recursion order ($k = 0$ is the main trunk, $k = 1$ main scaffolds, $k = 2$ secondary boughs, $k \ge 3$ terminal twigs).

### 2.1 Orthonormal Rotation Operators in the Local $[\vec{H}, \vec{L}, \vec{U}]$ Basis

1. **Yaw / Turn around $\vec{U}$ by angle $\theta$** (`+` and `-`):
   $$R_U(\theta) = \begin{pmatrix} \cos\theta & \sin\theta & 0 \\ -\sin\theta & \cos\theta & 0 \\ 0 & 0 & 1 \end{pmatrix}$$
   $$\vec{H}' = \vec{H}\cos\theta - \vec{L}\sin\theta, \qquad \vec{L}' = \vec{H}\sin\theta + \vec{L}\cos\theta, \qquad \vec{U}' = \vec{U}$$

2. **Pitch around $\vec{L}$ by angle $\alpha$** (`&` and `^`):
   $$R_L(\alpha) = \begin{pmatrix} \cos\alpha & 0 & -\sin\alpha \\ 0 & 1 & 0 \\ \sin\alpha & 0 & \cos\alpha \end{pmatrix}$$
   $$\vec{H}' = \vec{H}\cos\alpha + \vec{U}\sin\alpha, \qquad \vec{U}' = -\vec{H}\sin\alpha + \vec{U}\cos\alpha, \qquad \vec{L}' = \vec{L}$$

3. **Roll around $\vec{H}$ by phyllotactic divergence angle $\phi$** (`/` and `\`):
   $$R_H(\phi) = \begin{pmatrix} 1 & 0 & 0 \\ 0 & \cos\phi & -\sin\phi \\ 0 & \sin\phi & \cos\phi \end{pmatrix}$$
   $$\vec{L}' = \vec{L}\cos\phi + \vec{U}\sin\phi, \qquad \vec{U}' = -\vec{L}\sin\phi + \vec{U}\cos\phi, \qquad \vec{H}' = \vec{H}$$

4. **Stack Push `[` and Stack Pop `]`**:
   - `[`: Saves the complete current turtle state $\mathcal{T}$ onto the LIFO branch stack before evaluating a lateral sub-branch.
   - `]`: Pops and restores $\mathcal{T}$ to return to the parent node and continue the main shoot or spawn alternate branches.

---

## 3. Stochastic Parametric L-System Grammars

A stochastic parametric L-system is formally defined as:
$$G_\pi = \langle V, \; \Sigma, \; \omega, \; P, \; \pi \rangle$$

where $V$ is the alphabet of structural modules, $\Sigma$ is the set of continuous parameters, $\omega$ is the initial axiom, $P$ is the set of production rules, and $\pi : P \to (0, 1]$ represents the rule selection probabilities ($\sum \pi_i = 1$).

### 3.1 Axiom & Productions

- **Axiom:**
  $$\omega = F(l_0, r_0) \; A(0, l_0, r_0)$$
  where $F(l, r)$ instantiates a generalized truncated cone internode cylinder of length $l$ and radius $r$, and $A(k, l, r)$ represents an active apical meristem bud of order $k$.

- **Continuous Stochastic Perturbations:**
  At each evaluation step, geometric variations are drawn from independent random distributions:
  $$\delta_\theta \sim \mathcal{N}(0, \sigma_\theta^2), \qquad \delta_\phi \sim \mathcal{N}(0, \sigma_\phi^2), \qquad \delta_l \sim \mathcal{U}(1 - \varepsilon_l, 1 + \varepsilon_l)$$

- **Production Rules for $k < k_{max}$:**
  1. **Monopodial Dominant Axis ($p_1$, probability $\pi_1$):**
     $$A(k, l, r) \xrightarrow{\pi_1} F(l \cdot \delta_l, r) \; \Big[ /(\phi_1) +(\theta_1 + \delta_\theta) A(k+1, l \cdot \lambda_{lat}, r_{lat,1}) \Big] \; \Big[ /(\phi_2) -(\theta_2 + \delta_\theta) A(k+1, l \cdot \lambda_{lat}, r_{lat,2}) \Big] \; /(\psi) \: \&(\alpha_{ax}) \: A(k, l \cdot \lambda_{dom}, r_{dom})$$

  2. **Sympodial Dichotomous Forking ($p_2$, probability $\pi_2$):**
     $$A(k, l, r) \xrightarrow{\pi_2} F(l \cdot \delta_l, r) \; \Big[ /(\phi_1) +(\theta_{sym,1} + \delta_\theta) A(k+1, l \cdot \lambda_1, r_1) \Big] \; \Big[ /(\phi_1 + \pi + \delta_\phi) +(\theta_{sym,2} + \delta_\theta) A(k+1, l \cdot \lambda_2, r_2) \Big]$$

  3. **Whorled Sub-Canopy Radiations ($p_3$, probability $\pi_3$):**
     $$A(k, l, r) \xrightarrow{\pi_3} F(l \cdot \delta_l, r) \; \prod_{m=1}^{M} \Big[ /\left(\phi_0 + m \frac{2\pi}{M} + \delta_\phi\right) +(\theta_m) A(k+1, l \cdot \lambda_{lat}, r_{lat,m}) \Big] \; A(k, l \cdot \lambda_{dom}, r_{dom})$$

- **Terminal Foliage Rule ($k \ge k_{max}$):**
  $$A(k_{max}, l, r) \xrightarrow{1.0} F(l, r) \; \mathcal{L}_{cluster}(N_{leaves}, L_{blade}, \phi_g)$$
  where $\phi_g = 137.50776^\circ$ is the botanical Fibonacci golden divergence angle.

---

## 4. Allometric Radius Taper (Leonardo da Vinci's Pipe Model)

To guarantee physically plausible branch thickness across all branching junctions, parent branch radius $r_p$ and child branch radii $r_1, r_2, \dots, r_N$ obey Leonardo da Vinci's generalized vascular pipe conservation law:

$$r_p^\eta = \sum_{i=1}^{N} r_i^\eta, \qquad \eta \in [1.8, 2.4]$$

- For canonical area-preserving vascular transport, $\eta = 2.0$.
- For mature wind-adapted hardwood trunks resisting bending moments, $\eta \approx 2.3 - 2.5$.
- For slender flexible conifer branches, $\eta \approx 1.8 - 2.0$.

### 4.1 Dominant vs. Lateral Branch Radii Allocation
Given area split ratio $\gamma = \frac{r_{dom}^\eta}{r_p^\eta} \in [0.45, 0.75]$ and $M$ lateral sub-branches:
$$r_{dom} = r_p \cdot \gamma^{1/\eta}$$
$$r_{lat, i} = r_p \cdot \left( \frac{1 - \gamma}{M} \right)^{1/\eta}$$

### 4.2 Root Flare & Collar Expansion
At the base of the trunk ($z < h_{flare}$), the trunk expands exponentially to model structural root anchoring:
$$r_{trunk}(z) = r_0 \left( 1 + \beta_{flare} \cdot \exp\left( -\frac{z}{h_{flare}} \right) \right)$$
where $\beta_{flare} \in [0.3, 0.8]$ and $h_{flare} \approx 0.15 \cdot H_{total}$.

---

## 5. Detailed Botanical Leaf & Foliage Anatomy

Leaves are not treated as generic billboards; they possess distinct botanical anatomy: an axial insertion point, a cylindrical petiole (leaf stem), a cambered central midrib, secondary lateral veins, and a 2D boundary contour $W(s)$.

```
           Apex (s = 1.0)
                /\
               /  \
              / |  \  <- Leaf Blade Contour W(s)
             /  |   \
            |  / \   |
            | /   \  | <- Secondary Pinnate Veins (alpha_v)
             \  |   /
              \ |  /
               \| /   <- Curved Central Midrib kappa
                ||
                ||    <- Petiole Stem (L_p, r_p)
                ||
               (  )   <- Axillary Bud / Node on Twig
```

### 5.1 Petiole (Leaf Stem) Mechanics
The petiole connects the leaf blade to the twig internode:
- **Length:** $L_{petiole} = \rho_{petiole} \cdot L_{blade}$, where $\rho_{petiole} \in [0.2, 0.6]$.
- **Radius:** $r_{petiole} \approx 0.12 \cdot r_{twig}$.
- **Insertion Angle:** Divergence angle $\theta_{petiole} \in [30^\circ, 65^\circ]$ relative to the branch axis.
- **Gravitational Drooping:** Under blade self-weight, the petiole curves downward according to cantilever bending:
  $$\Delta z_{petiole} = \frac{m_{leaf} g L_{petiole}^3}{3 E I}$$

### 5.2 Central Midrib Curvature (Camber Profile)
The primary vascular vein (midrib) traverses normalized arc-length $s \in [0, 1]$ from petiole junction ($s = 0$) to apex tip ($s = 1$). To create realistic 3D curl, the vertical deflection $\Delta z(s)$ follows a parabolic arch:
$$\vec{r}_{midrib}(s) = s \cdot L_{blade} \cdot \vec{H}_{leaf} + \kappa_{midrib} \cdot s(1 - s) \cdot L_{blade} \cdot \vec{U}_{leaf}$$
where $\kappa_{midrib} \in [0.0, 0.5]$ is the midrib curvature camber factor.

### 5.3 Leaf Blade Width Contour Functions $W(s)$
The half-width of the blade perpendicular to the midrib is parameterized as a continuous function $W(s)$ for $s \in [0, 1]$:

1. **Simple Ovate (Broadleaf Hardwoods — Oak, Beech, Birch):**
   $$W(s) = 4 W_{max} \cdot s(1 - s)$$
   Maximum width occurs at mid-blade ($s = 0.5$).

2. **Lanceolate (Willow, Olive, Laurel):**
   $$W(s) = W_{max} \cdot \left( \sin(\pi s) \right)^{0.65} \cdot (1 - 0.35 s)$$
   Narrow elongated blade with maximum width shifted toward the proximal third ($s \approx 0.35$).

3. **Maple Palmate (Multi-lobed Deep Margins):**
   $$W(s) = W_{max} \cdot \Big[ \sin(\pi s) + 0.45 \sin(3\pi s) + 0.25 \sin(5\pi s) \Big] \cdot \text{rectify}(s)$$
   Fourier series harmonic lobes producing prominent palmate lobes and sinuses.

4. **Compound Pinnate (Ash, Walnut, Acacia):**
   Multiple pairs of paired leaflets distributed along a common central rachis of length $L_{rachis}$:
   $$\vec{P}_{leaflet}(i) = \frac{i}{N_{pairs}} L_{rachis} \vec{H}_{rachis} \pm \vec{L}_{rachis} W_{leaflet}(s)$$

5. **Pine Needle (Coniferous Fascicles):**
   Extremely slender linear fascicles:
   $$W(s) = W_{max} \cdot \left( 1 - s^{12} \right), \qquad \frac{L_{blade}}{W_{max}} \ge 25.0$$

### 5.4 Secondary Pinnate Vein Architecture
Secondary lateral veins branch from the midrib at regular intervals $s_j = \frac{j}{N_{veins}}$ with insertion angle $\alpha_{vein} \approx 42^\circ - 55^\circ$:
$$\vec{V}_{left}(s, t) = \vec{r}_{midrib}(s) - t \cdot W(s) \left( \cos\alpha_v \vec{H}_{leaf} + \sin\alpha_v \vec{L}_{leaf} \right)$$
$$\vec{V}_{right}(s, t) = \vec{r}_{midrib}(s) + t \cdot W(s) \left( \cos\alpha_v \vec{H}_{leaf} + \sin\alpha_v \vec{L}_{leaf} \right), \qquad t \in [0, 1]$$

---

## 6. Biomechanical Tropisms & Aerodynamic Wind Response

### 6.1 Phototropism and Gravitropism Bending
At each turtle step, an environmental tropism vector $\vec{T} \in \mathbb{R}^3$ exerts a bending moment on the heading $\vec{H}$:
$$\vec{T} = w_{photo} \begin{pmatrix} 0 \\ 0 \\ 1 \end{pmatrix} - w_{grav} \begin{pmatrix} 0 \\ 0 \\ 1 \end{pmatrix} + w_{wind} \frac{\vec{W}}{\|\vec{W}\|}$$

The rotation axis $\vec{\tau}$ and angular correction $\Delta\alpha$ are evaluated as:
$$\vec{\tau} = \frac{\vec{H} \times \vec{T}}{\|\vec{H} \times \vec{T}\|}, \qquad \Delta\alpha = \zeta(k) \cdot \|\vec{H} \times \vec{T}\|$$
where the susceptibility factor $\zeta(k) = \zeta_0 \left( \frac{k+1}{k_{max}+1} \right)^2 \frac{1}{r}$ increases drastically on slender distal twigs.

### 6.2 Aerodynamic Wind Swaying
Dynamic wind response is simulated as an oscillatory displacement vector $\vec{D}_{wind}(t)$ superimposed along the branch hierarchy:
$$\vec{D}_{wind}(t, k) = A_{sway} \left( \frac{k+1}{k_{max}+1} \right)^2 \frac{1}{\sqrt{r}} \sin\left( \omega_{wind} t + \phi_k \right) \cdot \vec{W}_{dir}$$

---

## 7. Native C++17 Species Specifications

| Species Archetype | Botanical Name | Branching Scheme | Branch Angle $\theta$ | Pipe Model $\eta$ | Leaf Morphology |
|---|---|---|---|---|---|
| **English Oak** | *Quercus robur* | Monopodial dominant with tortuous gnarls | $48^\circ \pm 8^\circ$ | $2.25$ | Simple Ovate / Sinuate Lobed |
| **Scots Pine** | *Pinus sylvestris* | Whorled horizontal tiers, tall clear bole | $68^\circ \pm 5^\circ$ | $1.95$ | Acicular Needle Fascicles |
| **Silver Birch** | *Betula pendula* | Deliquescent slender weeping shoots | $36^\circ \pm 6^\circ$ | $2.10$ | Deltoid / Rhomboid Serrate |
| **Date Palm** | *Phoenix dactylifera* | Columnar unbranched trunk ($k_{max}=0$), apical crown | $0^\circ$ (Trunk) / $52^\circ$ (Fronds) | $1.00$ | Large Pinnate Fronds |
