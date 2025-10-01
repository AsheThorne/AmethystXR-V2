#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/assets.h"
#include "axr/graphicsSystem.h"

// ----------------------------------------- //
// C/C++ Headers
// ----------------------------------------- //
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------- //
//                               Shader Engine Assets                                 //
// ---------------------------------------------------------------------------------- //

/// Check if the given engine asset is a shader
/// @param engineAssetEnum Engine asset to check
/// @returns True if the given engine asset is a shader
bool axrEngineAssetIsShader(AxrEngineAssetEnum engineAssetEnum);

/// Get the name for the given shader engine asset
/// @param engineAssetEnum Engine asset to get the name of
/// @returns The name of the given shader engine asset
const char* axrEngineAssetGetShaderName(AxrEngineAssetEnum engineAssetEnum);
/// Check if the given name is reserved for a shader engine asset
/// @param name Name to check
/// @returns True if the given name is reserved for a shader engine asset
bool axrEngineAssetIsShaderNameReserved(const char* name);

/// Create an engine asset shader
/// @param graphicsApi The graphics api to use this shader with
/// @param engineAssetEnum Shader engine asset
/// @param shader Output created shader
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateShader(
    AxrGraphicsApiEnum graphicsApi,
    AxrEngineAssetEnum engineAssetEnum,
    AxrShader& shader
);

/// Create the 'Default Vert' engine asset shader
/// @param graphicsApi The graphics api to use this shader with
/// @param shader Output created shader
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateShader_DefaultVert(AxrGraphicsApiEnum graphicsApi, AxrShader& shader);
/// Create the 'Default Frag' engine asset shader
/// @param graphicsApi The graphics api to use this shader with
/// @param shader Output created shader
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateShader_DefaultFrag(AxrGraphicsApiEnum graphicsApi, AxrShader& shader);
/// Create the 'Default Frag (Mask)' engine asset shader
/// @param graphicsApi The graphics api to use this shader with
/// @param shader Output created shader
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateShader_DefaultFrag_Mask(AxrGraphicsApiEnum graphicsApi, AxrShader& shader);
/// Create the 'UI Element Vert' engine asset shader
/// @param graphicsApi The graphics api to use this shader with
/// @param shader Output created shader
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateShader_UIElementVert(AxrGraphicsApiEnum graphicsApi, AxrShader& shader);
/// Create the 'UI Rectangle Frag' engine asset shader
/// @param graphicsApi The graphics api to use this shader with
/// @param shader Output created shader
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateShader_UIRectangleFrag(AxrGraphicsApiEnum graphicsApi, AxrShader& shader);
/// Create the 'UI Border Frag' engine asset shader
/// @param graphicsApi The graphics api to use this shader with
/// @param shader Output created shader
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateShader_UIBorderFrag(AxrGraphicsApiEnum graphicsApi, AxrShader& shader);
/// Create the 'UI Image Frag' engine asset shader
/// @param graphicsApi The graphics api to use this shader with
/// @param shader Output created shader
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateShader_UIImageFrag(AxrGraphicsApiEnum graphicsApi, AxrShader& shader);

// ---------------------------------------------------------------------------------- //
//                           Uniform Buffer Engine Assets                             //
// ---------------------------------------------------------------------------------- //

/// Check if the given engine asset is a uniform buffer
/// @param engineAssetEnum Engine asset to check
/// @returns True if the given engine asset is a uniform buffer
bool axrEngineAssetIsUniformBuffer(AxrEngineAssetEnum engineAssetEnum);
/// Get the name for the given uniform buffer engine asset
/// @param engineAssetEnum Engine asset to get the name of
/// @returns The name of the given uniform buffer engine asset
const char* axrEngineAssetGetUniformBufferName(AxrEngineAssetEnum engineAssetEnum);
/// Check if the given name is reserved for a uniform buffer engine asset
/// @param name Name to check
/// @returns True if the given name is reserved for a uniform buffer engine asset
bool axrEngineAssetIsUniformBufferNameReserved(const char* name);

/// Create an engine asset uniform buffer
/// @param engineAssetEnum Uniform buffer engine asset
/// @param uniformBuffer Output created uniform buffer
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateUniformBuffer(AxrEngineAssetEnum engineAssetEnum, AxrUniformBuffer& uniformBuffer);

/// Create the 'Scene Data' engine asset uniform buffer
/// @param uniformBuffer Output created uniform buffer
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateUniformBuffer_SceneData(AxrUniformBuffer& uniformBuffer);
/// Create the 'Camera Data' engine asset uniform buffer
/// @param uniformBuffer Output created uniform buffer
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateUniformBuffer_CameraData(AxrUniformBuffer& uniformBuffer);
/// Create the 'UI Canvas' engine asset uniform buffer
/// @param uniformBuffer Output created uniform buffer
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateUniformBuffer_UICanvas(AxrUniformBuffer& uniformBuffer);
/// Create the 'UI Elements' engine asset uniform buffer
/// @param uniformBuffer Output created uniform buffer
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateUniformBuffer_UIElements(AxrUniformBuffer& uniformBuffer);

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
// ---------------------------------------------------------------------------------- //
//                        Push Constant Buffer Engine Assets                          //
// ---------------------------------------------------------------------------------- //

