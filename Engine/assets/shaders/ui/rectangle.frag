#version 450

struct RoundedCorners {
    float topLeftRadius;
    float topRightRadius;
    float bottomLeftRadius;
    float bottomRightRadius;
};

layout (location = 0) in vec2 fragPixelCoord;

layout (std140, binding = 1) uniform UICanvas {
    vec2 size;
} uiCanvas;

layout (std140, binding = 2) uniform UIElement {
    vec2 position;
    vec2 size;
    vec4 backgroundColor;
    RoundedCorners corners;
} uiElement;

layout (location = 0) out vec4 outColor;

float rectangleSdf(vec2 samplePosition, vec2 halfSize, float radius) {
    vec2 componentWiseEdgeDistance = abs(samplePosition) - halfSize + radius;
    float outsideDistance = length(max(componentWiseEdgeDistance, 0));
    float insideDistance = min(max(componentWiseEdgeDistance.x, componentWiseEdgeDistance.y), 0);
    return (outsideDistance + insideDistance) - radius;
}

vec2 translateSdf(vec2 position, vec2 offset) {
    return position - offset;
}

void main() {
    vec2 elementCenter = uiElement.size / 2.0;

    // x: -1, y: -1 => Bottom Left.
    // x: -1, y: 1 => Bottom Right.
    // x: 1, y: -1 => Top Left.
    // x: 1, y: 1 => Top Right.
    vec2 fragQuadrant = sign(translateSdf(fragPixelCoord, elementCenter));

    // ---- Calculate corner radius ----

    float topHalfRadius = mix(uiElement.corners.topRightRadius, uiElement.corners.topLeftRadius, step(fragQuadrant.x, 0.0));
    float bottomHalfRadius = mix(uiElement.corners.bottomRightRadius, uiElement.corners.bottomLeftRadius, step(fragQuadrant.x, 0.0));
    float radius = mix(topHalfRadius, bottomHalfRadius, step(fragQuadrant.y, 0.0));
    // Clamp radius
    radius = min(radius, min(elementCenter.x, elementCenter.y));

    // ---- Calculate sdf distance ----

    float sdfDistance = rectangleSdf(translateSdf(fragPixelCoord, elementCenter), elementCenter, radius);

    // ---- Calculate color with antialiasing ----

    float smoothedAlpha = 1.0 - smoothstep(-1.0, 1.0, sdfDistance);
    outColor = mix(vec4(0.0), uiElement.backgroundColor, smoothedAlpha);
}