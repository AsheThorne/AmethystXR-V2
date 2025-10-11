#pragma once

// ----------------------------------------- //
// C/C++ Headers
// ----------------------------------------- //
#include <string>

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/assets.h"
#include "shaderValues.hpp"

/// Axr Material
class AxrMaterial {
public:
    // ----------------------------------------- //
    // Special Functions
    // ----------------------------------------- //

    // ---- Constructors ----

    /// Default Constructor
    AxrMaterial();
    /// Constructor
    /// @param config Material config
    explicit AxrMaterial(const AxrMaterialConfig& config);
    /// Copy Constructor
    /// @param src Source AxrMaterial to copy from
    AxrMaterial(const AxrMaterial& src);
    /// Move Constructor
    /// @param src Source AxrMaterial to move from
    AxrMaterial(AxrMaterial&& src) noexcept;

    // ---- Destructor ----

    /// Destructor
    ~AxrMaterial();

    // ---- Operator Overloads ----

    /// Copy Assignment Operator
    /// @param src Source AxrMaterial to copy from
    AxrMaterial& operator=(const AxrMaterial& src);
    /// Move Assignment Operator
    /// @param src Source AxrMaterial to move from
    AxrMaterial& operator=(AxrMaterial&& src) noexcept;

    // ----------------------------------------- //
    // Public Headers
    // ----------------------------------------- //

    /// Get the material's name
    /// @returns The material's name
    [[nodiscard]] const std::string& getName() const;
    
    // ---- For Internal Use ----
    // These functions are only to be used internally in the AmethystXr engine.
    // They have not been given a publicly accessible function in the 'include headers' to be used by an application.

    /// Get the vertex shader's name
    /// @returns The vertex shader's name
    [[nodiscard]] const std::string& getVertexShaderName() const;
    /// Get the fragment shader's name
    /// @returns The fragment shader's name
    [[nodiscard]] const std::string& getFragmentShaderName() const;

    /// Get the vertex shader values
    /// @returns The vertex shader values
    [[nodiscard]] const AxrShaderValuesRAII& getVertexShaderValues() const;
    /// Get the fragment shader values
    /// @returns The fragment shader values
    [[nodiscard]] const AxrShaderValuesRAII& getFragmentShaderValues() const;

    /// Get the backface culling mode
    /// @returns The backface culling mode
    [[nodiscard]] AxrMaterialBackfaceCullModeEnum getBackfaceCullMode() const;
    /// Get the alpha rendering mode mode
    /// @returns The alpha rendering mode mode
    [[nodiscard]] AxrMaterialAlphaRenderModeEnum getAlphaRenderMode() const;
    /// True if this material enables depth testing
    /// @returns True if this material enables depth testing
    [[nodiscard]] bool enableDepthTest() const;
    /// True if this material enables depth writing
    /// @returns True if this material enables depth writing
    [[nodiscard]] bool enableDepthWrite() const;

    /// Get all uniform buffer links
    /// @returns A collection of uniform buffer links
    [[nodiscard]] std::vector<AxrShaderUniformBufferLinkConst_T> getUniformBufferLinks() const;
    /// Get all image sampler buffer links
    /// @returns A collection of image sampler buffer links
    [[nodiscard]] std::vector<AxrShaderImageSamplerBufferLinkConst_T> getImageSamplerBufferLinks() const;

    /// Find the shader uniform buffer at the given binding
    /// @param binding Binding to use
    /// @returns The uniform buffer at the given binding
    [[nodiscard]] AxrShaderUniformBufferLinkConst_T findShaderUniformBuffer(uint32_t binding) const;
    /// Find the shader image sampler buffer at the given binding
    /// @param binding Binding to use
    /// @returns The image sampler buffer at the given binding
    [[nodiscard]] AxrShaderImageSamplerBufferLinkConst_T findShaderImageSamplerBuffer(uint32_t binding) const;

    /// Get the material layout's name
    /// @returns The material layout's name
    [[nodiscard]] std::string getMaterialLayoutName() const;
    /// Get the dynamic uniform buffer offsets
    /// @returns The dynamic uniform buffer offsets
    [[nodiscard]] const std::vector<AxrDynamicUniformBufferOffsetConfig>& getDynamicUniformBufferOffsets() const;

    /// Check if this material is valid
    /// @returns True if this material is valid
    [[nodiscard]] bool isValid() const;

    /// Clean up this class
    void cleanup();

private:
    // ----------------------------------------- //
    // Private Variables
    // ----------------------------------------- //

    // ---- Config Variables ----
    std::string m_Name;
    std::string m_VertexShaderName;
    std::string m_FragmentShaderName;
    AxrShaderValuesRAII m_VertexShaderValues;
    AxrShaderValuesRAII m_FragmentShaderValues;
    AxrMaterialBackfaceCullModeEnum m_BackfaceCullMode;
    AxrMaterialAlphaRenderModeEnum m_AlphaRenderMode;
    bool m_EnableDepthTest;
    bool m_EnableDepthWrite;
    std::vector<AxrDynamicUniformBufferOffsetConfig> m_DynamicUniformBufferOffsets;

    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //
};
