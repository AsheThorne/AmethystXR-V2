#pragma once

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

/// Axr Rounded Corners
struct alignas(16) AxrRoundedCorners {
    float TopLeftRadius;
    float TopRightRadius;
    float BottomLeftRadius;
    float BottomRightRadius;
};
