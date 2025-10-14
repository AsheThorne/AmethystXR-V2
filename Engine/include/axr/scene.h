#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/common/defines.h"
#include "axr/assets.h"

// ----------------------------------------- //
// EnTT Headers
// ----------------------------------------- //
#include <entt/entt.hpp>

// ----------------------------------------- //
// GLM Headers
// ----------------------------------------- //
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

// ----------------------------------------- //
// Clay
// ----------------------------------------- //
#include <clay.h>

// ---------------------------------------------------------------------------------- //
//                                      Entity                                         //
// ---------------------------------------------------------------------------------- //

// ----------------------------------------- //
// Types
// ----------------------------------------- //

/// Entity handle
// TODO: Maybe define this in the hpp includes. here, in the .h includes, it should probably be entt::entity
typedef entt::handle AxrEntity_T;
/// Const Entity handle
typedef entt::const_handle AxrEntityConst_T;

// ---------------------------------------------------------------------------------- //
//                                 Entity Components                                  //
// ---------------------------------------------------------------------------------- //

// ----------------------------------------- //
// Structs
// ----------------------------------------- //

/// Entity transform component
struct AxrTransformComponent {
    glm::vec3 Position;
    glm::vec3 Scale;
    glm::quat Orientation;
};

/// Entity model component
struct AxrModelComponent {
    struct Submesh {
        char MaterialName[AXR_MAX_ASSET_NAME_SIZE];
    };

    struct Mesh {
        std::vector<Submesh> Submeshes;
    };

    char ModelName[AXR_MAX_ASSET_NAME_SIZE];
    std::vector<Mesh> Meshes;

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
    char PushConstantBufferName[AXR_MAX_ASSET_NAME_SIZE];
#endif
};

/// Entity camera component
struct AxrCameraComponent {
    /// Horizontal fov in degrees
    float Fov;
    float ZNear;
    float ZFar;
};

/// Entity mirror pose input action component
///
/// Requires the AxrTransformComponent to apply the pose transforms to
struct AxrMirrorPoseInputActionComponent {
    char ActionSetName[AXR_MAX_ACTION_SET_NAME_SIZE];
    char PoseInputActionName[AXR_MAX_ACTION_NAME_SIZE];
    glm::vec3 OffsetPosition;
    glm::quat OffsetOrientation;
};

// ----------------------------------------- //
// External Function Definitions
// ----------------------------------------- //
extern "C" {
    /// Get a transform component relative to a parent transform component
    /// @param child The child transform component
    /// @param parent The parent transform component
    /// @returns The child transform component relative to the parent
    AXR_API AxrTransformComponent axrTransformComponentRelativeTo(
        const AxrTransformComponent* child,
        const AxrTransformComponent* parent
    );
}

// ---------------------------------------------------------------------------------- //
//                                     UI Canvas                                      //
// ---------------------------------------------------------------------------------- //

// ----------------------------------------- //
// Enums
// ----------------------------------------- //

/// UI Reference space enum
enum AxrUIReferenceSpaceEnum {
    AXR_UI_REFERENCE_SPACE_CAMERA = 0,
    AXR_UI_REFERENCE_SPACE_WORLD,
};

// ----------------------------------------- //
// Structs
// ----------------------------------------- //

/// UI Canvas Config
struct AxrUICanvasConfig {
    bool Enabled;
    Clay_RenderCommandArray ClayRenderCommands;
};

// ----------------------------------------- //
// Types
// ----------------------------------------- //

/// `Build UI Canvas` Callback function type
typedef AxrUICanvasConfig (*AxrBuildUICanvasCallback_T)(
    void* userData,
    AxrPlatformType platformType,
    Clay_Context* context
);

// ---------------------------------------------------------------------------------- //
//                                      Scene                                         //
// ---------------------------------------------------------------------------------- //

// ----------------------------------------- //
// Forward Declared Handles
// ----------------------------------------- //

/// AxrScene Handle
typedef class AxrScene* AxrScene_T;
/// const AxrScene Handle
typedef const AxrScene* AxrSceneConst_T;

// ----------------------------------------- //
// External Function Definitions
// ----------------------------------------- //
extern "C" {
    /// Get the scene name
    /// @param scene The scene to use
    /// @returns The name of the scene
    AXR_API const char* axrSceneGetName(AxrSceneConst_T scene);
    /// Get the scene asset collection
    /// @param scene The scene to use
    /// @returns A handle to the scene asset collection
    AXR_API AxrAssetCollection_T axrSceneGetAssetCollection(AxrScene_T scene);
    /// Get the scene ECS registry
    /// @param scene The scene to use
    /// @returns A handle to the scene ECS registry
    AXR_API entt::registry* axrSceneGetEcsRegistry(AxrScene_T scene);

    /// Create a new entity
    /// @param scene The scene to use
    /// @returns The entity id
    AXR_API entt::entity axrSceneCreateEntity(AxrScene_T scene);

    /// Set the scene's main camera
    /// @param scene The scene to use
    /// @param entity Entity with a camera component
    AXR_API void axrSceneSetMainCamera(AxrScene_T scene, AxrEntityConst_T entity);

    /// Set the number of ui image resources to preload when the scene gets loaded
    /// @param scene The scene to use
    /// @param imageCount Number of image resources to preload
    AXR_API void axrSceneSetUIImageResourcesPreloadCount(AxrScene_T scene, uint32_t imageCount);

    /// Set the `build ui canvas` callback function
    /// @param scene The scene to use
    /// @param userData User data
    /// @param buildCanvasCallback Callback function
    /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrSceneSetBuildUICanvasCallback(
        AxrScene_T scene,
        void* userData,
        AxrBuildUICanvasCallback_T buildCanvasCallback
    );
}
