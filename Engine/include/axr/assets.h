#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/common/defines.h"
#include "axr/common/enums.h"
#include "axr/common/types.h"

// ----------------------------------------- //
// GLM Headers
// ----------------------------------------- //
#include <glm/glm.hpp>

// ----------------------------------------- //
// STB Headers
// ----------------------------------------- //
#include <stb_image.h>

// ---------------------------------------------------------------------------------- //
//                                  Shader Properties                                 //
// ---------------------------------------------------------------------------------- //

// ----------------------------------------- //
// Enums
// ----------------------------------------- //

// ---- Shader Properties ----

/// Shader stage enum
enum AxrShaderStageEnum {
    AXR_SHADER_STAGE_UNDEFINED = 0,
    AXR_SHADER_STAGE_VERTEX,
    AXR_SHADER_STAGE_FRAGMENT,
};

// ---- Shader Buffer Layout ----

/// Shader buffer layout enum
enum AxrShaderBufferLayoutEnum {
    AXR_SHADER_BUFFER_LAYOUT_UNDEFINED = 0,
    AXR_SHADER_BUFFER_LAYOUT_UNIFORM_BUFFER,
    AXR_SHADER_BUFFER_LAYOUT_DYNAMIC_UNIFORM_BUFFER,
    AXR_SHADER_BUFFER_LAYOUT_IMAGE_SAMPLER_BUFFER,
    AXR_SHADER_BUFFER_LAYOUT_PUSH_CONSTANT_BUFFER,
};

// ---- Vertex Attributes ----

/// Shader vertex attribute enum
enum AxrShaderVertexAttributeEnum {
    AXR_SHADER_VERTEX_ATTRIBUTE_UNDEFINED = 0,
    AXR_SHADER_VERTEX_ATTRIBUTE_POSITION,
    AXR_SHADER_VERTEX_ATTRIBUTE_COLOR,
    AXR_SHADER_VERTEX_ATTRIBUTE_TEXCOORD_0,
    AXR_SHADER_VERTEX_ATTRIBUTE_TEXCOORD_1,
    AXR_SHADER_VERTEX_ATTRIBUTE_TEXCOORD_2,
    AXR_SHADER_VERTEX_ATTRIBUTE_TEXCOORD_3,
};

// ----------------------------------------- //
// Structs
// ----------------------------------------- //

// ---- Shader Buffer Layout ----

/// Shader Buffer Layout Base Structure
struct AxrShaderBufferLayoutStructure {
    AxrShaderBufferLayoutEnum Type = AXR_SHADER_BUFFER_LAYOUT_UNDEFINED;
};

/// AxrShaderBufferLayoutStructure Handle Type
typedef AxrShaderBufferLayoutStructure* AxrShaderBufferLayout_T;
/// Const AxrShaderBufferLayoutStructure Handle Type
typedef const AxrShaderBufferLayoutStructure* AxrShaderBufferLayoutConst_T;

/// Shader Uniform Buffer Layout
struct AxrShaderUniformBufferLayout {
    const AxrShaderBufferLayoutEnum Type = AXR_SHADER_BUFFER_LAYOUT_UNIFORM_BUFFER;
    uint32_t Binding;
    uint64_t BufferSize;
};

/// AxrShaderUniformBufferLayout Handle Type
typedef AxrShaderUniformBufferLayout* AxrShaderUniformBufferLayout_T;
/// Const AxrShaderUniformBufferLayout Handle Type
typedef const AxrShaderUniformBufferLayout* AxrShaderUniformBufferLayoutConst_T;

/// Shader Dynamic Uniform Buffer Layout
struct AxrShaderDynamicUniformBufferLayout {
    const AxrShaderBufferLayoutEnum Type = AXR_SHADER_BUFFER_LAYOUT_DYNAMIC_UNIFORM_BUFFER;
    uint32_t Binding;
    uint64_t InstanceSize;
};

/// AxrShaderDynamicUniformBufferLayout Handle Type
typedef AxrShaderDynamicUniformBufferLayout* AxrShaderDynamicUniformBufferLayout_T;
/// Const AxrShaderDynamicUniformBufferLayout Handle Type
typedef const AxrShaderDynamicUniformBufferLayout* AxrShaderDynamicUniformBufferLayoutConst_T;

/// Shader Image Sampler Buffer Layout
struct AxrShaderImageSamplerBufferLayout {
    const AxrShaderBufferLayoutEnum Type = AXR_SHADER_BUFFER_LAYOUT_IMAGE_SAMPLER_BUFFER;
    uint32_t Binding;
};

/// AxrShaderImageSamplerBufferLayout Handle Type
typedef AxrShaderImageSamplerBufferLayout* AxrShaderImageSamplerBufferLayout_T;
/// Const AxrShaderImageSamplerBufferLayout Handle Type
typedef const AxrShaderImageSamplerBufferLayout* AxrShaderImageSamplerBufferLayoutConst_T;

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
/// Shader Push Constant Buffer Layout
struct AxrShaderPushConstantBufferLayout {
    const AxrShaderBufferLayoutEnum Type = AXR_SHADER_BUFFER_LAYOUT_PUSH_CONSTANT_BUFFER;
    uint32_t BufferSize;
};

/// AxrShaderPushConstantBufferLayout Handle Type
typedef AxrShaderPushConstantBufferLayout* AxrShaderPushConstantBufferLayout_T;
/// Const AxrShaderPushConstantBufferLayout Handle Type
typedef const AxrShaderPushConstantBufferLayout* AxrShaderPushConstantBufferLayoutConst_T;
#endif

// ---- Shader Vertex Attribute ----

/// Shader Vertex Attribute
struct AxrShaderVertexAttribute {
    AxrShaderVertexAttributeEnum Type;
    uint32_t Location;
};

// ---- Shader Properties ----

/// Shader Properties Base Structure
struct AxrShaderPropertiesStructure {
    AxrShaderStageEnum Type = AXR_SHADER_STAGE_UNDEFINED;
};

/// AxrShaderPropertiesStructure Handle Type
typedef AxrShaderPropertiesStructure* AxrShaderProperties_T;
/// Const AxrShaderPropertiesStructure Handle Type
typedef const AxrShaderPropertiesStructure* AxrShaderPropertiesConst_T;

/// Vertex shader properties
struct AxrVertexShaderProperties {
    const AxrShaderStageEnum Type = AXR_SHADER_STAGE_VERTEX;
    uint32_t VertexAttributeCount;
    AxrShaderVertexAttribute* VertexAttributes;
    uint32_t BufferLayoutCount;
    AxrShaderBufferLayout_T* BufferLayouts;
};

/// AxrVertexShaderProperties Handle Type
typedef AxrVertexShaderProperties* AxrVertexShaderProperties_T;
/// const AxrVertexShaderProperties Handle Type
typedef const AxrVertexShaderProperties* AxrVertexShaderPropertiesConst_T;

