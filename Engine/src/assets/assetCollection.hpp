#pragma once

// ----------------------------------------- //
// C/C++ Headers
// ----------------------------------------- //
#include <unordered_map>
#include <string>

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/assets.h"
#include "shader.hpp"
#include "material.hpp"
#include "model.hpp"
#include "image.hpp"
#include "uniformBuffer.hpp"
#include "imageSampler.hpp"
#include "font.hpp"
#include "axr/common/callback.h"
#include "axr/graphicsSystem.h"

/// Axr Asset Collection
class AxrAssetCollection {
public:
    // ----------------------------------------- //
    // Types
    // ----------------------------------------- //

    /// On material created callback function type
    /// @param 1: Material that was created
    using OnMaterialCreatedCallback_T = AxrCallback<void(AxrMaterialConst_T)>;
    /// On model created callback function type
    /// @param 1: Model that was created
    using OnModelCreatedCallback_T = AxrCallback<void(AxrModelConst_T)>;
    /// On uniform buffer created callback function type
    /// @param 1: Uniform buffer that was created
    using OnUniformBufferCreatedCallback_T = AxrCallback<void(AxrUniformBufferConst_T)>;
#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
    /// On push constant buffer created callback function type
    /// @param 1: Push constant buffer that was created
    using OnPushConstantBufferCreatedCallback_T = AxrCallback<void(AxrPushConstantBufferConst_T)>;
#endif
    /// On image created callback function type
    /// @param 1: Image that was created
    using OnImageCreatedCallback_T = AxrCallback<void(AxrImageConst_T)>;
    /// On image sampler created callback function type
    /// @param 1: Image sampler that was created
    using OnImageSamplerCreatedCallback_T = AxrCallback<void(AxrImageSamplerConst_T)>;
    /// On font created callback function type
    /// @param 1: Font that was created
    using OnFontCreatedCallback_T = AxrCallback<void(AxrFontConst_T)>;

    // ----------------------------------------- //
    // Public Variables
    // ----------------------------------------- //

    /// On material created for the graphics system
    OnMaterialCreatedCallback_T OnMaterialCreatedCallbackGraphics;
    /// On model created for the graphics system
    OnModelCreatedCallback_T OnModelCreatedCallbackGraphics;
    /// On uniform buffer created for the graphics system
    OnUniformBufferCreatedCallback_T OnUniformBufferCreatedCallbackGraphics;
#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
    /// On push constant buffer created for the graphics system
    OnPushConstantBufferCreatedCallback_T OnPushConstantBufferCreatedCallbackGraphics;
#endif
    /// On image created for the graphics system
    OnImageCreatedCallback_T OnImageCreatedCallbackGraphics;
    /// On image sampler created for the graphics system
    OnImageSamplerCreatedCallback_T OnImageSamplerCreatedCallbackGraphics;
    /// On font created for the graphics system
    OnFontCreatedCallback_T OnFontCreatedCallbackGraphics;

    // ----------------------------------------- //
    // Special Functions
    // ----------------------------------------- //

    // ---- Constructors ----

    /// Constructor
    /// @param isGlobalAssetCollection True if this asset collection is used as the global asset collection
    /// @param graphicsApi Graphics API to use
    explicit AxrAssetCollection(bool isGlobalAssetCollection, AxrGraphicsApiEnum graphicsApi);
    /// Copy Constructor
    /// @param src Source AxrAssetCollection to copy from
    AxrAssetCollection(const AxrAssetCollection& src) = delete;
    /// Move Constructor
    /// @param src Source AxrAssetCollection to move from
    AxrAssetCollection(AxrAssetCollection&& src) noexcept;

    // ---- Destructor ----

    /// Destructor
    ~AxrAssetCollection();

    // ---- Operator Overloads ----

    /// Copy Assignment Operator
    /// @param src Source AxrAssetCollection to copy from
    AxrAssetCollection& operator=(const AxrAssetCollection& src) = delete;
    /// Move Assignment Operator
    /// @param src Source AxrAssetCollection to move from
    AxrAssetCollection& operator=(AxrAssetCollection&& src) noexcept;

    // ----------------------------------------- //
    // Public Functions
    // ----------------------------------------- //

    // ---- Shader ----

    /// Create a new shader
    /// @param shaderConfig Shader config
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createShader(const AxrShaderConfig& shaderConfig);
    /// Create a new engine asset shader
    /// @param engineAssetEnum Engine asset enum
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createShader(AxrEngineAssetEnum engineAssetEnum);

    // ---- Material ----

    /// Create a new material
    /// @param materialConfig Material config
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createMaterial(const AxrMaterialConfig& materialConfig);
    /// Create a material using the engine defined material named 'Default Material'
    /// @param materialName Material name
    /// @param materialValues Material values
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createMaterial(
        const std::string& materialName,
        const AxrEngineAssetMaterial_DefaultMaterial& materialValues
    );

    // ---- Model ----

    /// Create a new model
    /// @param modelConfig Model config
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createModel(const AxrModelConfig& modelConfig);
    /// Create a new engine asset model
    /// @param modelName Model name
    /// @param engineAssetEnum Engine asset enum
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createModel(const std::string& modelName, AxrEngineAssetEnum engineAssetEnum);

    // ---- Uniform Buffer ----

