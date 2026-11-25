# Forest Floor Organic Debris & Litter Subsystem

> **Subsystem Path:** `native-assets/src/vegetation/debris/` (Mirrored in `app/src/main/assets/vegetation/debris/`)  
> **Domain:** Forest Floor Litter, Fallen Twigs, Needle Mulch, Organic Soil Humus  
> **Conforming Specification:** `Structureassets.md` Section 3.7 (`debris/`)

---

## 1. Mathematical Modeling of Forest Floor Litter

### 1.1 Stochastic Twig & Branchlet Distribution
Fallen woody debris consists of fragmented cylindrical internodes parameterized by length $L$ and radius $r$:
$$L \sim \text{Exponential}(\lambda_{twig}), \qquad \theta_{orient} \sim \mathcal{U}(0, 2\pi), \qquad \phi_{tilt} \sim \mathcal{N}(0, \sigma_{slope}^2)$$
where $\lambda_{twig} \approx 4.0\,\text{m}^{-1}$ yields characteristic twig lengths between $0.05\,\text{m}$ and $0.45\,\text{m}$.

### 1.2 Foliage Mulch & Pine Needle Mattresses
Fallen leaf platelets and needle fascicles are distributed via a 2D Poisson point process with spatial intensity $\lambda_{litter}(x, z)$ proportional to the overhead canopy Leaf Area Index (LAI):
$$\lambda_{litter}(x, z) = \lambda_0 \cdot \sum_{i=1}^{N_{trees}} \text{LAI}_i \cdot \exp\left( -\frac{\|\vec{x} - \vec{x}_i\|^2}{2 R_{crown, i}^2} \right)$$
creating realistic organic leaf-fall mounds clustered beneath crown perimeters.
