#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUV;

layout(push_constant) uniform PushConstants {
    int hasTexture;
    mat4 model;
} pc;

layout(set = 0, binding = 0) uniform UBO {
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    //gl_Position = ubo.proj * ubo.view * pc.model * vec4(inPosition,0.0, 1.0);
    //gl_Position = pc.model * vec4(inPosition, 0.0, 1.0);
    gl_Position = vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
    fragUV = inUV;
}