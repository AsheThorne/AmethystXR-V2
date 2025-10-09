#version 450

struct RoundedCorners {
    float topLeftRadius;
    float topRightRadius;
    float bottomLeftRadius;
    float bottomRightRadius;
};

struct Borders {
    uint left;
    uint right;
    uint top;
    uint bottom;
};

layout (location = 0) in vec2 fragPixelCoord;

layout (std140, binding = 2) uniform UIElement {
    vec2 position;
    vec2 size;
    vec4 color;
    RoundedCorners corners;
    Borders borders;
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
    vec2 elementCenter = uiElement.size * 0.5;

    // x: -1, y: -1 => Bottom Left.
    // x: -1, y: 1 => Bottom Right.
    // x: 1, y: -1 => Top Left.
    // x: 1, y: 1 => Top Right.
    vec2 fragQuadrant = sign(translateSdf(fragPixelCoord, elementCenter));

    // ---- Calculate outer corner radius ----

    float topHalfOuterRadius = mix(uiElement.corners.topRightRadius, uiElement.corners.topLeftRadius, step(fragQuadrant.x, 0.0));
    float bottomHalfOuterRadius = mix(uiElement.corners.bottomRightRadius, uiElement.corners.bottomLeftRadius, step(fragQuadrant.x, 0.0));
    float outerRadius = mix(topHalfOuterRadius, bottomHalfOuterRadius, step(fragQuadrant.y, 0.0));
    // Clamp radius
    outerRadius = min(outerRadius, min(elementCenter.x, elementCenter.y));

    // ---- Calculate the center of the inner borderd area ----

    float horizontalBorderWidth = mix(uiElement.borders.right, uiElement.borders.left, step(fragQuadrant.x, 0.0));
    float verticalBorderWidth = mix(uiElement.borders.top, uiElement.borders.bottom, step(fragQuadrant.y, 0.0));
    vec2 innerElementCenter = vec2((uiElement.size.x - horizontalBorderWidth) * 0.5, (uiElement.size.y - verticalBorderWidth) * 0.5);

    // ---- Calculate inner corner radius ----

    float innerRadius = min(outerRadius - horizontalBorderWidth, outerRadius - verticalBorderWidth);

    // ---- Calculate inner boarder sdf distance ----

    vec2 innerElementOffset = vec2(
    mix(0.0, uiElement.borders.left, step(fragQuadrant.x, 0.0)),
    mix(0.0, uiElement.borders.bottom, step(fragQuadrant.y, 0.0))
    );
    float innerSdfDistance = rectangleSdf(
        translateSdf(fragPixelCoord, innerElementCenter + innerElementOffset),
        innerElementCenter,
        innerRadius
    );

    // ---- Calculate outer boarder sdf distance ----

    float sdfDistanceOuter = rectangleSdf(translateSdf(fragPixelCoord, elementCenter), elementCenter, outerRadius);

    // ---- Calculate color with antialiasing ----

    float smoothedAlpha = 1.0 - smoothstep(-1.0, 1.0, max(sdfDistanceOuter, -innerSdfDistance));
    outColor = mix(vec4(0), uiElement.color, smoothedAlpha);
}