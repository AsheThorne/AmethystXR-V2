#pragma once

// ----------------------------------------- //
// C/C++ Headers
// ----------------------------------------- //
#include <cstdint>

/// Axr Vec2
struct AxrVec2 {
    float x;
    float y;
};

/// Axr Vec3
struct AxrVec3 {
    float x;
    float y;
    float z;
};

/// Axr Color
struct AxrColor {
    float r;
    float g;
    float b;
    float a;
};

/// Axr Quaternion
struct AxrQuaternion {
    float x;
    float y;
    float z;
    float w;
};

/// Axr Pose
struct AxrPose {
    AxrVec3 position;
    AxrQuaternion orientation;
};

/// Axr UI Rounded Corners
struct alignas(16) AxrUIRoundedCorners {
    alignas(4) float TopLeftRadius;
    alignas(4) float TopRightRadius;
    alignas(4) float BottomLeftRadius;
    alignas(4) float BottomRightRadius;
};

/// Axr UI Borders
struct alignas(16) AxrUIBorders {
    alignas(4) uint32_t Left;
    alignas(4) uint32_t Right;
    alignas(4) uint32_t Top;
    alignas(4) uint32_t Bottom;
};
