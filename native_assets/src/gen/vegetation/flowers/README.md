# Procedural Wildflowers & Meadow Flora

> **Subsystem Path:** `native-assets/src/vegetation/flowers/` (Mirrored in `app/src/main/assets/vegetation/flowers/`)  
> **Domain:** Floral Anatomy, Corolla Petal Geometry, Vogel's Fibonacci Phyllotaxis, Wildflower Species  
> **Conforming Specification:** `Structureassets.md` Section 3.7 (`flowers/`)

---

## 1. Mathematical Modeling of Floral Anatomy

### 1.1 Corolla & Petal Polar Boundary Formulation
The outer perimeter of a flower corolla with $N_{petals}$ distinct petals is defined in polar coordinates $(r, \theta)$:
$$r(\theta) = R_{flower} \cdot \left( 1 + \epsilon_{petal} \cdot \left| \cos\left( \frac{N_{petals}}{2} \theta \right) \right|^{p_{sharp}} \right)$$
where $\epsilon_{petal} \in [0.4, 0.9]$ controls petal extension and $p_{sharp} \in [1.0, 3.0]$ controls petal tip sharpness.

### 1.2 Central Nectary & Seed Head (Vogel's Golden Spiral)
The central disc floret layout obeys Helmut Vogel's phyllotactic spiral formula:
$$r_n = c_{scale} \sqrt{n}, \qquad \theta_n = n \cdot \phi_g = n \cdot 137.50776^\circ$$
for seed/floret index $n \in \{0, 1, \dots, N_{florets}\}$, ensuring maximum packing density without radial gaps.

### 1.3 Petal Curvature & Camber
Each petal curves upward/outward from the receptacle:
$$z(r) = H_{cup} \cdot \left( \frac{r - R_{center}}{R_{flower} - R_{center}} \right)^2$$
creating realistic bowl, trumpet, or reflexed bell shapes.