/// Fragment shader properties
struct AxrFragmentShaderProperties {
    const AxrShaderStageEnum Type = AXR_SHADER_STAGE_FRAGMENT;
    uint32_t BufferLayoutCount;
    AxrShaderBufferLayout_T* BufferLayouts;
};

/// AxrFragmentShaderProperties Handle Type
typedef AxrFragmentShaderProperties* AxrFragmentShaderProperties_T;
/// Const AxrFragmentShaderProperties Handle Type
typedef const AxrFragmentShaderProperties* AxrFragmentShaderPropertiesConst_T;

// ----------------------------------------- //
// External Function Definitions
// ----------------------------------------- //
extern "C" {
    // ---- Shader Buffer Layouts ----

    /// Clone the given shader buffer layout
    /// @param bufferLayout Shader buffer layout to clone
    /// @returns The cloned shader buffer layout
    AXR_API AxrShaderBufferLayout_T axrShaderBufferLayoutClone(AxrShaderBufferLayoutConst_T bufferLayout);
    /// Destroy the given shader buffer layout
    /// @param bufferLayout Shader buffer layout to destroy
    AXR_API void axrShaderBufferLayoutDestroy(AxrShaderBufferLayout_T* bufferLayout);

    /// Clone the given shader uniform buffer layout
    /// @param bufferLayout Shader buffer layout to clone
    /// @returns The cloned shader buffer layout
    AXR_API AxrShaderUniformBufferLayout_T axrShaderUniformBufferLayoutClone(
        AxrShaderUniformBufferLayoutConst_T bufferLayout
    );
    /// Destroy the given shader uniform buffer layout
    /// @param bufferLayout Shader buffer layout to destroy
    AXR_API void axrShaderUniformBufferLayoutDestroy(AxrShaderUniformBufferLayout_T* bufferLayout);

    /// Clone the given shader dynamic uniform buffer layout
    /// @param bufferLayout Shader buffer layout to clone
    /// @returns The cloned shader buffer layout
    AXR_API AxrShaderDynamicUniformBufferLayout_T axrShaderDynamicUniformBufferLayoutClone(
        AxrShaderDynamicUniformBufferLayoutConst_T bufferLayout
    );
    /// Destroy the given shader dynamic uniform buffer layout
    /// @param bufferLayout Shader buffer layout to destroy
    AXR_API void axrShaderDynamicUniformBufferLayoutDestroy(AxrShaderDynamicUniformBufferLayout_T* bufferLayout);

    /// Clone the given shader image sampler buffer layout
    /// @param bufferLayout Shader buffer layout to clone
    /// @returns The cloned shader buffer layout
    AXR_API AxrShaderImageSamplerBufferLayout_T axrShaderImageSamplerBufferLayoutClone(
        AxrShaderImageSamplerBufferLayoutConst_T bufferLayout
    );
    /// Destroy the given shader image sampler buffer layout
    /// @param bufferLayout Shader buffer layout to destroy
    AXR_API void axrShaderImageSamplerBufferLayoutDestroy(AxrShaderImageSamplerBufferLayout_T* bufferLayout);

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
    /// Clone the given shader push constant buffer layout
    /// @param bufferLayout Shader buffer layout to clone
    /// @returns The cloned shader buffer layout
    AXR_API AxrShaderPushConstantBufferLayout_T axrShaderPushConstantBufferLayoutClone(
        AxrShaderPushConstantBufferLayoutConst_T bufferLayout
    );
    /// Destroy the given shader push constant buffer layout
    /// @param bufferLayout Shader buffer layout to destroy
    AXR_API void axrShaderPushConstantBufferLayoutDestroy(AxrShaderPushConstantBufferLayout_T* bufferLayout);
#endif

    // ---- Shader Vertex Attributes ----

    /// Clone the given shader vertex attribute
    /// @param vertexAttribute Shader vertex attribute to clone
    /// @returns The cloned shader vertex attribute
    AXR_API AxrShaderVertexAttribute axrShaderVertexAttributeClone(AxrShaderVertexAttribute vertexAttribute);
    /// Destroy the given shader vertex attribute
    /// @param vertexAttribute Shader vertex attribute to destroy
    AXR_API void axrShaderVertexAttributeDestroy(AxrShaderVertexAttribute* vertexAttribute);

    // ---- Shader Properties ----

    /// Check if the given shader properties are valid
    /// @param properties Properties to check
    /// @returns True if the given shader properties are valid
    AXR_API bool axrShaderPropertiesIsValid(AxrShaderPropertiesConst_T properties);
    /// Clone the given shader properties
    /// @param properties Shader properties to clone
    /// @returns The cloned shader properties
    AXR_API AxrShaderProperties_T axrShaderPropertiesClone(AxrShaderPropertiesConst_T properties);
    /// Destroy the given shader properties
    /// @param properties Shader properties to destroy
    AXR_API void axrShaderPropertiesDestroy(AxrShaderProperties_T* properties);

    /// Check if the given vertex shader properties are valid
    /// @param properties Properties to check
    /// @returns True if the given shader properties are valid
    AXR_API bool axrVertexShaderPropertiesIsValid(AxrVertexShaderPropertiesConst_T properties);
    /// Clone the given vertex shader properties
    /// @param properties Shader properties to clone
    /// @returns The cloned shader properties
    AXR_API AxrVertexShaderProperties_T axrVertexShaderPropertiesClone(
        AxrVertexShaderPropertiesConst_T properties
    );
    /// Destroy the given vertex shader properties
    /// @param properties Shader properties to destroy
    AXR_API void axrVertexShaderPropertiesDestroy(AxrVertexShaderProperties_T* properties);

    /// Check if the given fragment shader properties are valid
    /// @param properties Properties to check
    /// @returns True if the given shader properties are valid
    AXR_API bool axrFragmentShaderPropertiesIsValid(AxrFragmentShaderPropertiesConst_T properties);
    /// Clone the given fragment shader properties
    /// @param properties Shader properties to clone
    /// @returns The cloned shader properties
    AXR_API AxrFragmentShaderProperties_T axrFragmentShaderPropertiesClone(
        AxrFragmentShaderPropertiesConst_T properties
    );
    /// Destroy the given fragment shader properties
    /// @param properties Shader properties to destroy
    AXR_API void axrFragmentShaderPropertiesDestroy(AxrFragmentShaderProperties_T* properties);
}

// ---------------------------------------------------------------------------------- //
//                                   Shader Assets                                    //
// ---------------------------------------------------------------------------------- //

// ----------------------------------------- //
// Structs
// ----------------------------------------- //

/// Shader Config
struct AxrShaderConfig {
    char Name[AXR_MAX_ASSET_NAME_SIZE];
    char FilePath[AXR_MAX_FILE_PATH_SIZE];
    AxrShaderProperties_T Properties;
};

