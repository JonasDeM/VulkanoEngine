#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 world;
    mat4 wvp;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec3 fragNormal;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    gl_Position = ubo.wvp * vec4(inPosition, 1.0);
    fragNormal = inNormal * mat3(ubo.world);
    fragNormal = normalize(fragNormal);
    fragColor = inColor;
}