// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "engineAssets.hpp"
#include "axr/logger.h"
#include "model.hpp"
#include "assetsUtils.hpp"
#include "shader.hpp"
#include "material.hpp"
#include "image.hpp"
#include "imageSampler.hpp"
#include "uniformBuffer.hpp"

// ----------------------------------------- //
// C/C++ Headers
// ----------------------------------------- //
#include <ranges>
#include <unordered_map>
#include <string>

// ----------------------------------------- //
// Clay Headers
// ----------------------------------------- //
#include "clay.h"

// ---------------------------------------------------------------------------------- //
//                                 Global Variables                                   //
// ---------------------------------------------------------------------------------- //

// ----------------------------------------- //
// Shader Engine Assets
// ----------------------------------------- //

/// Engine asset shader names
const std::unordered_map EngineAssetShaderNames{
    std::pair(
        AXR_ENGINE_ASSET_SHADER_DEFAULT_VERT,
        "AXR:ShaderDefaultVert"
    ),
    std::pair(
        AXR_ENGINE_ASSET_SHADER_DEFAULT_FRAG,
        "AXR:ShaderDefaultFrag"
    ),
    std::pair(
        AXR_ENGINE_ASSET_SHADER_DEFAULT_FRAG_MASK,
        "AXR:ShaderDefaultFrag_Mask"
    ),
    std::pair(
        AXR_ENGINE_ASSET_SHADER_UI_ELEMENT_VERT,
        "AXR:ShaderUIElementVert"
    ),
    std::pair(
        AXR_ENGINE_ASSET_SHADER_UI_RECTANGLE_FRAG,
        "AXR:ShaderUIRectangleFrag"
    ),
    std::pair(
        AXR_ENGINE_ASSET_SHADER_UI_BORDER_FRAG,
        "AXR:ShaderUIBorderFrag"
    ),
};

// ----------------------------------------- //
// Buffer Engine Assets
// ----------------------------------------- //

/// Engine asset buffer names
const std::unordered_map EngineAssetBufferNames{
    std::pair(
        AXR_ENGINE_ASSET_UNIFORM_BUFFER_SCENE_DATA,
        "AXR:UniformBufferSceneData"
    ),
    std::pair(
        AXR_ENGINE_ASSET_UNIFORM_BUFFER_CAMERA_DATA,
        "AXR:UniformBufferCameraData"
    ),
    std::pair(
        AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_CANVAS,
        "AXR:UniformBufferUICanvas"
    ),
    std::pair(
        AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_ELEMENTS,
        "AXR:UniformBufferUIElements"
    ),
    std::pair(
        AXR_ENGINE_ASSET_PUSH_CONSTANT_BUFFER_MODEL_MATRIX,
        "AXR:PushConstantBufferModelMatrix"
    ),
    std::pair(
        AXR_ENGINE_ASSET_PUSH_CONSTANT_BUFFER_MVP_MATRIX,
        "AXR:PushConstantBufferMvpMatrix"
    ),
};

// ----------------------------------------- //
// Model Engine Assets
// ----------------------------------------- //

/// Engine asset image names
const std::unordered_map EngineAssetModelNames{
    std::pair(
        AXR_ENGINE_ASSET_MODEL_UI_RECTANGLE,
        "AXR:ModelUIRectangle"
    ),
};

// ----------------------------------------- //
// Image Sampler Engine Assets
// ----------------------------------------- //

/// Engine asset image sampler names
const std::unordered_map EngineAssetImageSamplerNames{
    std::pair(
        AXR_ENGINE_ASSET_IMAGE_SAMPLER_NEAREST_REPEAT,
        "AXR:ImageSamplerNearestRepeat"
    ),
    std::pair(
        AXR_ENGINE_ASSET_IMAGE_SAMPLER_LINEAR_REPEAT,
        "AXR:ImageSamplerLinearRepeat"
    ),
};

// ----------------------------------------- //
// Image Engine Assets
// ----------------------------------------- //

/// Engine asset image names
const std::unordered_map EngineAssetImageNames{
    std::pair(
        AXR_ENGINE_ASSET_IMAGE_MISSING_TEXTURE,
        "AXR:ImageMissingTexture"
    ),
};

// ----------------------------------------- //
// Material Engine Assets
// ----------------------------------------- //

/// Engine asset buffer properties
const std::unordered_map EngineAssetMaterialNames{
    std::pair(
        AXR_ENGINE_ASSET_MATERIAL_UI_RECTANGLE,
        "AXR:MaterialUIRectangle"
    ),
    std::pair(
        AXR_ENGINE_ASSET_MATERIAL_UI_BORDER,
        "AXR:MaterialUIBorder"
    ),
};

// ---------------------------------------------------------------------------------- //
//                                External Functions                                  //
// ---------------------------------------------------------------------------------- //

bool axrEngineAssetIsNameReserved(const char* name) {
    return axrEngineAssetIsShaderNameReserved(name) ||
        axrEngineAssetIsBufferNameReserved(name) ||
        axrEngineAssetIsImageSamplerNameReserved(name) ||
        axrEngineAssetIsImageNameReserved(name) ||
        axrEngineAssetIsModelNameReserved(name) ||
        axrEngineAssetIsMaterialNameReserved(name);
}

const char* axrEngineAssetGetName(const AxrEngineAssetEnum engineAssetEnum) {
    if (axrEngineAssetIsShader(engineAssetEnum)) {
        return axrEngineAssetGetShaderName(engineAssetEnum);
    }
    if (axrEngineAssetIsUniformBuffer(engineAssetEnum)
#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
        || axrEngineAssetIsPushConstantBuffer(engineAssetEnum)
#endif
    ) {
        return axrEngineAssetGetBufferName(engineAssetEnum);
    }
    if (axrEngineAssetIsImageSampler(engineAssetEnum)) {
        return axrEngineAssetGetImageSamplerName(engineAssetEnum);
    }
    if (axrEngineAssetIsImage(engineAssetEnum)) {
        return axrEngineAssetGetImageName(engineAssetEnum);
    }
    if (axrEngineAssetIsModel(engineAssetEnum)) {
        return axrEngineAssetGetModelName(engineAssetEnum);
    }
    if (axrEngineAssetIsMaterial(engineAssetEnum)) {
        return axrEngineAssetGetMaterialName(engineAssetEnum);
    }

    axrLogErrorLocation("Unknown engine asset enum.");
    return "";
}

// ----------------------------------------- //
// Buffer Engine Assets
// ----------------------------------------- //

uint64_t axrEngineAssetGetUniformBufferSize(const AxrEngineAssetEnum engineAssetEnum) {
    if (!axrEngineAssetIsUniformBuffer(engineAssetEnum)) {
        axrLogErrorLocation("Engine asset is not a uniform buffer.");
        return 0;
    }

    switch (engineAssetEnum) {
        case AXR_ENGINE_ASSET_UNIFORM_BUFFER_SCENE_DATA: {
            return sizeof(AxrEngineAssetUniformBuffer_SceneData);
        }
        case AXR_ENGINE_ASSET_UNIFORM_BUFFER_CAMERA_DATA: {
            return sizeof(AxrEngineAssetUniformBuffer_CameraData);
        }
        case AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_CANVAS: {
            return sizeof(AxrEngineAssetUniformBuffer_UICanvas);
        }
        case AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_ELEMENTS: {
            if (Clay_GetCurrentContext() == nullptr) {
                axrLogWarningLocation("Clay context is null.");
                return sizeof(AxrEngineAssetUniformBuffer_UIElement);
            }

            return sizeof(AxrEngineAssetUniformBuffer_UIElement) * Clay_GetMaxElementCount();
        }
        case AXR_ENGINE_ASSET_UNDEFINED:
        default: { // NOLINT(clang-diagnostic-covered-switch-default)
            return 0;
        }
    }
}

uint64_t axrEngineAssetGetUniformBufferInstanceSize(const AxrEngineAssetEnum engineAssetEnum) {
    if (!axrEngineAssetIsUniformBuffer(engineAssetEnum)) {
        axrLogErrorLocation("Engine asset is not a uniform buffer.");
        return 0;
    }

    switch (engineAssetEnum) {
        case AXR_ENGINE_ASSET_UNIFORM_BUFFER_SCENE_DATA: {
            return sizeof(AxrEngineAssetUniformBuffer_SceneData);
        }
        case AXR_ENGINE_ASSET_UNIFORM_BUFFER_CAMERA_DATA: {
            return sizeof(AxrEngineAssetUniformBuffer_CameraData);
        }
        case AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_CANVAS: {
            return sizeof(AxrEngineAssetUniformBuffer_UIElement);
        }
        case AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_ELEMENTS: {
            return sizeof(AxrEngineAssetUniformBuffer_UIElement);
        }
        case AXR_ENGINE_ASSET_UNDEFINED:
        default: { // NOLINT(clang-diagnostic-covered-switch-default)
            return 0;
        }
    }
}

