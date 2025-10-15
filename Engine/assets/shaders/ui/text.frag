#version 450

layout (location = 0) in vec2 fragPixelCoord;
layout (location = 1) in vec2 fragTexCoord;

layout (std140, binding = 1) uniform UIElement {
    vec2 position;
    vec2 size;
    vec4 textColor;
} uiElement;

layout (binding = 2) uniform sampler2D texSampler;

layout (location = 0) out vec4 outColor;

void main() {
    vec4 texColor = texture(texSampler, fragTexCoord);
    outColor = texColor;
}