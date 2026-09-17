#version 450
layout(location = 0) in  vec3 vColour;
layout(location = 0) out vec4 oColour;
void main() { oColour = vec4(vColour, 1.0); }
