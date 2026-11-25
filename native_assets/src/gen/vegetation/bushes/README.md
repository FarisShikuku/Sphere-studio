# Procedural Shrubs & Bushes Subsystem

> **Subsystem Path:** `native-assets/src/vegetation/bushes/` (Mirrored in `app/src/main/assets/vegetation/bushes/`)  
> **Domain:** Volumetric Ellipsoidal Shrub Hulls, Multi-Cluster Foliage, Low-Canopy Understory  
> **Conforming Specification:** `Structureassets.md` Section 3.7 (`bushes/`)

---

## 1. Mathematical Modeling

Shrubs differ from arborescent trees by lacking a single clear trunk bole; they initiate multiple basal stems radiating from ground level:

### 1.1 Multi-Cluster Volumetric Ellipsoids
Each bush is composed of $K$ intersecting tri-axial ellipsoids:
$$\mathcal{E}_k = \left\{ \vec{x} \in \mathbb{R}^3 \;:\; \left(\frac{x - x_k}{a_k}\right)^2 + \left(\frac{y - y_k}{b_k}\right)^2 + \left(\frac{z - z_k}{c_k}\right)^2 \le 1 \right\}$$
where:
- $(x_k, y_k, z_k)$: Centroid of cluster $k$.
- $(a_k, b_k, c_k)$: Semi-axes radii along $X, Y, Z$.

### 1.2 Basal Stem Radiations
$N_{stems}$ basal woody canes emerge from ground origin $\vec{P}_0 = (0, 0, 0)$ with azimuthal divergence $\phi_i = i \frac{2\pi}{N_{stems}} + \delta_\phi$ and inclination $\theta_i \in [25^\circ, 55^\circ]$:
$$\vec{H}_{stem, i} = \begin{pmatrix} \sin\theta_i \cos\phi_i \\ \cos\theta_i \\ \sin\theta_i \sin\phi_i \end{pmatrix}$$
Along each cane, secondary lateral twigs fork to support terminal foliage clusters.
