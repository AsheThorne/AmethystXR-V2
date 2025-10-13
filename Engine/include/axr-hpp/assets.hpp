#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/assets.h"
#include "common/enums.hpp"
#include "common/types.hpp"

namespace axr {
    // ---------------------------------------------------------------------------------- //
    //                                  Shader Properties                                 //
    // ---------------------------------------------------------------------------------- //

    // ----------------------------------------- //
    // Enums
    // ----------------------------------------- //

    // ---- Shader Properties ----

    /// Shader stage enum
    enum class ShaderStageEnum {
        Undefined = AXR_SHADER_STAGE_UNDEFINED,
        Vertex = AXR_SHADER_STAGE_VERTEX,
        Fragment = AXR_SHADER_STAGE_FRAGMENT,
    };

    // ---- Shader Buffer Layout ----

    /// Shader buffer layout enum
    enum class ShaderBufferLayoutEnum {
        Undefined = AXR_SHADER_BUFFER_LAYOUT_UNDEFINED,
        UniformBuffer = AXR_SHADER_BUFFER_LAYOUT_UNIFORM_BUFFER,
        DynamicUniformBuffer = AXR_SHADER_BUFFER_LAYOUT_DYNAMIC_UNIFORM_BUFFER,
        ImageSamplerBuffer = AXR_SHADER_BUFFER_LAYOUT_IMAGE_SAMPLER_BUFFER,
        PushConstantBuffer = AXR_SHADER_BUFFER_LAYOUT_PUSH_CONSTANT_BUFFER,
    };

    // ---- Vertex Attribute ----

    /// Shader vertex attribute enum
    enum class ShaderVertexAttributeEnum {
        Undefined = AXR_SHADER_VERTEX_ATTRIBUTE_UNDEFINED,
        Position = AXR_SHADER_VERTEX_ATTRIBUTE_POSITION,
        Color = AXR_SHADER_VERTEX_ATTRIBUTE_COLOR,
        TexCoord_0 = AXR_SHADER_VERTEX_ATTRIBUTE_TEXCOORD_0,
        TexCoord_1 = AXR_SHADER_VERTEX_ATTRIBUTE_TEXCOORD_1,
        TexCoord_2 = AXR_SHADER_VERTEX_ATTRIBUTE_TEXCOORD_2,
        TexCoord_3 = AXR_SHADER_VERTEX_ATTRIBUTE_TEXCOORD_3,
    };

    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    // ---- Shader Properties ----

    /// Vertex shader properties
    struct VertexShaderProperties {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        const AxrShaderStageEnum Type = AXR_SHADER_STAGE_VERTEX;
        uint32_t VertexAttributeCount;
        AxrShaderVertexAttribute* VertexAttributes;
        uint32_t BufferLayoutCount;
        AxrShaderBufferLayout_T* BufferLayouts;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        VertexShaderProperties():
            VertexAttributeCount(0),
            VertexAttributes(nullptr),
            BufferLayoutCount(0),
            BufferLayouts(nullptr) {
        }

        /// Copy Constructor
        /// @param src Source VertexShaderProperties to copy from
        VertexShaderProperties(const VertexShaderProperties& src) = delete;
        /// Move Constructor
        /// @param src Source VertexShaderProperties to move from
        VertexShaderProperties(VertexShaderProperties&& src) noexcept = delete;

        // ---- Destructor ----

