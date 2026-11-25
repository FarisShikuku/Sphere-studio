# Forest 3D Ecosystem & Canopy Subsystem

> **Subsystem Path:** `native-assets/src/vegetation/forest/` (Mirrored in `app/src/main/assets/vegetation/forest/`)  
> **Domain:** Stand-Level Forest Architecture, PlantGL Asymmetric Crown Hulls, Volumetric Scattered Leaf Clouds, Canopy Radiative Transfer, Stratification  
> **Scientific Citation:** Christophe Pradal & Frédéric Boudon (UMR AMAP / INRIA Virtual Plants), *Asymmetric Crown Hulls: A Simple Geometric Model of the Tree Crown* (`plantgl.pdf`).

---

## 1. Architectural Scope & Problem Statement

In real-time 3D forest simulation, rendering thousands of individual trees using full branching recursion and millions of leaves is computationally prohibitive. Conversely, representing tree crowns as symmetric spheres, cones, or cylinders fails because:
1. **Canopy Plasticity & Asymmetry:** Trees in dense stands compete for sunlight, pushing their crowns into canopy gaps and yielding pronounced directional asymmetry.
2. **Vertical Meridian Diversity:** Conifers, broadleaf hardwoods, and savanna trees exhibit vastly different vertical profiles (concave spires, ellipsoids, or umbrella-like flat tops).
3. **Radiative Transfer & Microclimate:** Light extinction (Beer-Lambert law) and sub-canopy wind attenuation depend directly on the volume, horizontal projected area, and peripheral height distribution of crowns.

The `forest/` module solves this by implementing **PlantGL Asymmetric Crown Hulls** populated with **Volumetric Scattered 3D Leaf Clouds**, working alongside detailed hero trees in a unified hybrid architecture.

---

## 2. Geometric Parameterization of the Asymmetric Crown Hull

In its local coordinate frame with origin at the base of the crown and vertical axis $+Z$, each crown is defined by **13 scalar parameters**:

### 2.1 The 6 Extreme 3D Control Points

1. **Four Cardinal Peripheral Points ($P_0, P_1, P_2, P_3$):**
   Located along the four cardinal horizontal directions ($+X, -X, +Y, -Y$):
   $$P_0 = \begin{pmatrix} R_{x^+} \\ 0 \\ Hz_{x^+} \end{pmatrix}, \quad P_1 = \begin{pmatrix} -R_{x^-} \\ 0 \\ Hz_{x^-} \end{pmatrix}, \quad P_2 = \begin{pmatrix} 0 \\ R_{y^+} \\ Hz_{y^+} \end{pmatrix}, \quad P_3 = \begin{pmatrix} 0 \\ -R_{y^-} \\ Hz_{y^-} \end{pmatrix}$$
   where:
   - $R_{x^+}, R_{x^-}, R_{y^+}, R_{y^-} > 0$: Independent horizontal semi-axes (North, South, East, West radii).
   - $Hz_{x^+}, Hz_{x^-}, Hz_{y^+}, Hz_{y^-} \in [0, H_c]$: Heights above crown base at which maximum lateral extension occurs.

2. **Two Vertical Extremum Poles ($P_b$ and $P_t$):**
   - **Crown Base ($P_b$):** Lower junction with the trunk:
     $$P_b = \begin{pmatrix} Ox \\ Oy \\ 0 \end{pmatrix}$$
   - **Crown Apex ($P_t$):** Upper vertical terminal point:
     $$P_t = \begin{pmatrix} Ox \\ Oy \\ H_c \end{pmatrix}$$
   - where $(Ox, Oy)$ is an optional lateral eccentricity vector capturing leaning crowns, and $H_c > 0$ is the total crown length ($H_{total} = H_{trunk} + H_c$).

### 2.2 Vertical Shape Exponents ($C_{top}$ and $C_{bot}$)

- $C_{top} > 0$: Controls curvature of the upper crown hull (between the peripheral equator $P(\theta)$ and apex $P_t$).
- $C_{bot} > 0$: Controls curvature of the lower crown hull (between the base $P_b$ and peripheral equator $P(\theta)$).

| Shape Factor $C$ | Geometric Profile | Botanical Morphotype |
|---|---|---|
| $0 < C < 1$ (e.g., $0.6$) | **Concave / Cuspidate** | Subalpine Fir (*Abies*), Norway Spruce spire |
| $C = 1.0$ | **Linear / Conical** | Scots Pine (*Pinus sylvestris*), Larch |
| $1 < C < 2$ (e.g., $1.5$) | **Parabolic / Ellipsoidal** | Silver Birch (*Betula*), European Beech (*Fagus*) |
| $C = 2.0$ | **Hemispherical / Ellipsoid** | Mature English Oak (*Quercus robur*) |
| $C > 2.0$ (e.g., $2.8$) | **Hyper-elliptical / Flat-topped** | Umbrella Pine (*Pinus pinea*), Savanna Acacia |

---

## 3. Parametric Surface Evaluation Equations

The hull surface is parameterized by azimuth $\theta \in [0, 2\pi)$ and profile elevation angle $\varphi \in [-\frac{\pi}{2}, +\frac{\pi}{2}]$.