AxrUniformBufferTypeEnum axrEngineAssetGetUniformBufferType(const AxrEngineAssetEnum engineAssetEnum) {
    if (!axrEngineAssetIsUniformBuffer(engineAssetEnum)) {
        axrLogErrorLocation("Engine asset is not a uniform buffer.");
        return AXR_UNIFORM_BUFFER_TYPE_UNDEFINED;
    }

    switch (engineAssetEnum) {
        case AXR_ENGINE_ASSET_UNIFORM_BUFFER_SCENE_DATA: {
            return AXR_UNIFORM_BUFFER_TYPE_STANDARD;
        }
        case AXR_ENGINE_ASSET_UNIFORM_BUFFER_CAMERA_DATA: {
            return AXR_UNIFORM_BUFFER_TYPE_STANDARD;
        }
        case AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_CANVAS: {
            return AXR_UNIFORM_BUFFER_TYPE_STANDARD;
        }
        case AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_ELEMENTS: {
            return AXR_UNIFORM_BUFFER_TYPE_DYNAMIC;
        }
        case AXR_ENGINE_ASSET_UNDEFINED:
        default: { // NOLINT(clang-diagnostic-covered-switch-default)
            return AXR_UNIFORM_BUFFER_TYPE_UNDEFINED;
        }
    }
}

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
uint32_t axrEngineAssetGetPushConstantBufferSize(const AxrEngineAssetEnum engineAssetEnum) {
    if (!axrEngineAssetIsPushConstantBuffer(engineAssetEnum)) {
        axrLogErrorLocation("Engine asset is not a push constant buffer.");
        return 0;
    }

    switch (engineAssetEnum) {
        case AXR_ENGINE_ASSET_PUSH_CONSTANT_BUFFER_MODEL_MATRIX: {
            return sizeof(AxrEngineAssetPushConstantBuffer_ModelMatrix);
        }
        case AXR_ENGINE_ASSET_PUSH_CONSTANT_BUFFER_MVP_MATRIX: {
            return sizeof(AxrEngineAssetPushConstantBuffer_MvpMatrix);
        }
        case AXR_ENGINE_ASSET_UNDEFINED:
        default: { // NOLINT(clang-diagnostic-covered-switch-default)
            return 0;
        }
    }
}
#endif

// ---------------------------------------------------------------------------------- //
//                                Internal Functions                                  //
// ---------------------------------------------------------------------------------- //

// ----------------------------------------- //
// Shader Engine Assets
// ----------------------------------------- //

bool axrEngineAssetIsShader(const AxrEngineAssetEnum engineAssetEnum) {
    return engineAssetEnum >= AXR_ENGINE_ASSET_SHADER_START &&
        engineAssetEnum <= AXR_ENGINE_ASSET_SHADER_END;
}

const char* axrEngineAssetGetShaderName(const AxrEngineAssetEnum engineAssetEnum) {
    if (!axrEngineAssetIsShader(engineAssetEnum)) {
        axrLogErrorLocation("Engine asset is not a shader.");
        return "";
    }

    const auto foundEngineAssetIt = EngineAssetShaderNames.find(engineAssetEnum);
    if (foundEngineAssetIt == EngineAssetShaderNames.end()) {
        axrLogError("Failed to find name for engine asset: {0}.", static_cast<int>(engineAssetEnum));
        return "";
    }

    return foundEngineAssetIt->second;
}

bool axrEngineAssetIsShaderNameReserved(const char* name) {
    for (const auto& engineAssetName : EngineAssetShaderNames | std::views::values) {
        if (std::strcmp(engineAssetName, name) == 0) {
            return true;
        }
    }

    return false;
}

AxrResult axrEngineAssetCreateShader(
    const AxrGraphicsApiEnum graphicsApi,
    const AxrEngineAssetEnum engineAssetEnum,
    AxrShader& shader
) {
    if (!axrEngineAssetIsShader(engineAssetEnum)) {
        axrLogErrorLocation("Engine asset is not a shader.");
        return AXR_ERROR;
    }

    switch (engineAssetEnum) {
        case AXR_ENGINE_ASSET_SHADER_DEFAULT_VERT: {
            return axrEngineAssetCreateShader_DefaultVert(graphicsApi, shader);
        }
        case AXR_ENGINE_ASSET_SHADER_DEFAULT_FRAG: {
            return axrEngineAssetCreateShader_DefaultFrag(graphicsApi, shader);
        }
        case AXR_ENGINE_ASSET_SHADER_DEFAULT_FRAG_MASK: {
            return axrEngineAssetCreateShader_DefaultFrag_Mask(graphicsApi, shader);
        }
        case AXR_ENGINE_ASSET_SHADER_UI_ELEMENT_VERT: {
            return axrEngineAssetCreateShader_UIElementVert(graphicsApi, shader);
        }
        case AXR_ENGINE_ASSET_SHADER_UI_RECTANGLE_FRAG: {
            return axrEngineAssetCreateShader_UIRectangleFrag(graphicsApi, shader);
        }
        case AXR_ENGINE_ASSET_SHADER_UI_BORDER_FRAG: {
            return axrEngineAssetCreateShader_UIBorderFrag(graphicsApi, shader);
        }
        case AXR_ENGINE_ASSET_UNDEFINED:
        default: { // NOLINT(clang-diagnostic-covered-switch-default)
            axrLogErrorLocation("Unknown shader engine asset.");
            return AXR_ERROR;
        }
    }
}

AxrResult axrEngineAssetCreateShader_DefaultVert(const AxrGraphicsApiEnum graphicsApi, AxrShader& shader) {
    std::array vertexAttributes{
        AxrShaderVertexAttribute{
            .Type = AXR_SHADER_VERTEX_ATTRIBUTE_POSITION,
            .Location = 0,
        },
        AxrShaderVertexAttribute{
            .Type = AXR_SHADER_VERTEX_ATTRIBUTE_COLOR,
            .Location = 1,
        },
        AxrShaderVertexAttribute{
            .Type = AXR_SHADER_VERTEX_ATTRIBUTE_TEXCOORD_0,
            .Location = 2,
        }
    };

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
    AxrShaderPushConstantBufferLayout modelMatrixBufferLayout{
        .BufferSize = axrEngineAssetGetPushConstantBufferSize(AXR_ENGINE_ASSET_PUSH_CONSTANT_BUFFER_MVP_MATRIX)
    };
#endif

    std::array bufferLayouts{
#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
        reinterpret_cast<AxrShaderBufferLayout_T>(&modelMatrixBufferLayout),
#endif
    };

    AxrVertexShaderProperties shaderProperties{
        .VertexAttributeCount = static_cast<uint32_t>(vertexAttributes.size()),
        .VertexAttributes = vertexAttributes.data(),
        .BufferLayoutCount = static_cast<uint32_t>(bufferLayouts.size()),
        .BufferLayouts = bufferLayouts.data(),
    };

    std::string shaderPath;
    if (graphicsApi == AXR_GRAPHICS_API_VULKAN) {
        shaderPath = axrGetEngineAssetsDirectoryPath().append("shaders/shader.vert.spv").generic_string();
    } else {
        shaderPath = axrGetEngineAssetsDirectoryPath().append("shaders/shader.vert").generic_string();
    }

    AxrShaderConfig shaderConfig{
        .Name = {},
        .FilePath = {},
        .Properties = reinterpret_cast<AxrShaderProperties_T>(&shaderProperties)
    };
    strncpy_s(
        shaderConfig.Name,
        axrEngineAssetGetShaderName(AXR_ENGINE_ASSET_SHADER_DEFAULT_VERT),
        AXR_MAX_ASSET_NAME_SIZE
    );
    strncpy_s(shaderConfig.FilePath, shaderPath.c_str(), AXR_MAX_FILE_PATH_SIZE);

    if (!axrShaderConfigIsValid(&shaderConfig)) {
        return AXR_ERROR;
    }

    shader = AxrShader(shaderConfig);

    return AXR_SUCCESS;
}

