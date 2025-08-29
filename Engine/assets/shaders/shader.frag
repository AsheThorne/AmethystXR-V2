#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexCoord;
layout (binding = 0) uniform sampler2D texSampler;
#ifdef ALPHA_MODE_MASK
layout (std140, binding = 1) uniform MaterialProperties {
    float alphaCutoff;
};
#endif

layout (location = 0) out vec4 outColor;

void main() {
    vec4 texColor = texture(texSampler, fragTexCoord);

    #ifdef ALPHA_MODE_MASK
    // Discard transparent pixels
    if (texColor.a < alphaCutoff) {
        discard;
    }
    #endif

    outColor = vec4(fragColor * texColor.rgb, texColor.a);
}