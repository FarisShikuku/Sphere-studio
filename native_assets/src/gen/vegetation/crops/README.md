# Agricultural Crops & Cultivated Fields Subsystem

> **Subsystem Path:** `native-assets/src/vegetation/crops/` (Mirrored in `app/src/main/assets/vegetation/crops/`)  
> **Domain:** Cultivated Agriculture, Furrow Splines, Equidistant Stalk Planting, Corn / Wheat Allometry  
> **Conforming Specification:** `Structureassets.md` Section 3.7 (`crops/`)

---

## 1. Agricultural Field Geometry

### 1.1 Parallel Furrow Offset Splines
Given a primary baseline ridge spline $\vec{C}_0(s) \in \mathbb{R}^3$ for $s \in [0, 1]$ and unit normal $\vec{N}_\perp(s)$ perpendicular to both the spline tangent $\vec{T}(s)$ and terrain normal $\vec{N}_{ground}$:
$$\vec{C}_j(s) = \vec{C}_0(s) + \left( j - \frac{N_{rows} - 1}{2} \right) \cdot d_{spacing} \cdot \vec{N}_\perp(s)$$
where $j \in \{0, 1, \dots, N_{rows} - 1\}$ is the furrow row index and $d_{spacing} \in [0.6, 1.2\,\text{m}]$ is the tractor furrow separation.

### 1.2 Equidistant Stalk Planting Along Rows
Individual crop stalks (e.g. Zea mays / Triticum aestivum) are placed at discrete arc-length intervals along row $j$:
$$\vec{P}_{stalk}(j, k) = \vec{C}_j\left( \frac{k}{N_{stalks}} \right) + \vec{\delta}_{wobble}$$
where $\vec{\delta}_{wobble} \sim \mathcal{N}(0, \sigma^2)$ introduces natural planting irregularities.
Each stalk grows upright internodes with alternating distichous leaves ($180^\circ$ divergence angle).
