#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/common/types.h"

namespace axr {
    /// Axr Vec2
    struct Vec2 {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        float x = 0.0f;
        float y = 0.0f;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default constructor
        Vec2() = default;

        /// Constructor
        /// @param x X value
        /// @param y Y value
        Vec2(const float x, const float y):
            x(x),
            y(y) {
        }

        /// Constructor
        /// @param vec2 Raw AxrVec2
        explicit Vec2(const AxrVec2& vec2):
            x(vec2.x),
            y(vec2.y) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the Vec2 as an AxrVec2
        /// @returns This as an AxrVec2
        [[nodiscard]] const AxrVec2* toRaw() const {
            return reinterpret_cast<const AxrVec2*>(this);
        }

        /// Get a handle to the Vec2 as an AxrVec2
        /// @returns This as an AxrVec2
        [[nodiscard]] AxrVec2* toRaw() {
            return reinterpret_cast<AxrVec2*>(this);
        }
    };

    static_assert(
        sizeof(AxrVec2) == sizeof(axr::Vec2),
        "Original type and wrapper have different size!"
    );

    /// Axr Vec3
    struct Vec3 {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default constructor
        Vec3() = default;

        /// Constructor
        /// @param x X value
        /// @param y Y value
        /// @param z Z value
        Vec3(const float x, const float y, const float z):
            x(x),
            y(y),
            z(z) {
        }

        /// Constructor
        /// @param vec3 Raw AxrVec3
        explicit Vec3(const AxrVec3& vec3):
            x(vec3.x),
            y(vec3.y),
            z(vec3.z) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the Vec3 as an AxrVec3
        /// @returns This as an AxrVec3
        [[nodiscard]] const AxrVec3* toRaw() const {
            return reinterpret_cast<const AxrVec3*>(this);
        }

        /// Get a handle to the Vec3 as an AxrVec3
        /// @returns This as an AxrVec3
        [[nodiscard]] AxrVec3* toRaw() {
            return reinterpret_cast<AxrVec3*>(this);
        }
    };

    static_assert(
        sizeof(AxrVec3) == sizeof(axr::Vec3),
        "Original type and wrapper have different size!"
    );

    /// Axr Color
    struct Color {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        float r = 0.0f;
        float g = 0.0f;
        float b = 0.0f;
        float a = 1.0f;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default constructor
        Color() = default;

        /// Constructor
        /// @param r R value
        /// @param g G value
        /// @param b B value
        /// @param a A value
        Color(const float r, const float g, const float b, const float a):
            r(r),
            g(g),
            b(b),
            a(a) {
        }

        /// Constructor
        /// @param color Raw AxrColor
        explicit Color(const AxrColor& color):
            r(color.r),
            g(color.g),
            b(color.b),
            a(color.a) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the Color as an AxrColor
        /// @returns This as an AxrColor
        [[nodiscard]] const AxrColor* toRaw() const {
            return reinterpret_cast<const AxrColor*>(this);
        }

        /// Get a handle to the Color as an AxrColor
        /// @returns This as an AxrColor
        [[nodiscard]] AxrColor* toRaw() {
            return reinterpret_cast<AxrColor*>(this);
        }
    };

    static_assert(
        sizeof(AxrColor) == sizeof(axr::Color),
        "Original type and wrapper have different size!"
    );

    /// Axr Quaternion
    struct Quaternion {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 1.0f;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default constructor
        Quaternion() = default;

        /// Constructor
        /// @param x X value
        /// @param y Y value
        /// @param z Z value
        /// @param w W value
        Quaternion(const float x, const float y, const float z, const float w):
            x(x),
            y(y),
            z(z),
            w(w) {
        }

        /// Constructor
        /// @param quaternion Raw AxrQuaternion
        explicit Quaternion(const AxrQuaternion& quaternion):
            x(quaternion.x),
            y(quaternion.y),
            z(quaternion.z),
            w(quaternion.w) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the Quaternion as an AxrQuaternion
        /// @returns This as an AxrQuaternion
        [[nodiscard]] const AxrQuaternion* toRaw() const {
            return reinterpret_cast<const AxrQuaternion*>(this);
        }

        /// Get a handle to the Quaternion as an AxrQuaternion
        /// @returns This as an AxrQuaternion
        [[nodiscard]] AxrQuaternion* toRaw() {
            return reinterpret_cast<AxrQuaternion*>(this);
        }
    };

    static_assert(
        sizeof(AxrQuaternion) == sizeof(axr::Quaternion),
        "Original type and wrapper have different size!"
    );

    /// Axr Pose
    struct Pose {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        axr::Vec3 position = {};
        axr::Quaternion orientation = {};

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default constructor
        Pose() = default;