// ----------------------------------------- //
// Forward Declared Handles
// ----------------------------------------- //

/// AxrShader Handle
typedef class AxrShader* AxrShader_T;
/// const AxrShader Handle
typedef const AxrShader* AxrShaderConst_T;

// ----------------------------------------- //
// External Function Definitions
// ----------------------------------------- //
extern "C" {
    /// Check if the given shader config is valid
    /// @param shaderConfig Shader config to check
    /// @returns True if the given shader config is valid
    AXR_API bool axrShaderConfigIsValid(const AxrShaderConfig* shaderConfig);
    /// Get the shader's name
    /// @param shader Shader to use
    /// @returns The shader's name
    AXR_API const char* axrShaderGetName(AxrShaderConst_T shader);
}

// ---------------------------------------------------------------------------------- //
//                                   Shader Values                                    //
// ---------------------------------------------------------------------------------- //

// ----------------------------------------- //
// Enums
// ----------------------------------------- //

/// Shader buffer link type enum
enum AxrShaderBufferLinkEnum {
    AXR_SHADER_BUFFER_LINK_UNDEFINED = 0,
    AXR_SHADER_BUFFER_LINK_UNIFORM_BUFFER,
    AXR_SHADER_BUFFER_LINK_IMAGE_SAMPLER_BUFFER,
};

// ----------------------------------------- //
// Structs
// ----------------------------------------- //

/// Shader Buffer Link Base Structure
struct AxrShaderBufferLinkStructure {
    const AxrShaderBufferLinkEnum Type = AXR_SHADER_BUFFER_LINK_UNDEFINED;
};

/// AxrShaderBufferLinkStructure Handle Type
typedef AxrShaderBufferLinkStructure* AxrShaderBufferLink_T;
/// Const AxrShaderBufferLinkStructure Handle Type
typedef const AxrShaderBufferLinkStructure* AxrShaderBufferLinkConst_T;

/// Shader Uniform Buffer Link
struct AxrShaderUniformBufferLink {
    const AxrShaderBufferLinkEnum Type = AXR_SHADER_BUFFER_LINK_UNIFORM_BUFFER;
    uint32_t Binding;
    char BufferName[AXR_MAX_ASSET_NAME_SIZE];
};

/// AxrShaderUniformBufferLink Handle Type
typedef AxrShaderUniformBufferLink* AxrShaderUniformBufferLink_T;
/// Const AxrShaderUniformBufferLink Handle Type
typedef const AxrShaderUniformBufferLink* AxrShaderUniformBufferLinkConst_T;

/// Shader Image Sampler Buffer Link
struct AxrShaderImageSamplerBufferLink {
    const AxrShaderBufferLinkEnum Type = AXR_SHADER_BUFFER_LINK_IMAGE_SAMPLER_BUFFER;
    uint32_t Binding;
    char ImageName[AXR_MAX_ASSET_NAME_SIZE];
    char ImageSamplerName[AXR_MAX_ASSET_NAME_SIZE];
};

/// AxrShaderImageSamplerBufferLink Handle Type
typedef AxrShaderImageSamplerBufferLink* AxrShaderImageSamplerBufferLink_T;
/// Const AxrShaderImageSamplerBufferLink Handle Type
typedef const AxrShaderImageSamplerBufferLink* AxrShaderImageSamplerBufferLinkConst_T;

/// Shader Values
struct AxrShaderValues {
    uint32_t BufferLinkCount;
    AxrShaderBufferLink_T* BufferLinks;
};

/// AxrShaderValues Handle Type
typedef AxrShaderValues* AxrShaderValues_T;
/// Const AxrShaderValues Handle Type
typedef const AxrShaderValues* AxrShaderValuesConst_T;

// ----------------------------------------- //
// External Function Definitions
// ----------------------------------------- //
extern "C" {
    // ---- Shader Buffer Links ----

    /// Clone the given shader buffer link
    /// @param bufferLink Shader buffer link to clone
    /// @returns The cloned shader buffer link
    AXR_API AxrShaderBufferLink_T axrShaderBufferLinkClone(AxrShaderBufferLinkConst_T bufferLink);
    /// Destroy the given shader buffer link
    /// @param bufferLink Shader buffer link to destroy
    AXR_API void axrShaderBufferLinkDestroy(AxrShaderBufferLink_T* bufferLink);

    /// Clone the given shader uniform buffer link
    /// @param bufferLink Shader buffer link to clone
    /// @returns The cloned shader buffer link
    AXR_API AxrShaderUniformBufferLink_T axrShaderUniformBufferLinkClone(
        AxrShaderUniformBufferLinkConst_T bufferLink
    );
    /// Destroy the given shader uniform buffer link
    /// @param bufferLink Shader buffer link to destroy
    AXR_API void axrShaderUniformBufferLinkDestroy(AxrShaderUniformBufferLink_T* bufferLink);

    /// Clone the given shader image sampler buffer link
    /// @param bufferLink Shader buffer link to clone
    /// @returns The cloned shader buffer link
    AXR_API AxrShaderImageSamplerBufferLink_T axrShaderImageSamplerBufferLinkClone(
        AxrShaderImageSamplerBufferLinkConst_T bufferLink
    );
    /// Destroy the given shader image sampler buffer link
    /// @param bufferLink Shader buffer link to destroy
    AXR_API void axrShaderImageSamplerBufferLinkDestroy(AxrShaderImageSamplerBufferLink_T* bufferLink);

    // ---- Shader Values ----

    /// Check if the given shader values are valid
    /// @param values Shader values to check
    /// @returns True if the given shader values are valid
    AXR_API bool axrShaderValuesIsValid(AxrShaderValuesConst_T values);
    /// Clone the given shader values
    /// @param values Shader values to clone
    /// @returns The cloned shader values
    AXR_API AxrShaderValues_T axrShaderValuesClone(AxrShaderValuesConst_T values);
    /// Destroy the given shader values
    /// @param values Shader values to destroy
    AXR_API void axrShaderValuesDestroy(AxrShaderValues_T* values);
}

// ---------------------------------------------------------------------------------- //
//                                   Model Assets                                     //
// ---------------------------------------------------------------------------------- //

// ----------------------------------------- //
// Structs
// ----------------------------------------- //

/// Vertex
struct AxrVertex {
    glm::vec3 Position;
    glm::vec3 Color;
    // NOTE: We could go up to 8 TexCoords but 4 is enough for now
    glm::vec2 TexCoord_0;
    glm::vec2 TexCoord_1;
    glm::vec2 TexCoord_2;
    glm::vec2 TexCoord_3;
};

/// Submesh
struct AxrSubmesh {
    uint32_t VertexCount;
    AxrVertex* Vertices;
    uint32_t IndexCount;
    uint32_t* Indices;
};

/// Mesh
struct AxrMesh {
    uint32_t SubmeshCount;
    AxrSubmesh* Submeshes;
};

