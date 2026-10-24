#version 460
// Shadow Map Offscreen Depth Fragment Shader
// Reference: SaschaWillems/Vulkan (shadowmapping/offscreen.frag)

layout(location = 0) in vec2 inTexCoord;

layout(push_constant) uniform ShadowFragPush {
    layout(offset = 128)
    uint alphaMaskTexIndex;
    float alphaCutoff;
} fragPush;

void main() {
    // If alpha testing is active (e.g. foliage/leaf clusters), discard transparent fragments:
    // if (fragPush.alphaMaskTexIndex != 0) {
    //     float alpha = SampleBindless2D(fragPush.alphaMaskTexIndex, inTexCoord).a;
    //     if (alpha < fragPush.alphaCutoff) discard;
    // }
    // Otherwise, depth is written automatically by the rasterizer hardware to the depth attachment.
}