        /// Constructor
        /// @param position Position value
        /// @param orientation Orientation value
        Pose(const axr::Vec3 position, const axr::Quaternion orientation):
            position(position),
            orientation(orientation) {
        }

        /// Constructor
        /// @param pose Raw AxrPose
        explicit Pose(const AxrPose& pose):
            position(pose.position),
            orientation(pose.orientation) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the Pose as an AxrPose
        /// @returns This as an AxrPose
        [[nodiscard]] const AxrPose* toRaw() const {
            return reinterpret_cast<const AxrPose*>(this);
        }

        /// Get a handle to the Pose as an AxrPose
        /// @returns This as an AxrPose
        [[nodiscard]] AxrPose* toRaw() {
            return reinterpret_cast<AxrPose*>(this);
        }
    };

    static_assert(
        sizeof(AxrPose) == sizeof(axr::Pose),
        "Original type and wrapper have different size!"
    );

    /// Axr UI Rounded Corners
    struct alignas (16) UIRoundedCorners {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        alignas(4) float TopLeftRadius = 0.0f;
        alignas(4) float TopRightRadius = 0.0f;
        alignas(4) float BottomLeftRadius = 0.0f;
        alignas(4) float BottomRightRadius = 0.0f;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default constructor
        UIRoundedCorners() = default;

        /// Constructor
        /// @param topLeftRadius Top left radius
        /// @param topRightRadius Top right radius
        /// @param bottomLeftRadius Bottom left radius
        /// @param bottomRightRadius Bottom right radius
        UIRoundedCorners(
            const float topLeftRadius,
            const float topRightRadius,
            const float bottomLeftRadius,
            const float bottomRightRadius
        ):
            TopLeftRadius(topLeftRadius),
            TopRightRadius(topRightRadius),
            BottomLeftRadius(bottomLeftRadius),
            BottomRightRadius(bottomRightRadius) {
        }

        /// Constructor
        /// @param roundedCorners Raw AxrUIRoundedCorners
        explicit UIRoundedCorners(const AxrUIRoundedCorners& roundedCorners):
            TopLeftRadius(roundedCorners.TopLeftRadius),
            TopRightRadius(roundedCorners.TopRightRadius),
            BottomLeftRadius(roundedCorners.BottomLeftRadius),
            BottomRightRadius(roundedCorners.BottomRightRadius) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the UIRoundedCorners as an AxrUIRoundedCorners
        /// @returns This as an AxrUIRoundedCorners
        [[nodiscard]] const AxrUIRoundedCorners* toRaw() const {
            return reinterpret_cast<const AxrUIRoundedCorners*>(this);
        }

        /// Get a handle to the UIRoundedCorners as an AxrUIRoundedCorners
        /// @returns This as an AxrUIRoundedCorners
        [[nodiscard]] AxrUIRoundedCorners* toRaw() {
            return reinterpret_cast<AxrUIRoundedCorners*>(this);
        }
    };

    static_assert(
        sizeof(AxrUIRoundedCorners) == sizeof(axr::UIRoundedCorners),
        "Original type and wrapper have different size!"
    );

    /// Axr UI Borders
    struct alignas (16) UIBorders {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        alignas(4) uint32_t Left = 0;
        alignas(4) uint32_t Right = 0;
        alignas(4) uint32_t Top = 0;
        alignas(4) uint32_t Bottom = 0;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default constructor
        UIBorders() = default;

        /// Constructor
        /// @param left Left border thickness
        /// @param right Right border thickness
        /// @param top Top border thickness
        /// @param bottom Bottom border thickness
        UIBorders(
            const uint32_t left,
            const uint32_t right,
            const uint32_t top,
            const uint32_t bottom
        ):
            Left(left),
            Right(right),
            Top(top),
            Bottom(bottom) {
        }

        /// Constructor
        /// @param borders Raw AxrUIBorders
        explicit UIBorders(const AxrUIBorders& borders):
            Left(borders.Left),
            Right(borders.Right),
            Top(borders.Top),
            Bottom(borders.Bottom) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the UIBorders as an AxrUIBorders
        /// @returns This as an AxrUIBorders
        [[nodiscard]] const AxrUIBorders* toRaw() const {
            return reinterpret_cast<const AxrUIBorders*>(this);
        }

        /// Get a handle to the UIBorders as an AxrUIBorders
        /// @returns This as an AxrUIBorders
        [[nodiscard]] AxrUIBorders* toRaw() {
            return reinterpret_cast<AxrUIBorders*>(this);
        }
    };

    static_assert(
        sizeof(AxrUIBorders) == sizeof(axr::UIBorders),
        "Original type and wrapper have different size!"
    );
}
