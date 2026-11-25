# Weathered Rocks, Boulders & Scree Subsystem

> **Subsystem Path:** `native-assets/src/vegetation/rocks/` (Mirrored in `app/src/main/assets/vegetation/rocks/`)  
> **Domain:** Geomorphology, Superquadric Boulders, 3D Voronoi Facets, Hydraulic Sediment Weathering  
> **Conforming Specification:** `Structureassets.md` Section 3.7 (`rocks/`)

---

## 1. Mathematical Formulation of Weathered Boulders

### 1.1 Superquadric Base Geometry
The underlying volumetric envelope is defined by a superquadric implicit surface:
$$F(x, y, z) = \left( \left|\frac{x}{a}\right|^{2/\epsilon_2} + \left|\frac{y}{b}\right|^{2/\epsilon_2} \right)^{\epsilon_2/\epsilon_1} + \left|\frac{z}{c}\right|^{2/\epsilon_1} - 1 = 0$$
where $\epsilon_1, \epsilon_2 \in [0.4, 1.6]$ control squareness vs. roundness.

### 1.2 Voronoi Faceting & Hydraulic Erosion
Sharp corners and edges are eroded based on local mean curvature $\mathcal{H}$:
$$r_{eroded}(\theta, \varphi) = r_0(\theta, \varphi) \cdot \left( 1 - \alpha_{erosion} \cdot \max(0, \mathcal{H}(\theta, \varphi)) \right) + \sum_{o=1}^{4} \frac{1}{2^o} \mathcal{N}_{simplex}(2^o \vec{x})$$
producing natural lichen-covered glacial erratics, granite tors, and river-worn cobbles.