/// Model Config
struct AxrModelConfig {
    char Name[AXR_MAX_ASSET_NAME_SIZE];
    char FilePath[AXR_MAX_FILE_PATH_SIZE];
};

// ----------------------------------------- //
// Forward Declared Handles
// ----------------------------------------- //

/// AxrModel Handle
typedef class AxrModel* AxrModel_T;
/// const  AxrModel Handle
typedef const AxrModel* AxrModelConst_T;

// ----------------------------------------- //
// External Function Definitions
// ----------------------------------------- //
extern "C" {
    /// Clone the given vertices
    /// @param vertexCount Number of vertices in the given array
    /// @param vertices Vertex array to clone
    /// @returns A cloned array of the given vertices
    AXR_API AxrVertex* axrSubmeshCloneVertices(uint32_t vertexCount, const AxrVertex* vertices);
    /// Destroy the given vertices
    /// @param vertexCount Number of vertices in the given array
    /// @param vertices Vertex array to destroy
    AXR_API void axrSubmeshDestroyVertices(uint32_t* vertexCount, AxrVertex** vertices);

    /// Clone the given indices
    /// @param indexCount Number of indices in the given array
    /// @param indices Index array to clone
    /// @returns A cloned array of the given indices
    AXR_API uint32_t* axrSubmeshCloneIndices(uint32_t indexCount, const uint32_t* indices);
    /// Destroy the given indices
    /// @param indexCount Number of indices in the given array
    /// @param indices Index array to destroy
    AXR_API void axrSubmeshDestroyIndices(uint32_t* indexCount, uint32_t** indices);

    /// Clone the given submeshes
    /// @param submeshCount Number of submeshes in the given array
    /// @param submeshes Submesh array to clone
    /// @returns A cloned array of the given submeshes
    AXR_API AxrSubmesh* axrMeshCloneSubmeshes(uint32_t submeshCount, const AxrSubmesh* submeshes);
    /// Destroy the given submeshes
    /// @param submeshCount Number of submeshes in the given array
    /// @param submeshes Submesh array to destroy
    AXR_API void axrMeshDestroySubmeshes(uint32_t* submeshCount, AxrSubmesh** submeshes);

    /// Clone the given meshes
    /// @param meshCount Number of meshes in the given array
    /// @param meshes Mesh array to clone
    /// @returns A cloned array of the given meshes
    AXR_API AxrMesh* axrModelCloneMeshes(uint32_t meshCount, const AxrMesh* meshes);
    /// Destroy the given meshes
    /// @param meshCount Number of meshes in the given array
    /// @param meshes Mesh array to destroy
    AXR_API void axrModelDestroyMeshes(uint32_t* meshCount, AxrMesh** meshes);

    /// Get the model's name
    /// @param model Model to use
    /// @returns The model's name
    AXR_API const char* axrModelGetName(AxrModelConst_T model);
    /// Set the mesh data for the model
    /// @param model Model to use
    /// @param meshCount Number of meshes in the array
    /// @param meshes Meshes array
    /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrModelSetData(AxrModel_T model, uint32_t meshCount, const AxrMesh* meshes);
}

// ---------------------------------------------------------------------------------- //
//                                   Buffer Assets                                    //
// ---------------------------------------------------------------------------------- //

// ----------------------------------------- //
// Enums
// ----------------------------------------- //

/// Uniform buffer type enum
enum AxrUniformBufferTypeEnum {
    AXR_UNIFORM_BUFFER_TYPE_UNDEFINED = 0,
    AXR_UNIFORM_BUFFER_TYPE_STANDARD,
    AXR_UNIFORM_BUFFER_TYPE_DYNAMIC,
};

// ----------------------------------------- //
// Structs
// ----------------------------------------- //

/// Dynamic uniform buffer offset config
// TODO: This should probably be per mesh instead of per material right?
struct AxrDynamicUniformBufferOffsetConfig {
    uint32_t Binding;
    uint32_t OffsetIndex;
};

/// Uniform Buffer Config
struct AxrUniformBufferConfig {
    char Name[AXR_MAX_ASSET_NAME_SIZE];
    uint64_t DataSize;
    void* Data;
};

/// Dynamic Uniform Buffer Config
struct AxrDynamicUniformBufferConfig {
    char Name[AXR_MAX_ASSET_NAME_SIZE];
    uint32_t InstanceCount;
    uint64_t InstanceSize;
    void* Data;
};

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
/// Push Constant Buffer Config
struct AxrPushConstantBufferConfig {
    char Name[AXR_MAX_ASSET_NAME_SIZE];
    uint32_t DataSize;
    void* Data;
};
#endif

// ----------------------------------------- //
// Forward Declared Handles
// ----------------------------------------- //

/// AxrUniformBuffer Handle
typedef class AxrUniformBuffer* AxrUniformBuffer_T;
/// const AxrUniformBuffer Handle
typedef const AxrUniformBuffer* AxrUniformBufferConst_T;

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
/// AxrPushConstantBuffer Handle
typedef class AxrPushConstantBuffer* AxrPushConstantBuffer_T;
/// const AxrPushConstantBuffer Handle
typedef const AxrPushConstantBuffer* AxrPushConstantBufferConst_T;
#endif

// ----------------------------------------- //
// External Function Definitions
// ----------------------------------------- //
extern "C" {
    /// Clone the given uniform buffer data
    /// @param size Buffer size
    /// @param data Buffer data
    /// @returns The cloned uniform buffer data
    AXR_API void* axrUniformBufferCloneData(uint64_t size, const void* data);
    /// Destroy the given uniform buffer data
    /// @param size Buffer size
    /// @param data Buffer data
    AXR_API void axrUniformBufferDestroyData(uint64_t* size, void** data);

    /// Get the uniform buffer's name
    /// @param uniformBuffer Uniform buffer to use
    /// @returns The uniform buffer's name
    AXR_API const char* axrUniformBufferGetName(AxrUniformBufferConst_T uniformBuffer);

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
    /// Clone the given push constant buffer data
    /// @param size Buffer size
    /// @param data Buffer data
    /// @returns The cloned push constant buffer data
    AXR_API void* axrPushConstantBufferCloneData(uint32_t size, const void* data);
    /// Destroy the given push constant buffer data
    /// @param size Buffer size
    /// @param data Buffer data
    AXR_API void axrPushConstantBufferDestroyData(uint32_t* size, void** data);

    /// Get the push constant buffer's name
    /// @param pushConstantBuffer Push constant buffer to use
    /// @returns The push constant buffer's name
    AXR_API const char* axrPushConstantBufferGetName(AxrPushConstantBufferConst_T pushConstantBuffer);
#endif
}

// ---------------------------------------------------------------------------------- //
//                               Image Sampler Assets                                 //
// ---------------------------------------------------------------------------------- //

// ----------------------------------------- //
// Enums
// ----------------------------------------- //