AxrResult axrEngineAssetCreateShader_DefaultFrag(const AxrGraphicsApiEnum graphicsApi, AxrShader& shader) {
    AxrShaderImageSamplerBufferLayout imageSamplerBufferLayout{
        .Binding = 0,
    };

    std::array bufferLayouts{
        reinterpret_cast<AxrShaderBufferLayout_T>(&imageSamplerBufferLayout)
    };

    AxrFragmentShaderProperties shaderProperties{
        .BufferLayoutCount = static_cast<uint32_t>(bufferLayouts.size()),
        .BufferLayouts = bufferLayouts.data(),
    };

    std::string shaderPath;
    if (graphicsApi == AXR_GRAPHICS_API_VULKAN) {
        shaderPath = axrGetEngineAssetsDirectoryPath().append("shaders/shader.frag.spv").generic_string();
    } else {
        shaderPath = axrGetEngineAssetsDirectoryPath().append("shaders/shader.frag").generic_string();
    }

    AxrShaderConfig shaderConfig{
        .Name = {},
        .FilePath = {},
        .Properties = reinterpret_cast<AxrShaderProperties_T>(&shaderProperties)
    };
    strncpy_s(
        shaderConfig.Name,
        axrEngineAssetGetShaderName(AXR_ENGINE_ASSET_SHADER_DEFAULT_FRAG),
        AXR_MAX_ASSET_NAME_SIZE
    );
    strncpy_s(shaderConfig.FilePath, shaderPath.c_str(), AXR_MAX_FILE_PATH_SIZE);

    if (!axrShaderConfigIsValid(&shaderConfig)) {
        return AXR_ERROR;
    }

    shader = AxrShader(shaderConfig);

    return AXR_SUCCESS;
}

AxrResult axrEngineAssetCreateShader_DefaultFrag_Mask(const AxrGraphicsApiEnum graphicsApi, AxrShader& shader) {
    AxrShaderImageSamplerBufferLayout imageSamplerBufferLayout{
        .Binding = 0,
    };

    AxrShaderUniformBufferLayout uniformBufferLayout{
        .Binding = 1,
        .BufferSize = sizeof(float),
    };

    std::array bufferLayouts{
        reinterpret_cast<AxrShaderBufferLayout_T>(&imageSamplerBufferLayout),
        reinterpret_cast<AxrShaderBufferLayout_T>(&uniformBufferLayout),
    };

    AxrFragmentShaderProperties shaderProperties{
        .BufferLayoutCount = static_cast<uint32_t>(bufferLayouts.size()),
        .BufferLayouts = bufferLayouts.data(),
    };

    std::string shaderPath;
    if (graphicsApi == AXR_GRAPHICS_API_VULKAN) {
        shaderPath = axrGetEngineAssetsDirectoryPath().append("shaders/shader_mask.frag.spv").generic_string();
    } else {
        shaderPath = axrGetEngineAssetsDirectoryPath().append("shaders/shader.frag").generic_string();
    }

    AxrShaderConfig shaderConfig{
        .Name = {},
        .FilePath = {},
        .Properties = reinterpret_cast<AxrShaderProperties_T>(&shaderProperties)
    };
    strncpy_s(
        shaderConfig.Name,
        axrEngineAssetGetShaderName(AXR_ENGINE_ASSET_SHADER_DEFAULT_FRAG_MASK),
        AXR_MAX_ASSET_NAME_SIZE
    );
    strncpy_s(shaderConfig.FilePath, shaderPath.c_str(), AXR_MAX_FILE_PATH_SIZE);

    if (!axrShaderConfigIsValid(&shaderConfig)) {
        return AXR_ERROR;
    }

    shader = AxrShader(shaderConfig);

    return AXR_SUCCESS;
}

AxrResult axrEngineAssetCreateShader_UIElementVert(const AxrGraphicsApiEnum graphicsApi, AxrShader& shader) {
    std::array vertexAttributes{
        AxrShaderVertexAttribute{
            .Type = AXR_SHADER_VERTEX_ATTRIBUTE_POSITION,
            .Location = 0,
        },
        AxrShaderVertexAttribute{
            .Type = AXR_SHADER_VERTEX_ATTRIBUTE_TEXCOORD_0,
            .Location = 1,
        }
    };

    AxrShaderUniformBufferLayout sceneDataBufferLayout{
        .Binding = 0,
        .BufferSize = axrEngineAssetGetUniformBufferSize(AXR_ENGINE_ASSET_UNIFORM_BUFFER_SCENE_DATA)
    };

    AxrShaderUniformBufferLayout uiCanvasBufferLayout{
        .Binding = 1,
        .BufferSize = axrEngineAssetGetUniformBufferSize(AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_CANVAS)
    };

    AxrShaderDynamicUniformBufferLayout uiElementBufferLayout{
        .Binding = 2,
        .InstanceSize = axrEngineAssetGetUniformBufferInstanceSize(AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_ELEMENTS)
    };

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
    AxrShaderPushConstantBufferLayout modelMatrixBufferLayout{
        .BufferSize = axrEngineAssetGetPushConstantBufferSize(AXR_ENGINE_ASSET_PUSH_CONSTANT_BUFFER_MODEL_MATRIX)
    };
#endif

    std::array bufferLayouts{
        reinterpret_cast<AxrShaderBufferLayout_T>(&sceneDataBufferLayout),
        reinterpret_cast<AxrShaderBufferLayout_T>(&uiCanvasBufferLayout),
        reinterpret_cast<AxrShaderBufferLayout_T>(&uiElementBufferLayout),
#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
        reinterpret_cast<AxrShaderBufferLayout_T>(&modelMatrixBufferLayout),
#endif
    };

    AxrVertexShaderProperties shaderProperties{
        .VertexAttributeCount = static_cast<uint32_t>(vertexAttributes.size()),
        .VertexAttributes = vertexAttributes.data(),
        .BufferLayoutCount = static_cast<uint32_t>(bufferLayouts.size()),
        .BufferLayouts = bufferLayouts.data(),
    };

    std::string shaderPath;
    if (graphicsApi == AXR_GRAPHICS_API_VULKAN) {
        shaderPath = axrGetEngineAssetsDirectoryPath().append("shaders/ui/element.vert.spv").generic_string();
    } else {
        shaderPath = axrGetEngineAssetsDirectoryPath().append("shaders/ui/element.vert").generic_string();
    }

    AxrShaderConfig shaderConfig{
        .Name = {},
        .FilePath = {},
        .Properties = reinterpret_cast<AxrShaderProperties_T>(&shaderProperties)
    };
    strncpy_s(
        shaderConfig.Name,
        axrEngineAssetGetShaderName(AXR_ENGINE_ASSET_SHADER_UI_ELEMENT_VERT),
        AXR_MAX_ASSET_NAME_SIZE
    );
    strncpy_s(shaderConfig.FilePath, shaderPath.c_str(), AXR_MAX_FILE_PATH_SIZE);

    if (!axrShaderConfigIsValid(&shaderConfig)) {
        return AXR_ERROR;
    }

    shader = AxrShader(shaderConfig);

    return AXR_SUCCESS;
}

AxrResult axrEngineAssetCreateShader_UIRectangleFrag(const AxrGraphicsApiEnum graphicsApi, AxrShader& shader) {
    AxrShaderUniformBufferLayout uiCanvasBufferLayout{
        .Binding = 1,
        .BufferSize = axrEngineAssetGetUniformBufferSize(AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_CANVAS)
    };

    AxrShaderDynamicUniformBufferLayout dynamicUniformBufferLayout{
        .Binding = 2,
        .InstanceSize = axrEngineAssetGetUniformBufferInstanceSize(AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_ELEMENTS),
    };

    std::array bufferLayouts{
        reinterpret_cast<AxrShaderBufferLayout_T>(&uiCanvasBufferLayout),
        reinterpret_cast<AxrShaderBufferLayout_T>(&dynamicUniformBufferLayout),
    };

    AxrFragmentShaderProperties shaderProperties{
        .BufferLayoutCount = static_cast<uint32_t>(bufferLayouts.size()),
        .BufferLayouts = bufferLayouts.data(),
    };

    std::string shaderPath;
    if (graphicsApi == AXR_GRAPHICS_API_VULKAN) {
        shaderPath = axrGetEngineAssetsDirectoryPath().append("shaders/ui/rectangle.frag.spv").generic_string();
    } else {
        shaderPath = axrGetEngineAssetsDirectoryPath().append("shaders/ui/rectangle.frag").generic_string();
    }

    AxrShaderConfig shaderConfig{
        .Name = {},
        .FilePath = {},
        .Properties = reinterpret_cast<AxrShaderProperties_T>(&shaderProperties)
    };
    strncpy_s(
        shaderConfig.Name,
        axrEngineAssetGetShaderName(AXR_ENGINE_ASSET_SHADER_UI_RECTANGLE_FRAG),
        AXR_MAX_ASSET_NAME_SIZE
    );
    strncpy_s(shaderConfig.FilePath, shaderPath.c_str(), AXR_MAX_FILE_PATH_SIZE);

    if (!axrShaderConfigIsValid(&shaderConfig)) {
        return AXR_ERROR;
    }

    shader = AxrShader(shaderConfig);

    return AXR_SUCCESS;
}

