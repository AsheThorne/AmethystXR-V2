#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;

layout (push_constant) uniform PushConstants {
    mat4 modelMatrix;
};

layout (std140, binding = 0) uniform SceneData {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 viewProjectionMatrix;
} scene;

layout (std140, binding = 1) uniform UIElement {
    vec2 position;
    vec2 size;
} uiElement;

layout (location = 0) out vec2 fragPixelCoord;
layout (location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = scene.viewProjectionMatrix * modelMatrix * vec4(inPosition, 1.0);
    fragPixelCoord = inTexCoord * uiElement.size;
    fragTexCoord = inTexCoord;
}