/// Image sampler filter enum
enum AxrImageSamplerFilterEnum {
    AXR_IMAGE_SAMPLER_FILTER_UNDEFINED = 0,
    AXR_IMAGE_SAMPLER_FILTER_NEAREST,
    AXR_IMAGE_SAMPLER_FILTER_LINEAR,
};

/// Image sampler wrap enum
enum AxrImageSamplerWrapEnum {
    AXR_IMAGE_SAMPLER_WRAP_UNDEFINED = 0,
    AXR_IMAGE_SAMPLER_WRAP_REPEAT,
    AXR_IMAGE_SAMPLER_WRAP_MIRRORED_REPEAT,
    AXR_IMAGE_SAMPLER_WRAP_CLAMP_TO_EDGE,
    AXR_IMAGE_SAMPLER_WRAP_CLAMP_TO_BORDER,
};

// ----------------------------------------- //
// Structs
// ----------------------------------------- //

/// Image Sampler Config
struct AxrImageSamplerConfig {
    char Name[AXR_MAX_ASSET_NAME_SIZE];
    AxrImageSamplerFilterEnum MinFilter;
    AxrImageSamplerFilterEnum MagFilter;
    AxrImageSamplerFilterEnum MipmapFilter;
    AxrImageSamplerWrapEnum WrapU;
    AxrImageSamplerWrapEnum WrapV;
};

// ----------------------------------------- //
// Forward Declared Handles
// ----------------------------------------- //

/// AxrImageSampler Handle
typedef class AxrImageSampler* AxrImageSampler_T;
/// const AxrImageSampler Handle
typedef const AxrImageSampler* AxrImageSamplerConst_T;

// ----------------------------------------- //
// External Function Definitions
// ----------------------------------------- //
extern "C" {
    /// Get the image sampler's name
    /// @param imageSampler ImageSampler to use
    /// @returns The imageSampler's name
    AXR_API const char* axrImageSamplerGetName(AxrImageSamplerConst_T imageSampler);
}

// ---------------------------------------------------------------------------------- //
//                                   Image Assets                                     //
// ---------------------------------------------------------------------------------- //

// ----------------------------------------- //
// Enums
// ----------------------------------------- //

/// Image color channels enum
enum AxrImageColorChannelsEnum {
    AXR_IMAGE_COLOR_CHANNELS_UNDEFINED = 0,
    AXR_IMAGE_COLOR_CHANNELS_GRAY = 1,
    AXR_IMAGE_COLOR_CHANNELS_GRAY_ALPHA = 2,
    AXR_IMAGE_COLOR_CHANNELS_RGB = 3,
    AXR_IMAGE_COLOR_CHANNELS_RGB_ALPHA = 4,
};

// ----------------------------------------- //
// Structs
// ----------------------------------------- //

/// Image Config
struct AxrImageConfig {
    char Name[AXR_MAX_ASSET_NAME_SIZE];
    char FilePath[AXR_MAX_FILE_PATH_SIZE];
};

/// UI Image Data
struct AxrUIImageData {
    char ImageName[AXR_MAX_ASSET_NAME_SIZE];
    char ImageSamplerName[AXR_MAX_ASSET_NAME_SIZE];
};

// ----------------------------------------- //
// Forward Declared Handles
// ----------------------------------------- //

/// AxrImage Handle
typedef class AxrImage* AxrImage_T;
// const AxrImage Handle
typedef const AxrImage* AxrImageConst_T;

// ----------------------------------------- //
// External Function Definitions
// ----------------------------------------- //
extern "C" {
    /// Get the image's name
    /// @param image Image to use
    /// @returns The image's name
    AXR_API const char* axrImageGetName(AxrImageConst_T image);

    /// Set the image data
    /// @param image Image to use
    /// @param width Image width
    /// @param height Image height
    /// @param colorChannels Image number of color channels
    /// @param data Image data. Stored from left-to-right, top-to-bottom. Each pixel contains a value for each 'colorChannel', stored with 8-bits
    /// per channel, in the following order: 1=Y, 2=YA, 3=RGB, 4=RGBA. (Y is monochrome color.)
    /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrImageSetData(
        AxrImage_T image,
        uint32_t width,
        uint32_t height,
        AxrImageColorChannelsEnum colorChannels,
        const stbi_uc* data
    );
}

// ---------------------------------------------------------------------------------- //
//                                  Material Assets                                   //
// ---------------------------------------------------------------------------------- //

// ----------------------------------------- //
// Enums
// ----------------------------------------- //

/// Material backface culling mode enum
enum AxrMaterialBackfaceCullModeEnum {
    AXR_MATERIAL_BACKFACE_CULL_MODE_NONE = 0,
    AXR_MATERIAL_BACKFACE_CULL_MODE_FRONT,
    AXR_MATERIAL_BACKFACE_CULL_MODE_BACK,
    AXR_MATERIAL_BACKFACE_CULL_MODE_FRONT_AND_BACK,
};

/// Material alpha rendering mode enum
enum AxrMaterialAlphaRenderModeEnum {
    AXR_MATERIAL_ALPHA_RENDER_MODE_OPAQUE = 0,
    /// Depth sorted alpha blending transparency.
    /// Useful for glass windows or objects with minimal or no overlapping transparency.
    AXR_MATERIAL_ALPHA_RENDER_MODE_ALPHA_BLEND,
    /// Order independent transparency.
    /// Useful when there are multiple layers of transparency overlapping.
    /// Whether it's multiple objects or a single complex object. 
    AXR_MATERIAL_ALPHA_RENDER_MODE_OIT,
};

// ----------------------------------------- //
// Structs
// ----------------------------------------- //

/// Material Config
struct AxrMaterialConfig {
    char Name[AXR_MAX_ASSET_NAME_SIZE];
    char VertexShaderName[AXR_MAX_ASSET_NAME_SIZE];
    char FragmentShaderName[AXR_MAX_ASSET_NAME_SIZE];
    AxrShaderValues_T VertexShaderValues;
    AxrShaderValues_T FragmentShaderValues;
    AxrMaterialBackfaceCullModeEnum BackfaceCullMode;
    AxrMaterialAlphaRenderModeEnum AlphaRenderMode;
    bool EnableDepthTest;
    bool EnableDepthWrite;
    uint32_t DynamicUniformBufferOffsetCount;
    AxrDynamicUniformBufferOffsetConfig* DynamicUniformBufferOffsets;
};

// ----------------------------------------- //
// Forward Declared Handles
// ----------------------------------------- //

/// AxrMaterial Handle
typedef class AxrMaterial* AxrMaterial_T;
/// const AxrMaterial Handle
typedef const AxrMaterial* AxrMaterialConst_T;

