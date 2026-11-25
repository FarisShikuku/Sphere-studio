# Whittaker Ecological Biomes Subsystem

> **Subsystem Path:** `native-assets/src/vegetation/biomes/` (Mirrored in `app/src/main/assets/vegetation/biomes/`)  
> **Domain:** Macro-Ecology, Climate Classification, Whittaker Biome Diagram, Ecological Transition Blending  
> **Conforming Specification:** `Structureassets.md` Section 3.7 (`biomes/`)  
> **Theoretical Citation:** Robert H. Whittaker (1975), *Communities and Ecosystems*.

---

## 1. Whittaker Climate Space Formulation

Vegetation community structure and dominant plant functional types are determined by two continuous bioclimatic variables:
1. $T \in [-15^\circ\text{C}, +30^\circ\text{C}]$: Mean Annual Temperature (MAT).
2. $P \in [0, 450\,\text{cm}]$: Mean Annual Precipitation (MAP).

### 1.1 Mathematical Biome Partitioning

The Whittaker 2D climate plane is partitioned into primary ecological zones:

$$\mathcal{B}(T, P) = \begin{cases}
\text{Tundra / Arctic Alpine} & \text{if } T < 0^\circ\text{C} \\
\text{Boreal Taiga Forest} & \text{if } 0^\circ\text{C} \le T < 6^\circ\text{C} \text{ and } P \ge 40\,\text{cm} \\
\text{Temperate Deciduous Forest} & \text{if } 6^\circ\text{C} \le T < 18^\circ\text{C} \text{ and } P \ge 75\,\text{cm} \\
\text{Temperate Grassland / Prairie} & \text{if } 6^\circ\text{C} \le T < 20^\circ\text{C} \text{ and } 25\,\text{cm} \le P < 75\,\text{cm} \\
\text{Subtropical / Arid Desert} & \text{if } P < 25\,\text{cm} \\
\text{Tropical Rainforest} & \text{if } T \ge 20^\circ\text{C} \text{ and } P \ge 200\,\text{cm}
\end{cases}$$

### 1.2 Soft Transition Blending
At ecotone boundaries, species density weights $w_b(\vec{x})$ are smoothly blended using softmax distance weighting in normalized climate space $(T_{norm}, P_{norm})$:
$$w_b(\vec{x}) = \frac{\exp\left( -\frac{\|\vec{c}(\vec{x}) - \vec{\mu}_b\|^2}{2\sigma_b^2} \right)}{\sum_{k} \exp\left( -\frac{\|\vec{c}(\vec{x}) - \vec{\mu}_k\|^2}{2\sigma_k^2} \right)}$$
preventing hard cutoff seams between adjacent forest and grassland ecosystems.