AxrResult axrEngineAssetCreateShader_UIBorderFrag(const AxrGraphicsApiEnum graphicsApi, AxrShader& shader) {
    AxrShaderUniformBufferLayout uiCanvasBufferLayout{
        .Binding = 1,
        .BufferSize = axrEngineAssetGetUniformBufferSize(AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_CANVAS)
    };

    AxrShaderDynamicUniformBufferLayout dynamicUniformBufferLayout{
        .Binding = 2,
        .InstanceSize = axrEngineAssetGetUniformBufferInstanceSize(AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_ELEMENTS),
    };

    std::array bufferLayouts{
        reinterpret_cast<AxrShaderBufferLayout_T>(&uiCanvasBufferLayout),
        reinterpret_cast<AxrShaderBufferLayout_T>(&dynamicUniformBufferLayout),
    };

    AxrFragmentShaderProperties shaderProperties{
        .BufferLayoutCount = static_cast<uint32_t>(bufferLayouts.size()),
        .BufferLayouts = bufferLayouts.data(),
    };

    std::string shaderPath;
    if (graphicsApi == AXR_GRAPHICS_API_VULKAN) {
        shaderPath = axrGetEngineAssetsDirectoryPath().append("shaders/ui/border.frag.spv").generic_string();
    } else {
        shaderPath = axrGetEngineAssetsDirectoryPath().append("shaders/ui/border.frag").generic_string();
    }

    AxrShaderConfig shaderConfig{
        .Name = {},
        .FilePath = {},
        .Properties = reinterpret_cast<AxrShaderProperties_T>(&shaderProperties)
    };
    strncpy_s(
        shaderConfig.Name,
        axrEngineAssetGetShaderName(AXR_ENGINE_ASSET_SHADER_UI_BORDER_FRAG),
        AXR_MAX_ASSET_NAME_SIZE
    );
    strncpy_s(shaderConfig.FilePath, shaderPath.c_str(), AXR_MAX_FILE_PATH_SIZE);

    if (!axrShaderConfigIsValid(&shaderConfig)) {
        return AXR_ERROR;
    }

    shader = AxrShader(shaderConfig);

    return AXR_SUCCESS;
}

// ----------------------------------------- //
// Buffer Engine Assets
// ----------------------------------------- //

bool axrEngineAssetIsUniformBuffer(const AxrEngineAssetEnum engineAssetEnum) {
    return engineAssetEnum >= AXR_ENGINE_ASSET_UNIFORM_BUFFER_START &&
        engineAssetEnum <= AXR_ENGINE_ASSET_UNIFORM_BUFFER_END;
}

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
bool axrEngineAssetIsPushConstantBuffer(const AxrEngineAssetEnum engineAssetEnum) {
    return engineAssetEnum >= AXR_ENGINE_ASSET_PUSH_CONSTANT_BUFFER_START &&
        engineAssetEnum <= AXR_ENGINE_ASSET_PUSH_CONSTANT_BUFFER_END;
}
#endif

const char* axrEngineAssetGetBufferName(const AxrEngineAssetEnum engineAssetEnum) {
    if (!(
        axrEngineAssetIsUniformBuffer(engineAssetEnum)
#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
        || axrEngineAssetIsPushConstantBuffer(engineAssetEnum)
#endif
    )) {
        axrLogErrorLocation("Engine asset is not a buffer.");
        return "";
    }

    const auto foundEngineAssetIt = EngineAssetBufferNames.find(engineAssetEnum);
    if (foundEngineAssetIt == EngineAssetBufferNames.end()) {
        axrLogError("Failed to find name for engine asset: {0}.", static_cast<int>(engineAssetEnum));
        return "";
    }

    return foundEngineAssetIt->second;
}

const char* axrEngineAssetGetUniformBufferName(const AxrEngineAssetEnum engineAssetEnum) {
    if (!axrEngineAssetIsUniformBuffer(engineAssetEnum)) {
        axrLogErrorLocation("Engine asset isn't a uniform buffer.");
        return "";
    }

    return axrEngineAssetGetBufferName(engineAssetEnum);
}

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
const char* axrEngineAssetGetPushConstantBufferName(const AxrEngineAssetEnum engineAssetEnum) {
    if (!axrEngineAssetIsPushConstantBuffer(engineAssetEnum)) {
        axrLogErrorLocation("Engine asset isn't a push constant buffer.");
        return "";
    }

    return axrEngineAssetGetBufferName(engineAssetEnum);
}
#endif

bool axrEngineAssetIsBufferNameReserved(const char* name) {
    for (const auto& bufferName : EngineAssetBufferNames | std::views::values) {
        if (std::strcmp(bufferName, name) == 0) {
            return true;
        }
    }

    return false;
}

bool axrEngineAssetIsUniformBufferNameReserved(const char* name) {
    for (const auto& [engineAsset, bufferName] : EngineAssetBufferNames) {
        if (axrEngineAssetIsUniformBuffer(engineAsset) && std::strcmp(bufferName, name) == 0) {
            return true;
        }
    }

    return false;
}

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
bool axrEngineAssetIsPushConstantBufferNameReserved(const char* name) {
    for (const auto& [engineAsset, bufferName] : EngineAssetBufferNames) {
        if (axrEngineAssetIsPushConstantBuffer(engineAsset) && std::strcmp(bufferName, name) == 0) {
            return true;
        }
    }

    return false;
}
#endif

AxrResult axrEngineAssetCreateUniformBuffer(const AxrEngineAssetEnum engineAssetEnum, AxrUniformBuffer& uniformBuffer) {
    if (!axrEngineAssetIsUniformBuffer(engineAssetEnum)) {
        axrLogErrorLocation("Engine asset is not a uniform buffer.");
        return AXR_ERROR;
    }

    switch (engineAssetEnum) {
        case AXR_ENGINE_ASSET_UNIFORM_BUFFER_SCENE_DATA: {
            return axrEngineAssetCreateUniformBuffer_SceneData(uniformBuffer);
        }
        case AXR_ENGINE_ASSET_UNIFORM_BUFFER_CAMERA_DATA: {
            return axrEngineAssetCreateUniformBuffer_CameraData(uniformBuffer);
        }
        case AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_CANVAS: {
            return axrEngineAssetCreateUniformBuffer_UICanvas(uniformBuffer);
        }
        case AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_ELEMENTS: {
            return axrEngineAssetCreateUniformBuffer_UIElements(uniformBuffer);
        }
        case AXR_ENGINE_ASSET_UNDEFINED:
        default: { // NOLINT(clang-diagnostic-covered-switch-default)
            axrLogErrorLocation("Unknown uniform buffer engine asset.");
            return AXR_ERROR;
        }
    }
}

AxrResult axrEngineAssetCreateUniformBuffer_SceneData(AxrUniformBuffer& uniformBuffer) {
    AxrUniformBufferConfig uniformBufferConfig{
        .Name = {},
        .DataSize = axrEngineAssetGetUniformBufferSize(AXR_ENGINE_ASSET_UNIFORM_BUFFER_SCENE_DATA),
        .Data = nullptr,
    };
    strncpy_s(
        uniformBufferConfig.Name,
        axrEngineAssetGetUniformBufferName(AXR_ENGINE_ASSET_UNIFORM_BUFFER_SCENE_DATA),
        AXR_MAX_ASSET_NAME_SIZE
    );

    uniformBuffer = AxrUniformBuffer(uniformBufferConfig);

    return AXR_SUCCESS;
}