// ----------------------------------------- //
// External Function Definitions
// ----------------------------------------- //
extern "C" {
    /// Check if the given material config is valid
    /// @param materialConfig Material config to check
    /// @returns True if the given material config is valid
    AXR_API bool axrMaterialConfigIsValid(const AxrMaterialConfig* materialConfig);
    /// Get the material's name
    /// @param material Material to use
    /// @returns The material's name
    AXR_API const char* axrMaterialGetName(AxrMaterialConst_T material);
}

// ---------------------------------------------------------------------------------- //
//                               Engine Defined Assets                                //
// ---------------------------------------------------------------------------------- //

// ----------------------------------------- //
// Enums
// ----------------------------------------- //

/// Axr engine defined assets enum
enum AxrEngineAssetEnum {
    AXR_ENGINE_ASSET_UNDEFINED = 0,

    // ---- Shaders - Max of 64 ----
    AXR_ENGINE_ASSET_SHADER_START = 1,
    AXR_ENGINE_ASSET_SHADER_DEFAULT_VERT = 1,
    AXR_ENGINE_ASSET_SHADER_DEFAULT_FRAG = 2,
    AXR_ENGINE_ASSET_SHADER_DEFAULT_FRAG_MASK = 3,
    AXR_ENGINE_ASSET_SHADER_UI_ELEMENT_VERT = 4,
    AXR_ENGINE_ASSET_SHADER_UI_RECTANGLE_FRAG = 5,
    AXR_ENGINE_ASSET_SHADER_UI_BORDER_FRAG = 6,
    AXR_ENGINE_ASSET_SHADER_UI_IMAGE_FRAG = 7,
    AXR_ENGINE_ASSET_SHADER_END = 64,

    // ---- Uniform Buffers - Max of 32 ----
    AXR_ENGINE_ASSET_UNIFORM_BUFFER_START = 65,
    AXR_ENGINE_ASSET_UNIFORM_BUFFER_SCENE_DATA = 65,
    AXR_ENGINE_ASSET_UNIFORM_BUFFER_CAMERA_DATA = 66,
    // TODO: When implementing world space UI, make sure we make AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_CANVAS constructable in the README.md
    //  Since we will have to create a uniform buffer for each canvas in the world.
    AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_CANVAS = 67,
    AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_ELEMENTS = 68,
    AXR_ENGINE_ASSET_UNIFORM_BUFFER_END = 96,

    // ---- Push Constant Buffers - Max of 32 ----
    AXR_ENGINE_ASSET_PUSH_CONSTANT_BUFFER_START = 97,
    AXR_ENGINE_ASSET_PUSH_CONSTANT_BUFFER_MODEL_MATRIX = 97,
    AXR_ENGINE_ASSET_PUSH_CONSTANT_BUFFER_MVP_MATRIX = 98,
    AXR_ENGINE_ASSET_PUSH_CONSTANT_BUFFER_END = 128,

    // ---- Images - Max of 64 ----
    AXR_ENGINE_ASSET_IMAGE_START = 129,
    AXR_ENGINE_ASSET_IMAGE_MISSING_TEXTURE = 129,
    AXR_ENGINE_ASSET_IMAGE_UV_TESTER = 130,
    AXR_ENGINE_ASSET_IMAGE_END = 192,

    // ---- Image Samplers - Max of 8 ----
    AXR_ENGINE_ASSET_IMAGE_SAMPLER_START = 193,
    AXR_ENGINE_ASSET_IMAGE_SAMPLER_NEAREST_REPEAT = 193,
    AXR_ENGINE_ASSET_IMAGE_SAMPLER_LINEAR_REPEAT = 194,
    AXR_ENGINE_ASSET_IMAGE_SAMPLER_END = 200,

    // ---- Materials - Max of 64 ----
    AXR_ENGINE_ASSET_MATERIAL_START = 201,
    AXR_ENGINE_ASSET_MATERIAL_UI_RECTANGLE = 201,
    AXR_ENGINE_ASSET_MATERIAL_UI_BORDER = 202,
    AXR_ENGINE_ASSET_MATERIAL_UI_IMAGE = 203,
    AXR_ENGINE_ASSET_MATERIAL_END = 264,

    // ---- Models - Max of 64 ----
    AXR_ENGINE_ASSET_MODEL_START = 265,
    AXR_ENGINE_ASSET_MODEL_TRIANGLE = 265,
    AXR_ENGINE_ASSET_MODEL_SQUARE = 266,
    AXR_ENGINE_ASSET_MODEL_CUBE = 267,
    AXR_ENGINE_ASSET_MODEL_UI_RECTANGLE = 268,
    AXR_ENGINE_ASSET_MODEL_END = 328,
};

// ----------------------------------------- //
// Structs
// ----------------------------------------- //

// ---- Buffers ----

/// Engine asset uniform buffer named 'Scene Data' structure
struct alignas(16) AxrEngineAssetUniformBuffer_SceneData {
    alignas(16) glm::mat4 ViewMatrix;
    alignas(16) glm::mat4 ProjectionMatrix;
    alignas(16) glm::mat4 ViewProjectionMatrix;
};

/// Engine asset uniform buffer named 'Camera Data' structure
struct alignas(16) AxrEngineAssetUniformBuffer_CameraData {
    alignas(8) glm::vec2 Dimensions;
    alignas(4) float ZNear;
    alignas(4) float ZFar;
};

/// Engine asset uniform buffer named 'UI Canvas' structure
struct alignas(16) AxrEngineAssetUniformBuffer_UICanvas {
    alignas(8) glm::vec2 Size;
    float _padding[2];
};

/// Engine asset uniform buffer named 'UI Rectangle' structure
struct alignas(16) AxrEngineAssetUniformBuffer_UIRectangle {
    // Every element MUST start with `position` and `size` since they're used in the vertex shader that all ui elements use
    alignas(8) glm::vec2 Position;
    alignas(8) glm::vec2 Size;
    alignas(16) glm::vec4 BackgroundColor;
    alignas(16) AxrUIRoundedCorners Corners;
};

/// Engine asset uniform buffer named 'UI Border' structure
struct alignas(16) AxrEngineAssetUniformBuffer_UIBorder {
    // Every element MUST start with `position` and `size` since they're used in the vertex shader that all ui elements use
    alignas(8) glm::vec2 Position;
    alignas(8) glm::vec2 Size;
    alignas(16) glm::vec4 Color;
    alignas(16) AxrUIRoundedCorners Corners;
    alignas(16) AxrUIBorders Borders;
};

/// Engine asset uniform buffer named 'UI Image' structure
struct alignas(16) AxrEngineAssetUniformBuffer_UIImage {
    // Every element MUST start with `position` and `size` since they're used in the vertex shader that all ui elements use
    alignas(8) glm::vec2 Position;
    alignas(8) glm::vec2 Size;
    alignas(16) glm::vec4 BackgroundColor;
    alignas(16) AxrUIRoundedCorners Corners;
};

