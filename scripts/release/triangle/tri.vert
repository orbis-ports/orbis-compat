#version 450
// ⚠ A TRIANGLE THAT FITS ON THE SCREEN, AND THE FIRST VERSION OF THIS FILE DID NOT.
// It used the fullscreen-triangle trick - vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2)*2-1,
// giving (-1,-1) (3,-1) (-1,3). That covers the whole viewport with every edge off-screen, so the
// console showed a full-screen gradient and no triangle. Correct for a fullscreen pass, useless as
// the thing this example exists to show.
//
// No vertex buffer either way: three positions from gl_VertexIndex is one less thing between "the
// driver came up" and "the driver rasterised", which is the only question this example asks.
layout(location = 0) out vec3 vColour;

const vec2 kPos[3] = vec2[3](
    vec2( 0.0, -0.6),   // top
    vec2( 0.6,  0.6),   // bottom right
    vec2(-0.6,  0.6));  // bottom left

const vec3 kCol[3] = vec3[3](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0));

void main() {
    gl_Position = vec4(kPos[gl_VertexIndex], 0.0, 1.0);
    vColour     = kCol[gl_VertexIndex];
}
