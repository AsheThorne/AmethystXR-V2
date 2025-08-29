#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "common/defines.h"
#include "common/enums.h"

// ----------------------------------------- //
// OpenXR Headers
// ----------------------------------------- //
#include <openxr/openxr.h>

// ---------------------------------------------------------------------------------- //
//                                     Api Layers                                     //
// ---------------------------------------------------------------------------------- //

// ----------------------------------------- //
// Enums
// ----------------------------------------- //

/// Xr Api Layer Type
enum AxrXrApiLayerTypeEnum {
    AXR_XR_API_LAYER_TYPE_UNDEFINED = 0,
    AXR_XR_API_LAYER_TYPE_CORE_VALIDATION,
    /// This value is used to mark the end of the enum.
    /// It's useful for knowing the max number of api layer types.
    AXR_XR_API_LAYER_TYPE_END
};

// ----------------------------------------- //
// Structs
// ----------------------------------------- //

/// Xr Api Layer Base Structure
struct AxrXrApiLayerStructure {
    AxrXrApiLayerTypeEnum Type = AXR_XR_API_LAYER_TYPE_UNDEFINED;
};

/// AxrXrApiLayerStructure Handle Type
typedef AxrXrApiLayerStructure* AxrXrApiLayer_T;
/// const AxrXrApiLayerStructure Handle Type
typedef const AxrXrApiLayerStructure* AxrXrApiLayerConst_T;

/// Xr Api Layer Core Validation Structure
struct AxrXrApiLayerCoreValidation {
    const AxrXrApiLayerTypeEnum Type = AXR_XR_API_LAYER_TYPE_CORE_VALIDATION;
};

// ---------------------------------------------------------------------------------- //
//                                     Extensions                                     //
// ---------------------------------------------------------------------------------- //

// ----------------------------------------- //
// Enums
// ----------------------------------------- //

/// Xr Extension Type
enum AxrXrExtensionTypeEnum {
    AXR_XR_EXTENSION_TYPE_UNDEFINED = 0,
    AXR_XR_EXTENSION_TYPE_DEBUG_UTILS,
    AXR_XR_EXTENSION_TYPE_VULKAN_ENABLE,
    /// This value is used to mark the end of the enum.
    /// It's useful for knowing the max number of extension types.
    AXR_XR_EXTENSION_TYPE_END
};

// Xr Extension Debug Utils Severity Flag Bits. See: XrDebugUtilsMessageSeverityFlagsEXT
enum AxrXrDebugUtilsSeverityFlagBits {
    AXR_XR_EXTENSION_DEBUG_UTILS_SEVERITY_VERBOSE_BIT = XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
    AXR_XR_EXTENSION_DEBUG_UTILS_SEVERITY_INFO_BIT = XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
    AXR_XR_EXTENSION_DEBUG_UTILS_SEVERITY_WARNING_BIT = XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
    AXR_XR_EXTENSION_DEBUG_UTILS_SEVERITY_ERROR_BIT = XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
};

/// Xr Extension Debug Utils Severity Flags Type
typedef XrDebugUtilsMessageSeverityFlagsEXT AxrXrDebugUtilsSeverityFlags_T;

// Xr Extension Debug Utils Type Flag Bits. See: XrDebugUtilsMessageTypeFlagsEXT
enum AxrXrDebugUtilsTypeFlagBits {
    AXR_XR_EXTENSION_DEBUG_UTILS_TYPE_GENERAL_BIT = XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
    AXR_XR_EXTENSION_DEBUG_UTILS_TYPE_VALIDATION_BIT = XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
    AXR_XR_EXTENSION_DEBUG_UTILS_TYPE_PERFORMANCE_BIT = XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
    AXR_XR_EXTENSION_DEBUG_UTILS_TYPE_CONFORMANCE_BIT = XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT,
};

/// Xr Extension Debug Utils Type Flags Type
typedef XrDebugUtilsMessageTypeFlagsEXT AxrXrDebugUtilsTypeFlags_T;