/// Engine asset uniform buffer for a general UI element.
union AxrEngineAssetUniformBuffer_UIElement {
    AxrEngineAssetUniformBuffer_UIRectangle Rectangle;
    AxrEngineAssetUniformBuffer_UIImage Image;
    AxrEngineAssetUniformBuffer_UIBorder Border;
};

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
/// Engine asset push constant buffer named 'Model Matrix' structure
struct alignas(16) AxrEngineAssetPushConstantBuffer_ModelMatrix {
    alignas(16) glm::mat4 ModelMatrix;
};

/// Engine asset push constant buffer named 'Mvp Matrix' structure
struct alignas(16) AxrEngineAssetPushConstantBuffer_MvpMatrix {
    alignas(16) glm::mat4 MvpMatrix;
};
#endif

// ---- Materials ----

/// Engine asset material named 'Default Material' values
struct AxrEngineAssetMaterial_DefaultMaterial {
    AxrMaterialBackfaceCullModeEnum BackfaceCullMode;
    AxrMaterialAlphaRenderModeEnum AlphaRenderMode;
    /// Alpha cutoff can be an empty string
    char AlphaCutoffBufferName[AXR_MAX_ASSET_NAME_SIZE];
    char ImageName[AXR_MAX_ASSET_NAME_SIZE];
    char ImageSamplerName[AXR_MAX_ASSET_NAME_SIZE];
};

// ----------------------------------------- //
// External Function Definitions
// ----------------------------------------- //
extern "C" {
    /// Check if the given name is reserved for an engine asset
    /// @param name Name to check
    /// @returns True if the given name is reserved for an engine asset
    AXR_API bool axrEngineAssetIsNameReserved(const char* name);
    /// Get the name for the given engine asset
    /// @param engineAssetEnum Engine asset to get the name of
    /// @returns The name of the given engine asset
    AXR_API const char* axrEngineAssetGetName(AxrEngineAssetEnum engineAssetEnum);

    // ---- Buffers ----

    /// Get the size for the given uniform buffer engine asset
    /// @param engineAssetEnum Engine asset to use
    /// @returns The size for the given uniform buffer engine asset
    AXR_API uint64_t axrEngineAssetGetUniformBufferSize(AxrEngineAssetEnum engineAssetEnum);
    /// Get the instance size for the given uniform buffer engine asset
    /// @param engineAssetEnum Engine asset to use
    /// @returns The instance size for the given uniform buffer engine asset
    AXR_API uint64_t axrEngineAssetGetUniformBufferInstanceSize(AxrEngineAssetEnum engineAssetEnum);
    /// Get the buffer type for the given uniform buffer engine asset
    /// @param engineAssetEnum Engine asset to use
    /// @returns The buffer type for the given uniform buffer engine asset
    AXR_API AxrUniformBufferTypeEnum axrEngineAssetGetUniformBufferType(AxrEngineAssetEnum engineAssetEnum);
#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
    /// Get the size for the given push constant buffer engine asset
    /// @param engineAssetEnum Engine asset to use
    /// @returns The size for the given push constant buffer engine asset
    AXR_API uint32_t axrEngineAssetGetPushConstantBufferSize(AxrEngineAssetEnum engineAssetEnum);
#endif
}

// ---------------------------------------------------------------------------------- //
//                                  Asset Collection                                  //
// ---------------------------------------------------------------------------------- //

// ----------------------------------------- //
// Forward Declared Handles
// ----------------------------------------- //

/// AxrAssetCollection Handle
typedef class AxrAssetCollection* AxrAssetCollection_T;
/// const AxrAssetCollection Handle
typedef const AxrAssetCollection* AxrAssetCollectionConst_T;

// ----------------------------------------- //
// External Function Definitions
// ----------------------------------------- //
extern "C" {
    // ---- Shader ----

    /// Create a new shader
    /// @param assetCollection Asset collection to use
    /// @param shaderConfig Shader config to use
    /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrAssetCollectionCreateShader(
        AxrAssetCollection_T assetCollection,
        const AxrShaderConfig* shaderConfig
    );
    /// Create a new engine asset shader
    /// @param assetCollection Asset collection to use
    /// @param engineAssetEnum Shader engine asset
    /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrAssetCollectionCreateEngineAssetShader(
        AxrAssetCollection_T assetCollection,
        AxrEngineAssetEnum engineAssetEnum
    );

    // ---- Material ----

    /// Create a new material
    /// @param assetCollection Asset collection to use
    /// @param materialConfig Material config
    /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrAssetCollectionCreateMaterial(
        AxrAssetCollection_T assetCollection,
        const AxrMaterialConfig* materialConfig
    );
    /// Create a material using the engine defined material named 'Default Material'
    /// @param assetCollection Asset collection to use
    /// @param materialName Material name
    /// @param materialValues Material values
    /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrAssetCollectionCreateEngineAssetMaterial_DefaultMaterial(
        AxrAssetCollection_T assetCollection,
        const char materialName[AXR_MAX_ASSET_NAME_SIZE],
        const AxrEngineAssetMaterial_DefaultMaterial* materialValues
    );

    // ---- Model ----

    /// Create a new model
    /// @param assetCollection Asset collection to use
    /// @param modelConfig Model config
    /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrAssetCollectionCreateModel(
        AxrAssetCollection_T assetCollection,
        const AxrModelConfig* modelConfig
    );

    /// Create a new engine asset model
    /// @param assetCollection Asset collection to use
    /// @param modelName Model name
    /// @param engineAssetEnum Model engine asset
    /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrAssetCollectionCreateEngineAssetModel(
        AxrAssetCollection_T assetCollection,
        char modelName[AXR_MAX_ASSET_NAME_SIZE],
        AxrEngineAssetEnum engineAssetEnum
    );

    // ---- Uniform Buffer ----

    /// Create a new uniform buffer
    /// @param assetCollection Asset collection to use
    /// @param uniformBufferConfig Uniform buffer config
    /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrAssetCollectionCreateUniformBuffer(
        AxrAssetCollection_T assetCollection,
        const AxrUniformBufferConfig* uniformBufferConfig
    );
    /// Create a new dynamic uniform buffer
    /// @param assetCollection Asset collection to use
    /// @param uniformBufferConfig Dynamic uniform buffer config
    /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrAssetCollectionCreateDynamicUniformBuffer(
        AxrAssetCollection_T assetCollection,
        const AxrDynamicUniformBufferConfig* uniformBufferConfig
    );

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
    // ---- Push Constant Buffer ----

    /// Create a new push constant buffer
    /// @param assetCollection Asset collection to use
    /// @param pushConstantBufferConfig Push constant buffer config
    /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrAssetCollectionCreatePushConstantBuffer(
        AxrAssetCollection_T assetCollection,
        const AxrPushConstantBufferConfig* pushConstantBufferConfig
    );
#endif

    // ---- Image ----

    /// Create a new image
    /// @param assetCollection Asset collection to use
    /// @param imageConfig Image config
    /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrAssetCollectionCreateImage(
        AxrAssetCollection_T assetCollection,
        const AxrImageConfig* imageConfig
    );

    /// Create a new engine asset image
    /// @param assetCollection Asset collection to use
    /// @param imageName Image name
    /// @param engineAssetEnum Image engine asset
    /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrAssetCollectionCreateEngineAssetImage(
        AxrAssetCollection_T assetCollection,
        char imageName[AXR_MAX_ASSET_NAME_SIZE],
        AxrEngineAssetEnum engineAssetEnum
    );

    // ---- Image Sampler ----

    /// Create a new image sampler
    /// @param assetCollection Asset collection to use
    /// @param imageSamplerConfig Image sampler config
    /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrAssetCollectionCreateImageSampler(
        AxrAssetCollection_T assetCollection,
        const AxrImageSamplerConfig* imageSamplerConfig
    );
}

