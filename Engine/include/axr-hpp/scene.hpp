#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/scene.h"

namespace axr {
    // ---------------------------------------------------------------------------------- //
    //                                     Entity                                         //
    // ---------------------------------------------------------------------------------- //

    // ----------------------------------------- //
    // Types
    // ----------------------------------------- //

    /// Entity handle
    using Entity_T = AxrEntity_T;
    /// Const Entity handle
    using EntityConst_T = AxrEntityConst_T;

    // ---------------------------------------------------------------------------------- //
    //                                     UI Canvas                                      //
    // ---------------------------------------------------------------------------------- //

    // ----------------------------------------- //
    // Enums
    // ----------------------------------------- //

    /// UI Reference space enum
    enum class UIReferenceSpaceEnum {
        Camera = AXR_UI_REFERENCE_SPACE_CAMERA,
        World = AXR_UI_REFERENCE_SPACE_WORLD,
    };

    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// UI Canvas Config
    struct UICanvasConfig {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        bool Enabled = false;
        Clay_RenderCommandArray ClayRenderCommands = {};

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Constructor
        UICanvasConfig() = default;

        /// Constructor
        /// @param enabled Enable rendering of the UI
        /// @param clayRenderCommands Clay render commands
        UICanvasConfig(
            const bool enabled,
            const Clay_RenderCommandArray& clayRenderCommands
        ):
            Enabled(enabled),
            ClayRenderCommands(clayRenderCommands) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the UICanvasConfig as an AxrUICanvasConfig
        /// @returns This as an AxrUICanvasConfig
        const AxrUICanvasConfig* toRaw() const {
            return reinterpret_cast<const AxrUICanvasConfig*>(this);
        }

        /// Get a handle to the UICanvasConfig as an AxrUICanvasConfig
        /// @returns This as an AxrUICanvasConfig
        AxrUICanvasConfig* toRaw() {
            return reinterpret_cast<AxrUICanvasConfig*>(this);
        }
    };

    static_assert(
        sizeof(AxrUICanvasConfig) == sizeof(axr::UICanvasConfig),
        "Original type and wrapper have different size!"
    );

    // ----------------------------------------- //
    // Types
    // ----------------------------------------- //

    /// `Build UI Canvas` Callback function type
    using BuildUICanvasCallback_T = axr::UICanvasConfig(*)(
        void* userData,
        axr::PlatformType platformType,
        Clay_Context* context
    );

    // TODO: Need a way to statically assert that BuildUICanvasCallback_T and AxrBuildUICanvasCallback_T are the same
    //  The following below doesn't work because we're using the hpp versions of AxrUICanvasConfig and AxrPlatformType
    //   static_assert(
    //       std::is_same_v<AxrBuildUICanvasCallback_T, axr::BuildUICanvasCallback_T>,
    //       "Callback function signatures differ!"
    //   );

    // ---------------------------------------------------------------------------------- //
    //                                     Scene                                          //
    // ---------------------------------------------------------------------------------- //

    /// Axr Scene
    class Scene {
    public:
        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Constructor
        /// @param scene The scene handle
        Scene(const AxrScene_T scene) {
            m_Scene = scene;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get the scene name
        /// @returns The name of the scene
        [[nodiscard]] const char* getName() const {
            return axrSceneGetName(m_Scene);
        }

        /// Get the scene asset collection
        /// @returns The scene asset collection
        [[nodiscard]] axr::AssetCollection getAssetCollection() const {
            return axrSceneGetAssetCollection(m_Scene);
        }

        /// Get the scene ECS registry
        /// @returns The scene ECS registry
        [[nodiscard]] entt::registry* getEcsRegistry() const {
            return axrSceneGetEcsRegistry(m_Scene);
        }

        /// Create a new entity
        /// @returns A handle to the created entity
        axr::Entity_T createEntity() const {
            return {*getEcsRegistry(), axrSceneCreateEntity(m_Scene)};
        }

        /// Set the scene's main camera
        /// @param entity Entity with a camera component
        void setMainCamera(const axr::EntityConst_T entity) const {
            axrSceneSetMainCamera(m_Scene, entity);
        }

        /// Set the number of ui images to preload when the scene gets loaded
        /// @param imageCount Number of images to preload
        void setUIImagePreloadCount(const uint32_t imageCount) const {
            axrSceneSetUIImagePreloadCount(m_Scene, imageCount);
        }

        /// Set the `build ui canvas` callback function
        /// @param userData User data
        /// @param buildCanvasCallback Callback function
        /// @returns AXR_SUCCESS if the function succeeded
        axr::Result setBuildUICanvasCallback(
            void* userData,
            const axr::BuildUICanvasCallback_T buildCanvasCallback
        ) const {
            return static_cast<axr::Result>(axrSceneSetBuildUICanvasCallback(
                m_Scene,
                userData,
                reinterpret_cast<AxrBuildUICanvasCallback_T>(buildCanvasCallback)
            ));
        }

    private:
        // ----------------------------------------- //
        // Private Variables
        // ----------------------------------------- //
        AxrScene_T m_Scene;
    };
}
