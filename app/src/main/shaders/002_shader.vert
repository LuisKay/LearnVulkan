#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;

// Colour passed to the fragment shader
layout(location = 0) out vec3 fragColor;

// Uniform buffer containing an MVP matrix.
// Currently the vulkan backend only sets the rotation matix
// required to handle device rotation.
layout(binding = 0) uniform UniformBufferObject {
    mat4 MVP;
} ubo;

void main() {
    gl_Position = ubo.MVP * vec4(inPos.xyz, 1.0);
    gl_PointSize = 20.0f;
    fragColor = inColor;
}