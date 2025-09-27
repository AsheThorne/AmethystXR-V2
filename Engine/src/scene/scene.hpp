#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/scene.h"
#include "../assets/assetCollection.hpp"

/// Axr Scene
class AxrScene {
public:
    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// Callback function data
    struct CallbackData {
        void* UserData;
        AxrBuildUICanvasCallback_T Function;
    };

    // ----------------------------------------- //
    // Special Functions
    // ----------------------------------------- //

    // ---- Constructors ----

    /// Default Constructor
    AxrScene();
    /// Constructor
    /// @param name Name of the scene
    /// @param graphicsApi The graphics api to use
    AxrScene(const std::string& name, AxrGraphicsApiEnum graphicsApi);
    /// Copy Constructor
    /// @param src Source AxrScene to copy from
    AxrScene(const AxrScene& src) = delete;
    /// Move Constructor
    /// @param src Source AxrScene to move from
    AxrScene(AxrScene&& src) noexcept;

    // ---- Destructor ----

    /// Destructor
    ~AxrScene();

    // ---- Operator Overloads ----

    /// Copy Assignment Operator
    /// @param src Source AxrScene to copy from
    AxrScene& operator=(const AxrScene& src) = delete;
    /// Move Assignment Operator
    /// @param src Source AxrScene to move from
    AxrScene& operator=(AxrScene&& src) noexcept;

    // ----------------------------------------- //
    // Public Functions
    // ----------------------------------------- //

    /// Get the scene name
    /// @returns The name of the scene
    [[nodiscard]] const std::string& getName() const;
    /// Get the scene asset collection
    /// @returns A handle to the scene asset collection
    [[nodiscard]] AxrAssetCollection_T getAssetCollection();
    /// Get the scene ECS registry
    /// @returns A handle to the scene ECS registry
    [[nodiscard]] entt::registry* getEcsRegistry();

    /// Create a new entity
    /// @returns The entity id
    [[nodiscard]] entt::entity createEntity();

    /// Set the scene's main camera
    /// @param entity Entity with a camera component
    void setMainCamera(AxrEntityConst_T entity);

    /// Set the number of ui images to preload when the scene gets loaded
    /// @param imageCount Number of images to preload
    void setUIImagePreloadCount(uint32_t imageCount);
    /// Set the `build ui canvas` callback function
    /// @param userData User data
    /// @param buildCanvasCallback Callback function
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setBuildUICanvasCallback(void* userData, AxrBuildUICanvasCallback_T buildCanvasCallback);

    // ---- For Internal Use ----
    // These functions are only to be used internally in the AmethystXr engine.
    // They have not been given a publicly accessible function in the 'include headers' to be used by an application.

    /// Check if the main camera is valid
    /// @returns True if the main camera is valid
    [[nodiscard]] bool isMainCameraValid() const;

    /// Get the main scene's main camera
    /// @returns The main camera
    [[nodiscard]] AxrEntityConst_T getMainCamera() const;

    /// Get the UI image preload count
    //// @returns The UI image preload count
    [[nodiscard]] uint32_t getUIImagePreloadCount() const;
    /// Get the `Build UI canvas` callback data
    /// @returns The `Build UI canvas` callback data
    [[nodiscard]] const CallbackData& getUICanvasCallback() const;

private:
    // ----------------------------------------- //
    // Private Variables
    // ----------------------------------------- //
    std::string m_Name;
    AxrAssetCollection m_AssetCollection;
    entt::registry m_Registry;
    AxrEntityConst_T m_MainCamera;
    uint32_t m_UIImagePreloadCount;
    CallbackData m_BuildUICanvasCallback;

    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //

    /// Clean up this class
    void cleanup();
};
