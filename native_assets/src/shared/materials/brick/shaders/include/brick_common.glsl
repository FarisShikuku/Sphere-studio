// Brick procedural math & UV layout
#ifndef BRICK_COMMON_GLSL
#define BRICK_COMMON_GLSL

struct BrickCell {
    vec2 localUv;
    vec2 cellCoord;
    float isMortar;
    float brickIndex;
};

BrickCell EvaluateBrickPattern(vec2 uv, vec2 brickDimMm, float mortarMm) {
    BrickCell cell;
    vec2 totalDim = brickDimMm + vec2(mortarMm);
    vec2 scaledUv = uv * 1000.0 / totalDim; // mm space
    
    // Stretcher bond offset on alternating rows
    float rowIndex = floor(scaledUv.y);
    if (mod(rowIndex, 2.0) == 1.0) {
        scaledUv.x += 0.5;
    }
    
    cell.cellCoord = floor(scaledUv);
    cell.localUv = fract(scaledUv);
    cell.brickIndex = cell.cellCoord.x + cell.cellCoord.y * 128.0;
    
    vec2 mortarRatio = vec2(mortarMm) / totalDim;
    float mortarMaskX = step(1.0 - mortarRatio.x, cell.localUv.x);
    float mortarMaskY = step(1.0 - mortarRatio.y, cell.localUv.y);
    cell.isMortar = max(mortarMaskX, mortarMaskY);
    
    return cell;
}

#endif
