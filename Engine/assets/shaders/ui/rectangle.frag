#version 450

struct RoundedCorners {
    float TopLeftRadius;
    float TopRightRadius;
    float BottomLeftRadius;
    float BottomRightRadius;
};

layout (location = 0) in vec2 fragPixelCoord;

layout (std140, binding = 1) uniform UICanvas {
    vec2 size;
    bool enableAntiAliasing;
} uiCanvas;

layout (std140, binding = 2) uniform UIElement {
    vec2 position;
    vec2 size;
    vec4 backgroundColor;
    RoundedCorners corners;
} uiElement;

layout (location = 0) out vec4 outColor;

float roundedRectangleSdf(vec2 position, vec2 halfSize, float radius) {
    return length(max(abs(position) - halfSize + radius, 0.0)) - radius;
}

float rectangleSdf(vec2 position, vec2 halfSize) {
    vec2 edgeDistance = abs(position) - halfSize;
    return max(edgeDistance.x, edgeDistance.y);
}
vec2 translate(vec2 position, vec2 offset) {
    return position - offset;
}

void main() {
    vec2 elementCenter = vec2(uiElement.size.x / 2, uiElement.size.y / 2);
    float radius = 0.0;
    float sdfDistance = 0.0;

    // Top Left corner
    if (fragPixelCoord.x < elementCenter.x && fragPixelCoord.y > elementCenter.y) {
        radius = uiElement.corners.TopLeftRadius;
    }
    // Top Right corner
    else if (fragPixelCoord.x > elementCenter.x && fragPixelCoord.y > elementCenter.y) {
        radius = uiElement.corners.TopRightRadius;
    }
    // Bottom Left corner
    else if (fragPixelCoord.x < elementCenter.x && fragPixelCoord.y < elementCenter.y) {
        radius = uiElement.corners.BottomLeftRadius;
    }
    // Bottom Right corner
    else if (fragPixelCoord.x > elementCenter.x && fragPixelCoord.y < elementCenter.y) {
        radius = uiElement.corners.BottomRightRadius;
    }

    // Clamp the radius
    radius = min(radius, min(elementCenter.x, elementCenter.y));

    if (radius <= 0.0) {
        sdfDistance = rectangleSdf(translate(fragPixelCoord, elementCenter), elementCenter);
    } else {
        sdfDistance = roundedRectangleSdf(translate(fragPixelCoord, elementCenter), elementCenter, radius);
    }
    if (uiCanvas.enableAntiAliasing) {
        float smoothedAlpha = 1.0 - smoothstep(-1.0, 1.0, sdfDistance);
        outColor = mix(vec4(0.0), uiElement.backgroundColor, smoothedAlpha);
    } else {
        if (sdfDistance <= 0.0) {
            outColor = uiElement.backgroundColor;
        } else {
            outColor = vec4(0.0);
        }
    }
}