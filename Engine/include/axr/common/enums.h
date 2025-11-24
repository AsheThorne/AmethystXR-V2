#pragma once

/// Axr Result
enum AxrResult {
    // ---- Failed----
    AXR_ERROR = -1,
    AXR_DONT_RENDER = -2,

    // ---- Succeeded ----
    AXR_SUCCESS = 0,
};

/// Axr Platform Type
enum AxrPlatformType {
    AXR_PLATFORM_TYPE_UNDEFINED = 0,
    AXR_PLATFORM_TYPE_WINDOW,
    AXR_PLATFORM_TYPE_XR_DEVICE,
};