AxrResult axrEngineAssetCreateUniformBuffer_CameraData(AxrUniformBuffer& uniformBuffer) {
    AxrUniformBufferConfig uniformBufferConfig{
        .Name = {},
        .DataSize = axrEngineAssetGetUniformBufferSize(AXR_ENGINE_ASSET_UNIFORM_BUFFER_CAMERA_DATA),
        .Data = nullptr,
    };
    strncpy_s(
        uniformBufferConfig.Name,
        axrEngineAssetGetUniformBufferName(AXR_ENGINE_ASSET_UNIFORM_BUFFER_CAMERA_DATA),
        AXR_MAX_ASSET_NAME_SIZE
    );

    uniformBuffer = AxrUniformBuffer(uniformBufferConfig);

    return AXR_SUCCESS;
}

AxrResult axrEngineAssetCreateUniformBuffer_UICanvas(AxrUniformBuffer& uniformBuffer) {
    AxrUniformBufferConfig uniformBufferConfig{
        .Name = {},
        .DataSize = axrEngineAssetGetUniformBufferSize(AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_CANVAS),
        .Data = nullptr,
    };
    strncpy_s(
        uniformBufferConfig.Name,
        axrEngineAssetGetUniformBufferName(AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_CANVAS),
        AXR_MAX_ASSET_NAME_SIZE
    );

    uniformBuffer = AxrUniformBuffer(uniformBufferConfig);

    return AXR_SUCCESS;
}

AxrResult axrEngineAssetCreateUniformBuffer_UIElements(AxrUniformBuffer& uniformBuffer) {
    uint32_t maxElementCount = 1;
    if (Clay_GetCurrentContext() != nullptr) {
        maxElementCount = Clay_GetMaxElementCount();
    } else {
        axrLogWarningLocation("Clay context is null.");
    }

    AxrDynamicUniformBufferConfig uniformBufferConfig{
        .Name = {},
        .InstanceCount = maxElementCount,
        .InstanceSize = axrEngineAssetGetUniformBufferInstanceSize(AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_ELEMENTS),
        .Data = nullptr,
    };
    strncpy_s(
        uniformBufferConfig.Name,
        axrEngineAssetGetUniformBufferName(AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_ELEMENTS),
        AXR_MAX_ASSET_NAME_SIZE
    );

    uniformBuffer = AxrUniformBuffer(uniformBufferConfig);

    return AXR_SUCCESS;
}

// ----------------------------------------- //
// Material Engine Assets
// ----------------------------------------- //

bool axrEngineAssetIsMaterial(const AxrEngineAssetEnum engineAssetEnum) {
    return engineAssetEnum >= AXR_ENGINE_ASSET_MATERIAL_START &&
        engineAssetEnum <= AXR_ENGINE_ASSET_MATERIAL_END;
}

const char* axrEngineAssetGetMaterialName(const AxrEngineAssetEnum engineAssetEnum) {
    if (!axrEngineAssetIsMaterial(engineAssetEnum)) {
        axrLogErrorLocation("Engine asset is not a material.");
        return "";
    }

    const auto foundEngineAssetIt = EngineAssetMaterialNames.find(engineAssetEnum);
    if (foundEngineAssetIt == EngineAssetMaterialNames.end()) {
        axrLogError("Failed to find name for engine asset: {0}.", static_cast<int>(engineAssetEnum));
        return "";
    }

    return foundEngineAssetIt->second;
}

bool axrEngineAssetIsMaterialNameReserved(const char* name) {
    for (const auto& engineAssetName : EngineAssetMaterialNames | std::views::values) {
        if (std::strcmp(engineAssetName, name) == 0) {
            return true;
        }
    }

    return false;
}

AxrResult axrEngineAssetCreateMaterial_DefaultMaterial(
    const std::string& materialName,
    const AxrEngineAssetMaterial_DefaultMaterial& materialValues,
    AxrMaterial& material,
    std::vector<AxrEngineAssetEnum>& materialShaders
) {
    AxrShaderValues vertexShaderValues{
        .BufferLinkCount = 0,
        .BufferLinks = nullptr,
    };

    AxrShaderImageSamplerBufferLink imageSamplerBufferLink{
        .Binding = 0,
        .ImageName = {},
        .ImageSamplerName = {},
    };
    strncpy_s(imageSamplerBufferLink.ImageName, materialValues.ImageName, AXR_MAX_ASSET_NAME_SIZE);
    strncpy_s(imageSamplerBufferLink.ImageSamplerName, materialValues.ImageSamplerName, AXR_MAX_ASSET_NAME_SIZE);

    AxrShaderUniformBufferLink uniformBufferLink{
        .Binding = 1,
        .BufferName = {},
    };
    strncpy_s(uniformBufferLink.BufferName, materialValues.AlphaCutoffBufferName, AXR_MAX_ASSET_NAME_SIZE);

    std::vector fragmentBufferLinks{
        reinterpret_cast<AxrShaderBufferLink_T>(&imageSamplerBufferLink),
    };

    if (materialValues.AlphaCutoffBufferName[0] != '\0') {
        fragmentBufferLinks.push_back(reinterpret_cast<AxrShaderBufferLink_T>(&uniformBufferLink));
    }

    AxrShaderValues fragmentShaderValues{
        .BufferLinkCount = static_cast<uint32_t>(fragmentBufferLinks.size()),
        .BufferLinks = fragmentBufferLinks.data()
    };

    AxrMaterialConfig materialConfig{
        .Name = {},
        .VertexShaderName = {},
        .FragmentShaderName = {},
        .VertexShaderValues = &vertexShaderValues,
        .FragmentShaderValues = &fragmentShaderValues,
        .BackfaceCullMode = materialValues.BackfaceCullMode,
        .AlphaRenderMode = materialValues.AlphaRenderMode,
        .EnableDepthTest = true,
        .EnableDepthWrite = materialValues.AlphaRenderMode == AXR_MATERIAL_ALPHA_RENDER_MODE_OPAQUE,
    };
    strncpy_s(
        materialConfig.Name,
        materialName.c_str(),
        AXR_MAX_ASSET_NAME_SIZE
    );
    strncpy_s(
        materialConfig.VertexShaderName,
        axrEngineAssetGetShaderName(AXR_ENGINE_ASSET_SHADER_DEFAULT_VERT),
        AXR_MAX_ASSET_NAME_SIZE
    );
    materialShaders.push_back(AXR_ENGINE_ASSET_SHADER_DEFAULT_VERT);

    if (materialValues.AlphaCutoffBufferName[0] != '\0') {
        strncpy_s(
            materialConfig.FragmentShaderName,
            axrEngineAssetGetShaderName(AXR_ENGINE_ASSET_SHADER_DEFAULT_FRAG_MASK),
            AXR_MAX_ASSET_NAME_SIZE
        );
        materialShaders.push_back(AXR_ENGINE_ASSET_SHADER_DEFAULT_FRAG_MASK);
    } else {
        strncpy_s(
            materialConfig.FragmentShaderName,
            axrEngineAssetGetShaderName(AXR_ENGINE_ASSET_SHADER_DEFAULT_FRAG),
            AXR_MAX_ASSET_NAME_SIZE
        );
        materialShaders.push_back(AXR_ENGINE_ASSET_SHADER_DEFAULT_FRAG);
    }

    if (!axrMaterialConfigIsValid(&materialConfig)) {
        return AXR_ERROR;
    }

    material = AxrMaterial(materialConfig);

    return AXR_SUCCESS;
}

