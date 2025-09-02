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

layout (std140, binding = 1) uniform CameraData {
    vec2 dimensions;
    float cameraNearPlane;
    float cameraFarPlane;
} camera;

layout (std140, binding = 2) uniform UIElement {
    vec2 position;
    vec2 size;
} uiElement;

layout (location = 0) out vec2 fragPixelCoord;

void main() {
    vec3 position = vec3(
    (inPosition.x * (uiElement.size.x / camera.dimensions.x)) + (uiElement.position.x / camera.dimensions.x),
    (inPosition.y * (uiElement.size.y / camera.dimensions.y)) + (uiElement.position.y / camera.dimensions.y),
    inPosition.z
    );
    
    gl_Position = scene.viewProjectionMatrix * modelMatrix * vec4(position, 1.0);
    fragPixelCoord = vec2(
    inTexCoord.x * uiElement.size.x,
    inTexCoord.y * uiElement.size.y
    );
}