/// Check if the given engine asset is a push constant buffer
/// @param engineAssetEnum Engine asset to check
/// @returns True if the given engine asset is a push constant buffer
bool axrEngineAssetIsPushConstantBuffer(AxrEngineAssetEnum engineAssetEnum);
/// Get the name for the given push constant buffer engine asset
/// @param engineAssetEnum Engine asset to get the name of
/// @returns The name of the given push constant buffer engine asset
const char* axrEngineAssetGetPushConstantBufferName(AxrEngineAssetEnum engineAssetEnum);
/// Check if the given name is reserved for a push constant buffer engine asset
/// @param name Name to check
/// @returns True if the given name is reserved for a push constant buffer engine asset
bool axrEngineAssetIsPushConstantBufferNameReserved(const char* name);
#endif

// ---------------------------------------------------------------------------------- //
//                              Material Engine Assets                                //
// ---------------------------------------------------------------------------------- //

/// Check if the given engine asset is a material
/// @param engineAssetEnum Engine asset to check
/// @returns True if the given engine asset is a material
bool axrEngineAssetIsMaterial(AxrEngineAssetEnum engineAssetEnum);

/// Get the name for the given material engine asset
/// @param engineAssetEnum Engine asset to get the name of
/// @returns The name of the given material engine asset
const char* axrEngineAssetGetMaterialName(AxrEngineAssetEnum engineAssetEnum);
/// Check if the given name is reserved for a material engine asset
/// @param name Name to check
/// @returns True if the given name is reserved for a material engine asset
bool axrEngineAssetIsMaterialNameReserved(const char* name);

/// Create the 'Default Material' engine asset material
/// @param materialName Material name
/// @param materialValues Material values
/// @param material Output created material
/// @param materialShaders Output required shaders for this material
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateMaterial_DefaultMaterial(
    const std::string& materialName,
    const AxrEngineAssetMaterial_DefaultMaterial& materialValues,
    AxrMaterial& material,
    std::vector<AxrEngineAssetEnum>& materialShaders
);

/// Create the 'UI Rectangle' engine asset material
/// @param material Output created material
/// @param materialShaders Output required shaders for this material
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateMaterial_UIRectangle(
    AxrMaterial& material,
    std::vector<AxrEngineAssetEnum>& materialShaders
);
/// Create the 'UI Border' engine asset material
/// @param material Output created material
/// @param materialShaders Output required shaders for this material
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateMaterial_UIBorder(
    AxrMaterial& material,
    std::vector<AxrEngineAssetEnum>& materialShaders
);
/// Create the 'UI Image' engine asset material
/// @param material Output created material
/// @param materialShaders Output required shaders for this material
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateMaterial_UIImage(
    AxrMaterial& material,
    std::vector<AxrEngineAssetEnum>& materialShaders
);

// ---------------------------------------------------------------------------------- //
//                                Model Engine Assets                                 //
// ---------------------------------------------------------------------------------- //

/// Check if the given engine asset is a model
/// @param engineAssetEnum Engine asset to check
/// @returns True if the given engine asset is a model
bool axrEngineAssetIsModel(AxrEngineAssetEnum engineAssetEnum);
/// Get the name for the given model engine asset
/// @param engineAssetEnum Engine asset to get the name of
/// @returns The name of the given model engine asset
const char* axrEngineAssetGetModelName(AxrEngineAssetEnum engineAssetEnum);
/// Check if the given name is reserved for a model engine asset
/// @param name Name to check
/// @returns True if the given name is reserved for a model engine asset
bool axrEngineAssetIsModelNameReserved(const char* name);

/// Create an engine asset model
/// @param modelName Model name
/// @param engineAssetEnum Model engine asset
/// @param model Output created model
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateModel(const std::string& modelName, AxrEngineAssetEnum engineAssetEnum, AxrModel& model);

/// Create the 'Triangle' engine asset model
/// @param modelName Model name
/// @param model Output created model
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateModel_Triangle(const std::string& modelName, AxrModel& model);
/// Create the 'Square' engine asset model
/// @param modelName Model name
/// @param model Output created model
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateModel_Square(const std::string& modelName, AxrModel& model);
/// Create the 'Cube' engine asset model
/// @param modelName Model name
/// @param model Output created model
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateModel_Cube(const std::string& modelName, AxrModel& model);
/// Create the 'UI Rectangle' engine asset model
/// @param modelName Model name
/// @param model Output created model
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateModel_UIRectangle(const std::string& modelName, AxrModel& model);