AxrResult axrEngineAssetCreateMaterial_UIRectangle(
    AxrMaterial& material,
    std::vector<AxrEngineAssetEnum>& materialShaders
) {
    AxrShaderUniformBufferLink sceneDataBufferLink{
        .Binding = 0,
        .BufferName = {},
    };
    strncpy_s(
        sceneDataBufferLink.BufferName,
        axrEngineAssetGetUniformBufferName(AXR_ENGINE_ASSET_UNIFORM_BUFFER_SCENE_DATA),
        AXR_MAX_ASSET_NAME_SIZE
    );

    AxrShaderUniformBufferLink cameraDataBufferLink{
        .Binding = 1,
        .BufferName = {},
    };
    strncpy_s(
        cameraDataBufferLink.BufferName,
        axrEngineAssetGetUniformBufferName(AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_CANVAS),
        AXR_MAX_ASSET_NAME_SIZE
    );

    std::array vertexBufferLinks{
        reinterpret_cast<AxrShaderBufferLink_T>(&sceneDataBufferLink),
        reinterpret_cast<AxrShaderBufferLink_T>(&cameraDataBufferLink),
    };

    AxrShaderValues vertexShaderValues{
        .BufferLinkCount = static_cast<uint32_t>(vertexBufferLinks.size()),
        .BufferLinks = vertexBufferLinks.data(),
    };

    AxrShaderUniformBufferLink dynamicUniformBufferLink{
        .Binding = 2,
        .BufferName = {},
    };
    strncpy_s(
        dynamicUniformBufferLink.BufferName,
        axrEngineAssetGetUniformBufferName(AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_ELEMENTS),
        AXR_MAX_ASSET_NAME_SIZE
    );

    std::array fragmentBufferLinks{
        reinterpret_cast<AxrShaderBufferLink_T>(&dynamicUniformBufferLink),
    };

    AxrShaderValues fragmentShaderValues{
        .BufferLinkCount = static_cast<uint32_t>(fragmentBufferLinks.size()),
        .BufferLinks = fragmentBufferLinks.data()
    };

    AxrMaterialConfig materialConfig{
        .Name = {},
        .VertexShaderName = {},
        .FragmentShaderName = {},
        .VertexShaderValues = &vertexShaderValues,
        .FragmentShaderValues = &fragmentShaderValues,
        .BackfaceCullMode = AXR_MATERIAL_BACKFACE_CULL_MODE_BACK,
        .AlphaRenderMode = AXR_MATERIAL_ALPHA_RENDER_MODE_ALPHA_BLEND,
        .EnableDepthTest = false,
        .EnableDepthWrite = false,
    };
    strncpy_s(
        materialConfig.Name,
        axrEngineAssetGetMaterialName(AXR_ENGINE_ASSET_MATERIAL_UI_RECTANGLE),
        AXR_MAX_ASSET_NAME_SIZE
    );
    strncpy_s(
        materialConfig.VertexShaderName,
        axrEngineAssetGetShaderName(AXR_ENGINE_ASSET_SHADER_UI_ELEMENT_VERT),
        AXR_MAX_ASSET_NAME_SIZE
    );
    materialShaders.push_back(AXR_ENGINE_ASSET_SHADER_UI_ELEMENT_VERT);
    strncpy_s(
        materialConfig.FragmentShaderName,
        axrEngineAssetGetShaderName(AXR_ENGINE_ASSET_SHADER_UI_RECTANGLE_FRAG),
        AXR_MAX_ASSET_NAME_SIZE
    );
    materialShaders.push_back(AXR_ENGINE_ASSET_SHADER_UI_RECTANGLE_FRAG);

    if (!axrMaterialConfigIsValid(&materialConfig)) {
        return AXR_ERROR;
    }

    material = AxrMaterial(materialConfig);

    return AXR_SUCCESS;
}

AxrResult axrEngineAssetCreateMaterial_UIBorder(
    AxrMaterial& material,
    std::vector<AxrEngineAssetEnum>& materialShaders
) {
    AxrShaderUniformBufferLink sceneDataBufferLink{
        .Binding = 0,
        .BufferName = {},
    };
    strncpy_s(
        sceneDataBufferLink.BufferName,
        axrEngineAssetGetUniformBufferName(AXR_ENGINE_ASSET_UNIFORM_BUFFER_SCENE_DATA),
        AXR_MAX_ASSET_NAME_SIZE
    );

    AxrShaderUniformBufferLink cameraDataBufferLink{
        .Binding = 1,
        .BufferName = {},
    };
    strncpy_s(
        cameraDataBufferLink.BufferName,
        axrEngineAssetGetUniformBufferName(AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_CANVAS),
        AXR_MAX_ASSET_NAME_SIZE
    );

    std::array vertexBufferLinks{
        reinterpret_cast<AxrShaderBufferLink_T>(&sceneDataBufferLink),
        reinterpret_cast<AxrShaderBufferLink_T>(&cameraDataBufferLink),
    };

    AxrShaderValues vertexShaderValues{
        .BufferLinkCount = static_cast<uint32_t>(vertexBufferLinks.size()),
        .BufferLinks = vertexBufferLinks.data(),
    };

    AxrShaderUniformBufferLink dynamicUniformBufferLink{
        .Binding = 2,
        .BufferName = {},
    };
    strncpy_s(
        dynamicUniformBufferLink.BufferName,
        axrEngineAssetGetUniformBufferName(AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_ELEMENTS),
        AXR_MAX_ASSET_NAME_SIZE
    );

    std::array fragmentBufferLinks{
        reinterpret_cast<AxrShaderBufferLink_T>(&dynamicUniformBufferLink),
    };

    AxrShaderValues fragmentShaderValues{
        .BufferLinkCount = static_cast<uint32_t>(fragmentBufferLinks.size()),
        .BufferLinks = fragmentBufferLinks.data()
    };

    AxrMaterialConfig materialConfig{
        .Name = {},
        .VertexShaderName = {},
        .FragmentShaderName = {},
        .VertexShaderValues = &vertexShaderValues,
        .FragmentShaderValues = &fragmentShaderValues,
        .BackfaceCullMode = AXR_MATERIAL_BACKFACE_CULL_MODE_BACK,
        .AlphaRenderMode = AXR_MATERIAL_ALPHA_RENDER_MODE_ALPHA_BLEND,
        .EnableDepthTest = false,
        .EnableDepthWrite = false,
    };
    strncpy_s(
        materialConfig.Name,
        axrEngineAssetGetMaterialName(AXR_ENGINE_ASSET_MATERIAL_UI_BORDER),
        AXR_MAX_ASSET_NAME_SIZE
    );
    strncpy_s(
        materialConfig.VertexShaderName,
        axrEngineAssetGetShaderName(AXR_ENGINE_ASSET_SHADER_UI_ELEMENT_VERT),
        AXR_MAX_ASSET_NAME_SIZE
    );
    materialShaders.push_back(AXR_ENGINE_ASSET_SHADER_UI_ELEMENT_VERT);
    strncpy_s(
        materialConfig.FragmentShaderName,
        axrEngineAssetGetShaderName(AXR_ENGINE_ASSET_SHADER_UI_BORDER_FRAG),
        AXR_MAX_ASSET_NAME_SIZE
    );
    materialShaders.push_back(AXR_ENGINE_ASSET_SHADER_UI_BORDER_FRAG);

    if (!axrMaterialConfigIsValid(&materialConfig)) {
        return AXR_ERROR;
    }

    material = AxrMaterial(materialConfig);

    return AXR_SUCCESS;
}

// ----------------------------------------- //
// Model Engine Assets
// ----------------------------------------- //

bool axrEngineAssetIsModel(const AxrEngineAssetEnum engineAssetEnum) {
    return engineAssetEnum >= AXR_ENGINE_ASSET_MODEL_START &&
        engineAssetEnum <= AXR_ENGINE_ASSET_MODEL_END;
}

const char* axrEngineAssetGetModelName(const AxrEngineAssetEnum engineAssetEnum) {
    if (!axrEngineAssetIsModel(engineAssetEnum)) {
        axrLogErrorLocation("Engine asset is not a model.");
        return "";
    }

    const auto foundEngineAssetIt = EngineAssetModelNames.find(engineAssetEnum);
    if (foundEngineAssetIt == EngineAssetModelNames.end()) {
        axrLogError("Failed to find name for engine asset: {0}.", static_cast<int>(engineAssetEnum));
        return "";
    }

    return foundEngineAssetIt->second;
}

bool axrEngineAssetIsModelNameReserved(const char* name) {
    for (const auto& engineAssetName : EngineAssetModelNames | std::views::values) {
        if (std::strcmp(engineAssetName, name) == 0) {
            return true;
        }
    }

    return false;
}

AxrResult axrEngineAssetCreateModel(
    const std::string& modelName,
    const AxrEngineAssetEnum engineAssetEnum,
    AxrModel& model
) {
    if (!axrEngineAssetIsModel(engineAssetEnum)) {
        axrLogErrorLocation("Engine asset is not a model.");
        return AXR_ERROR;
    }

    switch (engineAssetEnum) {
        case AXR_ENGINE_ASSET_MODEL_TRIANGLE: {
            return axrEngineAssetCreateModel_Triangle(modelName, model);
        }
        case AXR_ENGINE_ASSET_MODEL_SQUARE: {
            return axrEngineAssetCreateModel_Square(modelName, model);
        }
        case AXR_ENGINE_ASSET_MODEL_CUBE: {
            return axrEngineAssetCreateModel_Cube(modelName, model);
        }
        case AXR_ENGINE_ASSET_MODEL_UI_RECTANGLE: {
            return axrEngineAssetCreateModel_UIRectangle(modelName, model);
        }
        case AXR_ENGINE_ASSET_UNDEFINED:
        default: { // NOLINT(clang-diagnostic-covered-switch-default)
            axrLogErrorLocation("Unknown model engine asset.");
            return AXR_ERROR;
        }
    }
}