### Step 1: Quadrant Selection
$$\begin{aligned}
R_X(\theta) &= \begin{cases} R_{x^+} & \text{if } \cos\theta \ge 0 \\ R_{x^-} & \text{if } \cos\theta < 0 \end{cases}, \qquad &R_Y(\theta) &= \begin{cases} R_{y^+} & \text{if } \sin\theta \ge 0 \\ R_{y^-} & \text{if } \sin\theta < 0 \end{cases} \\
Hz_X(\theta) &= \begin{cases} Hz_{x^+} & \text{if } \cos\theta \ge 0 \\ Hz_{x^-} & \text{if } \cos\theta < 0 \end{cases}, \qquad &Hz_Y(\theta) &= \begin{cases} Hz_{y^+} & \text{if } \sin\theta \ge 0 \\ Hz_{y^-} & \text{if } \sin\theta < 0 \end{cases}
\end{aligned}$$

### Step 2: 3D Peripheral Equator Curve $P(\theta)$
The polar radius $R(\theta)$ and height $Hz(\theta)$ along the peripheral line are:
$$R(\theta) = \frac{R_X(\theta) R_Y(\theta)}{\sqrt{\left(R_Y(\theta)\cos\theta\right)^2 + \left(R_X(\theta)\sin\theta\right)^2}}$$
$$Hz(\theta) = Hz_X(\theta)\cos^2\theta + Hz_Y(\theta)\sin^2\theta$$
$$P(\theta) = \begin{pmatrix} R(\theta)\cos\theta \\ R(\theta)\sin\theta \\ Hz(\theta) \end{pmatrix}$$

### Step 3: Upper and Lower Hull Surface $S(\theta, \varphi)$

#### Case A: Upper Crown Hull ($\varphi \in [0, \frac{\pi}{2}]$)
With radial profile factor $u(\varphi) = (\cos\varphi)^{2 / C_{top}}$ and vertical profile factor $v(\varphi) = (\sin\varphi)^{2 / C_{top}}$:
$$S_{top}(\theta, \varphi) = \begin{pmatrix} Ox \cdot (1 - u(\varphi)) + R(\theta)\cos\theta \cdot u(\varphi) \\ Oy \cdot (1 - u(\varphi)) + R(\theta)\sin\theta \cdot u(\varphi) \\ Hz(\theta) + \left(H_c - Hz(\theta)\right) \cdot v(\varphi) \end{pmatrix}$$

#### Case B: Lower Crown Hull ($\varphi \in [-\frac{\pi}{2}, 0)$)
With radial profile factor $u(\varphi) = (\cos|\varphi|)^{2 / C_{bot}}$ and vertical profile factor $v(\varphi) = (\sin|\varphi|)^{2 / C_{bot}}$:
$$S_{bot}(\theta, \varphi) = \begin{pmatrix} Ox \cdot (1 - u(\varphi)) + R(\theta)\cos\theta \cdot u(\varphi) \\ Oy \cdot (1 - u(\varphi)) + R(\theta)\sin\theta \cdot u(\varphi) \\ Hz(\theta) \cdot (1 - v(\varphi)) \end{pmatrix}$$

---

## 4. Analytical Crown Volume & Projected Area

1. **Horizontal Projected Crown Area ($A_{proj}$):**
   $$A_{proj} = \frac{\pi}{4} \left( R_{x^+} + R_{x^-} \right) \left( R_{y^+} + R_{y^-} \right)$$

2. **Total Crown Volume ($V_{crown}$):**
   $$V_{crown} = \frac{\pi}{2} \left( R_{x^+} + R_{x^-} \right) \left( R_{y^+} + R_{y^-} \right) \cdot \left[ \frac{H_c - \overline{Hz}}{C_{top} + 1} + \frac{\overline{Hz}}{C_{bot} + 1} \right]$$
   where $\overline{Hz} = \frac{1}{4}(Hz_{x^+} + Hz_{x^-} + Hz_{y^+} + Hz_{y^-})$.

---

## 5. Volumetric Scattered 3D Leaves & Radiative Transfer

Inside each asymmetric hull, foliage platelets are stochastically scattered to form realistic dense foliage:

1. **Stochastic Coordinate Sampling:**
   $$\theta_i \sim \mathcal{U}(0, 2\pi), \qquad \varphi_i \sim \arcsin\left(\mathcal{U}(-1, 1)\right), \qquad \rho_i = \left( \rho_{min}^3 + (1 - \rho_{min}^3) \cdot \mathcal{U}(0, 1) \right)^{1/3}$$
   where $\rho_{min} \in [0.25, 0.80]$ concentrates leaves in the productive outer shell of the crown where sunlight penetration is highest.

2. **Beer-Lambert Light Extinction:**
   Light intensity $I(z)$ penetrating through the canopy diminishes exponentially with cumulative Leaf Area Index (LAI):
   $$I(z) = I_0 \cdot \exp\left( -k_{ext} \cdot \int_{z}^{H_{top}} \text{LAD}(z') \, dz' \right)$$
   where $k_{ext} \approx 0.5 - 0.7$ is the foliage extinction coefficient.

3. **Canopy Wind Speed Attenuation:**
   $$U(z) = U_{canopy\_top} \cdot \exp\left( -\alpha_{wind} \left( 1 - \frac{z}{H_{canopy}} \right) \right)$$
   where $\alpha_{wind} \approx 1.8 - 2.5$, creating calm, sheltered air currents near the forest floor.
