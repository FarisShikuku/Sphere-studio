# Bagged Goods Procedural Commodity Module

Generates physical sacks, bags, and bulk agricultural/construction bags using GPU compute shaders and parametric mesh inflation.

## Supported Variants
- `SUGAR_50KG`: White woven polypropylene with blue stencil branding.
- `MAIZE_100KG`: Yellowish coarse bulk polypropylene sack.
- `CEMENT_50KG`: Multiwall dusty kraft paper sack.
- `FERTILIZER_25KG`: Laminated agricultural polymer bag.
- `RICE_25KG`: Tight-weave white rice bag with red branding.
- `COFFEE_BEAN_60KG`: Traditional coarse brown jute sack.
- `TEA_LEAF_50KG`: Foil-lined moisture-proof kraft bag.

## Shaders
- `bag_mesh.comp`: Parametric pillow inflation formula with seam pinching.
- `bag_albedo.comp`: Woven polypropylene micro-thread pattern with stencil label.
- `bag_normal.comp`: Normal map generated from fold FBM and fabric weaves.
- `bag_orm.comp`: Packed ambient occlusion, roughness, and metalness.