        /// Destructor
        ~VertexShaderProperties() {
            clearVertexAttributes();
            clearBufferLayouts();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source VertexShaderProperties to copy from
        VertexShaderProperties& operator=(const VertexShaderProperties& src) = delete;
        /// Move Assignment Operator
        /// @param src Source VertexShaderProperties to move from
        VertexShaderProperties& operator=(VertexShaderProperties&& src) noexcept = delete;

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Check if these vertex shader properties are valid
        /// @returns True if these shader properties are valid
        [[nodiscard]] bool isValid() const {
            return axrVertexShaderPropertiesIsValid(toRaw());
        }

        /// Clone the raw vertex shader properties
        /// @returns The cloned raw shader properties
        [[nodiscard]] AxrVertexShaderProperties_T cloneRaw() const {
            return axrVertexShaderPropertiesClone(toRaw());
        }

        /// Get a handle to the VertexShaderProperties as an AxrVertexShaderProperties
        /// @returns This as an AxrVertexShaderProperties
        const AxrVertexShaderProperties* toRaw() const {
            return reinterpret_cast<const AxrVertexShaderProperties*>(this);
        }

        /// Get a handle to the VertexShaderProperties as an AxrVertexShaderProperties
        /// @returns This as an AxrVertexShaderProperties
        AxrVertexShaderProperties* toRaw() {
            return reinterpret_cast<AxrVertexShaderProperties*>(this);
        }

        // ---- Vertex Attributes ----

        /// Add a vertex attribute
        /// @param type Vertex attribute type
        /// @param location Vertex attribute location
        void addVertexAttribute(
            const axr::ShaderVertexAttributeEnum type,
            const uint32_t location
        ) {
            resizeVertexAttributes(VertexAttributeCount + 1);

            const AxrShaderVertexAttribute vertexAttribute{
                .Type = static_cast<AxrShaderVertexAttributeEnum>(type),
                .Location = location
            };
            VertexAttributes[VertexAttributeCount - 1] = axrShaderVertexAttributeClone(vertexAttribute);
        }

        /// Clear the vertex attributes
        void clearVertexAttributes() {
            if (VertexAttributes == nullptr) return;

            for (uint32_t i = 0; i < VertexAttributeCount; ++i) {
                axrShaderVertexAttributeDestroy(&VertexAttributes[i]);
            }

            delete[] VertexAttributes;
            VertexAttributes = nullptr;
            VertexAttributeCount = 0;
        }

        // ---- Buffer Layouts ----

        /// Add a uniform buffer layout
        /// @param binding Uniform buffer layout binding
        /// @param bufferSize Uniform buffer layout buffer size
        void addUniformBufferLayout(const uint32_t binding, const uint64_t bufferSize) {
            resizeBufferLayouts(BufferLayoutCount + 1);

            const AxrShaderUniformBufferLayout bufferLayout{
                .Binding = binding,
                .BufferSize = bufferSize
            };
            BufferLayouts[BufferLayoutCount - 1] = reinterpret_cast<AxrShaderBufferLayout_T>(
                axrShaderUniformBufferLayoutClone(&bufferLayout)
            );
        }

        /// Add a dynamic uniform buffer layout
        /// @param binding Dynamic uniform buffer layout binding
        /// @param instanceSize Dynamic uniform buffer layout instance size
        void addDynamicUniformBufferLayout(const uint32_t binding, const uint64_t instanceSize) {
            resizeBufferLayouts(BufferLayoutCount + 1);

            const AxrShaderDynamicUniformBufferLayout bufferLayout{
                .Binding = binding,
                .InstanceSize = instanceSize
            };
            BufferLayouts[BufferLayoutCount - 1] = reinterpret_cast<AxrShaderBufferLayout_T>(
                axrShaderDynamicUniformBufferLayoutClone(&bufferLayout)
            );
        }

        /// Add an image sampler buffer layout
        /// @param binding Image sampler buffer layout binding
        void addImageSamplerBufferLayout(const uint32_t binding) {
            resizeBufferLayouts(BufferLayoutCount + 1);

            const AxrShaderImageSamplerBufferLayout bufferLayout{
                .Binding = binding,
            };
            BufferLayouts[BufferLayoutCount - 1] = reinterpret_cast<AxrShaderBufferLayout_T>(
                axrShaderImageSamplerBufferLayoutClone(&bufferLayout)
            );
        }

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
        /// Add a push constant buffer layout
        /// @param bufferSize Push constant buffer layout buffer size
        void addPushConstantBufferLayout(const uint32_t bufferSize) {
            resizeBufferLayouts(BufferLayoutCount + 1);

            const AxrShaderPushConstantBufferLayout bufferLayout{
                .BufferSize = bufferSize
            };
            BufferLayouts[BufferLayoutCount - 1] = reinterpret_cast<AxrShaderBufferLayout_T>(
                axrShaderPushConstantBufferLayoutClone(&bufferLayout)
            );
        }
#endif

        /// CLear the buffer layouts
        void clearBufferLayouts() {
            if (BufferLayouts == nullptr) return;

            for (uint32_t i = 0; i < BufferLayoutCount; ++i) {
                if (BufferLayouts[i] == nullptr) continue;

                axrShaderBufferLayoutDestroy(&BufferLayouts[i]);
            }

            delete[] BufferLayouts;
            BufferLayouts = nullptr;
            BufferLayoutCount = 0;
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        // ---- Vertex Attributes ----

        /// Resize the vertex attributes
        /// @param size New size
        void resizeVertexAttributes(const uint32_t size) {
            if (size == 0) {
                clearVertexAttributes();
                return;
            }

            auto newVertexAttributes = new AxrShaderVertexAttribute[size]{};
            for (uint32_t i = 0; i < std::min(VertexAttributeCount, size); ++i) {
                // Move vertex attributes to new array
                newVertexAttributes[i] = VertexAttributes[i];
                VertexAttributes[i] = {};
            }

            clearVertexAttributes();
            VertexAttributes = newVertexAttributes;
            VertexAttributeCount = size;
        }

        // ---- Buffer Layouts ----

        /// Resize the buffer layouts
        /// @param size New size
        void resizeBufferLayouts(const uint32_t size) {
            if (size == 0) {
                clearBufferLayouts();
                return;
            }

            auto newBufferLayouts = new AxrShaderBufferLayout_T[size]{};
            for (uint32_t i = 0; i < std::min(BufferLayoutCount, size); ++i) {
                // Move buffer layouts to new array
                newBufferLayouts[i] = BufferLayouts[i];
                BufferLayouts[i] = nullptr;
            }

            clearBufferLayouts();
            BufferLayouts = newBufferLayouts;
            BufferLayoutCount = size;
        }
    };

    static_assert(
        sizeof(AxrVertexShaderProperties) == sizeof(axr::VertexShaderProperties),
        "Original type and wrapper have different size!"
    );

    /// Fragment shader properties
    struct FragmentShaderProperties {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        const AxrShaderStageEnum Type = AXR_SHADER_STAGE_FRAGMENT;
        uint32_t BufferLayoutCount;
        AxrShaderBufferLayout_T* BufferLayouts;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        FragmentShaderProperties():
            BufferLayoutCount(0),
            BufferLayouts(nullptr) {
        }

        /// Copy Constructor
        /// @param src Source FragmentShaderProperties to copy from
        FragmentShaderProperties(const FragmentShaderProperties& src) = delete;
        /// Move Constructor
        /// @param src Source FragmentShaderProperties to move from
        FragmentShaderProperties(FragmentShaderProperties&& src) noexcept = delete;

        // ---- Destructor ----

        /// Destructor
        ~FragmentShaderProperties() {
            clearBufferLayouts();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source FragmentShaderProperties to copy from
        FragmentShaderProperties& operator=(const FragmentShaderProperties& src) = delete;
        /// Move Assignment Operator
        /// @param src Source FragmentShaderProperties to move from
        FragmentShaderProperties& operator=(FragmentShaderProperties&& src) noexcept = delete;

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Check if these fragment shader properties are valid
        /// @returns True if these shader properties are valid
        [[nodiscard]] bool isValid() const {
            return axrFragmentShaderPropertiesIsValid(toRaw());
        }

        /// Clone the raw fragment shader properties
        /// @returns The cloned raw shader properties
        [[nodiscard]] AxrFragmentShaderProperties_T cloneRaw() const {
            return axrFragmentShaderPropertiesClone(toRaw());
        }

        /// Get a handle to the FragmentShaderProperties as an AxrFragmentShaderProperties
        /// @returns This as an AxrFragmentShaderProperties
        const AxrFragmentShaderProperties* toRaw() const {
            return reinterpret_cast<const AxrFragmentShaderProperties*>(this);
        }

        /// Get a handle to the FragmentShaderProperties as an AxrFragmentShaderProperties
        /// @returns This as an AxrFragmentShaderProperties
        AxrFragmentShaderProperties* toRaw() {
            return reinterpret_cast<AxrFragmentShaderProperties*>(this);
        }

        // ---- Buffer Layouts ----

        /// Add a uniform buffer layout
        /// @param binding Uniform buffer layout binding
        /// @param bufferSize Uniform buffer layout buffer size
        void addUniformBufferLayout(const uint32_t binding, const uint64_t bufferSize) {
            resizeBufferLayouts(BufferLayoutCount + 1);

            const AxrShaderUniformBufferLayout bufferLayout{
                .Binding = binding,
                .BufferSize = bufferSize
            };
            BufferLayouts[BufferLayoutCount - 1] = reinterpret_cast<AxrShaderBufferLayout_T>(
                axrShaderUniformBufferLayoutClone(&bufferLayout)
            );
        }

        /// Add a dynamic uniform buffer layout
        /// @param binding Dynamic uniform buffer layout binding
        /// @param instanceSize Dynamic uniform buffer layout buffer size
        void addDynamicUniformBufferLayout(const uint32_t binding, const uint64_t instanceSize) {
            resizeBufferLayouts(BufferLayoutCount + 1);

            const AxrShaderDynamicUniformBufferLayout bufferLayout{
                .Binding = binding,
                .InstanceSize = instanceSize
            };
            BufferLayouts[BufferLayoutCount - 1] = reinterpret_cast<AxrShaderBufferLayout_T>(
                axrShaderDynamicUniformBufferLayoutClone(&bufferLayout)
            );
        }

        /// Add an image sampler buffer layout
        /// @param binding Image sampler buffer layout binding
        void addImageSamplerBufferLayout(const uint32_t binding) {
            resizeBufferLayouts(BufferLayoutCount + 1);

            const AxrShaderImageSamplerBufferLayout bufferLayout{
                .Binding = binding,
            };
            BufferLayouts[BufferLayoutCount - 1] = reinterpret_cast<AxrShaderBufferLayout_T>(
                axrShaderImageSamplerBufferLayoutClone(&bufferLayout)
            );
        }

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
        /// Add a push constant buffer layout
        /// @param bufferSize Push constant buffer layout buffer size
        void addPushConstantBufferLayout(const uint32_t bufferSize) {
            resizeBufferLayouts(BufferLayoutCount + 1);

            const AxrShaderPushConstantBufferLayout bufferLayout{
                .BufferSize = bufferSize
            };
            BufferLayouts[BufferLayoutCount - 1] = reinterpret_cast<AxrShaderBufferLayout_T>(
                axrShaderPushConstantBufferLayoutClone(&bufferLayout)
            );
        }
#endif

        /// CLear the buffer layouts
        void clearBufferLayouts() {
            if (BufferLayouts == nullptr) return;

            for (uint32_t i = 0; i < BufferLayoutCount; ++i) {
                if (BufferLayouts[i] == nullptr) continue;

                axrShaderBufferLayoutDestroy(&BufferLayouts[i]);
            }

            delete[] BufferLayouts;
            BufferLayouts = nullptr;
            BufferLayoutCount = 0;
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        // ---- Buffer Layouts ----

        /// Resize the buffer layouts
        /// @param size New size
        void resizeBufferLayouts(const uint32_t size) {
            if (size == 0) {
                clearBufferLayouts();
                return;
            }

            auto newBufferLayouts = new AxrShaderBufferLayout_T[size]{};
            for (uint32_t i = 0; i < std::min(BufferLayoutCount, size); ++i) {
                // Move buffer layouts to new array
                newBufferLayouts[i] = BufferLayouts[i];
                BufferLayouts[i] = nullptr;
            }

            clearBufferLayouts();
            BufferLayouts = newBufferLayouts;
            BufferLayoutCount = size;
        }
    };

    static_assert(
        sizeof(AxrFragmentShaderProperties) == sizeof(axr::FragmentShaderProperties),
        "Original type and wrapper have different size!"
    );

    // ---------------------------------------------------------------------------------- //
    //                                   Shader Assets                                    //
    // ---------------------------------------------------------------------------------- //

    // ----------------------------------------- //
    // Shader Config Definition
    // ----------------------------------------- //

    /// Shader Config
    struct ShaderConfig {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        char Name[AXR_MAX_ASSET_NAME_SIZE]{};
        char FilePath[AXR_MAX_FILE_PATH_SIZE]{};
        AxrShaderProperties_T Properties;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        ShaderConfig():
            Properties(nullptr) {
        }

        /// Constructor
        /// @param name The shader name
        /// @param filePath The shader file path
        /// @param vertexShaderProperties The vertex shader properties to use
        ShaderConfig(
            const char* name,
            const char* filePath,
            const axr::VertexShaderProperties& vertexShaderProperties
        ): Properties(reinterpret_cast<AxrShaderProperties_T>(vertexShaderProperties.cloneRaw())) {
            if (name != nullptr) {
                strncpy_s(Name, name, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (filePath != nullptr) {
                strncpy_s(FilePath, filePath, AXR_MAX_FILE_PATH_SIZE);
            }
        }

        /// Constructor
        /// @param name The shader name
        /// @param filePath The shader file path
        /// @param fragmentShaderProperties The fragment shader properties to use
        ShaderConfig(
            const char* name,
            const char* filePath,
            const axr::FragmentShaderProperties& fragmentShaderProperties
        ): Properties(reinterpret_cast<AxrShaderProperties_T>(fragmentShaderProperties.cloneRaw())) {
            if (name != nullptr) {
                strncpy_s(Name, name, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (filePath != nullptr) {
                strncpy_s(FilePath, filePath, AXR_MAX_FILE_PATH_SIZE);
            }
        }

        /// Copy Constructor
        /// @param src Source ShaderConfig to copy from
        ShaderConfig(const ShaderConfig& src) {
            if (src.Name != nullptr) {
                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (src.FilePath != nullptr) {
                strncpy_s(FilePath, src.FilePath, AXR_MAX_FILE_PATH_SIZE);
            }
            if (src.Properties != nullptr) {
                Properties = axrShaderPropertiesClone(src.Properties);
            } else {
                Properties = nullptr;
            }
        }

        /// Move Constructor
        /// @param src Source ShaderConfig to move from
        ShaderConfig(ShaderConfig&& src) noexcept {
            if (src.Name != nullptr) {
                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (src.FilePath != nullptr) {
                strncpy_s(FilePath, src.FilePath, AXR_MAX_FILE_PATH_SIZE);
            }
            Properties = src.Properties;

            memset(src.Name, 0, sizeof(src.Name));
            memset(src.FilePath, 0, sizeof(src.FilePath));

            src.Properties = nullptr;
        }

        // ---- Destructor ----

        /// Destructor
        ~ShaderConfig() {
            cleanup();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source ShaderConfig to copy from
        ShaderConfig& operator=(const ShaderConfig& src) {
            if (this != &src) {
                cleanup();

                if (src.Name != nullptr) {
                    strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                }
                if (src.FilePath != nullptr) {
                    strncpy_s(FilePath, src.FilePath, AXR_MAX_FILE_PATH_SIZE);
                }
                if (src.Properties != nullptr) {
                    Properties = axrShaderPropertiesClone(src.Properties);
                } else {
                    Properties = nullptr;
                }
            }

            return *this;
        }

        /// Move Assignment Operator
        /// @param src Source ShaderConfig to move from
        ShaderConfig& operator=(ShaderConfig&& src) noexcept {
            if (this != &src) {
                cleanup();

                if (src.Name != nullptr) {
                    strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                }
                if (src.FilePath != nullptr) {
                    strncpy_s(FilePath, src.FilePath, AXR_MAX_FILE_PATH_SIZE);
                }
                Properties = src.Properties;

                memset(src.Name, 0, sizeof(src.Name));
                memset(src.FilePath, 0, sizeof(src.FilePath));
                src.Properties = nullptr;
            }

            return *this;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Check if this shader config is valid
        /// @returns True if this shader config is valid
        [[nodiscard]] bool isValid() const {
            return axrShaderConfigIsValid(toRaw());
        }

        /// Get a handle to the ShaderConfig as an AxrShaderConfig
        /// @returns This as an AxrShaderConfig
        const AxrShaderConfig* toRaw() const {
            return reinterpret_cast<const AxrShaderConfig*>(this);
        }

        /// Get a handle to the ShaderConfig as an AxrShaderConfig
        /// @returns This as an AxrShaderConfig
        AxrShaderConfig* toRaw() {
            return reinterpret_cast<AxrShaderConfig*>(this);
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            if (Properties != nullptr) {
                axrShaderPropertiesDestroy(&Properties);
            }

            memset(Name, 0, sizeof(Name));
            memset(FilePath, 0, sizeof(FilePath));
        }
    };

    static_assert(
        sizeof(AxrShaderConfig) == sizeof(axr::ShaderConfig),
        "Original type and wrapper have different size!"
    );

    // ----------------------------------------- //
    // Shader Definition
    // ----------------------------------------- //

    /// Shader
    class Shader {
    public:
        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Constructor
        /// @param shader Shader handle
        Shader(const AxrShader_T shader):
            m_Shader(shader) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get the shader's name
        /// @returns The shader's name
        [[nodiscard]] const char* getName() const {
            return axrShaderGetName(m_Shader);
        }

    private:
        // ----------------------------------------- //
        // Private Variables
        // ----------------------------------------- //
        AxrShader_T m_Shader;
    };

    // ---------------------------------------------------------------------------------- //
    //                                   Shader Values                                    //
    // ---------------------------------------------------------------------------------- //

    // ----------------------------------------- //
    // Enums
    // ----------------------------------------- //

    /// Shader buffer link type enum
    enum class ShaderBufferLinkEnum {
        Undefined = AXR_SHADER_BUFFER_LINK_UNDEFINED,
        UniformBuffer = AXR_SHADER_BUFFER_LINK_UNIFORM_BUFFER,
        ImageSamplerBuffer = AXR_SHADER_BUFFER_LINK_IMAGE_SAMPLER_BUFFER,
    };

    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// Shader values
    struct ShaderValues {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        uint32_t BufferLinkCount;
        AxrShaderBufferLink_T* BufferLinks;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        ShaderValues():
            BufferLinkCount(0),
            BufferLinks(nullptr) {
        }

        /// Copy Constructor
        /// @param src Source ShaderValues to copy from
        ShaderValues(const ShaderValues& src) = delete;
        /// Move Constructor
        /// @param src Source ShaderValues to move from
        ShaderValues(ShaderValues&& src) noexcept = delete;

        // ---- Destructor ----

        /// Destructor
        ~ShaderValues() {
            clearBufferLinks();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source ShaderValues to copy from
        ShaderValues& operator=(const ShaderValues& src) = delete;
        /// Move Assignment Operator
        /// @param src Source ShaderValues to move from
        ShaderValues& operator=(ShaderValues&& src) noexcept = delete;

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Check if these shader values are valid
        /// @returns True if these shader values are valid
        [[nodiscard]] bool isValid() const {
            return axrShaderValuesIsValid(toRaw());
        }

        /// Clone the raw shader values
        /// @returns The cloned raw shader values
        [[nodiscard]] AxrShaderValues_T cloneRaw() const {
            return axrShaderValuesClone(toRaw());
        }

        /// Get a handle to the ShaderValues as an AxrShaderValues
        /// @returns This as an AxrShaderValues
        const AxrShaderValues* toRaw() const {
            return reinterpret_cast<const AxrShaderValues*>(this);
        }

        /// Get a handle to the ShaderValues as an AxrShaderValues
        /// @returns This as an AxrShaderValues
        AxrShaderValues* toRaw() {
            return reinterpret_cast<AxrShaderValues*>(this);
        }

        // ---- Buffer Links ----

        /// Add a uniform buffer link
        /// @param binding Uniform buffer binding
        /// @param bufferName Uniform buffer name 
        void addUniformBufferLink(const uint32_t binding, const char* bufferName) {
            resizeBufferLinks(BufferLinkCount + 1);

            AxrShaderUniformBufferLink bufferLink{
                .Binding = binding,
                .BufferName = {},
            };
            if (bufferName != nullptr) {
                strncpy_s(bufferLink.BufferName, bufferName, AXR_MAX_ASSET_NAME_SIZE);
            }

            BufferLinks[BufferLinkCount - 1] = reinterpret_cast<AxrShaderBufferLink_T>(
                axrShaderUniformBufferLinkClone(&bufferLink)
            );
        }

        /// Add an image sampler buffer link
        /// @param binding Image sampler buffer binding
        /// @param imageName Image name
        /// @param imageSamplerName Image sampler name
        void addImageSamplerBufferLink(const uint32_t binding, const char* imageName, const char* imageSamplerName) {
            resizeBufferLinks(BufferLinkCount + 1);

            AxrShaderImageSamplerBufferLink bufferLink{
                .Binding = binding,
                .ImageName = {},
                .ImageSamplerName = {},
            };
            if (imageName != nullptr) {
                strncpy_s(bufferLink.ImageName, imageName, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (imageSamplerName != nullptr) {
                strncpy_s(bufferLink.ImageSamplerName, imageSamplerName, AXR_MAX_ASSET_NAME_SIZE);
            }

            BufferLinks[BufferLinkCount - 1] = reinterpret_cast<AxrShaderBufferLink_T>(
                axrShaderImageSamplerBufferLinkClone(&bufferLink)
            );
        }

        /// Clear the buffer links 
        void clearBufferLinks() {
            if (BufferLinks == nullptr) return;

            for (uint32_t i = 0; i < BufferLinkCount; ++i) {
                if (BufferLinks[i] == nullptr) continue;

                axrShaderBufferLinkDestroy(&BufferLinks[i]);
            }

            delete[] BufferLinks;
            BufferLinks = nullptr;
            BufferLinkCount = 0;
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        // ---- Buffer Links ----

        /// Resize the buffer links
        /// @param size New size
        void resizeBufferLinks(const uint32_t size) {
            if (size == 0) {
                clearBufferLinks();
                return;
            }

            auto newBufferLinks = new AxrShaderBufferLink_T[size]{};
            for (uint32_t i = 0; i < std::min(BufferLinkCount, size); ++i) {
                // Move buffer links to new array
                newBufferLinks[i] = BufferLinks[i];
                BufferLinks[i] = nullptr;
            }

            clearBufferLinks();
            BufferLinks = newBufferLinks;
            BufferLinkCount = size;
        }
    };

    static_assert(
        sizeof(AxrShaderValues) == sizeof(axr::ShaderValues),
        "Original type and wrapper have different size!"
    );

    // ---------------------------------------------------------------------------------- //
    //                                   Model Assets                                     //
    // ---------------------------------------------------------------------------------- //

    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// Vertex
    struct Vertex {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //

        glm::vec3 Position;
        glm::vec3 Color;
        glm::vec2 TexCoord_0;
        glm::vec2 TexCoord_1;
        glm::vec2 TexCoord_2;
        glm::vec2 TexCoord_3;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        Vertex(): Position(0.0f),
            Color(0.0f),
            TexCoord_0(0.0f),
            TexCoord_1(0.0f),
            TexCoord_2(0.0f),
            TexCoord_3(0.0f) {
        }

        /// Constructor
        /// @param position Vertex position
        /// @param color Vertex color
        /// @param texCoord_0 Vertex Texture Coordinates (Channel 0)
        /// @param texCoord_1 Vertex Texture Coordinates (Channel 1)
        /// @param texCoord_2 Vertex Texture Coordinates (Channel 2)
        /// @param texCoord_3 Vertex Texture Coordinates (Channel 3)
        Vertex(
            const glm::vec3 position,
            const glm::vec3 color,
            const glm::vec2 texCoord_0 = glm::vec2(0.0f),
            const glm::vec2 texCoord_1 = glm::vec2(0.0f),
            const glm::vec2 texCoord_2 = glm::vec2(0.0f),
            const glm::vec2 texCoord_3 = glm::vec2(0.0f)
        ):
            Position(position),
            Color(color),
            TexCoord_0(texCoord_0),
            TexCoord_1(texCoord_1),
            TexCoord_2(texCoord_2),
            TexCoord_3(texCoord_3) {
        }

        /// Copy Constructor
        /// @param src Source Vertex to copy from
        Vertex(const Vertex& src) {
            Position = src.Position;
            Color = src.Color;
            TexCoord_0 = src.TexCoord_0;
            TexCoord_1 = src.TexCoord_1;
            TexCoord_2 = src.TexCoord_2;
            TexCoord_3 = src.TexCoord_3;
        }

        /// Move Constructor
        /// @param src Source Vertex to move from
        Vertex(Vertex&& src) noexcept {
            Position = src.Position;
            Color = src.Color;
            TexCoord_0 = src.TexCoord_0;
            TexCoord_1 = src.TexCoord_1;
            TexCoord_2 = src.TexCoord_2;
            TexCoord_3 = src.TexCoord_3;

            src.Position = glm::vec3(0.0f);
            src.Color = glm::vec3(0.0f);
            src.TexCoord_0 = glm::vec3(0.0f);
            src.TexCoord_1 = glm::vec3(0.0f);
            src.TexCoord_2 = glm::vec3(0.0f);
            src.TexCoord_3 = glm::vec3(0.0f);
        }

        // ---- Destructor ----

        /// Destructor
        ~Vertex() {
            cleanup();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source Vertex to copy from
        Vertex& operator=(const Vertex& src) {
            if (this != &src) {
                cleanup();

                Position = src.Position;
                Color = src.Color;
                TexCoord_0 = src.TexCoord_0;
                TexCoord_1 = src.TexCoord_1;
                TexCoord_2 = src.TexCoord_2;
                TexCoord_3 = src.TexCoord_3;
            }

            return *this;
        }

        /// Move Assignment Operator
        /// @param src Source Vertex to move from
        Vertex& operator=(Vertex&& src) noexcept {
            if (this != &src) {
                cleanup();

                Position = src.Position;
                Color = src.Color;
                TexCoord_0 = src.TexCoord_0;
                TexCoord_1 = src.TexCoord_1;
                TexCoord_2 = src.TexCoord_2;
                TexCoord_3 = src.TexCoord_3;

                src.Position = glm::vec3(0.0f);
                src.Color = glm::vec3(0.0f);
                src.TexCoord_0 = glm::vec3(0.0f);
                src.TexCoord_1 = glm::vec3(0.0f);
                src.TexCoord_2 = glm::vec3(0.0f);
                src.TexCoord_3 = glm::vec3(0.0f);
            }

            return *this;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the Vertex as an AxrVertex
        /// @returns This as an AxrVertex
        const AxrVertex* toRaw() const {
            return reinterpret_cast<const AxrVertex*>(this);
        }

        /// Get a handle to the Vertex as an AxrVertex
        /// @returns This as an AxrVertex
        AxrVertex* toRaw() {
            return reinterpret_cast<AxrVertex*>(this);
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            Position = glm::vec3(0.0f);
            Color = glm::vec3(0.0f);
            TexCoord_0 = glm::vec3(0.0f);
            TexCoord_1 = glm::vec3(0.0f);
            TexCoord_2 = glm::vec3(0.0f);
            TexCoord_3 = glm::vec3(0.0f);
        }
    };

    static_assert(
        sizeof(AxrVertex) == sizeof(axr::Vertex),
        "Original type and wrapper have different size!"
    );

    /// Submesh
    struct Submesh {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //

        uint32_t VertexCount;
        axr::Vertex* Vertices;
        uint32_t IndexCount;
        uint32_t* Indices;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        Submesh(): VertexCount(0),
            Vertices(nullptr),
            IndexCount(0),
            Indices(nullptr) {
        }

        /// Constructor
        /// @param vertices Vertices to copy
        /// @param indices Indices to copy
        Submesh(const std::vector<axr::Vertex>& vertices, const std::vector<uint32_t>& indices) {
            VertexCount = static_cast<uint32_t>(vertices.size());
            Vertices = cloneVertices(VertexCount, vertices.data());
            IndexCount = static_cast<uint32_t>(indices.size());
            Indices = cloneIndices(IndexCount, indices.data());
        }

        /// Copy Constructor
        /// @param src Source Submesh to copy from
        Submesh(const Submesh& src) {
            VertexCount = src.VertexCount;
            Vertices = cloneVertices(src.VertexCount, src.Vertices);
            IndexCount = src.IndexCount;
            Indices = cloneIndices(src.IndexCount, src.Indices);
        }

        /// Move Constructor
        /// @param src Source Submesh to move from
        Submesh(Submesh&& src) noexcept {
            VertexCount = src.VertexCount;
            Vertices = src.Vertices;
            IndexCount = src.IndexCount;
            Indices = src.Indices;

            src.VertexCount = 0;
            src.Vertices = nullptr;
            src.IndexCount = 0;
            src.Indices = nullptr;
        }

        // ---- Destructor ----

        /// Destructor
        ~Submesh() {
            cleanup();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source Submesh to copy from
        Submesh& operator=(const Submesh& src) {
            if (this != &src) {
                cleanup();

                VertexCount = src.VertexCount;
                Vertices = cloneVertices(src.VertexCount, src.Vertices);
                IndexCount = src.IndexCount;
                Indices = cloneIndices(src.IndexCount, src.Indices);
            }

            return *this;
        }

        /// Move Assignment Operator
        /// @param src Source Submesh to move from
        Submesh& operator=(Submesh&& src) noexcept {
            if (this != &src) {
                cleanup();

                VertexCount = src.VertexCount;
                Vertices = src.Vertices;
                IndexCount = src.IndexCount;
                Indices = src.Indices;

                src.VertexCount = 0;
                src.Vertices = nullptr;
                src.IndexCount = 0;
                src.Indices = nullptr;
            }

            return *this;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the Submesh as an AxrSubmesh
        /// @returns This as an AxrSubmesh
        const AxrSubmesh* toRaw() const {
            return reinterpret_cast<const AxrSubmesh*>(this);
        }

        /// Get a handle to the Submesh as an AxrSubmesh
        /// @returns This as an AxrSubmesh
        AxrSubmesh* toRaw() {
            return reinterpret_cast<AxrSubmesh*>(this);
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            axrSubmeshDestroyVertices(&VertexCount, reinterpret_cast<AxrVertex**>(&Vertices));
            axrSubmeshDestroyIndices(&IndexCount, &Indices);
        }

        /// Clone the given vertices
        /// @param vertexCount Number of vertices in the given array
        /// @param vertices Vertex array to clone
        /// @returns A cloned array of the given vertices
        axr::Vertex* cloneVertices(const uint32_t vertexCount, const axr::Vertex* vertices) {
            return reinterpret_cast<axr::Vertex*>(axrSubmeshCloneVertices(
                vertexCount,
                reinterpret_cast<const AxrVertex*>(vertices)
            ));
        }

        /// Clone the given indices
        /// @param indexCount Number of indices in the given array
        /// @param indices Index array to clone
        /// @returns A cloned array of the given indices
        uint32_t* cloneIndices(const uint32_t indexCount, const uint32_t* indices) {
            return axrSubmeshCloneIndices(indexCount, indices);
        }
    };

    static_assert(
        sizeof(AxrSubmesh) == sizeof(axr::Submesh),
        "Original type and wrapper have different size!"
    );

    /// Mesh
    struct Mesh {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //

        uint32_t SubmeshCount;
        axr::Submesh* Submeshes;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        Mesh(): SubmeshCount(0),
            Submeshes(nullptr) {
        }

        /// Constructor
        /// @param submeshes Submeshes to copy
        Mesh(const std::vector<axr::Submesh>& submeshes) {
            SubmeshCount = static_cast<uint32_t>(submeshes.size());
            Submeshes = cloneSubmeshes(SubmeshCount, submeshes.data());
        }

        /// Copy Constructor
        /// @param src Source Mesh to copy from
        Mesh(const Mesh& src) {
            SubmeshCount = src.SubmeshCount;
            Submeshes = cloneSubmeshes(src.SubmeshCount, src.Submeshes);
        }

        /// Move Constructor
        /// @param src Source Mesh to move from
        Mesh(Mesh&& src) noexcept {
            SubmeshCount = src.SubmeshCount;
            Submeshes = src.Submeshes;

            src.SubmeshCount = 0;
            src.Submeshes = nullptr;
        }

        // ---- Destructor ----

        /// Destructor
        ~Mesh() {
            cleanup();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source Mesh to copy from
        Mesh& operator=(const Mesh& src) {
            if (this != &src) {
                cleanup();

                SubmeshCount = src.SubmeshCount;
                Submeshes = cloneSubmeshes(src.SubmeshCount, src.Submeshes);
            }

            return *this;
        }

        /// Move Assignment Operator
        /// @param src Source Mesh to move from
        Mesh& operator=(Mesh&& src) noexcept {
            if (this != &src) {
                cleanup();

                SubmeshCount = src.SubmeshCount;
                Submeshes = src.Submeshes;

                src.SubmeshCount = 0;
                src.Submeshes = nullptr;
            }

            return *this;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the Mesh as an AxrMesh
        /// @returns This as an AxrMesh
        const AxrMesh* toRaw() const {
            return reinterpret_cast<const AxrMesh*>(this);
        }

        /// Get a handle to the Mesh as an AxrMesh
        /// @returns This as an AxrMesh
        AxrMesh* toRaw() {
            return reinterpret_cast<AxrMesh*>(this);
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            axrMeshDestroySubmeshes(&SubmeshCount, reinterpret_cast<AxrSubmesh**>(&Submeshes));
        }

        /// Clone the given submeshes
        /// @param submeshCount Number of submeshes in the given array
        /// @param submeshes Submesh array to clone
        /// @returns A cloned array of the given submeshes
        axr::Submesh* cloneSubmeshes(const uint32_t submeshCount, const axr::Submesh* submeshes) {
            return reinterpret_cast<axr::Submesh*>(axrMeshCloneSubmeshes(
                submeshCount,
                reinterpret_cast<const AxrSubmesh*>(submeshes)
            ));
        }
    };

    static_assert(
        sizeof(AxrMesh) == sizeof(axr::Mesh),
        "Original type and wrapper have different size!"
    );

    /// Model Config
    struct ModelConfig {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //

        char Name[AXR_MAX_ASSET_NAME_SIZE]{};
        char FilePath[AXR_MAX_FILE_PATH_SIZE]{};

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        ModelConfig() {
        }

        /// Constructor
        /// @param name Name of the model
        /// @param filePath Filepath of the model
        ModelConfig(const char* name, const char* filePath) {
            if (name != nullptr) {
                strncpy_s(Name, name, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (filePath != nullptr) {
                strncpy_s(FilePath, filePath, AXR_MAX_FILE_PATH_SIZE);
            }
        }

        /// Copy Constructor
        /// @param src Source ModelConfig to copy from
        ModelConfig(const ModelConfig& src) {
            if (src.Name != nullptr) {
                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (src.FilePath != nullptr) {
                strncpy_s(FilePath, src.FilePath, AXR_MAX_FILE_PATH_SIZE);
            }
        }

        /// Move Constructor
        /// @param src Source ModelConfig to move from
        ModelConfig(ModelConfig&& src) noexcept {
            if (src.Name != nullptr) {
                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (src.FilePath != nullptr) {
                strncpy_s(FilePath, src.FilePath, AXR_MAX_FILE_PATH_SIZE);
            }

            memset(src.Name, 0, sizeof(src.Name));
            memset(src.FilePath, 0, sizeof(src.FilePath));
        }

        // ---- Destructor ----

        /// Destructor
        ~ModelConfig() {
            cleanup();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source ModelConfig to copy from
        ModelConfig& operator=(const ModelConfig& src) {
            if (this != &src) {
                cleanup();

                if (src.Name != nullptr) {
                    strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                }
                if (src.FilePath != nullptr) {
                    strncpy_s(FilePath, src.FilePath, AXR_MAX_FILE_PATH_SIZE);
                }
            }

            return *this;
        }

        /// Move Assignment Operator
        /// @param src Source ModelConfig to move from
        ModelConfig& operator=(ModelConfig&& src) noexcept {
            if (this != &src) {
                cleanup();

                if (src.Name != nullptr) {
                    strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                }
                if (src.FilePath != nullptr) {
                    strncpy_s(FilePath, src.FilePath, AXR_MAX_FILE_PATH_SIZE);
                }

                memset(src.Name, 0, sizeof(src.Name));
                memset(src.FilePath, 0, sizeof(src.FilePath));
            }

            return *this;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the ModelConfig as an AxrModelConfig
        /// @returns This as an AxrModelConfig
        const AxrModelConfig* toRaw() const {
            return reinterpret_cast<const AxrModelConfig*>(this);
        }

        /// Get a handle to the ModelConfig as an AxrModelConfig
        /// @returns This as an AxrModelConfig
        AxrModelConfig* toRaw() {
            return reinterpret_cast<AxrModelConfig*>(this);
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            memset(Name, 0, sizeof(Name));
            memset(FilePath, 0, sizeof(FilePath));
        }
    };

    static_assert(
        sizeof(AxrModelConfig) == sizeof(axr::ModelConfig),
        "Original type and wrapper have different size!"
    );

    // ----------------------------------------- //
    // Model Definition
    // ----------------------------------------- //

    /// Model
    class Model {
    public:
        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Constructor
        /// @param model Model handle
        Model(const AxrModel_T model):
            m_Model(model) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get the model's name
        /// @returns The model's name
        [[nodiscard]] const char* getName() const {
            return axrModelGetName(m_Model);
        }

        /// Set the mesh data for the model
        /// @param meshCount Number of meshes in the array
        /// @param meshes Meshes array
        /// @returns AXR_SUCCESS if the function succeeded
        [[nodiscard]] AxrResult setData(const uint32_t meshCount, const AxrMesh* meshes) {
            return axrModelSetData(m_Model, meshCount, meshes);
        }

    private:
        // ----------------------------------------- //
        // Private Variables
        // ----------------------------------------- //
        AxrModel_T m_Model;
    };

    // ---------------------------------------------------------------------------------- //
    //                                Uniform Buffer Assets                               //
    // ---------------------------------------------------------------------------------- //

    // ----------------------------------------- //
    // Enums
    // ----------------------------------------- //

    /// Uniform buffer type enum
    enum class UniformBufferTypeEnum {
        Undefined = AXR_UNIFORM_BUFFER_TYPE_UNDEFINED,
        Standard = AXR_UNIFORM_BUFFER_TYPE_STANDARD,
        Dynamic = AXR_UNIFORM_BUFFER_TYPE_DYNAMIC,
    };

    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// Dynamic uniform buffer offset config
    struct DynamicUniformBufferOffsetConfig {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        uint32_t Binding = 0;
        uint32_t OffsetIndex = 0;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        DynamicUniformBufferOffsetConfig() = default;

        /// Constructor
        /// @param binding Dynamic uniform buffer binding
        /// @param offsetIndex Dynamic uniform buffer instance index offset
        DynamicUniformBufferOffsetConfig(const uint32_t binding, const uint32_t offsetIndex):
            Binding(binding),
            OffsetIndex(offsetIndex) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the DynamicUniformBufferOffsetConfig as an AxrDynamicUniformBufferOffsetConfig
        /// @returns This as an AxrDynamicUniformBufferOffsetConfig
        const AxrDynamicUniformBufferOffsetConfig* toRaw() const {
            return reinterpret_cast<const AxrDynamicUniformBufferOffsetConfig*>(this);
        }

        /// Get a handle to the DynamicUniformBufferOffsetConfig as an AxrDynamicUniformBufferOffsetConfig
        /// @returns This as an AxrDynamicUniformBufferOffsetConfig
        AxrDynamicUniformBufferOffsetConfig* toRaw() {
            return reinterpret_cast<AxrDynamicUniformBufferOffsetConfig*>(this);
        }
    };

    static_assert(
        sizeof(AxrDynamicUniformBufferOffsetConfig) == sizeof(axr::DynamicUniformBufferOffsetConfig),
        "Original type and wrapper have different size!"
    );

    /// Uniform Buffer Config
    struct UniformBufferConfig {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //

        char Name[AXR_MAX_ASSET_NAME_SIZE]{};
        uint64_t DataSize;
        void* Data;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        UniformBufferConfig():
            DataSize(0),
            Data(nullptr) {
        }

        /// Constructor
        /// @param name Name of the uniform buffer
        /// @param dataSize Data size
        /// @param data Data
        UniformBufferConfig(const char* name, const uint64_t dataSize, const void* data):
            DataSize(dataSize) {
            if (name != nullptr) {
                strncpy_s(Name, name, AXR_MAX_ASSET_NAME_SIZE);
            }
            Data = axrUniformBufferCloneData(dataSize, data);
        }

        /// Copy Constructor
        /// @param src Source UniformBufferConfig to copy from
        UniformBufferConfig(const UniformBufferConfig& src) {
            if (src.Name != nullptr) {
                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            }
            DataSize = src.DataSize;
            Data = axrUniformBufferCloneData(src.DataSize, src.Data);
        }

        /// Move Constructor
        /// @param src Source UniformBufferConfig to move from
        UniformBufferConfig(UniformBufferConfig&& src) noexcept {
            if (src.Name != nullptr) {
                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            }
            DataSize = src.DataSize;
            Data = src.Data;

            memset(src.Name, 0, sizeof(src.Name));
            src.DataSize = 0;
            src.Data = nullptr;
        }

        // ---- Destructor ----

        /// Destructor
        ~UniformBufferConfig() {
            cleanup();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source UniformBufferConfig to copy from
        UniformBufferConfig& operator=(const UniformBufferConfig& src) {
            if (this != &src) {
                cleanup();

                if (src.Name != nullptr) {
                    strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                }
                DataSize = src.DataSize;
                Data = axrUniformBufferCloneData(src.DataSize, src.Data);
            }

            return *this;
        }

        /// Move Assignment Operator
        /// @param src Source UniformBufferConfig to move from
        UniformBufferConfig& operator=(UniformBufferConfig&& src) noexcept {
            if (this != &src) {
                cleanup();

                if (src.Name != nullptr) {
                    strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                }
                DataSize = src.DataSize;
                Data = src.Data;

                memset(src.Name, 0, sizeof(src.Name));
                src.DataSize = 0;
                src.Data = nullptr;
            }

            return *this;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the UniformBufferConfig as an AxrUniformBufferConfig
        /// @returns This as an AxrUniformBufferConfig
        const AxrUniformBufferConfig* toRaw() const {
            return reinterpret_cast<const AxrUniformBufferConfig*>(this);
        }

        /// Get a handle to the UniformBufferConfig as an AxrUniformBufferConfig
        /// @returns This as an AxrUniformBufferConfig
        AxrUniformBufferConfig* toRaw() {
            return reinterpret_cast<AxrUniformBufferConfig*>(this);
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            memset(Name, 0, sizeof(Name));
            axrUniformBufferDestroyData(&DataSize, &Data);
        }
    };

    static_assert(
        sizeof(AxrUniformBufferConfig) == sizeof(axr::UniformBufferConfig),
        "Original type and wrapper have different size!"
    );

    /// Dynamic Uniform Buffer Config
    struct DynamicUniformBufferConfig {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        char Name[AXR_MAX_ASSET_NAME_SIZE]{};
        uint32_t InstanceCount;
        uint64_t InstanceSize;
        void* Data;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        DynamicUniformBufferConfig():
            InstanceCount(0),
            InstanceSize(0),
            Data(nullptr) {
        }

        /// Constructor
        /// @param name Name of the dynamic uniform buffer
        /// @param instanceCount Instance count
        /// @param instanceSize Instance size
        /// @param data Data
        DynamicUniformBufferConfig(
            const char* name,
            const uint32_t instanceCount,
            const uint64_t instanceSize,
            const void* data
        ):
            InstanceCount(instanceCount),
            InstanceSize(instanceSize) {
            if (name != nullptr) {
                strncpy_s(Name, name, AXR_MAX_ASSET_NAME_SIZE);
            }
            Data = axrUniformBufferCloneData(InstanceCount * InstanceSize, data);
        }

        /// Copy Constructor
        /// @param src Source DynamicUniformBufferConfig to copy from
        DynamicUniformBufferConfig(const DynamicUniformBufferConfig& src) {
            strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            InstanceCount = src.InstanceCount;
            InstanceSize = src.InstanceSize;
            Data = axrUniformBufferCloneData(src.InstanceCount * src.InstanceSize, src.Data);
        }

        /// Move Constructor
        /// @param src Source DynamicUniformBufferConfig to move from
        DynamicUniformBufferConfig(DynamicUniformBufferConfig&& src) noexcept {
            strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            InstanceCount = src.InstanceCount;
            InstanceSize = src.InstanceSize;
            Data = src.Data;

            memset(src.Name, 0, sizeof(src.Name));
            src.InstanceCount = 0;
            src.InstanceSize = 0;
            src.Data = nullptr;
        }

        // ---- Destructor ----

        /// Destructor
        ~DynamicUniformBufferConfig() {
            cleanup();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source DynamicUniformBufferConfig to copy from
        DynamicUniformBufferConfig& operator=(const DynamicUniformBufferConfig& src) {
            if (this != &src) {
                cleanup();

                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                InstanceCount = src.InstanceCount;
                InstanceSize = src.InstanceSize;
                Data = axrUniformBufferCloneData(src.InstanceCount * src.InstanceSize, src.Data);
            }

            return *this;
        }

        /// Move Assignment Operator
        /// @param src Source DynamicUniformBufferConfig to move from
        DynamicUniformBufferConfig& operator=(DynamicUniformBufferConfig&& src) noexcept {
            if (this != &src) {
                cleanup();

                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                InstanceCount = src.InstanceCount;
                InstanceSize = src.InstanceSize;
                Data = src.Data;

                memset(src.Name, 0, sizeof(src.Name));
                src.InstanceCount = 0;
                src.InstanceSize = 0;
                src.Data = nullptr;
            }

            return *this;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the DynamicUniformBufferConfig as an AxrDynamicUniformBufferConfig
        /// @returns This as an AxrDynamicUniformBufferConfig
        const AxrDynamicUniformBufferConfig* toRaw() const {
            return reinterpret_cast<const AxrDynamicUniformBufferConfig*>(this);
        }

        /// Get a handle to the DynamicUniformBufferConfig as an AxrDynamicUniformBufferConfig
        /// @returns This as an AxrDynamicUniformBufferConfig
        AxrDynamicUniformBufferConfig* toRaw() {
            return reinterpret_cast<AxrDynamicUniformBufferConfig*>(this);
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            memset(Name, 0, sizeof(Name));
            uint64_t dataSize = InstanceCount * InstanceSize;
            axrUniformBufferDestroyData(&dataSize, &Data);
            InstanceCount = 0;
            InstanceSize = 0;
        }
    };

    static_assert(
        sizeof(AxrDynamicUniformBufferConfig) == sizeof(axr::DynamicUniformBufferConfig),
        "Original type and wrapper have different size!"
    );

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN

    // ---------------------------------------------------------------------------------- //
    //                             Push Constant Buffer Assets                            //
    // ---------------------------------------------------------------------------------- //

    /// Push Constant Buffer Config
    struct PushConstantBufferConfig {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //

        char Name[AXR_MAX_ASSET_NAME_SIZE]{};
        uint32_t DataSize;
        void* Data;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        PushConstantBufferConfig():
            DataSize(0),
            Data(nullptr) {
        }

        /// Constructor
        /// @param name Name of the push constant buffer
        /// @param dataSize Data size
        /// @param data Data
        PushConstantBufferConfig(const char* name, const uint32_t dataSize, const void* data):
            DataSize(dataSize) {
            if (name != nullptr) {
                strncpy_s(Name, name, AXR_MAX_ASSET_NAME_SIZE);
            }
            Data = axrPushConstantBufferCloneData(dataSize, data);
        }

        /// Copy Constructor
        /// @param src Source PushConstantBufferConfig to copy from
        PushConstantBufferConfig(const PushConstantBufferConfig& src) {
            if (src.Name != nullptr) {
                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            }
            DataSize = src.DataSize;
            Data = axrPushConstantBufferCloneData(src.DataSize, src.Data);
        }

        /// Move Constructor
        /// @param src Source PushConstantBufferConfig to move from
        PushConstantBufferConfig(PushConstantBufferConfig&& src) noexcept {
            if (src.Name != nullptr) {
                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            }
            DataSize = src.DataSize;
            Data = src.Data;

            memset(src.Name, 0, sizeof(src.Name));
            src.DataSize = 0;
            src.Data = nullptr;
        }

        // ---- Destructor ----

        /// Destructor
        ~PushConstantBufferConfig() {
            cleanup();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source PushConstantBufferConfig to copy from
        PushConstantBufferConfig& operator=(const PushConstantBufferConfig& src) {
            if (this != &src) {
                cleanup();

                if (src.Name != nullptr) {
                    strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                }
                DataSize = src.DataSize;
                Data = axrPushConstantBufferCloneData(src.DataSize, src.Data);
            }

            return *this;
        }

        /// Move Assignment Operator
        /// @param src Source PushConstantBufferConfig to move from
        PushConstantBufferConfig& operator=(PushConstantBufferConfig&& src) noexcept {
            if (this != &src) {
                cleanup();

                if (src.Name != nullptr) {
                    strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                }
                DataSize = src.DataSize;
                Data = src.Data;

                memset(src.Name, 0, sizeof(src.Name));
                src.DataSize = 0;
                src.Data = nullptr;
            }

            return *this;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the PushConstantBufferConfig as an AxrPushConstantBufferConfig
        /// @returns This as an AxrPushConstantBufferConfig
        const AxrPushConstantBufferConfig* toRaw() const {
            return reinterpret_cast<const AxrPushConstantBufferConfig*>(this);
        }

        /// Get a handle to the PushConstantBufferConfig as an AxrPushConstantBufferConfig
        /// @returns This as an AxrPushConstantBufferConfig
        AxrPushConstantBufferConfig* toRaw() {
            return reinterpret_cast<AxrPushConstantBufferConfig*>(this);
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            memset(Name, 0, sizeof(Name));
            axrPushConstantBufferDestroyData(&DataSize, &Data);
        }
    };

    static_assert(
        sizeof(AxrPushConstantBufferConfig) == sizeof(axr::PushConstantBufferConfig),
        "Original type and wrapper have different size!"
    );
#endif

    // ---------------------------------------------------------------------------------- //
    //                               Image Sampler Assets                                 //
    // ---------------------------------------------------------------------------------- //

    // ----------------------------------------- //
    // Enums
    // ----------------------------------------- //

    /// Image sampler filter enum
    enum class ImageSamplerFilterEnum {
        Undefined = AXR_IMAGE_SAMPLER_FILTER_UNDEFINED,
        Nearest = AXR_IMAGE_SAMPLER_FILTER_NEAREST,
        Linear = AXR_IMAGE_SAMPLER_FILTER_LINEAR,
    };

    /// Image sampler wrap enum
    enum class ImageSamplerWrapEnum {
        Undefined = AXR_IMAGE_SAMPLER_WRAP_UNDEFINED,
        Repeat = AXR_IMAGE_SAMPLER_WRAP_REPEAT,
        MirroredRepeat = AXR_IMAGE_SAMPLER_WRAP_MIRRORED_REPEAT,
        ClampToEdge = AXR_IMAGE_SAMPLER_WRAP_CLAMP_TO_EDGE,
        ClampToBorder = AXR_IMAGE_SAMPLER_WRAP_CLAMP_TO_BORDER,
    };

    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// Image Sampler Config
    struct ImageSamplerConfig {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //

        char Name[AXR_MAX_ASSET_NAME_SIZE]{};
        axr::ImageSamplerFilterEnum MinFilter;
        axr::ImageSamplerFilterEnum MagFilter;
        axr::ImageSamplerFilterEnum MipmapFilter;
        axr::ImageSamplerWrapEnum WrapU;
        axr::ImageSamplerWrapEnum WrapV;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        ImageSamplerConfig():
            MinFilter(axr::ImageSamplerFilterEnum::Undefined),
            MagFilter(axr::ImageSamplerFilterEnum::Undefined),
            MipmapFilter(axr::ImageSamplerFilterEnum::Undefined),
            WrapU(axr::ImageSamplerWrapEnum::Undefined),
            WrapV(axr::ImageSamplerWrapEnum::Undefined) {
        }

        /// Constructor
        /// @param name Name of the image sampler
        /// @param minFilter Image sampler min filter enum
        /// @param magFilter Image sampler mag filter enum
        /// @param mipmapFilter Image sampler mipmap filter enum
        /// @param wrapU Image sampler U axis wrap enum
        /// @param wrapV Image sampler V axis wrap enum
        ImageSamplerConfig(
            const char* name,
            const axr::ImageSamplerFilterEnum minFilter,
            const axr::ImageSamplerFilterEnum magFilter,
            const axr::ImageSamplerFilterEnum mipmapFilter,
            const axr::ImageSamplerWrapEnum wrapU,
            const axr::ImageSamplerWrapEnum wrapV
        ):
            MinFilter(minFilter),
            MagFilter(magFilter),
            MipmapFilter(mipmapFilter),
            WrapU(wrapU),
            WrapV(wrapV) {
            if (name != nullptr) {
                strncpy_s(Name, name, AXR_MAX_ASSET_NAME_SIZE);
            }
        }

        /// Copy Constructor
        /// @param src Source ImageSamplerConfig to copy from
        ImageSamplerConfig(const ImageSamplerConfig& src) {
            if (src.Name != nullptr) {
                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            }
            MinFilter = src.MinFilter;
            MagFilter = src.MagFilter;
            MipmapFilter = src.MipmapFilter;
            WrapU = src.WrapU;
            WrapV = src.WrapV;
        }

        /// Move Constructor
        /// @param src Source ImageSamplerConfig to move from
        ImageSamplerConfig(ImageSamplerConfig&& src) noexcept {
            if (src.Name != nullptr) {
                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            }
            MinFilter = src.MinFilter;
            MagFilter = src.MagFilter;
            MipmapFilter = src.MipmapFilter;
            WrapU = src.WrapU;
            WrapV = src.WrapV;

            memset(src.Name, 0, sizeof(src.Name));
            MinFilter = axr::ImageSamplerFilterEnum::Undefined;
            MagFilter = axr::ImageSamplerFilterEnum::Undefined;
            MipmapFilter = axr::ImageSamplerFilterEnum::Undefined;
            WrapU = axr::ImageSamplerWrapEnum::Undefined;
            WrapV = axr::ImageSamplerWrapEnum::Undefined;
        }

        // ---- Destructor ----

        /// Destructor
        ~ImageSamplerConfig() {
            cleanup();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source ImageSamplerConfig to copy from
        ImageSamplerConfig& operator=(const ImageSamplerConfig& src) {
            if (this != &src) {
                cleanup();

                if (src.Name != nullptr) {
                    strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                }
                MinFilter = src.MinFilter;
                MagFilter = src.MagFilter;
                MipmapFilter = src.MipmapFilter;
                WrapU = src.WrapU;
                WrapV = src.WrapV;
            }

            return *this;
        }

        /// Move Assignment Operator
        /// @param src Source ImageSamplerConfig to move from
        ImageSamplerConfig& operator=(ImageSamplerConfig&& src) noexcept {
            if (this != &src) {
                cleanup();

                if (src.Name != nullptr) {
                    strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                }
                MinFilter = src.MinFilter;
                MagFilter = src.MagFilter;
                MipmapFilter = src.MipmapFilter;
                WrapU = src.WrapU;
                WrapV = src.WrapV;

                memset(src.Name, 0, sizeof(src.Name));
                MinFilter = axr::ImageSamplerFilterEnum::Undefined;
                MagFilter = axr::ImageSamplerFilterEnum::Undefined;
                MipmapFilter = axr::ImageSamplerFilterEnum::Undefined;
                WrapU = axr::ImageSamplerWrapEnum::Undefined;
                WrapV = axr::ImageSamplerWrapEnum::Undefined;
            }

            return *this;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the ImageSamplerConfig as an AxrImageSamplerConfig
        /// @returns This as an AxrImageSamplerConfig
        const AxrImageSamplerConfig* toRaw() const {
            return reinterpret_cast<const AxrImageSamplerConfig*>(this);
        }

        /// Get a handle to the ImageSamplerConfig as an AxrImageSamplerConfig
        /// @returns This as an AxrImageSamplerConfig
        AxrImageSamplerConfig* toRaw() {
            return reinterpret_cast<AxrImageSamplerConfig*>(this);
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            memset(Name, 0, sizeof(Name));
            MinFilter = axr::ImageSamplerFilterEnum::Undefined;
            MagFilter = axr::ImageSamplerFilterEnum::Undefined;
            MipmapFilter = axr::ImageSamplerFilterEnum::Undefined;
            WrapU = axr::ImageSamplerWrapEnum::Undefined;
            WrapV = axr::ImageSamplerWrapEnum::Undefined;
        }
    };

    static_assert(
        sizeof(AxrImageSamplerConfig) == sizeof(axr::ImageSamplerConfig),
        "Original type and wrapper have different size!"
    );

    // ----------------------------------------- //
    // Image Sampler Definition
    // ----------------------------------------- //

    /// Image Sampler
    class ImageSampler {
    public:
        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Constructor
        /// @param imageSampler Image sampler handle
        ImageSampler(const AxrImageSampler_T imageSampler):
            m_ImageSampler(imageSampler) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get the image sampler's name
        /// @returns The image sampler's name
        [[nodiscard]] const char* getName() const {
            return axrImageSamplerGetName(m_ImageSampler);
        }

    private:
        // ----------------------------------------- //
        // Private Variables
        // ----------------------------------------- //
        AxrImageSampler_T m_ImageSampler;
    };

    // ---------------------------------------------------------------------------------- //
    //                                   Image Assets                                     //
    // ---------------------------------------------------------------------------------- //

    // ----------------------------------------- //
    // Enums
    // ----------------------------------------- //

    /// Image color channels enum
    enum class ImageColorChannelsEnum {
        Undefined = AXR_IMAGE_COLOR_CHANNELS_UNDEFINED,
        Gray = AXR_IMAGE_COLOR_CHANNELS_GRAY,
        GrayAlpha = AXR_IMAGE_COLOR_CHANNELS_GRAY_ALPHA,
        RGB = AXR_IMAGE_COLOR_CHANNELS_RGB,
        RGBAlpha = AXR_IMAGE_COLOR_CHANNELS_RGB_ALPHA,
    };

    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// Image Config
    struct ImageConfig {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //

        char Name[AXR_MAX_ASSET_NAME_SIZE]{};
        char FilePath[AXR_MAX_FILE_PATH_SIZE]{};

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        ImageConfig() = default;

        /// Constructor
        /// @param name Name of the image
        /// @param filePath Image file path
        ImageConfig(
            const char* name,
            const char* filePath
        ) {
            if (name != nullptr) {
                strncpy_s(Name, name, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (filePath != nullptr) {
                strncpy_s(FilePath, filePath, AXR_MAX_FILE_PATH_SIZE);
            }
        }

        /// Copy Constructor
        /// @param src Source ImageConfig to copy from
        ImageConfig(const ImageConfig& src) {
            if (src.Name != nullptr) {
                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (src.FilePath != nullptr) {
                strncpy_s(FilePath, src.FilePath, AXR_MAX_FILE_PATH_SIZE);
            }
        }

        /// Move Constructor
        /// @param src Source ImageConfig to move from
        ImageConfig(ImageConfig&& src) noexcept {
            if (src.Name != nullptr) {
                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (src.FilePath != nullptr) {
                strncpy_s(FilePath, src.FilePath, AXR_MAX_FILE_PATH_SIZE);
            }

            memset(src.Name, 0, sizeof(src.Name));
            memset(src.FilePath, 0, sizeof(src.FilePath));
        }

        // ---- Destructor ----

        /// Destructor
        ~ImageConfig() {
            cleanup();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source ImageConfig to copy from
        ImageConfig& operator=(const ImageConfig& src) {
            if (this != &src) {
                cleanup();

                if (src.Name != nullptr) {
                    strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                }
                if (src.FilePath != nullptr) {
                    strncpy_s(FilePath, src.FilePath, AXR_MAX_FILE_PATH_SIZE);
                }
            }

            return *this;
        }

        /// Move Assignment Operator
        /// @param src Source ImageConfig to move from
        ImageConfig& operator=(ImageConfig&& src) noexcept {
            if (this != &src) {
                cleanup();

                if (src.Name != nullptr) {
                    strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                }
                if (src.FilePath != nullptr) {
                    strncpy_s(FilePath, src.FilePath, AXR_MAX_FILE_PATH_SIZE);
                }

                memset(src.Name, 0, sizeof(src.Name));
                memset(src.FilePath, 0, sizeof(src.FilePath));
            }

            return *this;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the ImageConfig as an AxrImageConfig
        /// @returns This as an AxrImageConfig
        const AxrImageConfig* toRaw() const {
            return reinterpret_cast<const AxrImageConfig*>(this);
        }

        /// Get a handle to the ImageConfig as an AxrImageConfig
        /// @returns This as an AxrImageConfig
        AxrImageConfig* toRaw() {
            return reinterpret_cast<AxrImageConfig*>(this);
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            memset(Name, 0, sizeof(Name));
            memset(FilePath, 0, sizeof(FilePath));
        }
    };

    static_assert(
        sizeof(AxrImageConfig) == sizeof(axr::ImageConfig),
        "Original type and wrapper have different size!"
    );

    /// UI Image Data
    struct UIImageData {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        char ImageName[AXR_MAX_ASSET_NAME_SIZE]{};
        char ImageSamplerName[AXR_MAX_ASSET_NAME_SIZE]{};

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        UIImageData() = default;

        /// Constructor
        /// @param imageName Name of the image
        /// @param imageSamplerName Name of the image sampler
        UIImageData(
            const char* imageName,
            const char* imageSamplerName
        ) {
            if (imageName != nullptr) {
                strncpy_s(ImageName, imageName, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (imageSamplerName != nullptr) {
                strncpy_s(ImageSamplerName, imageSamplerName, AXR_MAX_ASSET_NAME_SIZE);
            }
        }

        /// Copy Constructor
        /// @param src Source UIImageData to copy from
        UIImageData(const UIImageData& src) {
            strncpy_s(ImageName, src.ImageName, AXR_MAX_ASSET_NAME_SIZE);
            strncpy_s(ImageSamplerName, src.ImageSamplerName, AXR_MAX_ASSET_NAME_SIZE);
        }

        /// Move Constructor
        /// @param src Source UIImageData to move from
        UIImageData(UIImageData&& src) noexcept {
            strncpy_s(ImageName, src.ImageName, AXR_MAX_ASSET_NAME_SIZE);
            strncpy_s(ImageSamplerName, src.ImageSamplerName, AXR_MAX_ASSET_NAME_SIZE);

            memset(src.ImageName, 0, sizeof(src.ImageName));
            memset(src.ImageSamplerName, 0, sizeof(src.ImageSamplerName));
        }

        // ---- Destructor ----

        /// Destructor
        ~UIImageData() {
            cleanup();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source UIImageData to copy from
        UIImageData& operator=(const UIImageData& src) {
            if (this != &src) {
                cleanup();

                strncpy_s(ImageName, src.ImageName, AXR_MAX_ASSET_NAME_SIZE);
                strncpy_s(ImageSamplerName, src.ImageSamplerName, AXR_MAX_ASSET_NAME_SIZE);
            }

            return *this;
        }

        /// Move Assignment Operator
        /// @param src Source UIImageData to move from
        UIImageData& operator=(UIImageData&& src) noexcept {
            if (this != &src) {
                cleanup();

                strncpy_s(ImageName, src.ImageName, AXR_MAX_ASSET_NAME_SIZE);
                strncpy_s(ImageSamplerName, src.ImageSamplerName, AXR_MAX_ASSET_NAME_SIZE);

                memset(src.ImageName, 0, sizeof(src.ImageName));
                memset(src.ImageSamplerName, 0, sizeof(src.ImageSamplerName));
            }

            return *this;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the UIImageData as an AxrUIImageData
        /// @returns This as an AxrUIImageData
        const AxrUIImageData* toRaw() const {
            return reinterpret_cast<const AxrUIImageData*>(this);
        }

        /// Get a handle to the UIImageData as an AxrUIImageData
        /// @returns This as an AxrUIImageData
        AxrUIImageData* toRaw() {
            return reinterpret_cast<AxrUIImageData*>(this);
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            memset(ImageName, 0, sizeof(ImageName));
            memset(ImageSamplerName, 0, sizeof(ImageSamplerName));
        }
    };

    static_assert(
        sizeof(AxrUIImageData) == sizeof(axr::UIImageData),
        "Original type and wrapper have different size!"
    );

    // ----------------------------------------- //
    // Image Definition
    // ----------------------------------------- //

    /// Image
    class Image {
    public:
        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Constructor
        /// @param image Image handle
        Image(const AxrImage_T image):
            m_Image(image) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get the image's name
        /// @returns The image's name
        [[nodiscard]] const char* getName() const {
            return axrImageGetName(m_Image);
        }

        /// Set the image data
        /// @param width Image width
        /// @param height Image height
        /// @param colorChannels Image number of color channels
        /// @param data Image data. Stored from left-to-right, top-to-bottom. Each pixel contains a value for each 'colorChannel', stored with 8-bits
        /// per channel, in the following order: 1=Y, 2=YA, 3=RGB, 4=RGBA. (Y is monochrome color.)
        /// @returns AXR_SUCCESS if the function succeeded
        [[nodiscard]] axr::Result setData(
            const uint32_t width,
            const uint32_t height,
            const axr::ImageColorChannelsEnum colorChannels,
            const stbi_uc* data
        ) const {
            return static_cast<axr::Result>(axrImageSetData(
                m_Image,
                width,
                height,
                static_cast<AxrImageColorChannelsEnum>(colorChannels),
                data
            ));
        }

    private:
        // ----------------------------------------- //
        // Private Variables
        // ----------------------------------------- //
        AxrImage_T m_Image;
    };

    // ---------------------------------------------------------------------------------- //
    //                                  Material Assets                                   //
    // ---------------------------------------------------------------------------------- //

    // ----------------------------------------- //
    // Enums
    // ----------------------------------------- //

    /// Material backface culling mode enum
    enum class MaterialBackfaceCullModeEnum {
        None = AXR_MATERIAL_BACKFACE_CULL_MODE_NONE,
        Front = AXR_MATERIAL_BACKFACE_CULL_MODE_FRONT,
        Back = AXR_MATERIAL_BACKFACE_CULL_MODE_BACK,
        FrontAndBack = AXR_MATERIAL_BACKFACE_CULL_MODE_FRONT_AND_BACK,
    };

    /// Material alpha rendering mode enum
    enum class MaterialAlphaRenderModeEnum {
        Opaque = AXR_MATERIAL_ALPHA_RENDER_MODE_OPAQUE,
        /// Depth sorted alpha blending transparency.
        /// Useful for glass windows or objects with minimal or no overlapping transparency.
        AlphaBlend = AXR_MATERIAL_ALPHA_RENDER_MODE_ALPHA_BLEND,
        /// Order independent transparency.
        /// Useful when there are multiple layers of transparency overlapping.
        /// Whether it's multiple objects or a single complex object. 
        OIT = AXR_MATERIAL_ALPHA_RENDER_MODE_OIT,
    };

    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// Material Config
    struct MaterialConfig {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        char Name[AXR_MAX_ASSET_NAME_SIZE]{};
        char VertexShaderName[AXR_MAX_ASSET_NAME_SIZE]{};
        char FragmentShaderName[AXR_MAX_ASSET_NAME_SIZE]{};
        AxrShaderValues_T VertexShaderValues = nullptr;
        AxrShaderValues_T FragmentShaderValues = nullptr;
        axr::MaterialBackfaceCullModeEnum BackfaceCullMode = axr::MaterialBackfaceCullModeEnum::None;
        axr::MaterialAlphaRenderModeEnum AlphaRenderMode = axr::MaterialAlphaRenderModeEnum::Opaque;
        bool EnableDepthTest = false;
        bool EnableDepthWrite = false;
        uint32_t DynamicUniformBufferOffsetCount = 0;
        axr::DynamicUniformBufferOffsetConfig* DynamicUniformBufferOffsets = nullptr;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        MaterialConfig() = default;

        /// Constructor
        /// @param name The material name
        /// @param vertexShaderName The vertex shader name
        /// @param fragmentShaderName The fragment shader name
        /// @param vertexShaderValues The vertex shader values to use
        /// @param fragmentShaderValues The fragment shader values to use
        /// @param backfaceCullMode The material backface culling mode
        /// @param alphaRenderMode The material alpha rendering mode
        /// @param enableDepthTest Whether to enable depth testing or not
        /// @param enableDepthWrite Whether to enable depth writing or not
        /// @param dynamicUniformBufferOffsetCount Dynamic uniform buffer offset count
        /// @param dynamicUniformBufferOffsets Dynamic uniform buffer offsets
        MaterialConfig(
            const char* name,
            const char* vertexShaderName,
            const char* fragmentShaderName,
            const axr::ShaderValues& vertexShaderValues,
            const axr::ShaderValues& fragmentShaderValues,
            const axr::MaterialBackfaceCullModeEnum backfaceCullMode,
            const axr::MaterialAlphaRenderModeEnum alphaRenderMode,
            const bool enableDepthTest,
            const bool enableDepthWrite,
            const uint32_t dynamicUniformBufferOffsetCount,
            const DynamicUniformBufferOffsetConfig* dynamicUniformBufferOffsets
        ): VertexShaderValues(vertexShaderValues.cloneRaw()),
            FragmentShaderValues(fragmentShaderValues.cloneRaw()),
            BackfaceCullMode(backfaceCullMode),
            AlphaRenderMode(alphaRenderMode),
            EnableDepthTest(enableDepthTest),
            EnableDepthWrite(enableDepthWrite) {
            if (name != nullptr) {
                strncpy_s(Name, name, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (vertexShaderName != nullptr) {
                strncpy_s(VertexShaderName, vertexShaderName, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (fragmentShaderName != nullptr) {
                strncpy_s(FragmentShaderName, fragmentShaderName, AXR_MAX_ASSET_NAME_SIZE);
            }

            DynamicUniformBufferOffsetCount = dynamicUniformBufferOffsetCount;
            DynamicUniformBufferOffsets = clone(
                dynamicUniformBufferOffsetCount,
                dynamicUniformBufferOffsets
            );
        }

        /// Copy Constructor
        /// @param src Source MaterialConfig to copy from
        MaterialConfig(const axr::MaterialConfig& src) {
            strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            strncpy_s(VertexShaderName, src.VertexShaderName, AXR_MAX_ASSET_NAME_SIZE);
            strncpy_s(FragmentShaderName, src.FragmentShaderName, AXR_MAX_ASSET_NAME_SIZE);

            if (src.VertexShaderValues != nullptr) {
                VertexShaderValues = axrShaderValuesClone(src.VertexShaderValues);
            } else {
                VertexShaderValues = nullptr;
            }

            if (src.FragmentShaderValues != nullptr) {
                FragmentShaderValues = axrShaderValuesClone(src.FragmentShaderValues);
            } else {
                FragmentShaderValues = nullptr;
            }

            BackfaceCullMode = src.BackfaceCullMode;
            AlphaRenderMode = src.AlphaRenderMode;
            EnableDepthTest = src.EnableDepthTest;
            EnableDepthWrite = src.EnableDepthWrite;
            DynamicUniformBufferOffsetCount = src.DynamicUniformBufferOffsetCount;
            DynamicUniformBufferOffsets = clone(
                src.DynamicUniformBufferOffsetCount,
                src.DynamicUniformBufferOffsets
            );
        }

        /// Move Constructor
        /// @param src Source MaterialConfig to move from
        MaterialConfig(MaterialConfig&& src) noexcept {
            strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            strncpy_s(VertexShaderName, src.VertexShaderName, AXR_MAX_ASSET_NAME_SIZE);
            strncpy_s(FragmentShaderName, src.FragmentShaderName, AXR_MAX_ASSET_NAME_SIZE);

            VertexShaderValues = src.VertexShaderValues;
            FragmentShaderValues = src.FragmentShaderValues;
            BackfaceCullMode = src.BackfaceCullMode;
            AlphaRenderMode = src.AlphaRenderMode;
            EnableDepthTest = src.EnableDepthTest;
            EnableDepthWrite = src.EnableDepthWrite;
            DynamicUniformBufferOffsetCount = src.DynamicUniformBufferOffsetCount;
            DynamicUniformBufferOffsets = src.DynamicUniformBufferOffsets;

            memset(src.Name, 0, sizeof(src.Name));
            memset(src.VertexShaderName, 0, sizeof(src.VertexShaderName));
            memset(src.FragmentShaderName, 0, sizeof(src.FragmentShaderName));
            src.VertexShaderValues = nullptr;
            src.FragmentShaderValues = nullptr;
            src.BackfaceCullMode = axr::MaterialBackfaceCullModeEnum::None;
            src.AlphaRenderMode = axr::MaterialAlphaRenderModeEnum::Opaque;
            src.EnableDepthTest = false;
            src.EnableDepthWrite = false;
            src.DynamicUniformBufferOffsetCount = 0;
            src.DynamicUniformBufferOffsets = nullptr;
        }

        // ---- Destructor ----

        /// Destructor
        ~MaterialConfig() {
            cleanup();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source MaterialConfig to copy from
        MaterialConfig& operator=(const MaterialConfig& src) {
            if (this != &src) {
                cleanup();

                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                strncpy_s(VertexShaderName, src.VertexShaderName, AXR_MAX_ASSET_NAME_SIZE);
                strncpy_s(FragmentShaderName, src.FragmentShaderName, AXR_MAX_ASSET_NAME_SIZE);

                if (src.VertexShaderValues != nullptr) {
                    VertexShaderValues = axrShaderValuesClone(src.VertexShaderValues);
                } else {
                    VertexShaderValues = nullptr;
                }

                if (src.FragmentShaderValues != nullptr) {
                    FragmentShaderValues = axrShaderValuesClone(src.FragmentShaderValues);
                } else {
                    FragmentShaderValues = nullptr;
                }

                BackfaceCullMode = src.BackfaceCullMode;
                AlphaRenderMode = src.AlphaRenderMode;
                EnableDepthTest = src.EnableDepthTest;
                EnableDepthWrite = src.EnableDepthWrite;
                DynamicUniformBufferOffsetCount = src.DynamicUniformBufferOffsetCount;
                DynamicUniformBufferOffsets = clone(
                    src.DynamicUniformBufferOffsetCount,
                    src.DynamicUniformBufferOffsets
                );
            }

            return *this;
        }

        /// Move Assignment Operator
        /// @param src Source MaterialConfig to move from
        MaterialConfig& operator=(MaterialConfig&& src) noexcept {
            if (this != &src) {
                cleanup();

                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                strncpy_s(VertexShaderName, src.VertexShaderName, AXR_MAX_ASSET_NAME_SIZE);
                strncpy_s(FragmentShaderName, src.FragmentShaderName, AXR_MAX_ASSET_NAME_SIZE);

                VertexShaderValues = src.VertexShaderValues;
                FragmentShaderValues = src.FragmentShaderValues;
                BackfaceCullMode = src.BackfaceCullMode;
                AlphaRenderMode = src.AlphaRenderMode;
                EnableDepthTest = src.EnableDepthTest;
                EnableDepthWrite = src.EnableDepthWrite;
                DynamicUniformBufferOffsetCount = src.DynamicUniformBufferOffsetCount;
                DynamicUniformBufferOffsets = src.DynamicUniformBufferOffsets;

                memset(src.Name, 0, sizeof(src.Name));
                memset(src.VertexShaderName, 0, sizeof(src.VertexShaderName));
                memset(src.FragmentShaderName, 0, sizeof(src.FragmentShaderName));
                src.VertexShaderValues = nullptr;
                src.FragmentShaderValues = nullptr;
                src.BackfaceCullMode = axr::MaterialBackfaceCullModeEnum::None;
                src.AlphaRenderMode = axr::MaterialAlphaRenderModeEnum::Opaque;
                src.EnableDepthTest = false;
                src.EnableDepthWrite = false;
                src.DynamicUniformBufferOffsetCount = 0;
                src.DynamicUniformBufferOffsets = nullptr;
            }

            return *this;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Check if this material config is valid
        /// @returns True if this material config is valid
        [[nodiscard]] bool isValid() const {
            return axrMaterialConfigIsValid(toRaw());
        }

        /// Get a handle to the MaterialConfig as an AxrMaterialConfig
        /// @returns This as an AxrMaterialConfig
        const AxrMaterialConfig* toRaw() const {
            return reinterpret_cast<const AxrMaterialConfig*>(this);
        }

        /// Get a handle to the MaterialConfig as an AxrMaterialConfig
        /// @returns This as an AxrMaterialConfig
        AxrMaterialConfig* toRaw() {
            return reinterpret_cast<AxrMaterialConfig*>(this);
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            if (VertexShaderValues != nullptr) {
                axrShaderValuesDestroy(&VertexShaderValues);
            }
            if (FragmentShaderValues != nullptr) {
                axrShaderValuesDestroy(&FragmentShaderValues);
            }

            destroy(DynamicUniformBufferOffsetCount, DynamicUniformBufferOffsets);

            memset(Name, 0, sizeof(Name));
            memset(VertexShaderName, 0, sizeof(VertexShaderName));
            memset(FragmentShaderName, 0, sizeof(FragmentShaderName));
            BackfaceCullMode = axr::MaterialBackfaceCullModeEnum::None;
            AlphaRenderMode = axr::MaterialAlphaRenderModeEnum::Opaque;
            EnableDepthTest = false;
            EnableDepthWrite = false;
        }

        /// Clone the given dynamic uniform buffer configs
        /// @param count Dynamic uniform buffer config array count
        /// @param data Dynamic uniform buffer config array
        /// @returns The cloned dynamic uniform buffer configs
        DynamicUniformBufferOffsetConfig* clone(
            const uint32_t count,
            const DynamicUniformBufferOffsetConfig* data
        ) const {
            if (count == 0 || data == nullptr) return nullptr;

            const auto clonedData = new DynamicUniformBufferOffsetConfig[count];
            for (uint32_t i = 0; i < count; ++i) {
                clonedData[i] = data[i];
            }
            return clonedData;
        }

        /// Destroy the given dynamic uniform buffer configs
        /// @param count Dynamic uniform buffer config array count
        /// @param data Dynamic uniform buffer config array
        void destroy(uint32_t& count, DynamicUniformBufferOffsetConfig*& data) const {
            if (count == 0 || data == nullptr) return;

            delete[] data;
            data = nullptr;
            count = 0;
        }
    };

    static_assert(
        sizeof(AxrMaterialConfig) == sizeof(axr::MaterialConfig),
        "Original type and wrapper have different size!"
    );

    // ----------------------------------------- //
    // Material Definition
    // ----------------------------------------- //

    /// Material
    class Material {
    public:
        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Constructor
        /// @param material Material handle
        Material(const AxrMaterial_T material):
            m_Material(material) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get the material's name
        /// @returns The material's name
        [[nodiscard]] const char* getName() const {
            return axrMaterialGetName(m_Material);
        }

    private:
        // ----------------------------------------- //
        // Private Variables
        // ----------------------------------------- //
        AxrMaterial_T m_Material;
    };

    // ---------------------------------------------------------------------------------- //
    //                                   Font Assets                                     //
    // ---------------------------------------------------------------------------------- //

    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// Font Config
    struct FontConfig {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        char Name[AXR_MAX_ASSET_NAME_SIZE]{};
        char AtlasImageName[AXR_MAX_ASSET_NAME_SIZE]{};
        char AtlasImageSamplerName[AXR_MAX_ASSET_NAME_SIZE]{};
        /// Must be a .json file
        char AtlasLayoutFilePath[AXR_MAX_FILE_PATH_SIZE]{};

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        FontConfig() = default;

        /// Constructor
        /// @param name Name of the font
        /// @param atlasImageName Name of the font atlas image
        /// @param atlasImageSamplerName Name of the font atlas image sampler
        /// @param atlasLayoutFilePath Font atlas layout file path
        FontConfig(
            const char* name,
            const char* atlasImageName,
            const char* atlasImageSamplerName,
            const char* atlasLayoutFilePath
        ) {
            if (name != nullptr) {
                strncpy_s(Name, name, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (atlasImageName != nullptr) {
                strncpy_s(AtlasImageName, atlasImageName, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (atlasImageSamplerName != nullptr) {
                strncpy_s(AtlasImageSamplerName, atlasImageSamplerName, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (atlasLayoutFilePath != nullptr) {
                strncpy_s(AtlasLayoutFilePath, atlasLayoutFilePath, AXR_MAX_FILE_PATH_SIZE);
            }
        }

        /// Copy Constructor
        /// @param src Source FontConfig to copy from
        FontConfig(const FontConfig& src) {
            strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            strncpy_s(AtlasImageName, src.AtlasImageName, AXR_MAX_ASSET_NAME_SIZE);
            strncpy_s(AtlasImageSamplerName, src.AtlasImageSamplerName, AXR_MAX_ASSET_NAME_SIZE);
            strncpy_s(AtlasLayoutFilePath, src.AtlasLayoutFilePath, AXR_MAX_FILE_PATH_SIZE);
        }

        /// Move Constructor
        /// @param src Source FontConfig to move from
        FontConfig(FontConfig&& src) noexcept {
            strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            strncpy_s(AtlasImageName, src.AtlasImageName, AXR_MAX_ASSET_NAME_SIZE);
            strncpy_s(AtlasImageSamplerName, src.AtlasImageSamplerName, AXR_MAX_ASSET_NAME_SIZE);
            strncpy_s(AtlasLayoutFilePath, src.AtlasLayoutFilePath, AXR_MAX_FILE_PATH_SIZE);

            memset(src.Name, 0, sizeof(src.Name));
            memset(src.AtlasImageName, 0, sizeof(src.AtlasImageName));
            memset(src.AtlasImageSamplerName, 0, sizeof(src.AtlasImageSamplerName));
            memset(src.AtlasLayoutFilePath, 0, sizeof(src.AtlasLayoutFilePath));
        }

        // ---- Destructor ----

        /// Destructor
        ~FontConfig() {
            cleanup();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source FontConfig to copy from
        FontConfig& operator=(const FontConfig& src) {
            if (this != &src) {
                cleanup();

                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                strncpy_s(AtlasImageName, src.AtlasImageName, AXR_MAX_ASSET_NAME_SIZE);
                strncpy_s(AtlasImageSamplerName, src.AtlasImageSamplerName, AXR_MAX_ASSET_NAME_SIZE);
                strncpy_s(AtlasLayoutFilePath, src.AtlasLayoutFilePath, AXR_MAX_FILE_PATH_SIZE);
            }

            return *this;
        }

        /// Move Assignment Operator
        /// @param src Source FontConfig to move from
        FontConfig& operator=(FontConfig&& src) noexcept {
            if (this != &src) {
                cleanup();

                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                strncpy_s(AtlasImageName, src.AtlasImageName, AXR_MAX_ASSET_NAME_SIZE);
                strncpy_s(AtlasImageSamplerName, src.AtlasImageSamplerName, AXR_MAX_ASSET_NAME_SIZE);
                strncpy_s(AtlasLayoutFilePath, src.AtlasLayoutFilePath, AXR_MAX_FILE_PATH_SIZE);

                memset(src.Name, 0, sizeof(src.Name));
                memset(src.AtlasImageName, 0, sizeof(src.AtlasImageName));
                memset(src.AtlasImageSamplerName, 0, sizeof(src.AtlasImageSamplerName));
                memset(src.AtlasLayoutFilePath, 0, sizeof(src.AtlasLayoutFilePath));
            }

            return *this;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the FontConfig as an AxrFontConfig
        /// @returns This as an AxrFontConfig
        const AxrFontConfig* toRaw() const {
            return reinterpret_cast<const AxrFontConfig*>(this);
        }

        /// Get a handle to the FontConfig as an AxrFontConfig
        /// @returns This as an AxrFontConfig
        AxrFontConfig* toRaw() {
            return reinterpret_cast<AxrFontConfig*>(this);
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            memset(Name, 0, sizeof(Name));
            memset(AtlasImageName, 0, sizeof(AtlasImageName));
            memset(AtlasImageSamplerName, 0, sizeof(AtlasImageSamplerName));
            memset(AtlasLayoutFilePath, 0, sizeof(AtlasLayoutFilePath));
        }
    };

    static_assert(
        sizeof(AxrFontConfig) == sizeof(axr::FontConfig),
        "Original type and wrapper have different size!"
    );

    // ----------------------------------------- //
    // Font Definition
    // ----------------------------------------- //

    /// Font
    class Font {
    public:
        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Constructor
        /// @param font Font handle
        explicit Font(const AxrFont_T font):
            m_Font(font) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get the font's name
        /// @returns The font's name
        [[nodiscard]] const char* getName() const {
            return axrFontGetName(m_Font);
        }

        /// Get the font's ID
        /// @returns The font's ID
        [[nodiscard]] uint16_t getID() const {
            return axrFontGetID(m_Font);
        }

    private:
        // ----------------------------------------- //
        // Private Variables
        // ----------------------------------------- //
        AxrFont_T m_Font;
    };

    // ---------------------------------------------------------------------------------- //
    //                               Engine Defined Assets                                //
    // ---------------------------------------------------------------------------------- //

    // ----------------------------------------- //
    // Enums
    // ----------------------------------------- //

    /// Axr engine defined assets enum
    enum class EngineAssetEnum {
        Undefined = AXR_ENGINE_ASSET_UNDEFINED,

        // ---- Shaders ----
        ShaderStart = AXR_ENGINE_ASSET_SHADER_START,
        ShaderDefaultVert = AXR_ENGINE_ASSET_SHADER_DEFAULT_VERT,
        ShaderDefaultFrag = AXR_ENGINE_ASSET_SHADER_DEFAULT_FRAG,
        ShaderDefaultFrag_Mask = AXR_ENGINE_ASSET_SHADER_DEFAULT_FRAG_MASK,
        ShaderUIElementVert = AXR_ENGINE_ASSET_SHADER_UI_ELEMENT_VERT,
        ShaderUIRectangleFrag = AXR_ENGINE_ASSET_SHADER_UI_RECTANGLE_FRAG,
        ShaderUIBorderFrag = AXR_ENGINE_ASSET_SHADER_UI_BORDER_FRAG,
        ShaderUIImageFrag = AXR_ENGINE_ASSET_SHADER_UI_IMAGE_FRAG,
        ShaderUITextFrag = AXR_ENGINE_ASSET_SHADER_UI_TEXT_FRAG,
        ShaderEnd = AXR_ENGINE_ASSET_SHADER_END,

        // ---- Uniform Buffers ----
        UniformBufferStart = AXR_ENGINE_ASSET_UNIFORM_BUFFER_START,
        UniformBufferSceneData = AXR_ENGINE_ASSET_UNIFORM_BUFFER_SCENE_DATA,
        UniformBufferCameraData = AXR_ENGINE_ASSET_UNIFORM_BUFFER_CAMERA_DATA,
        UniformBufferUICanvas = AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_CANVAS,
        UniformBufferUIElements = AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_ELEMENTS,
        UniformBufferEnd = AXR_ENGINE_ASSET_UNIFORM_BUFFER_END,

        // ---- Push Constant Buffers ----
        PushConstantBufferStart = AXR_ENGINE_ASSET_PUSH_CONSTANT_BUFFER_START,
        PushConstantBufferModelMatrix = AXR_ENGINE_ASSET_PUSH_CONSTANT_BUFFER_MODEL_MATRIX,
        PushConstantBufferMvpMatrix = AXR_ENGINE_ASSET_PUSH_CONSTANT_BUFFER_MVP_MATRIX,
        PushConstantBufferEnd = AXR_ENGINE_ASSET_PUSH_CONSTANT_BUFFER_END,

        // ---- Images ----
        ImageStart = AXR_ENGINE_ASSET_IMAGE_START,
        ImageMissingTexture = AXR_ENGINE_ASSET_IMAGE_MISSING_TEXTURE,
        ImageUvTester = AXR_ENGINE_ASSET_IMAGE_UV_TESTER,
        ImageFontAtlasJetbrainsMono_Regular = AXR_ENGINE_ASSET_IMAGE_FONT_ATLAS_JETBRAINS_MONO_REGULAR,
        ImageEnd = AXR_ENGINE_ASSET_IMAGE_END,

        // ---- Image Samplers ----
        ImageSamplerStart = AXR_ENGINE_ASSET_IMAGE_SAMPLER_START,
        ImageSamplerNearestRepeat = AXR_ENGINE_ASSET_IMAGE_SAMPLER_NEAREST_REPEAT,
        ImageSamplerLinearRepeat = AXR_ENGINE_ASSET_IMAGE_SAMPLER_LINEAR_REPEAT,
        ImageSamplerEnd = AXR_ENGINE_ASSET_IMAGE_SAMPLER_END,

        // ---- Materials ----
        MaterialStart = AXR_ENGINE_ASSET_MATERIAL_START,
        MaterialUIRectangle = AXR_ENGINE_ASSET_MATERIAL_UI_RECTANGLE,
        MaterialUIBorder = AXR_ENGINE_ASSET_MATERIAL_UI_BORDER,
        MaterialUIImage = AXR_ENGINE_ASSET_MATERIAL_UI_IMAGE,
        MaterialUIText = AXR_ENGINE_ASSET_MATERIAL_UI_TEXT,
        MaterialEnd = AXR_ENGINE_ASSET_MATERIAL_END,

        // ---- Models ----
        ModelStart = AXR_ENGINE_ASSET_MODEL_START,
        ModelTriangle = AXR_ENGINE_ASSET_MODEL_TRIANGLE,
        ModelSquare = AXR_ENGINE_ASSET_MODEL_SQUARE,
        ModelCube = AXR_ENGINE_ASSET_MODEL_CUBE,
        ModelUISquare = AXR_ENGINE_ASSET_MODEL_UI_RECTANGLE,
        ModelEnd = AXR_ENGINE_ASSET_MODEL_END,

        // ---- Fonts ----
        FontStart = AXR_ENGINE_ASSET_FONT_START,
        FontJetbrainsMono_Regular = AXR_ENGINE_ASSET_FONT_JETBRAINS_MONO_REGULAR,
        FontEnd = AXR_ENGINE_ASSET_FONT_END,
    };

    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    // ---- Uniform Buffers ----

    /// Engine asset uniform buffer named 'Scene Data' structure
    struct alignas(16) EngineAssetUniformBuffer_SceneData {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        alignas(16) glm::mat4 ViewMatrix = {};
        alignas(16) glm::mat4 ProjectionMatrix = {};
        alignas(16) glm::mat4 ViewProjectionMatrix = {};

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        EngineAssetUniformBuffer_SceneData() = default;

        /// Constructor
        /// @param viewMatrix The view matrix
        /// @param projectionMatrix The projection matrix
        EngineAssetUniformBuffer_SceneData(
            const glm::mat4& viewMatrix,
            const glm::mat4& projectionMatrix
        ): ViewMatrix(viewMatrix),
            ProjectionMatrix(projectionMatrix),
            ViewProjectionMatrix(projectionMatrix * viewMatrix) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the EngineAssetUniformBuffer_SceneData as an AxrEngineAssetUniformBuffer_SceneData
        /// @returns This as an AxrEngineAssetUniformBuffer_SceneData
        const AxrEngineAssetUniformBuffer_SceneData* toRaw() const {
            return reinterpret_cast<const AxrEngineAssetUniformBuffer_SceneData*>(this);
        }

        /// Get a handle to the EngineAssetUniformBuffer_SceneData as an AxrEngineAssetUniformBuffer_SceneData
        /// @returns This as an AxrEngineAssetUniformBuffer_SceneData
        AxrEngineAssetUniformBuffer_SceneData* toRaw() {
            return reinterpret_cast<AxrEngineAssetUniformBuffer_SceneData*>(this);
        }
    };

    static_assert(
        sizeof(AxrEngineAssetUniformBuffer_SceneData) == sizeof(axr::EngineAssetUniformBuffer_SceneData),
        "Original type and wrapper have different size!"
    );

    /// Engine asset uniform buffer named 'Camera Data' structure
    struct alignas(16) EngineAssetUniformBuffer_CameraData {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        alignas(8) glm::vec2 Dimensions = {};
        alignas(4) float CameraNearPlane = 0.0f;
        alignas(4) float CameraFarPlane = 0.0f;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        EngineAssetUniformBuffer_CameraData() = default;

        /// Constructor
        /// @param dimensions The screen dimensions
        /// @param cameraNearPlane The cameras near clipping plane
        /// @param cameraFarPlane The cameras far clipping plane
        EngineAssetUniformBuffer_CameraData(
            const glm::vec2& dimensions,
            const float cameraNearPlane,
            const float cameraFarPlane
        ): Dimensions(dimensions),
            CameraNearPlane(cameraNearPlane),
            CameraFarPlane(cameraFarPlane) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the EngineAssetUniformBuffer_CameraData as an AxrEngineAssetUniformBuffer_CameraData
        /// @returns This as an AxrEngineAssetUniformBuffer_CameraData
        const AxrEngineAssetUniformBuffer_CameraData* toRaw() const {
            return reinterpret_cast<const AxrEngineAssetUniformBuffer_CameraData*>(this);
        }

        /// Get a handle to the EngineAssetUniformBuffer_CameraData as an AxrEngineAssetUniformBuffer_CameraData
        /// @returns This as an AxrEngineAssetUniformBuffer_CameraData
        AxrEngineAssetUniformBuffer_CameraData* toRaw() {
            return reinterpret_cast<AxrEngineAssetUniformBuffer_CameraData*>(this);
        }
    };

    static_assert(
        sizeof(AxrEngineAssetUniformBuffer_CameraData) == sizeof(axr::EngineAssetUniformBuffer_CameraData),
        "Original type and wrapper have different size!"
    );

    /// Engine asset uniform buffer named 'UI Canvas' structure
    struct alignas(16) EngineAssetUniformBuffer_UICanvas {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        alignas(8) glm::vec2 Size = {};
        float _padding[2] = {0.0f, 0.0f};

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        EngineAssetUniformBuffer_UICanvas() = default;

        /// Constructor
        /// @param size The UI canvas size
        EngineAssetUniformBuffer_UICanvas(
            const glm::vec2& size
        ): Size(size) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the EngineAssetUniformBuffer_UICanvas as an AxrEngineAssetUniformBuffer_UICanvas
        /// @returns This as an AxrEngineAssetUniformBuffer_UICanvas
        const AxrEngineAssetUniformBuffer_UICanvas* toRaw() const {
            return reinterpret_cast<const AxrEngineAssetUniformBuffer_UICanvas*>(this);
        }

        /// Get a handle to the EngineAssetUniformBuffer_UICanvas as an AxrEngineAssetUniformBuffer_UICanvas
        /// @returns This as an AxrEngineAssetUniformBuffer_UICanvas
        AxrEngineAssetUniformBuffer_UICanvas* toRaw() {
            return reinterpret_cast<AxrEngineAssetUniformBuffer_UICanvas*>(this);
        }
    };

    static_assert(
        sizeof(AxrEngineAssetUniformBuffer_UICanvas) == sizeof(axr::EngineAssetUniformBuffer_UICanvas),
        "Original type and wrapper have different size!"
    );

    /// Engine asset uniform buffer named 'UI Rectangle' structure
    struct alignas(16) EngineAssetUniformBuffer_UIRectangle {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        // Every element MUST start with `position` and `size` since they're used in the vertex shader that all ui elements use
        alignas(8) glm::vec2 Position = {};
        alignas(8) glm::vec2 Size = {};
        alignas(16) glm::vec4 BackgroundColor = {};
        alignas(16) axr::UIRoundedCorners Corners = {};

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        EngineAssetUniformBuffer_UIRectangle() = default;

        /// Constructor
        /// @param position The UI element position
        /// @param size The UI element size
        /// @param backgroundColor The background color
        /// @param corners The rounded corners
        EngineAssetUniformBuffer_UIRectangle(
            const glm::vec2& position,
            const glm::vec2& size,
            const glm::vec4& backgroundColor,
            const axr::UIRoundedCorners& corners
        ): Position(position),
            Size(size),
            BackgroundColor(backgroundColor),
            Corners(corners) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the EngineAssetUniformBuffer_UIRectangle as an AxrEngineAssetUniformBuffer_UIRectangle
        /// @returns This as an AxrEngineAssetUniformBuffer_UIRectangle
        const AxrEngineAssetUniformBuffer_UIRectangle* toRaw() const {
            return reinterpret_cast<const AxrEngineAssetUniformBuffer_UIRectangle*>(this);
        }

        /// Get a handle to the EngineAssetUniformBuffer_UIRectangle as an AxrEngineAssetUniformBuffer_UIRectangle
        /// @returns This as an AxrEngineAssetUniformBuffer_UIRectangle
        AxrEngineAssetUniformBuffer_UIRectangle* toRaw() {
            return reinterpret_cast<AxrEngineAssetUniformBuffer_UIRectangle*>(this);
        }
    };

    static_assert(
        sizeof(AxrEngineAssetUniformBuffer_UIRectangle) == sizeof(axr::EngineAssetUniformBuffer_UIRectangle),
        "Original type and wrapper have different size!"
    );

    /// Engine asset uniform buffer named 'UI Border' structure
    struct alignas(16) EngineAssetUniformBuffer_UIBorder {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        // Every element MUST start with `position` and `size` since they're used in the vertex shader that all ui elements use
        alignas(8) glm::vec2 Position = {};
        alignas(8) glm::vec2 Size = {};
        alignas(16) glm::vec4 Color = {};
        alignas(16) axr::UIRoundedCorners Corners = {};
        alignas(16) axr::UIBorders Borders = {};

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        EngineAssetUniformBuffer_UIBorder() = default;

        /// Constructor
        /// @param position The UI element position
        /// @param size The UI element size
        /// @param color The color
        /// @param corners The rounded corners
        /// @param borders The border thicknesses
        EngineAssetUniformBuffer_UIBorder(
            const glm::vec2& position,
            const glm::vec2& size,
            const glm::vec4& color,
            const axr::UIRoundedCorners& corners,
            const axr::UIBorders& borders
        ): Position(position),
            Size(size),
            Color(color),
            Corners(corners),
            Borders(borders) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the EngineAssetUniformBuffer_UIBorder as an AxrEngineAssetUniformBuffer_UIBorder
        /// @returns This as an AxrEngineAssetUniformBuffer_UIBorder
        const AxrEngineAssetUniformBuffer_UIBorder* toRaw() const {
            return reinterpret_cast<const AxrEngineAssetUniformBuffer_UIBorder*>(this);
        }

        /// Get a handle to the EngineAssetUniformBuffer_UIBorder as an AxrEngineAssetUniformBuffer_UIBorder
        /// @returns This as an AxrEngineAssetUniformBuffer_UIBorder
        AxrEngineAssetUniformBuffer_UIBorder* toRaw() {
            return reinterpret_cast<AxrEngineAssetUniformBuffer_UIBorder*>(this);
        }
    };

    static_assert(
        sizeof(AxrEngineAssetUniformBuffer_UIBorder) == sizeof(axr::EngineAssetUniformBuffer_UIBorder),
        "Original type and wrapper have different size!"
    );

    /// Engine asset uniform buffer named 'UI Image' structure
    struct alignas(16) EngineAssetUniformBuffer_UIImage {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        // Every element MUST start with `position` and `size` since they're used in the vertex shader that all ui elements use
        alignas(8) glm::vec2 Position = {};
        alignas(8) glm::vec2 Size = {};
        alignas(16) glm::vec4 BackgroundColor = {};
        alignas(16) axr::UIRoundedCorners Corners = {};

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        EngineAssetUniformBuffer_UIImage() = default;

        /// Constructor
        /// @param position The UI element position
        /// @param size The UI element size
        /// @param backgroundColor The background color
        /// @param corners The rounded corners
        EngineAssetUniformBuffer_UIImage(
            const glm::vec2& position,
            const glm::vec2& size,
            const glm::vec4& backgroundColor,
            const axr::UIRoundedCorners& corners
        ): Position(position),
            Size(size),
            BackgroundColor(backgroundColor),
            Corners(corners) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the EngineAssetUniformBuffer_UIImage as an AxrEngineAssetUniformBuffer_UIImage
        /// @returns This as an AxrEngineAssetUniformBuffer_UIImage
        const AxrEngineAssetUniformBuffer_UIImage* toRaw() const {
            return reinterpret_cast<const AxrEngineAssetUniformBuffer_UIImage*>(this);
        }

        /// Get a handle to the EngineAssetUniformBuffer_UIImage as an AxrEngineAssetUniformBuffer_UIImage
        /// @returns This as an AxrEngineAssetUniformBuffer_UIImage
        AxrEngineAssetUniformBuffer_UIImage* toRaw() {
            return reinterpret_cast<AxrEngineAssetUniformBuffer_UIImage*>(this);
        }
    };

    static_assert(
        sizeof(AxrEngineAssetUniformBuffer_UIImage) == sizeof(axr::EngineAssetUniformBuffer_UIImage),
        "Original type and wrapper have different size!"
    );

    /// Engine asset uniform buffer named 'UI Text' structure
    struct alignas(16) EngineAssetUniformBuffer_UIText {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        // Every element MUST start with `position` and `size` since they're used in the vertex shader that all ui elements use
        alignas(8) glm::vec2 Position = {};
        alignas(8) glm::vec2 Size = {};
        alignas(16) glm::vec4 TextColor = {};

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        EngineAssetUniformBuffer_UIText() = default;

        /// Constructor
        /// @param position The UI element position
        /// @param size The UI element size
        /// @param textColor The text color
        EngineAssetUniformBuffer_UIText(
            const glm::vec2& position,
            const glm::vec2& size,
            const glm::vec4& textColor
        ): Position(position),
            Size(size),
            TextColor(textColor) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the EngineAssetUniformBuffer_UIText as an AxrEngineAssetUniformBuffer_UIText
        /// @returns This as an AxrEngineAssetUniformBuffer_UIText
        const AxrEngineAssetUniformBuffer_UIText* toRaw() const {
            return reinterpret_cast<const AxrEngineAssetUniformBuffer_UIText*>(this);
        }

        /// Get a handle to the EngineAssetUniformBuffer_UIText as an AxrEngineAssetUniformBuffer_UIText
        /// @returns This as an AxrEngineAssetUniformBuffer_UIText
        AxrEngineAssetUniformBuffer_UIText* toRaw() {
            return reinterpret_cast<AxrEngineAssetUniformBuffer_UIText*>(this);
        }
    };

    static_assert(
        sizeof(AxrEngineAssetUniformBuffer_UIText) == sizeof(axr::EngineAssetUniformBuffer_UIText),
        "Original type and wrapper have different size!"
    );

    /// Engine asset uniform buffer for a general UI element.
    using EngineAssetUniformBuffer_UIElement = AxrEngineAssetUniformBuffer_UIElement;

    // ---- Push Constant Buffers ----

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
    /// Engine asset push constant buffer named 'Model Matrix' structure
    struct alignas(16) EngineAssetPushConstantBuffer_ModelMatrix {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        alignas(16) glm::mat4 ModelMatrix = {};

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        EngineAssetPushConstantBuffer_ModelMatrix() = default;

        /// Constructor
        /// @param modelMatrix The model matrix
        EngineAssetPushConstantBuffer_ModelMatrix(
            const glm::mat4& modelMatrix
        ): ModelMatrix(modelMatrix) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the EngineAssetPushConstantBuffer_ModelMatrix as an AxrEngineAssetPushConstantBuffer_ModelMatrix
        /// @returns This as an AxrEngineAssetPushConstantBuffer_ModelMatrix
        const AxrEngineAssetPushConstantBuffer_ModelMatrix* toRaw() const {
            return reinterpret_cast<const AxrEngineAssetPushConstantBuffer_ModelMatrix*>(this);
        }

        /// Get a handle to the EngineAssetPushConstantBuffer_ModelMatrix as an AxrEngineAssetPushConstantBuffer_ModelMatrix
        /// @returns This as an AxrEngineAssetPushConstantBuffer_ModelMatrix
        AxrEngineAssetPushConstantBuffer_ModelMatrix* toRaw() {
            return reinterpret_cast<AxrEngineAssetPushConstantBuffer_ModelMatrix*>(this);
        }
    };

    static_assert(
        sizeof(AxrEngineAssetPushConstantBuffer_ModelMatrix) == sizeof(axr::EngineAssetPushConstantBuffer_ModelMatrix),
        "Original type and wrapper have different size!"
    );

    /// Engine asset push constant buffer named 'Model Matrix' structure
    struct alignas(16) EngineAssetPushConstantBuffer_MvpMatrix {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        alignas(16) glm::mat4 MvpMatrix = {};

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        EngineAssetPushConstantBuffer_MvpMatrix() = default;

        /// Constructor
        /// @param mvpMatrix The mvp matrix
        EngineAssetPushConstantBuffer_MvpMatrix(
            const glm::mat4& mvpMatrix
        ): MvpMatrix(mvpMatrix) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the EngineAssetPushConstantBuffer_MvpMatrix as an AxrEngineAssetPushConstantBuffer_MvpMatrix
        /// @returns This as an AxrEngineAssetPushConstantBuffer_MvpMatrix
        const AxrEngineAssetPushConstantBuffer_MvpMatrix* toRaw() const {
            return reinterpret_cast<const AxrEngineAssetPushConstantBuffer_MvpMatrix*>(this);
        }

        /// Get a handle to the EngineAssetPushConstantBuffer_MvpMatrix as an AxrEngineAssetPushConstantBuffer_MvpMatrix
        /// @returns This as an AxrEngineAssetPushConstantBuffer_MvpMatrix
        AxrEngineAssetPushConstantBuffer_MvpMatrix* toRaw() {
            return reinterpret_cast<AxrEngineAssetPushConstantBuffer_MvpMatrix*>(this);
        }
    };

    static_assert(
        sizeof(AxrEngineAssetPushConstantBuffer_MvpMatrix) == sizeof(axr::EngineAssetPushConstantBuffer_MvpMatrix),
        "Original type and wrapper have different size!"
    );
#endif

    // ---- Materials ----

    /// Engine asset material named 'Default Material' values
    struct EngineAssetMaterial_DefaultMaterial {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        axr::MaterialBackfaceCullModeEnum BackfaceCullMode = axr::MaterialBackfaceCullModeEnum::None;
        axr::MaterialAlphaRenderModeEnum AlphaRenderMode = axr::MaterialAlphaRenderModeEnum::Opaque;
        /// This can be an empty string
        char AlphaCutoffBufferName[AXR_MAX_ASSET_NAME_SIZE]{};
        char ImageName[AXR_MAX_ASSET_NAME_SIZE]{};
        char ImageSamplerName[AXR_MAX_ASSET_NAME_SIZE]{};

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        EngineAssetMaterial_DefaultMaterial() = default;

        /// Constructor
        /// @param backfaceCullMode The image backface cull mode
        /// @param alphaRenderMode The image alpha render mode
        /// @param alphaCutoffBufferName The image alpha cut off buffer name. This can be an empty string.
        /// @param imageName The image name
        /// @param imageSamplerName The image sampler name
        EngineAssetMaterial_DefaultMaterial(
            const axr::MaterialBackfaceCullModeEnum backfaceCullMode,
            const axr::MaterialAlphaRenderModeEnum alphaRenderMode,
            const char* alphaCutoffBufferName,
            const char* imageName,
            const char* imageSamplerName
        ): BackfaceCullMode(backfaceCullMode),
            AlphaRenderMode(alphaRenderMode) {
            if (alphaCutoffBufferName != nullptr) {
                strncpy_s(AlphaCutoffBufferName, alphaCutoffBufferName, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (imageName != nullptr) {
                strncpy_s(ImageName, imageName, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (imageSamplerName != nullptr) {
                strncpy_s(ImageSamplerName, imageSamplerName, AXR_MAX_ASSET_NAME_SIZE);
            }
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the EngineAssetMaterial_DefaultMaterial as an AxrEngineAssetMaterial_DefaultMaterial
        /// @returns This as an AxrEngineAssetMaterial_DefaultMaterial
        const AxrEngineAssetMaterial_DefaultMaterial* toRaw() const {
            return reinterpret_cast<const AxrEngineAssetMaterial_DefaultMaterial*>(this);
        }

        /// Get a handle to the EngineAssetMaterial_DefaultMaterial as an AxrEngineAssetMaterial_DefaultMaterial
        /// @returns This as an AxrEngineAssetMaterial_DefaultMaterial
        AxrEngineAssetMaterial_DefaultMaterial* toRaw() {
            return reinterpret_cast<AxrEngineAssetMaterial_DefaultMaterial*>(this);
        }
    };

    static_assert(
        sizeof(AxrEngineAssetMaterial_DefaultMaterial) == sizeof(axr::EngineAssetMaterial_DefaultMaterial),
        "Original type and wrapper have different size!"
    );

    // ----------------------------------------- //
    // Function Definitions
    // ----------------------------------------- //

    /// Check if the given name is reserved for an engine asset
    /// @param name Name to check
    /// @returns True if the given name is reserved for an engine asset
    inline bool engineAssetIsNameReserved(const char* name) {
        return axrEngineAssetIsNameReserved(name);
    }

    /// Get the name for the given engine asset
    /// @param engineAssetEnum Engine asset to get the name of
    /// @returns The name of the given engine asset
    inline const char* engineAssetGetName(axr::EngineAssetEnum engineAssetEnum) {
        return axrEngineAssetGetName(static_cast<AxrEngineAssetEnum>(engineAssetEnum));
    }

    // ---- Buffers ----

    /// Get the size for the given uniform buffer engine asset
    /// @param engineAssetEnum Engine asset to use
    /// @returns The size for the given uniform buffer engine asset
    inline uint64_t engineAssetGetUniformBufferSize(axr::EngineAssetEnum engineAssetEnum) {
        return axrEngineAssetGetUniformBufferSize(static_cast<AxrEngineAssetEnum>(engineAssetEnum));
    }

    /// Get the instance size for the given uniform buffer engine asset
    /// @param engineAssetEnum Engine asset to use
    /// @returns The instance size for the given uniform buffer engine asset
    inline uint64_t engineAssetGetUniformBufferInstanceSize(const AxrEngineAssetEnum engineAssetEnum) {
        return axrEngineAssetGetUniformBufferInstanceSize(engineAssetEnum);
    }

    /// Get the buffer type for the given uniform buffer engine asset
    /// @param engineAssetEnum Engine asset to use
    /// @returns The buffer type for the given uniform buffer engine asset
    inline AxrUniformBufferTypeEnum engineAssetGetUniformBufferType(const AxrEngineAssetEnum engineAssetEnum) {
        return axrEngineAssetGetUniformBufferType(engineAssetEnum);
    }

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
    /// Get the size for the given push constant buffer engine asset
    /// @param engineAssetEnum Engine asset to use
    /// @returns The size for the given push constant buffer engine asset
    inline uint32_t engineAssetGetPushConstantBufferSize(axr::EngineAssetEnum engineAssetEnum) {
        return axrEngineAssetGetPushConstantBufferSize(static_cast<AxrEngineAssetEnum>(engineAssetEnum));
    }
#endif

    // ---------------------------------------------------------------------------------- //
    //                                  Asset Collection                                  //
    // ---------------------------------------------------------------------------------- //

    // ----------------------------------------- //
    // Asset Collection Definition
    // ----------------------------------------- //

    /// Asset Collection
    class AssetCollection {
    public:
        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Constructor
        /// @param assetCollection Asset collection handle
        AssetCollection(const AxrAssetCollection_T assetCollection):
            m_AssetCollection(assetCollection) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        // ---- Shaders ----

        /// Create a new shader
        /// @param shaderConfig Shader config to use
        /// @returns AXR_SUCCESS if the function succeeded
        [[nodiscard]] axr::Result createShader(const axr::ShaderConfig& shaderConfig) const {
            return static_cast<axr::Result>(axrAssetCollectionCreateShader(m_AssetCollection, shaderConfig.toRaw()));
        }

        /// Create a new engine asset shader
        /// @param engineAssetEnum Shader engine asset
        /// @returns AXR_SUCCESS if the function succeeded
        [[nodiscard]] axr::Result createShader(axr::EngineAssetEnum engineAssetEnum) const {
            return static_cast<axr::Result>(axrAssetCollectionCreateEngineAssetShader(
                m_AssetCollection,
                static_cast<AxrEngineAssetEnum>(engineAssetEnum)
            ));
        }

        // ---- Materials ----

        /// Create a new material
        /// @param materialConfig Material config
        /// @returns AXR_SUCCESS if the function succeeded
        [[nodiscard]] axr::Result createMaterial(
            const axr::MaterialConfig& materialConfig
        ) const {
            return static_cast<axr::Result>(axrAssetCollectionCreateMaterial(
                m_AssetCollection,
                materialConfig.toRaw()
            ));
        }

        /// Create a material using the engine defined material named 'Default Material'
        /// @param materialName Material name
        /// @param materialValues Material values
        /// @returns AXR_SUCCESS if the function succeeded
        axr::Result createMaterial(
            const char* materialName,
            const axr::EngineAssetMaterial_DefaultMaterial& materialValues
        ) const {
            char materialNameBuffer[AXR_MAX_ASSET_NAME_SIZE]{};
            strncpy_s(materialNameBuffer, materialName, AXR_MAX_ASSET_NAME_SIZE);

            return static_cast<axr::Result>(
                axrAssetCollectionCreateEngineAssetMaterial_DefaultMaterial(
                    m_AssetCollection,
                    materialNameBuffer,
                    materialValues.toRaw()
                )
            );
        }

        // ---- Model ----

        /// Create a new model
        /// @param modelConfig Model config
        /// @returns AXR_SUCCESS if the function succeeded
        [[nodiscard]] axr::Result createModel(
            const axr::ModelConfig& modelConfig
        ) const {
            return static_cast<axr::Result>(axrAssetCollectionCreateModel(
                m_AssetCollection,
                modelConfig.toRaw()
            ));
        }

        /// Create a new engine asset model
        /// @param modelName Model name
        /// @param engineAssetEnum Model engine asset
        /// @returns AXR_SUCCESS if the function succeeded
        [[nodiscard]] axr::Result createModel(
            const char* modelName,
            axr::EngineAssetEnum engineAssetEnum
        ) const {
            char modelNameBuffer[AXR_MAX_ASSET_NAME_SIZE]{};
            strncpy_s(modelNameBuffer, modelName, AXR_MAX_ASSET_NAME_SIZE);

            return static_cast<axr::Result>(axrAssetCollectionCreateEngineAssetModel(
                m_AssetCollection,
                modelNameBuffer,
                static_cast<AxrEngineAssetEnum>(engineAssetEnum)
            ));
        }

        // ---- Uniform Buffer ----

        /// Create a new uniform buffer
        /// @param uniformBufferConfig Uniform buffer config
        /// @returns AXR_SUCCESS if the function succeeded
        [[nodiscard]] axr::Result createUniformBuffer(
            const axr::UniformBufferConfig& uniformBufferConfig
        ) const {
            return static_cast<axr::Result>(axrAssetCollectionCreateUniformBuffer(
                m_AssetCollection,
                uniformBufferConfig.toRaw()
            ));
        }

        /// Create a new dynamic uniform buffer
        /// @param uniformBufferConfig Dynamic uniform buffer config
        /// @returns AXR_SUCCESS if the function succeeded
        [[nodiscard]] axr::Result createDynamicUniformBuffer(
            const axr::DynamicUniformBufferConfig& uniformBufferConfig
        ) const {
            return static_cast<axr::Result>(axrAssetCollectionCreateDynamicUniformBuffer(
                m_AssetCollection,
                uniformBufferConfig.toRaw()
            ));
        }

#ifdef AXR_SUPPORTED_GRAPHICS_VULKAN
        // ---- Push Constant Buffer ----

        /// Create a new push constant buffer
        /// @param pushConstantBufferConfig Push constant buffer config
        /// @returns AXR_SUCCESS if the function succeeded
        [[nodiscard]] axr::Result createPushConstantBuffer(
            const axr::PushConstantBufferConfig& pushConstantBufferConfig
        ) const {
            return static_cast<axr::Result>(axrAssetCollectionCreatePushConstantBuffer(
                m_AssetCollection,
                pushConstantBufferConfig.toRaw()
            ));
        }
#endif

        // ---- Image ----

        /// Create a new image
        /// @param imageConfig Image config
        /// @returns AXR_SUCCESS if the function succeeded
        [[nodiscard]] axr::Result createImage(
            const axr::ImageConfig& imageConfig
        ) const {
            return static_cast<axr::Result>(axrAssetCollectionCreateImage(
                m_AssetCollection,
                imageConfig.toRaw()
            ));
        }

        /// Create a new engine asset image
        /// @param imageName Image name
        /// @param engineAssetEnum Image engine asset
        /// @returns AXR_SUCCESS if the function succeeded
        [[nodiscard]] axr::Result createImage(
            const char* imageName,
            axr::EngineAssetEnum engineAssetEnum
        ) const {
            char imageNameBuffer[AXR_MAX_ASSET_NAME_SIZE]{};
            strncpy_s(imageNameBuffer, imageName, AXR_MAX_ASSET_NAME_SIZE);

            return static_cast<axr::Result>(axrAssetCollectionCreateEngineAssetImage(
                m_AssetCollection,
                imageNameBuffer,
                static_cast<AxrEngineAssetEnum>(engineAssetEnum)
            ));
        }

        // ---- Image Sampler ----

        /// Create a new image sampler
        /// @param imageSamplerConfig Image sampler config
        /// @returns AXR_SUCCESS if the function succeeded
        [[nodiscard]] axr::Result createImageSampler(
            const axr::ImageSamplerConfig& imageSamplerConfig
        ) const {
            return static_cast<axr::Result>(axrAssetCollectionCreateImageSampler(
                m_AssetCollection,
                imageSamplerConfig.toRaw()
            ));
        }

        // ---- Font ----

        /// Create a new font
        /// @param fontConfig Font config
        /// @returns AXR_SUCCESS if the function succeeded
        [[nodiscard]] axr::Result createFont(
            const axr::FontConfig& fontConfig
        ) const {
            return static_cast<axr::Result>(axrAssetCollectionCreateFont(
                m_AssetCollection,
                fontConfig.toRaw()
            ));
        }

        /// Create a new engine asset font
        /// @param engineAssetEnum Font engine asset
        /// @returns AXR_SUCCESS if the function succeeded
        [[nodiscard]] axr::Result createFont(
            axr::EngineAssetEnum engineAssetEnum
        ) const {
            return static_cast<axr::Result>(axrAssetCollectionCreateEngineAssetFont(
                m_AssetCollection,
                static_cast<AxrEngineAssetEnum>(engineAssetEnum)
            ));
        }

    private:
        // ----------------------------------------- //
        // Private Variables
        // ----------------------------------------- //
        AxrAssetCollection_T m_AssetCollection;
    };

    // ---------------------------------------------------------------------------------- //
    //                                    Asset Utils                                     //
    // ---------------------------------------------------------------------------------- //

    // ----------------------------------------- //
    // Enums
    // ----------------------------------------- //

    /// Model file material info alpha mode
    enum class ModelFileMaterialInfoAlphaModeEnum {
        Opaque = AXR_MODEL_FILE_MATERIAL_INFO_ALPHA_MODE_OPAQUE,
        Blend = AXR_MODEL_FILE_MATERIAL_INFO_ALPHA_MODE_BLEND,
        Mask = AXR_MODEL_FILE_MATERIAL_INFO_ALPHA_MODE_MASK,
    };

    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// Model file image sampler info
    struct ModelFileImageSamplerInfo {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        char Name[AXR_MAX_ASSET_NAME_SIZE]{};
        axr::ImageSamplerFilterEnum MinFilter;
        axr::ImageSamplerFilterEnum MagFilter;
        axr::ImageSamplerFilterEnum MipmapFilter;
        axr::ImageSamplerWrapEnum WrapU;
        axr::ImageSamplerWrapEnum WrapV;
        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        ModelFileImageSamplerInfo():
            MinFilter(axr::ImageSamplerFilterEnum::Undefined),
            MagFilter(axr::ImageSamplerFilterEnum::Undefined),
            MipmapFilter(axr::ImageSamplerFilterEnum::Undefined),
            WrapU(axr::ImageSamplerWrapEnum::Undefined),
            WrapV(axr::ImageSamplerWrapEnum::Undefined) {
        }

        /// Constructor
        /// @param src Source ModelFileImageSamplerInfo
        explicit ModelFileImageSamplerInfo(AxrModelFileImageSamplerInfo&& src) {
            if (src.Name != nullptr) {
                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            }
            MinFilter = static_cast<axr::ImageSamplerFilterEnum>(src.MinFilter);
            MagFilter = static_cast<axr::ImageSamplerFilterEnum>(src.MagFilter);
            MipmapFilter = static_cast<axr::ImageSamplerFilterEnum>(src.MipmapFilter);
            WrapU = static_cast<axr::ImageSamplerWrapEnum>(src.WrapU);
            WrapV = static_cast<axr::ImageSamplerWrapEnum>(src.WrapV);

            memset(src.Name, 0, sizeof(src.Name));
            src.MinFilter = AXR_IMAGE_SAMPLER_FILTER_UNDEFINED;
            src.MagFilter = AXR_IMAGE_SAMPLER_FILTER_UNDEFINED;
            src.MipmapFilter = AXR_IMAGE_SAMPLER_FILTER_UNDEFINED;
            src.WrapU = AXR_IMAGE_SAMPLER_WRAP_UNDEFINED;
            src.WrapV = AXR_IMAGE_SAMPLER_WRAP_UNDEFINED;
        }

        /// Copy Constructor
        /// @param src Source ModelFileImageSamplerInfo to copy from
        ModelFileImageSamplerInfo(const axr::ModelFileImageSamplerInfo& src) {
            *this = static_cast<axr::ModelFileImageSamplerInfo>(axrModelFileImageSamplerInfoClone(src.toRaw()));
        }

        /// Move Constructor
        /// @param src Source ModelFileImageSamplerInfo to move from
        ModelFileImageSamplerInfo(ModelFileImageSamplerInfo&& src) noexcept {
            if (src.Name != nullptr) {
                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            }
            MinFilter = src.MinFilter;
            MagFilter = src.MagFilter;
            MipmapFilter = src.MipmapFilter;
            WrapU = src.WrapU;
            WrapV = src.WrapV;

            memset(src.Name, 0, sizeof(src.Name));
            src.MinFilter = axr::ImageSamplerFilterEnum::Undefined;
            src.MagFilter = axr::ImageSamplerFilterEnum::Undefined;
            src.MipmapFilter = axr::ImageSamplerFilterEnum::Undefined;
            src.WrapU = axr::ImageSamplerWrapEnum::Undefined;
            src.WrapV = axr::ImageSamplerWrapEnum::Undefined;
        }

        // ---- Destructor ----

        /// Destructor
        ~ModelFileImageSamplerInfo() {
            cleanup();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source ModelFileImageSamplerInfo to copy from
        ModelFileImageSamplerInfo& operator=(const ModelFileImageSamplerInfo& src) {
            if (this != &src) {
                cleanup();

                *this = static_cast<axr::ModelFileImageSamplerInfo>(axrModelFileImageSamplerInfoClone(src.toRaw()));
            }

            return *this;
        }

        /// Move Assignment Operator
        /// @param src Source ModelFileImageSamplerInfo to move from
        ModelFileImageSamplerInfo& operator=(ModelFileImageSamplerInfo&& src) noexcept {
            if (this != &src) {
                cleanup();

                if (src.Name != nullptr) {
                    strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                }
                MinFilter = src.MinFilter;
                MagFilter = src.MagFilter;
                MipmapFilter = src.MipmapFilter;
                WrapU = src.WrapU;
                WrapV = src.WrapV;

                memset(src.Name, 0, sizeof(src.Name));
                src.MinFilter = axr::ImageSamplerFilterEnum::Undefined;
                src.MagFilter = axr::ImageSamplerFilterEnum::Undefined;
                src.MipmapFilter = axr::ImageSamplerFilterEnum::Undefined;
                src.WrapU = axr::ImageSamplerWrapEnum::Undefined;
                src.WrapV = axr::ImageSamplerWrapEnum::Undefined;
            }

            return *this;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the ModelFileImageSamplerInfo as an AxrModelFileImageSamplerInfo
        /// @returns This as an AxrModelFileImageSamplerInfo
        const AxrModelFileImageSamplerInfo* toRaw() const {
            return reinterpret_cast<const AxrModelFileImageSamplerInfo*>(this);
        }

        /// Get a handle to the ModelFileImageSamplerInfo as an AxrModelFileImageSamplerInfo
        /// @returns This as an AxrModelFileImageSamplerInfo
        AxrModelFileImageSamplerInfo* toRaw() {
            return reinterpret_cast<AxrModelFileImageSamplerInfo*>(this);
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            axrModelFileImageSamplerInfoDestroy(this->toRaw());
        }
    };

    static_assert(
        sizeof(AxrModelFileImageSamplerInfo) == sizeof(axr::ModelFileImageSamplerInfo),
        "Original type and wrapper have different size!"
    );

    /// Model file image info
    struct ModelFileImageInfo {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        char Name[AXR_MAX_ASSET_NAME_SIZE]{};
        char FilePath[AXR_MAX_FILE_PATH_SIZE]{};

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        ModelFileImageInfo() {
        }

        /// Constructor
        /// @param src Source ModelFileImageInfo
        explicit ModelFileImageInfo(AxrModelFileImageInfo&& src) {
            if (src.Name != nullptr) {
                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (src.FilePath != nullptr) {
                strncpy_s(FilePath, src.FilePath, AXR_MAX_FILE_PATH_SIZE);
            }

            memset(src.Name, 0, sizeof(src.Name));
            memset(src.FilePath, 0, sizeof(src.FilePath));
        }

        /// Copy Constructor
        /// @param src Source ModelFileImageInfo to copy from
        ModelFileImageInfo(const axr::ModelFileImageInfo& src) {
            *this = static_cast<axr::ModelFileImageInfo>(axrModelFileImageInfoClone(src.toRaw()));
        }

        /// Move Constructor
        /// @param src Source ModelFileImageInfo to move from
        ModelFileImageInfo(ModelFileImageInfo&& src) noexcept {
            if (src.Name != nullptr) {
                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            }
            if (src.FilePath != nullptr) {
                strncpy_s(FilePath, src.FilePath, AXR_MAX_FILE_PATH_SIZE);
            }

            memset(src.Name, 0, sizeof(src.Name));
            memset(src.FilePath, 0, sizeof(src.FilePath));
        }

        // ---- Destructor ----

        /// Destructor
        ~ModelFileImageInfo() {
            cleanup();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source ModelFileImageInfo to copy from
        ModelFileImageInfo& operator=(const ModelFileImageInfo& src) {
            if (this != &src) {
                cleanup();

                *this = static_cast<axr::ModelFileImageInfo>(axrModelFileImageInfoClone(src.toRaw()));
            }

            return *this;
        }

        /// Move Assignment Operator
        /// @param src Source ModelFileImageInfo to move from
        ModelFileImageInfo& operator=(ModelFileImageInfo&& src) noexcept {
            if (this != &src) {
                cleanup();

                if (src.Name != nullptr) {
                    strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                }
                if (src.FilePath != nullptr) {
                    strncpy_s(FilePath, src.FilePath, AXR_MAX_FILE_PATH_SIZE);
                }

                memset(src.Name, 0, sizeof(src.Name));
                memset(src.FilePath, 0, sizeof(src.FilePath));
            }

            return *this;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the ModelFileImageInfo as an AxrModelFileImageInfo
        /// @returns This as an AxrModelFileImageInfo
        const AxrModelFileImageInfo* toRaw() const {
            return reinterpret_cast<const AxrModelFileImageInfo*>(this);
        }

        /// Get a handle to the ModelFileImageInfo as an AxrModelFileImageInfo
        /// @returns This as an AxrModelFileImageInfo
        AxrModelFileImageInfo* toRaw() {
            return reinterpret_cast<AxrModelFileImageInfo*>(this);
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            axrModelFileImageInfoDestroy(this->toRaw());
        }
    };

    static_assert(
        sizeof(AxrModelFileImageInfo) == sizeof(axr::ModelFileImageInfo),
        "Original type and wrapper have different size!"
    );

    /// Model file material info
    struct ModelFileMaterialInfo {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        char Name[AXR_MAX_ASSET_NAME_SIZE]{};
        int32_t ColorImageIndex = -1;
        int32_t ColorImageSamplerIndex = -1;
        glm::vec4 ColorFactor = {};
        axr::MaterialBackfaceCullModeEnum BackfaceCullMode = axr::MaterialBackfaceCullModeEnum::None;
        axr::ModelFileMaterialInfoAlphaModeEnum AlphaMode = axr::ModelFileMaterialInfoAlphaModeEnum::Opaque;
        float AlphaCutoff = 0.0f;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        ModelFileMaterialInfo() = default;

        /// Constructor
        /// @param src Source ModelFileMaterialInfo
        explicit ModelFileMaterialInfo(AxrModelFileMaterialInfo&& src) {
            if (src.Name != nullptr) {
                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            }
            ColorImageIndex = src.ColorImageIndex;
            ColorImageSamplerIndex = src.ColorImageSamplerIndex;
            ColorFactor = src.ColorFactor;
            BackfaceCullMode = static_cast<axr::MaterialBackfaceCullModeEnum>(src.BackfaceCullMode);
            AlphaMode = static_cast<axr::ModelFileMaterialInfoAlphaModeEnum>(src.AlphaMode);
            AlphaCutoff = src.AlphaCutoff;

            memset(src.Name, 0, sizeof(src.Name));
            src.ColorImageIndex = -1;
            src.ColorImageSamplerIndex = -1;
            src.ColorFactor = {};
            src.BackfaceCullMode = AXR_MATERIAL_BACKFACE_CULL_MODE_NONE;
            src.AlphaMode = AXR_MODEL_FILE_MATERIAL_INFO_ALPHA_MODE_OPAQUE;
            src.AlphaCutoff = 0.0f;
        }

        /// Copy Constructor
        /// @param src Source ModelFileMaterialInfo to copy from
        ModelFileMaterialInfo(const axr::ModelFileMaterialInfo& src) {
            *this = static_cast<axr::ModelFileMaterialInfo>(axrModelFileMaterialInfoClone(src.toRaw()));
        }

        /// Move Constructor
        /// @param src Source ModelFileMaterialInfo to move from
        ModelFileMaterialInfo(ModelFileMaterialInfo&& src) noexcept {
            if (src.Name != nullptr) {
                strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
            }
            ColorImageIndex = src.ColorImageIndex;
            ColorImageSamplerIndex = src.ColorImageSamplerIndex;
            ColorFactor = src.ColorFactor;
            BackfaceCullMode = src.BackfaceCullMode;
            AlphaMode = src.AlphaMode;
            AlphaCutoff = src.AlphaCutoff;

            memset(src.Name, 0, sizeof(src.Name));
            src.ColorImageIndex = -1;
            src.ColorImageSamplerIndex = -1;
            src.ColorFactor = {};
            src.BackfaceCullMode = axr::MaterialBackfaceCullModeEnum::None;
            src.AlphaMode = axr::ModelFileMaterialInfoAlphaModeEnum::Opaque;
            src.AlphaCutoff = 0.0f;
        }

        // ---- Destructor ----

        /// Destructor
        ~ModelFileMaterialInfo() {
            cleanup();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source ModelFileMaterialInfo to copy from
        ModelFileMaterialInfo& operator=(const ModelFileMaterialInfo& src) {
            if (this != &src) {
                cleanup();

                *this = static_cast<axr::ModelFileMaterialInfo>(axrModelFileMaterialInfoClone(src.toRaw()));
            }

            return *this;
        }

        /// Move Assignment Operator
        /// @param src Source ModelFileMaterialInfo to move from
        ModelFileMaterialInfo& operator=(ModelFileMaterialInfo&& src) noexcept {
            if (this != &src) {
                cleanup();

                if (src.Name != nullptr) {
                    strncpy_s(Name, src.Name, AXR_MAX_ASSET_NAME_SIZE);
                }
                ColorImageIndex = src.ColorImageIndex;
                ColorImageSamplerIndex = src.ColorImageSamplerIndex;
                ColorFactor = src.ColorFactor;
                BackfaceCullMode = src.BackfaceCullMode;
                AlphaMode = src.AlphaMode;
                AlphaCutoff = src.AlphaCutoff;

                memset(src.Name, 0, sizeof(src.Name));
                src.ColorImageIndex = -1;
                src.ColorImageSamplerIndex = -1;
                src.ColorFactor = {};
                src.BackfaceCullMode = axr::MaterialBackfaceCullModeEnum::None;
                src.AlphaMode = axr::ModelFileMaterialInfoAlphaModeEnum::Opaque;
                src.AlphaCutoff = 0.0f;
            }

            return *this;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the ModelFileMaterialInfo as an AxrModelFileMaterialInfo
        /// @returns This as an AxrModelFileMaterialInfo
        const AxrModelFileMaterialInfo* toRaw() const {
            return reinterpret_cast<const AxrModelFileMaterialInfo*>(this);
        }

        /// Get a handle to the ModelFileMaterialInfo as an AxrModelFileMaterialInfo
        /// @returns This as an AxrModelFileMaterialInfo
        AxrModelFileMaterialInfo* toRaw() {
            return reinterpret_cast<AxrModelFileMaterialInfo*>(this);
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            axrModelFileMaterialInfoDestroy(this->toRaw());
        }
    };

    static_assert(
        sizeof(AxrModelFileMaterialInfo) == sizeof(axr::ModelFileMaterialInfo),
        "Original type and wrapper have different size!"
    );

    /// Model file submesh info
    struct ModelFileSubmeshInfo {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        int32_t MaterialIndex;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        ModelFileSubmeshInfo():
            MaterialIndex(-1) {
        }

        /// Constructor
        /// @param src Source ModelFileSubmeshInfo
        explicit ModelFileSubmeshInfo(AxrModelFileSubmeshInfo&& src) {
            MaterialIndex = src.MaterialIndex;

            src.MaterialIndex = -1;
        }

        /// Copy Constructor
        /// @param src Source ModelFileSubmeshInfo to copy from
        ModelFileSubmeshInfo(const axr::ModelFileSubmeshInfo& src) {
            *this = static_cast<axr::ModelFileSubmeshInfo>(axrModelFileSubmeshInfoClone(src.toRaw()));
        }

        /// Move Constructor
        /// @param src Source ModelFileSubmeshInfo to move from
        ModelFileSubmeshInfo(ModelFileSubmeshInfo&& src) noexcept {
            MaterialIndex = src.MaterialIndex;

            src.MaterialIndex = -1;
        }

        // ---- Destructor ----

        /// Destructor
        ~ModelFileSubmeshInfo() {
            cleanup();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source ModelFileSubmeshInfo to copy from
        ModelFileSubmeshInfo& operator=(const ModelFileSubmeshInfo& src) {
            if (this != &src) {
                cleanup();

                *this = static_cast<axr::ModelFileSubmeshInfo>(axrModelFileSubmeshInfoClone(src.toRaw()));
            }

            return *this;
        }

        /// Move Assignment Operator
        /// @param src Source ModelFileSubmeshInfo to move from
        ModelFileSubmeshInfo& operator=(ModelFileSubmeshInfo&& src) noexcept {
            if (this != &src) {
                cleanup();

                MaterialIndex = src.MaterialIndex;

                src.MaterialIndex = -1;
            }

            return *this;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the ModelFileSubmeshInfo as an AxrModelFileSubmeshInfo
        /// @returns This as an AxrModelFileSubmeshInfo
        const AxrModelFileSubmeshInfo* toRaw() const {
            return reinterpret_cast<const AxrModelFileSubmeshInfo*>(this);
        }

        /// Get a handle to the ModelFileSubmeshInfo as an AxrModelFileSubmeshInfo
        /// @returns This as an AxrModelFileSubmeshInfo
        AxrModelFileSubmeshInfo* toRaw() {
            return reinterpret_cast<AxrModelFileSubmeshInfo*>(this);
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            axrModelFileSubmeshInfoDestroy(this->toRaw());
        }
    };

    static_assert(
        sizeof(AxrModelFileSubmeshInfo) == sizeof(axr::ModelFileSubmeshInfo),
        "Original type and wrapper have different size!"
    );

    /// Model file mesh info
    struct ModelFileMeshInfo {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        uint32_t SubmeshCount;
        axr::ModelFileSubmeshInfo* Submeshes;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        ModelFileMeshInfo():
            SubmeshCount(0),
            Submeshes(nullptr) {
        }

        /// Constructor
        /// @param src Source ModelFileMeshInfo
        explicit ModelFileMeshInfo(AxrModelFileMeshInfo&& src) {
            SubmeshCount = src.SubmeshCount;
            Submeshes = reinterpret_cast<axr::ModelFileSubmeshInfo*>(src.Submeshes);

            src.SubmeshCount = 0;
            src.Submeshes = nullptr;
        }

        /// Copy Constructor
        /// @param src Source ModelFileMeshInfo to copy from
        ModelFileMeshInfo(const axr::ModelFileMeshInfo& src) {
            *this = static_cast<axr::ModelFileMeshInfo>(axrModelFileMeshInfoClone(src.toRaw()));
        }

        /// Move Constructor
        /// @param src Source ModelFileMeshInfo to move from
        ModelFileMeshInfo(ModelFileMeshInfo&& src) noexcept {
            SubmeshCount = src.SubmeshCount;
            Submeshes = src.Submeshes;

            src.SubmeshCount = 0;
            src.Submeshes = nullptr;
        }

        // ---- Destructor ----

        /// Destructor
        ~ModelFileMeshInfo() {
            cleanup();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source ModelFileMeshInfo to copy from
        ModelFileMeshInfo& operator=(const ModelFileMeshInfo& src) {
            if (this != &src) {
                cleanup();

                *this = static_cast<axr::ModelFileMeshInfo>(axrModelFileMeshInfoClone(src.toRaw()));
            }

            return *this;
        }

        /// Move Assignment Operator
        /// @param src Source ModelFileMeshInfo to move from
        ModelFileMeshInfo& operator=(ModelFileMeshInfo&& src) noexcept {
            if (this != &src) {
                cleanup();

                SubmeshCount = src.SubmeshCount;
                Submeshes = src.Submeshes;

                src.SubmeshCount = 0;
                src.Submeshes = nullptr;
            }

            return *this;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the ModelFileMeshInfo as an AxrModelFileMeshInfo
        /// @returns This as an AxrModelFileMeshInfo
        const AxrModelFileMeshInfo* toRaw() const {
            return reinterpret_cast<const AxrModelFileMeshInfo*>(this);
        }

        /// Get a handle to the ModelFileMeshInfo as an AxrModelFileMeshInfo
        /// @returns This as an AxrModelFileMeshInfo
        AxrModelFileMeshInfo* toRaw() {
            return reinterpret_cast<AxrModelFileMeshInfo*>(this);
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            axrModelFileMeshInfoDestroy(this->toRaw());
        }
    };

    static_assert(
        sizeof(AxrModelFileMeshInfo) == sizeof(axr::ModelFileMeshInfo),
        "Original type and wrapper have different size!"
    );

    /// Model file info
    struct ModelFileInfo {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        uint32_t ImageSamplerCount;
        axr::ModelFileImageSamplerInfo* ImageSamplers;
        uint32_t ImageCount;
        axr::ModelFileImageInfo* Images;
        uint32_t MaterialCount;
        axr::ModelFileMaterialInfo* Materials;
        uint32_t MeshCount;
        axr::ModelFileMeshInfo* Meshes;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        ModelFileInfo():
            ImageSamplerCount(0),
            ImageSamplers(nullptr),
            ImageCount(0),
            Images(nullptr),
            MaterialCount(0),
            Materials(nullptr),
            MeshCount(0),
            Meshes(nullptr) {
        }

        /// Constructor
        /// @param src Source ModelFileInfo
        explicit ModelFileInfo(AxrModelFileInfo&& src) {
            ImageSamplerCount = src.ImageSamplerCount;
            ImageSamplers = reinterpret_cast<axr::ModelFileImageSamplerInfo*>(src.ImageSamplers);
            ImageCount = src.ImageCount;
            Images = reinterpret_cast<axr::ModelFileImageInfo*>(src.Images);
            MaterialCount = src.MaterialCount;
            Materials = reinterpret_cast<axr::ModelFileMaterialInfo*>(src.Materials);
            MeshCount = src.MeshCount;
            Meshes = reinterpret_cast<axr::ModelFileMeshInfo*>(src.Meshes);

            src.ImageSamplerCount = 0;
            src.ImageSamplers = nullptr;
            src.ImageCount = 0;
            src.Images = nullptr;
            src.MaterialCount = 0;
            src.Materials = nullptr;
            src.MeshCount = 0;
            src.Meshes = nullptr;
        }

        /// Copy Constructor
        /// @param src Source ModelFileInfo to copy from
        ModelFileInfo(const axr::ModelFileInfo& src) {
            *this = static_cast<axr::ModelFileInfo>(axrModelFileInfoClone(src.toRaw()));
        }

        /// Move Constructor
        /// @param src Source ModelFileInfo to move from
        ModelFileInfo(ModelFileInfo&& src) noexcept {
            ImageSamplerCount = src.ImageSamplerCount;
            ImageSamplers = src.ImageSamplers;
            ImageCount = src.ImageCount;
            Images = src.Images;
            MaterialCount = src.MaterialCount;
            Materials = src.Materials;
            MeshCount = src.MeshCount;
            Meshes = src.Meshes;

            src.ImageSamplerCount = 0;
            src.ImageSamplers = nullptr;
            src.ImageCount = 0;
            src.Images = nullptr;
            src.MaterialCount = 0;
            src.Materials = nullptr;
            src.MeshCount = 0;
            src.Meshes = nullptr;
        }

        // ---- Destructor ----

        /// Destructor
        ~ModelFileInfo() {
            cleanup();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source ModelFileInfo to copy from
        ModelFileInfo& operator=(const ModelFileInfo& src) {
            if (this != &src) {
                cleanup();

                *this = static_cast<axr::ModelFileInfo>(axrModelFileInfoClone(src.toRaw()));
            }

            return *this;
        }

        /// Move Assignment Operator
        /// @param src Source ModelFileInfo to move from
        ModelFileInfo& operator=(ModelFileInfo&& src) noexcept {
            if (this != &src) {
                cleanup();

                ImageSamplerCount = src.ImageSamplerCount;
                ImageSamplers = src.ImageSamplers;
                ImageCount = src.ImageCount;
                Images = src.Images;
                MaterialCount = src.MaterialCount;
                Materials = src.Materials;
                MeshCount = src.MeshCount;
                Meshes = src.Meshes;

                src.ImageSamplerCount = 0;
                src.ImageSamplers = nullptr;
                src.ImageCount = 0;
                src.Images = nullptr;
                src.MaterialCount = 0;
                src.Materials = nullptr;
                src.MeshCount = 0;
                src.Meshes = nullptr;
            }

            return *this;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Get a handle to the ModelFileInfo as an AxrModelFileInfo
        /// @returns This as an AxrModelFileInfo
        const AxrModelFileInfo* toRaw() const {
            return reinterpret_cast<const AxrModelFileInfo*>(this);
        }

        /// Get a handle to the ModelFileInfo as an AxrModelFileInfo
        /// @returns This as an AxrModelFileInfo
        AxrModelFileInfo* toRaw() {
            return reinterpret_cast<AxrModelFileInfo*>(this);
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            axrModelFileInfoDestroy(this->toRaw());
        }
    };

    static_assert(
        sizeof(AxrModelFileInfo) == sizeof(axr::ModelFileInfo),
        "Original type and wrapper have different size!"
    );

    // ----------------------------------------- //
    // Function Definitions
    // ----------------------------------------- //

    /// Get a model's file info
    /// @param path The model's file path
    /// @param modelFileInfo Output model file info
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] inline AxrResult getModelFileData(
        const char* path,
        axr::ModelFileInfo& modelFileInfo
    ) {
        AxrModelFileInfo rawModelFileInfo;
        const AxrResult axrResult = axrGetModelFileInfo(path, &rawModelFileInfo);
        if (AXR_FAILED(axrResult)) {
            return axrResult;
        }
        modelFileInfo = axr::ModelFileInfo(std::move(rawModelFileInfo));

        return AXR_SUCCESS;
    }
}
