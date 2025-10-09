#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/application.h"
#include "../windowSystem/windowSystem.hpp"
#include "../graphicsSystem/graphicsSystem.hpp"
#include "../assets/assetCollection.hpp"
#include "../scene/scene.hpp"
#include "../xrSystem/xrSystem.hpp"
#include "../actionSystem/actionSystem.hpp"

/// AmethystXr Application
class AxrApplication {
public:
    // ----------------------------------------- //
    // Special Functions
    // ----------------------------------------- //

    // ---- Constructors ----

    /// Constructor
    /// @param config Application config
    AxrApplication(const AxrApplicationConfig& config);
    /// Copy Constructor
    /// @param src Source AxrApplication to copy from
    AxrApplication(const AxrApplication& src) = delete;
    /// Move Constructor
    /// @param src Source AxrApplication to move from
    AxrApplication(AxrApplication&& src) = delete;

    // ---- Destructor ----

    /// Destructor
    ~AxrApplication();

    // ---- Operator Overloads ----

    /// Copy Assignment Operator
    /// @param src Source AxrApplication to copy from
    AxrApplication& operator=(const AxrApplication& src) = delete;
    /// Move Assignment Operator
    /// @param src Source AxrApplication to move from
    AxrApplication& operator=(AxrApplication&& src) noexcept = delete;

    // ----------------------------------------- //
    // Public Functions
    // ----------------------------------------- //

    /// Set up the AxrApplication class
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setup();
    /// Check if the application is running
    /// @returns True if the application is running
    [[nodiscard]] bool isRunning() const;
    /// Process application events
    void processEvents();

    /// Get the window system
    /// @returns A handle to the window system
    [[nodiscard]] AxrWindowSystem_T getWindowSystem();
    /// Get the xr system
    /// @returns A handle to the xr system
    [[nodiscard]] AxrXrSystem_T getXrSystem();
    /// Get the graphics system
    /// @returns A handle to the graphics system
    [[nodiscard]] AxrGraphicsSystem_T getGraphicsSystem();
    /// Get the action system
    /// @returns A handle to the action system
    [[nodiscard]] AxrActionSystem_T getActionSystem();
    /// Get the global asset collection
    /// @returns A handle to the global asset collection
    [[nodiscard]] AxrAssetCollection_T getGlobalAssetCollection();
    /// Get the delta time from the last time processEvents() was called
    /// @returns The delta time
    [[nodiscard]] float getDeltaTime() const;

    /// Create a new scene
    /// @param sceneName Name of the scene
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createScene(const std::string& sceneName);
    /// Find the named scene
    /// @param sceneName Name of the scene
    /// @returns A handle to the scene
    [[nodiscard]] AxrScene_T findScene(const std::string& sceneName);
    /// Load the named scene
    /// @param sceneName Name of the scene
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult loadScene(const std::string& sceneName);
    /// Set the active scene to the named scene
    /// @param sceneName Name of the scene
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setActiveScene(const std::string& sceneName);

    /// Measure the clay text
    /// @param text Text to measure
    /// @param config Text element config
    /// @returns The dimensions of the text
    [[nodiscard]] Clay_Dimensions measureClayText(Clay_StringSlice text, Clay_TextElementConfig* config) const;

private:
    // ----------------------------------------- //
    // Private Variables
    // ----------------------------------------- //

    // ---- Config Variables ----
    std::string m_ApplicationName;
    uint32_t m_ApplicationVersion;

    AxrGraphicsSystem m_GraphicsSystem;
    AxrWindowSystem m_WindowSystem;
    AxrXrSystem m_XrSystem;
    AxrActionSystem m_ActionSystem;
    AxrAssetCollection m_GlobalAssetCollection;
    std::unordered_map<std::string, AxrScene> m_Scenes;
    Clay_Context* m_ClayContext;
    Clay_Arena m_ClayArena;
    float m_DeltaTime;

    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //

    /// Set up the global asset collection
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setupGlobalAssetCollection();

    // ---- Clay ----

    /// Set up the clay data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setupClay();
    /// Reset setupClay()
    void resetSetupClay();
    /// Callback function to handle clay errors
    /// @param errorData Clay error data
    void handleClayErrors(const Clay_ErrorData& errorData) const;
};
