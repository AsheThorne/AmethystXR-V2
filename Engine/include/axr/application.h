#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/common/defines.h"
#include "axr/common/enums.h"
#include "axr/windowSystem.h"
#include "axr/graphicsSystem.h"
#include "axr/assets.h"
#include "axr/scene.h"
#include "axr/xrSystem.h"
#include "axr/actionSystem.h"

// ----------------------------------------- //
// Structs
// ----------------------------------------- //

/// Config for the AxrApplication
struct AxrApplicationConfig {
    char ApplicationName[AXR_MAX_APPLICATION_NAME_SIZE];
    uint32_t ApplicationVersion;
    AxrGraphicsSystemConfig GraphicsSystemConfig;
    AxrActionSystemConfig ActionSystemConfig;
    const AxrWindowSystemConfig* WindowSystemConfig;
    const AxrXrSystemConfig* XrSystemConfig;
};

// ----------------------------------------- //
// Forward Declared Handles
// ----------------------------------------- //

/// AxrApplication Handle
typedef class AxrApplication* AxrApplication_T;
/// const AxrApplication Handle
typedef const AxrApplication* AxrApplicationConst_T;

// ----------------------------------------- //
// External Function Definitions
// ----------------------------------------- //
extern "C" {
    /// Create an AxrApplication
    /// @param config A handle to the application config
    /// @returns The created AxrApplication
    AXR_API AxrApplication_T axrCreateApplication(const AxrApplicationConfig* config);
    /// Destroy the given AxrApplication
    /// @param app The AxrApplication to destroy
    AXR_API void axrDestroyApplication(AxrApplication_T* app);

    /// Set up the given axrApplication
    /// @param app The AxrApplication to use
    /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrApplicationSetup(AxrApplication_T app);
    /// Check if the application is running
    /// @param app The AxrApplication to use
    /// @returns True if the application is running
    AXR_API bool axrApplicationIsRunning(AxrApplicationConst_T app);
    /// Process application events
    /// @param app The AxrApplication to use
    AXR_API void axrApplicationProcessEvents(AxrApplication_T app);

    /// Get the axrApplication window system
    /// @param app The AxrApplication to use
    /// @returns A handle to the window system
    AXR_API AxrWindowSystem_T axrApplicationGetWindowSystem(AxrApplication_T app);
    /// Get the axrApplication xr system
    /// @param app The AxrApplication to use
    /// @returns A handle to the xr system
    AXR_API AxrXrSystem_T axrApplicationGetXrSystem(AxrApplication_T app);
    /// Get the axrApplication graphics system
    /// @param app The AxrApplication to use
    /// @returns A handle to the graphics system
    AXR_API AxrGraphicsSystem_T axrApplicationGetGraphicsSystem(AxrApplication_T app);
    /// Get the axrApplication action system
    /// @param app The AxrApplication to use
    /// @returns A handle to the action system
    AXR_API AxrActionSystem_T axrApplicationGetActionSystem(AxrApplication_T app);
    /// Get the axrApplication global asset collection
    /// @param app The AxrApplication to use
    /// @returns A handle to the global asset collection
    AXR_API AxrAssetCollection_T axrApplicationGetGlobalAssetCollection(AxrApplication_T app);
    /// Get the delta time from the last time processEvents() was called
    /// @param app The AxrApplication to use
    /// @returns The delta time
    AXR_API float axrApplicationGetDeltaTime(AxrApplication_T app);

    /// Create a new scene
    /// @param app The AxrApplication to use
    /// @param sceneName Name of the scene
    /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrApplicationCreateScene(AxrApplication_T app, char sceneName[AXR_MAX_SCENE_NAME_SIZE]);
    /// Find the named scene
    /// @param app The AxrApplication to use
    /// @param sceneName Name of the scene
    /// @returns A handle to the scene
    AXR_API AxrScene_T axrApplicationFindScene(AxrApplication_T app, const char* sceneName);
    /// Load the named scene
    /// @param app The AxrApplication to use
    /// @param sceneName Name of the scene
        /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrApplicationLoadScene(AxrApplication_T app, const char* sceneName);
    /// Set the active scene to the named scene
    /// @param app The AxrApplication to use
    /// @param sceneName Name of the scene
    /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrApplicationSetActiveScene(AxrApplication_T app, const char* sceneName);

    /// Callback function to handle measuring clay text
    /// @param text Text to measure
    /// @param config Text element config
    /// @param app The AxrApplication to use
    /// @returns The dimensions of the text
    AXR_API Clay_Dimensions axrApplicationHandleClayMeasureText(
        Clay_StringSlice text,
        Clay_TextElementConfig* config,
        void* app
    );
}