// ----------------------------------------- //
// Structs
// ----------------------------------------- //

/// Xr Extension Base Structure
struct AxrXrExtensionStructure {
    AxrXrExtensionTypeEnum Type = AXR_XR_EXTENSION_TYPE_UNDEFINED;
    bool IsRequired = false;
};

/// AxrXrExtensionStructure Handle Type
typedef AxrXrExtensionStructure* AxrXrExtension_T;
/// const AxrXrExtensionStructure Handle Type
typedef const AxrXrExtensionStructure* AxrXrExtensionConst_T;

/// Xr Extension Debug Utils
struct AxrXrExtensionDebugUtils {
    const AxrXrExtensionTypeEnum Type = AXR_XR_EXTENSION_TYPE_DEBUG_UTILS;
    bool IsRequired = false;
    AxrXrDebugUtilsSeverityFlags_T SeverityFlags;
    AxrXrDebugUtilsTypeFlags_T TypeFlags;
};

/// Xr Extension Vulkan Enable
struct AxrXrExtensionVulkanEnable {
    const AxrXrExtensionTypeEnum Type = AXR_XR_EXTENSION_TYPE_VULKAN_ENABLE;
    bool IsRequired = false;
};

// ---------------------------------------------------------------------------------- //
//                                 Xr System Config                                   //
// ---------------------------------------------------------------------------------- //

// ----------------------------------------- //
// Enums
// ----------------------------------------- //

/// Xr reference space Enum
enum AxrXrReferenceSpaceEnum {
    AXR_XR_REFERENCE_SPACE_UNDEFINED = 0,
    AXR_XR_REFERENCE_SPACE_LOCAL,
    AXR_XR_REFERENCE_SPACE_STAGE,
};

// ----------------------------------------- //
// Structs
// ----------------------------------------- //

/// Xr System Config
struct AxrXrSystemConfig {
    AxrXrReferenceSpaceEnum StageReferenceSpace;
    uint32_t ApiLayerCount;
    AxrXrApiLayer_T* ApiLayers;
    uint32_t ExtensionCount;
    AxrXrExtension_T* Extensions;
};

/// AxrXrSystemConfig Handle
typedef AxrXrSystemConfig* AxrXrSystemConfig_T;
/// const AxrXrSystemConfig Handle
typedef const AxrXrSystemConfig* AxrXrSystemConfigConst_T;

// ----------------------------------------- //
// Forward Declared Handles
// ----------------------------------------- //

/// AxrXrSystem Handle
typedef class AxrXrSystem* AxrXrSystem_T;
/// const AxrXrSystem Handle
typedef const AxrXrSystem* AxrXrSystemConst_T;

// ----------------------------------------- //
// External Function Definitions
// ----------------------------------------- //
extern "C" {
    /// Check if the xr system is valid for use
    /// @param xrSystem The xr system to use
    /// @returns True if the xr system is valid for use
   AXR_API bool axrXrSystemIsValid(AxrXrSystemConst_T xrSystem);
    /// Check if the xr session is running
    /// @param xrSystem The xr system to use
    /// @returns True if the xr session is running
    AXR_API bool axrXrSystemIsXrSessionRunning(AxrXrSystemConst_T xrSystem);
    /// Set the xr rendering clipping planes
    /// @param xrSystem The xr system to use
    /// @param zNear Near clipping plane
    /// @param zFar Far clipping plane
    AXR_API void axrXrSystemSetClippingPlane(AxrXrSystem_T xrSystem, float zNear, float zFar);
    /// Start the xr session
    /// @param xrSystem The xr system to use
    /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrXrSystemStartXrSession(AxrXrSystem_T xrSystem);
    /// Signal that we want to close the xr session
    /// @param xrSystem The xr system to use
    AXR_API void axrXrSystemStopXrSession(AxrXrSystem_T xrSystem);
}