// ---------------------------------------------------------------------------------- //
//                                Image Sampler Engine Assets                                 //
// ---------------------------------------------------------------------------------- //

/// Check if the given engine asset is an image sampler
/// @param engineAssetEnum Engine asset to check
/// @returns True if the given engine asset is an image sampler
bool axrEngineAssetIsImageSampler(AxrEngineAssetEnum engineAssetEnum);
/// Get the name for the given image sampler engine asset
/// @param engineAssetEnum Engine asset to get the name of
/// @returns The name of the given image sampler engine asset
const char* axrEngineAssetGetImageSamplerName(AxrEngineAssetEnum engineAssetEnum);
/// Check if the given name is reserved for an image sampler engine asset
/// @param name Name to check
/// @returns True if the given name is reserved for an image sampler engine asset
bool axrEngineAssetIsImageSamplerNameReserved(const char* name);

/// Create an engine asset image sampler
/// @param imageSamplerName Image sampler name
/// @param engineAssetEnum Image sampler engine asset
/// @param imageSampler Output created image sampler
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateImageSampler(
    const std::string& imageSamplerName,
    AxrEngineAssetEnum engineAssetEnum,
    AxrImageSampler& imageSampler
);

/// Create the 'NearestRepeat' engine asset image sampler
/// @param imageSamplerName Image sampler name
/// @param imageSampler Output created image sampler
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateImageSampler_NearestRepeat(
    const std::string& imageSamplerName,
    AxrImageSampler& imageSampler
);

/// Create the 'LinearRepeat' engine asset image sampler
/// @param imageSamplerName Image sampler name
/// @param imageSampler Output created image sampler
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateImageSampler_LinearRepeat(
    const std::string& imageSamplerName,
    AxrImageSampler& imageSampler
);

// ---------------------------------------------------------------------------------- //
//                                Image Engine Assets                                 //
// ---------------------------------------------------------------------------------- //

/// Check if the given engine asset is an image
/// @param engineAssetEnum Engine asset to check
/// @returns True if the given engine asset is an image
bool axrEngineAssetIsImage(AxrEngineAssetEnum engineAssetEnum);
/// Get the name for the given image engine asset
/// @param engineAssetEnum Engine asset to get the name of
/// @returns The name of the given image engine asset
const char* axrEngineAssetGetImageName(AxrEngineAssetEnum engineAssetEnum);
/// Check if the given name is reserved for an image engine asset
/// @param name Name to check
/// @returns True if the given name is reserved for an image engine asset
bool axrEngineAssetIsImageNameReserved(const char* name);

/// Create an engine asset image
/// @param imageName Image name
/// @param engineAssetEnum Image engine asset
/// @param image Output created image
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateImage(const std::string& imageName, AxrEngineAssetEnum engineAssetEnum, AxrImage& image);

/// Create the 'MissingTexture' engine asset image
/// @param imageName Image name
/// @param image Output created image
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateImage_MissingTexture(const std::string& imageName, AxrImage& image);

/// Create the 'UvTester' engine asset image
/// @param imageName Image name
/// @param image Output created image
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateImage_UvTester(const std::string& imageName, AxrImage& image);

/// Create the 'FontAtlasJetbrainsMono_Regular' engine asset image
/// @param imageName Image name
/// @param image Output created image
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateImage_FontAtlasJetbrainsMono_Regular(const std::string& imageName, AxrImage& image);

// ---------------------------------------------------------------------------------- //
//                                Font Engine Assets                                 //
// ---------------------------------------------------------------------------------- //

/// Check if the given engine asset is a font
/// @param engineAssetEnum Engine asset to check
/// @returns True if the given engine asset is a font
bool axrEngineAssetIsFont(AxrEngineAssetEnum engineAssetEnum);
/// Get the name for the given font engine asset
/// @param engineAssetEnum Engine asset to get the name of
/// @returns The name of the given font engine asset
const char* axrEngineAssetGetFontName(AxrEngineAssetEnum engineAssetEnum);
/// Check if the given name is reserved for a font engine asset
/// @param name Name to check
/// @returns True if the given name is reserved for a font engine asset
bool axrEngineAssetIsFontNameReserved(const char* name);

/// Create an engine asset font
/// @param id Font id
/// @param engineAssetEnum Font engine asset
/// @param font Output created font
/// @param imageAtlas Output required image atlas for this font
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateFont(
    uint16_t id,
    AxrEngineAssetEnum engineAssetEnum,
    AxrFont& font,
    AxrEngineAssetEnum& imageAtlas
);

/// Create the 'JetbrainsMono_Regular' engine asset font
/// @param id Font id
/// @param font Output created font
/// @param imageAtlas Output required image atlas for this font
/// @returns AXR_SUCCESS if the function succeeded
AxrResult axrEngineAssetCreateFont_JetbrainsMono_Regular(
    uint16_t id,
    AxrFont& font,
    AxrEngineAssetEnum& imageAtlas
);
