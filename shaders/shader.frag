#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUV;

layout(binding = 1) uniform sampler2D texSampler;

layout(push_constant) uniform PushConstants {
    int hasTexture;
    mat4 model; // not used here, but MUST match layout
} pc;

layout(location = 0) out vec4 outColor;

void main() {
    
    if (pc.hasTexture == 1) {
        outColor = texture(texSampler, fragUV);
    } else {
        outColor = vec4(fragColor, 1.0);
    }
}