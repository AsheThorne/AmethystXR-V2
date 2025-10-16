#version 450

layout (location = 0) in vec2 fragPixelCoord;
layout (location = 1) in vec2 fragTexCoord;

layout (std140, binding = 1) uniform UIElement {
    vec2 size;
    vec4 textColor;
} uiElement;

layout (std140, binding = 2) uniform UIGlyph {
    vec2 size;
    vec2 atlasOffset;
} uiGlyph;

layout (binding = 3) uniform sampler2D texAtlas;

layout (location = 0) out vec4 outColor;

void main() {
    vec4 texColor = texture(texAtlas, uiGlyph.atlasOffset + uiGlyph.size * fragTexCoord);
    outColor = texColor;
}