// ---------------------------------------------------------------------------------- //
//                                    Asset Utils                                     //
// ---------------------------------------------------------------------------------- //

// ----------------------------------------- //
// Enums
// ----------------------------------------- //

/// Model file material info alpha mode
enum AxrModelFileMaterialInfoAlphaModeEnum {
    AXR_MODEL_FILE_MATERIAL_INFO_ALPHA_MODE_OPAQUE = 0,
    AXR_MODEL_FILE_MATERIAL_INFO_ALPHA_MODE_BLEND,
    AXR_MODEL_FILE_MATERIAL_INFO_ALPHA_MODE_MASK,
};

// ----------------------------------------- //
// Structs
// ----------------------------------------- //

/// Model file image sampler info
struct AxrModelFileImageSamplerInfo {
    char Name[AXR_MAX_ASSET_NAME_SIZE];
    AxrImageSamplerFilterEnum MinFilter;
    AxrImageSamplerFilterEnum MagFilter;
    AxrImageSamplerFilterEnum MipmapFilter;
    AxrImageSamplerWrapEnum WrapU;
    AxrImageSamplerWrapEnum WrapV;
};

/// Model file image info
struct AxrModelFileImageInfo {
    char Name[AXR_MAX_ASSET_NAME_SIZE];
    char FilePath[AXR_MAX_FILE_PATH_SIZE];
};

/// Model file material info
struct AxrModelFileMaterialInfo {
    char Name[AXR_MAX_ASSET_NAME_SIZE];
    int32_t ColorImageIndex;
    int32_t ColorImageSamplerIndex;
    glm::vec4 ColorFactor;
    AxrMaterialBackfaceCullModeEnum BackfaceCullMode;
    AxrModelFileMaterialInfoAlphaModeEnum AlphaMode;
    float AlphaCutoff;
};

/// Model file submesh info
struct AxrModelFileSubmeshInfo {
    int32_t MaterialIndex;
};

/// Model file mesh info
struct AxrModelFileMeshInfo {
    uint32_t SubmeshCount;
    AxrModelFileSubmeshInfo* Submeshes;
};

/// Model file info
struct AxrModelFileInfo {
    uint32_t ImageSamplerCount;
    AxrModelFileImageSamplerInfo* ImageSamplers;
    uint32_t ImageCount;
    AxrModelFileImageInfo* Images;
    uint32_t MaterialCount;
    AxrModelFileMaterialInfo* Materials;
    uint32_t MeshCount;
    AxrModelFileMeshInfo* Meshes;
};

// ----------------------------------------- //
// External Function Definitions
// ----------------------------------------- //
extern "C" {
    /// Clone the given model file image sampler info
    /// @param modelFileImageSamplerInfo Model file image sampler info
    /// @returns The cloned model file image sampler info
    AXR_API AxrModelFileImageSamplerInfo axrModelFileImageSamplerInfoClone(
        const AxrModelFileImageSamplerInfo* modelFileImageSamplerInfo
    );
    /// Destroy the given model file image sampler info
    /// @param modelFileImageSamplerInfo Model file image sampler info
    AXR_API void axrModelFileImageSamplerInfoDestroy(AxrModelFileImageSamplerInfo* modelFileImageSamplerInfo);

    /// Clone the given model file image info
    /// @param modelFileImageInfo Model file image info
    /// @returns The cloned model file image info
    AXR_API AxrModelFileImageInfo axrModelFileImageInfoClone(const AxrModelFileImageInfo* modelFileImageInfo);
    /// Destroy the given model file image info
    /// @param modelFileImageInfo Model file image info
    AXR_API void axrModelFileImageInfoDestroy(AxrModelFileImageInfo* modelFileImageInfo);

    /// Clone the given model file material info
    /// @param modelFileMaterialInfo Model file material info
    /// @returns The cloned model file material info
    AXR_API AxrModelFileMaterialInfo axrModelFileMaterialInfoClone(
        const AxrModelFileMaterialInfo* modelFileMaterialInfo
    );
    /// Destroy the given model file material info
    /// @param modelFileMaterialInfo Model file material info
    AXR_API void axrModelFileMaterialInfoDestroy(AxrModelFileMaterialInfo* modelFileMaterialInfo);

    /// Clone the given model file submesh info
    /// @param modelFileSubmeshInfo Model file submesh info
    /// @returns The cloned model file submesh info
    AXR_API AxrModelFileSubmeshInfo axrModelFileSubmeshInfoClone(
        const AxrModelFileSubmeshInfo* modelFileSubmeshInfo
    );
    /// Destroy the given model file submesh info
    /// @param modelFileSubmeshInfo Model file submesh info
    AXR_API void axrModelFileSubmeshInfoDestroy(AxrModelFileSubmeshInfo* modelFileSubmeshInfo);

    /// Clone the given model file mesh info
    /// @param modelFileMeshInfo Model file mesh info
    /// @returns The cloned model file mesh info
    AXR_API AxrModelFileMeshInfo axrModelFileMeshInfoClone(
        const AxrModelFileMeshInfo* modelFileMeshInfo
    );
    /// Destroy the given model file mesh info
    /// @param modelFileMeshInfo Model file mesh info
    AXR_API void axrModelFileMeshInfoDestroy(AxrModelFileMeshInfo* modelFileMeshInfo);

    /// Clone the given model file info
    /// @param modelFileInfo Model file info
    /// @returns The cloned model file info
    AXR_API AxrModelFileInfo axrModelFileInfoClone(const AxrModelFileInfo* modelFileInfo);
    /// Destroy the given model file info
    /// @param modelFileInfo Model file info
    AXR_API void axrModelFileInfoDestroy(AxrModelFileInfo* modelFileInfo);

    /// Get a model's file info
    /// @param path The model's file path
    /// @param modelFileInfo Output model file info
    /// @returns AXR_SUCCESS if the function succeeded
    AXR_API AxrResult axrGetModelFileInfo(
        const char* path,
        AxrModelFileInfo* modelFileInfo
    );
}
