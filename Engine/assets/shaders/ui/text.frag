#version 450

layout (location = 0) in vec2 fragPixelCoord;
layout (location = 1) in vec2 fragTexCoord;

layout (std140, binding = 1) uniform UIElement {
    vec2 size;
    vec4 textColor;
} uiElement;

layout (std140, binding = 2) uniform FontData {
    float sdfDistanceRange;
    vec2 sdfUnitRange;
} font;

layout (std140, binding = 3) uniform UIGlyph {
    vec2 size;
    vec2 atlasOffset;
} uiGlyph;

layout (binding = 4) uniform sampler2D texAtlas;

layout (location = 0) out vec4 outColor;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

vec2 atlasUV() {
    return uiGlyph.atlasOffset + uiGlyph.size * fragTexCoord;
}

float screenPxRange() {
    vec2 screenTexSize = vec2(1.0) / fwidth(atlasUV());
    return max(0.5 * dot(font.sdfUnitRange, screenTexSize), 1.0);
}

void main() {
    vec4 texel = texture(texAtlas, atlasUV());
    float dist = median(texel.r, texel.g, texel.b);

    float pxDist = screenPxRange() * (dist - 0.5);
    float opacity = clamp(pxDist + 0.5, 0.0, 1.0);

    outColor = mix(vec4(0.0f), uiElement.textColor, opacity);
}