#pragma once

// ----------------------------------------- //
// GLM Headers
// ----------------------------------------- //
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

/// Camera fov in radians
struct AxrCameraFov {
    float Up;
    float Down;
    float Left;
    float Right;
};

/// Camera info
struct AxrCameraInfo {
    glm::vec3 Position;
    glm::quat Orientation;
    AxrCameraFov Fov;
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
    float PixelWidth;
    float PixelHeight;
    float AspectRatio;
    float ZNear;
    float ZFar;
};