AxrResult axrEngineAssetCreateModel_Triangle(const std::string& modelName, AxrModel& model) {
    AxrModelConfig modelConfig{
        .Name = {},
        .FilePath = {},
    };
    strncpy_s(modelConfig.Name, modelName.c_str(), AXR_MAX_ASSET_NAME_SIZE);

    model = AxrModel(modelConfig);

    std::vector<AxrVertex> vertices{
        AxrVertex{
            .Position = {0.0f, 0.866f, 0.0f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {0.5f, 0.0f},
        },
        AxrVertex{
            .Position = {-0.5f, 0.0f, 0.0f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {0.0f, 1.0f},
        },
        AxrVertex{
            .Position = {0.5f, 0.0f, 0.0f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {1.0f, 1.0f},
        },
    };

    std::vector<uint32_t> indices{
        0, 1, 2,
    };

    AxrSubmesh submesh{
        .VertexCount = static_cast<uint32_t>(vertices.size()),
        .Vertices = vertices.data(),
        .IndexCount = static_cast<uint32_t>(indices.size()),
        .Indices = indices.data(),
    };

    const AxrMesh mesh{
        .SubmeshCount = 1,
        .Submeshes = &submesh,
    };

    const AxrResult axrResult = model.setData(1, &mesh);
    if (AXR_FAILED(axrResult)) {
        return axrResult;
    }

    return AXR_SUCCESS;
}

AxrResult axrEngineAssetCreateModel_Square(const std::string& modelName, AxrModel& model) {
    AxrModelConfig modelConfig{
        .Name = {},
        .FilePath = {},
    };
    strncpy_s(modelConfig.Name, modelName.c_str(), AXR_MAX_ASSET_NAME_SIZE);

    model = AxrModel(modelConfig);

    std::vector<AxrVertex> vertices{
        AxrVertex{
            .Position = {0.5f, 0.5f, 0.0f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {1.0f, 0.0f},
        },
        AxrVertex{
            .Position = {-0.5f, 0.5f, 0.0f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {0.0f, 0.0f},
        },
        AxrVertex{
            .Position = {-0.5f, -0.5f, 0.0f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {0.0f, 1.0f},
        },
        AxrVertex{
            .Position = {0.5f, -0.5f, 0.0f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {1.0f, 1.0f},
        },
    };

    std::vector<uint32_t> indices{
        0, 1, 2,
        2, 3, 0,
    };

    AxrSubmesh submesh{
        .VertexCount = static_cast<uint32_t>(vertices.size()),
        .Vertices = vertices.data(),
        .IndexCount = static_cast<uint32_t>(indices.size()),
        .Indices = indices.data(),
    };

    const AxrMesh mesh{
        .SubmeshCount = 1,
        .Submeshes = &submesh,
    };

    const AxrResult axrResult = model.setData(1, &mesh);
    if (AXR_FAILED(axrResult)) {
        return axrResult;
    }

    return AXR_SUCCESS;
}

AxrResult axrEngineAssetCreateModel_Cube(const std::string& modelName, AxrModel& model) {
    AxrModelConfig modelConfig{
        .Name = {},
        .FilePath = {},
    };
    strncpy_s(modelConfig.Name, modelName.c_str(), AXR_MAX_ASSET_NAME_SIZE);

    model = AxrModel(modelConfig);

    std::vector<AxrVertex> vertices{
        /// Front face
        AxrVertex{
            .Position = {0.5f, 0.5f, 0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {1.0f, 0.0f},
        },
        AxrVertex{
            .Position = {-0.5f, 0.5f, 0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {0.0f, 0.0f},
        },
        AxrVertex{
            .Position = {-0.5f, -0.5f, 0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {0.0f, 1.0f},
        },
        AxrVertex{
            .Position = {0.5f, -0.5f, 0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {1.0f, 1.0f},
        },

        // Back face
        AxrVertex{
            .Position = {-0.5f, 0.5f, -0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {1.0f, 0.0f},
        },
        AxrVertex{
            .Position = {0.5f, 0.5f, -0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {0.0f, 0.0f},
        },
        AxrVertex{
            .Position = {0.5f, -0.5f, -0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {0.0f, 1.0f},
        },
        AxrVertex{
            .Position = {-0.5f, -0.5f, -0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {1.0f, 1.0f},
        },

        // Top face
        AxrVertex{
            .Position = {0.5f, 0.5f, -0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {1.0f, 0.0f},
        },
        AxrVertex{
            .Position = {-0.5f, 0.5f, -0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {0.0f, 0.0f},
        },
        AxrVertex{
            .Position = {-0.5f, 0.5f, 0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {0.0f, 1.0f},
        },
        AxrVertex{
            .Position = {0.5f, 0.5f, 0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {1.0f, 1.0f},
        },

        // Bottom face
        AxrVertex{
            .Position = {-0.5f, -0.5f, -0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {1.0f, 0.0f},
        },
        AxrVertex{
            .Position = {0.5f, -0.5f, -0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {0.0f, 0.0f},
        },
        AxrVertex{
            .Position = {0.5f, -0.5f, 0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {0.0f, 1.0f},
        },
        AxrVertex{
            .Position = {-0.5f, -0.5f, 0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {1.0f, 1.0f},
        },

        // Left face
        AxrVertex{
            .Position = {-0.5f, 0.5f, 0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {1.0f, 0.0f},
        },
        AxrVertex{
            .Position = {-0.5f, 0.5f, -0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {0.0f, 0.0f},
        },
        AxrVertex{
            .Position = {-0.5f, -0.5f, -0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {0.0f, 1.0f},
        },
        AxrVertex{
            .Position = {-0.5f, -0.5f, 0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {1.0f, 1.0f},
        },

        // Right face
        AxrVertex{
            .Position = {0.5f, 0.5f, -0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {1.0f, 0.0f},
        },
        AxrVertex{
            .Position = {0.5f, 0.5f, 0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {0.0f, 0.0f},
        },
        AxrVertex{
            .Position = {0.5f, -0.5f, 0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {0.0f, 1.0f},
        },
        AxrVertex{
            .Position = {0.5f, -0.5f, -0.5f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {1.0f, 1.0f},
        },
    };

    std::vector<uint32_t> indices{
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 5, 6,
        6, 7, 4,

        // Top face
        8, 9, 10,
        10, 11, 8,

        // Bottom face
        12, 13, 14,
        14, 15, 12,

        // Left face
        16, 17, 18,
        18, 19, 16,

        // Right face
        20, 21, 22,
        22, 23, 20,
    };

    AxrSubmesh submesh{
        .VertexCount = static_cast<uint32_t>(vertices.size()),
        .Vertices = vertices.data(),
        .IndexCount = static_cast<uint32_t>(indices.size()),
        .Indices = indices.data(),
    };

    const AxrMesh mesh{
        .SubmeshCount = 1,
        .Submeshes = &submesh,
    };

    const AxrResult axrResult = model.setData(1, &mesh);
    if (AXR_FAILED(axrResult)) {
        return axrResult;
    }

    return AXR_SUCCESS;
}

AxrResult axrEngineAssetCreateModel_UIRectangle(const std::string& modelName, AxrModel& model) {
    AxrModelConfig modelConfig{
        .Name = {},
        .FilePath = {},
    };
    strncpy_s(modelConfig.Name, modelName.c_str(), AXR_MAX_ASSET_NAME_SIZE);

    model = AxrModel(modelConfig);

    std::vector<AxrVertex> vertices{
        AxrVertex{
            .Position = {1.0f, 1.0f, 0.0f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {1.0f, 1.0f},
        },
        AxrVertex{
            .Position = {0.0f, 1.0f, 0.0f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {0.0f, 1.0f},
        },
        AxrVertex{
            .Position = {0.0f, 0.0f, 0.0f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {0.0f, 0.0f},
        },
        AxrVertex{
            .Position = {1.0f, 0.0f, 0.0f},
            .Color = {1.0f, 1.0f, 1.0f},
            .TexCoord_0 = {1.0f, 0.0f},
        },
    };

    std::vector<uint32_t> indices{
        0, 1, 2,
        2, 3, 0,
    };

    AxrSubmesh submesh{
        .VertexCount = static_cast<uint32_t>(vertices.size()),
        .Vertices = vertices.data(),
        .IndexCount = static_cast<uint32_t>(indices.size()),
        .Indices = indices.data(),
    };

    const AxrMesh mesh{
        .SubmeshCount = 1,
        .Submeshes = &submesh,
    };

    const AxrResult axrResult = model.setData(1, &mesh);
    if (AXR_FAILED(axrResult)) {
        return axrResult;
    }

    return AXR_SUCCESS;
}

// ----------------------------------------- //
// Image Sampler Engine Assets
// ----------------------------------------- //

bool axrEngineAssetIsImageSampler(const AxrEngineAssetEnum engineAssetEnum) {
    return engineAssetEnum >= AXR_ENGINE_ASSET_IMAGE_SAMPLER_START &&
        engineAssetEnum <= AXR_ENGINE_ASSET_IMAGE_SAMPLER_END;
}

const char* axrEngineAssetGetImageSamplerName(const AxrEngineAssetEnum engineAssetEnum) {
    if (!axrEngineAssetIsImageSampler(engineAssetEnum)) {
        axrLogErrorLocation("Engine asset is not an image sampler.");
        return "";
    }

    const auto foundEngineAssetIt = EngineAssetImageSamplerNames.find(engineAssetEnum);
    if (foundEngineAssetIt == EngineAssetImageSamplerNames.end()) {
        axrLogError("Failed to find name for engine asset: {0}.", static_cast<int>(engineAssetEnum));
        return "";
    }

    return foundEngineAssetIt->second;
}

bool axrEngineAssetIsImageSamplerNameReserved(const char* name) {
    for (const auto& engineAssetName : EngineAssetImageSamplerNames | std::views::values) {
        if (std::strcmp(engineAssetName, name) == 0) {
            return true;
        }
    }

    return false;
}

AxrResult axrEngineAssetCreateImageSampler(
    const std::string& imageSamplerName,
    const AxrEngineAssetEnum engineAssetEnum,
    AxrImageSampler& imageSampler
) {
    if (!axrEngineAssetIsImageSampler(engineAssetEnum)) {
        axrLogErrorLocation("Engine asset is not an image sampler.");
        return AXR_ERROR;
    }

    switch (engineAssetEnum) {
        case AXR_ENGINE_ASSET_IMAGE_SAMPLER_NEAREST_REPEAT: {
            return axrEngineAssetCreateImageSampler_NearestRepeat(imageSamplerName, imageSampler);
        }
        case AXR_ENGINE_ASSET_IMAGE_SAMPLER_LINEAR_REPEAT: {
            return axrEngineAssetCreateImageSampler_LinearRepeat(imageSamplerName, imageSampler);
        }
        case AXR_ENGINE_ASSET_UNDEFINED:
        default: { // NOLINT(clang-diagnostic-covered-switch-default)
            axrLogErrorLocation("Unknown image sampler engine asset.");
            return AXR_ERROR;
        }
    }
}

AxrResult axrEngineAssetCreateImageSampler_NearestRepeat(
    const std::string& imageSamplerName,
    AxrImageSampler& imageSampler
) {
    AxrImageSamplerConfig imageSamplerConfig{
        .Name = {},
        .MinFilter = AXR_IMAGE_SAMPLER_FILTER_NEAREST,
        .MagFilter = AXR_IMAGE_SAMPLER_FILTER_NEAREST,
        .MipmapFilter = AXR_IMAGE_SAMPLER_FILTER_NEAREST,
        .WrapU = AXR_IMAGE_SAMPLER_WRAP_REPEAT,
        .WrapV = AXR_IMAGE_SAMPLER_WRAP_REPEAT,
    };
    strncpy_s(imageSamplerConfig.Name, imageSamplerName.c_str(), AXR_MAX_ASSET_NAME_SIZE);

    imageSampler = AxrImageSampler(imageSamplerConfig);

    return AXR_SUCCESS;
}

AxrResult axrEngineAssetCreateImageSampler_LinearRepeat(
    const std::string& imageSamplerName,
    AxrImageSampler& imageSampler
) {
    AxrImageSamplerConfig imageSamplerConfig{
        .Name = {},
        .MinFilter = AXR_IMAGE_SAMPLER_FILTER_LINEAR,
        .MagFilter = AXR_IMAGE_SAMPLER_FILTER_LINEAR,
        .MipmapFilter = AXR_IMAGE_SAMPLER_FILTER_LINEAR,
        .WrapU = AXR_IMAGE_SAMPLER_WRAP_REPEAT,
        .WrapV = AXR_IMAGE_SAMPLER_WRAP_REPEAT,
    };
    strncpy_s(imageSamplerConfig.Name, imageSamplerName.c_str(), AXR_MAX_ASSET_NAME_SIZE);

    imageSampler = AxrImageSampler(imageSamplerConfig);

    return AXR_SUCCESS;
}


// ----------------------------------------- //
// Image Engine Assets
// ----------------------------------------- //

bool axrEngineAssetIsImage(const AxrEngineAssetEnum engineAssetEnum) {
    return engineAssetEnum >= AXR_ENGINE_ASSET_IMAGE_START &&
        engineAssetEnum <= AXR_ENGINE_ASSET_IMAGE_END;
}

const char* axrEngineAssetGetImageName(const AxrEngineAssetEnum engineAssetEnum) {
    if (!axrEngineAssetIsImage(engineAssetEnum)) {
        axrLogErrorLocation("Engine asset is not an image.");
        return "";
    }

    const auto foundEngineAssetIt = EngineAssetImageNames.find(engineAssetEnum);
    if (foundEngineAssetIt == EngineAssetImageNames.end()) {
        axrLogError("Failed to find name for engine asset: {0}.", static_cast<int>(engineAssetEnum));
        return "";
    }

    return foundEngineAssetIt->second;
}

bool axrEngineAssetIsImageNameReserved(const char* name) {
    for (const auto& engineAssetName : EngineAssetImageNames | std::views::values) {
        if (std::strcmp(engineAssetName, name) == 0) {
            return true;
        }
    }

    return false;
}

AxrResult axrEngineAssetCreateImage(
    const std::string& imageName,
    const AxrEngineAssetEnum engineAssetEnum,
    AxrImage& image
) {
    if (!axrEngineAssetIsImage(engineAssetEnum)) {
        axrLogErrorLocation("Engine asset is not an image.");
        return AXR_ERROR;
    }

    switch (engineAssetEnum) {
        case AXR_ENGINE_ASSET_IMAGE_MISSING_TEXTURE: {
            return axrEngineAssetCreateImage_MissingTexture(imageName, image);
        }
        case AXR_ENGINE_ASSET_IMAGE_UV_TESTER: {
            return axrEngineAssetCreateImage_UvTester(imageName, image);
        }
        case AXR_ENGINE_ASSET_UNDEFINED:
        default: { // NOLINT(clang-diagnostic-covered-switch-default)
            axrLogErrorLocation("Unknown image engine asset.");
            return AXR_ERROR;
        }
    }
}

AxrResult axrEngineAssetCreateImage_MissingTexture(const std::string& imageName, AxrImage& image) {
    AxrImageConfig imageConfig{
        .Name = {},
        .FilePath = {},
    };
    strncpy_s(imageConfig.Name, imageName.c_str(), AXR_MAX_ASSET_NAME_SIZE);

    image = AxrImage(imageConfig);

    std::vector<stbi_uc> imageData;
    constexpr uint32_t size = 16;

    for (uint32_t h = 0; h < size; ++h) {
        for (uint32_t w = 0; w < size; ++w) {
            if ((h + w) % 2 == 0) {
                // Magenta
                imageData.push_back(255);
                imageData.push_back(0);
                imageData.push_back(255);
            } else {
                // Black
                imageData.push_back(0);
                imageData.push_back(0);
                imageData.push_back(0);
            }
        }
    }

    const AxrResult axrResult = image.setData(size, size, AXR_IMAGE_COLOR_CHANNELS_RGB, imageData.data());
    if (AXR_FAILED(axrResult)) {
        return axrResult;
    }
    return AXR_SUCCESS;
}

AxrResult axrEngineAssetCreateImage_UvTester(const std::string& imageName, AxrImage& image) {
    const std::string& filePath = axrGetEngineAssetsDirectoryPath().append("images/uv-tester.png").generic_string();

    AxrImageConfig imageConfig{
        .Name = {},
        .FilePath = {},
    };
    strncpy_s(imageConfig.Name, imageName.c_str(), AXR_MAX_ASSET_NAME_SIZE);
    strncpy_s(imageConfig.FilePath, filePath.c_str(), AXR_MAX_FILE_PATH_SIZE);

    image = AxrImage(imageConfig);

    return AXR_SUCCESS;
}