    /// Create a new uniform buffer
    /// @param uniformBufferConfig Uniform buffer config
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createUniformBuffer(const AxrUniformBufferConfig& uniformBufferConfig);
    /// Create a new uniform buffer
    /// @param uniformBufferConfig Dynamic uniform buffer config
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createUniformBuffer(const AxrDynamicUniformBufferConfig& uniformBufferConfig);

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
    // ---- Push Constant Buffer ----

    /// Create a new push constant buffer
    /// @param pushConstantBufferConfig Push constant buffer config
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createPushConstantBuffer(const AxrPushConstantBufferConfig& pushConstantBufferConfig);
#endif

    // ---- Image ----

    /// Create a new image
    /// @param imageConfig Image config
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createImage(const AxrImageConfig& imageConfig);
    /// Create a new engine asset image
    /// @param imageName Image name
    /// @param engineAssetEnum Engine asset enum
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createImage(const std::string& imageName, AxrEngineAssetEnum engineAssetEnum);

    // ---- Image Sampler ----

    /// Create a new image sampler
    /// @param imageSamplerConfig Image sampler config
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createImageSampler(const AxrImageSamplerConfig& imageSamplerConfig);

    // ---- Font ----

    /// Create a new font
    /// @param fontConfig Font config
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createFont(const AxrFontConfig& fontConfig);
    /// Create a new engine asset font
    /// @param engineAssetEnum Engine asset enum
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createFont(AxrEngineAssetEnum engineAssetEnum);

    // ---- For Internal Use ----
    // These functions are only to be used internally in the AmethystXr engine.
    // They have not been given a publicly accessible function in the 'include headers' to be used by an application.

    // ---- Model ----

    /// Create a new engine asset model
    /// @param engineAssetEnum Engine asset enum
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createModel(AxrEngineAssetEnum engineAssetEnum);

    // ---- Image ----

    /// Create a new engine asset image
    /// @param engineAssetEnum Engine asset enum
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createImage(AxrEngineAssetEnum engineAssetEnum);

    // ---- Image Sampler ----

    /// Create a new engine asset image sampler
    /// @param engineAssetEnum Engine asset enum
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult createImageSampler(AxrEngineAssetEnum engineAssetEnum);

    // ---- All Assets ----

    /// Clean up this class
    void cleanup();

    /// Check if all the assets in the collection have been loaded
    /// @returns True if all assets in the collection have been loaded
    [[nodiscard]] bool isLoaded();
    /// Load all assets in the collection
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult loadAssets();
    /// Unload all assets in the collection
    void unloadAssets();

    /// Find the named shader
    /// @param name Name of the shader
    /// @returns A handle to the shader if it was found. nullptr if it wasn't found
    [[nodiscard]] const AxrShader* findShader(const std::string& name);
#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
    /// Find the named push constant buffer
    /// @param name Name of the push constant buffer
    /// @returns A handle to the push constant buffer if it was found. nullptr if it wasn't found
    [[nodiscard]] const AxrPushConstantBuffer* findPushConstantBuffer(const std::string& name);
#endif

    /// Get the shaders
    /// @returns A map of the shaders 
    [[nodiscard]] const std::unordered_map<std::string, AxrShader>& getShaders();
    /// Get the materials
    /// @returns A map of the materials
    [[nodiscard]] const std::unordered_map<std::string, AxrMaterial>& getMaterials();
    /// Get the models
    /// @returns A map of the models
    [[nodiscard]] const std::unordered_map<std::string, AxrModel>& getModels();
    /// Get the uniform buffers
    /// @returns A map of the uniform buffers
    [[nodiscard]] const std::unordered_map<std::string, AxrUniformBuffer>& getUniformBuffers();
#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
    /// Get the push constant buffers
    /// @returns A map of the push constant buffers
    [[nodiscard]] const std::unordered_map<std::string, AxrPushConstantBuffer>& getPushConstantBuffers();
#endif
    /// Get the images
    /// @returns A map of the images
    [[nodiscard]] const std::unordered_map<std::string, AxrImage>& getImages();
    /// Get the image samplers
    /// @returns A map of the image samplers
    [[nodiscard]] const std::unordered_map<std::string, AxrImageSampler>& getImageSamplers();
    /// Get the fonts
    /// @returns A map of the fonts
    [[nodiscard]] const std::unordered_map<std::string, AxrFont>& getFonts();

    /// Check if the given font ID is for the global asset collection
    /// @param fontID Font ID to check
    /// @returns True if the given font ID is for the global asset collection
    [[nodiscard]] static bool isGlobalFont(uint16_t fontID);

private:
    // ----------------------------------------- //
    // Private Variables
    // ----------------------------------------- //

    // ---- Config Variables ----
    bool m_IsGlobalAssetCollection;
    AxrGraphicsApiEnum m_GraphicsApi;

    std::unordered_map<std::string, AxrShader> m_Shaders;
    std::unordered_map<std::string, AxrMaterial> m_Materials;
    std::unordered_map<std::string, AxrModel> m_Models;
    std::unordered_map<std::string, AxrUniformBuffer> m_UniformBuffers;
#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
    std::unordered_map<std::string, AxrPushConstantBuffer> m_PushConstantBuffers;
#endif
    std::unordered_map<std::string, AxrImage> m_Images;
    std::unordered_map<std::string, AxrImageSampler> m_ImageSamplers;
    std::unordered_map<std::string, AxrFont> m_Fonts;

    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //

    /// Generate the next font ID to use
    /// @param id Output ID
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult generateFontID(uint16_t& id) const;
};
