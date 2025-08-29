#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/xrSystem.h"
#include "flags.hpp"
#include "logger.hpp"
#include "common/enums.hpp"

namespace axr {
    // ---------------------------------------------------------------------------------- //
    //                                     Api Layers                                     //
    // ---------------------------------------------------------------------------------- //

    // ----------------------------------------- //
    // Enums
    // ----------------------------------------- //

    /// Xr Api Layer Type
    enum class XrApiLayerTypeEnum {
        Undefined = AXR_XR_API_LAYER_TYPE_UNDEFINED,
        CoreValidation = AXR_XR_API_LAYER_TYPE_CORE_VALIDATION,
        End = AXR_XR_API_LAYER_TYPE_END
    };

    // ---------------------------------------------------------------------------------- //
    //                                     Extensions                                     //
    // ---------------------------------------------------------------------------------- //

    // ----------------------------------------- //
    // Enums
    // ----------------------------------------- //

    /// Xr Extension Type
    enum class XrExtensionTypeEnum {
        Undefined = AXR_XR_EXTENSION_TYPE_UNDEFINED,
        DebugUtils = AXR_XR_EXTENSION_TYPE_DEBUG_UTILS,
        VulkanEnable = AXR_XR_EXTENSION_TYPE_VULKAN_ENABLE,
        End = AXR_XR_EXTENSION_TYPE_END
    };

    // Xr Extension Debug Utils Severity Flag Bits. See: XrDebugUtilsMessageSeverityFlagsEXT
    enum class XrDebugUtilsSeverityFlagBits : AxrXrDebugUtilsSeverityFlags_T {
        VerboseBit = AXR_XR_EXTENSION_DEBUG_UTILS_SEVERITY_VERBOSE_BIT,
        InfoBit = AXR_XR_EXTENSION_DEBUG_UTILS_SEVERITY_INFO_BIT,
        WarningBit = AXR_XR_EXTENSION_DEBUG_UTILS_SEVERITY_WARNING_BIT,
        ErrorBit = AXR_XR_EXTENSION_DEBUG_UTILS_SEVERITY_ERROR_BIT,
    };

    /// Xr Extension Debug Utils Severity Flags Type
    using XrDebugUtilsSeverityFlags_T = axr::Flags<XrDebugUtilsSeverityFlagBits>;

    // Xr Extension Debug Utils Type Flag Bits. See: XrDebugUtilsMessageTypeFlagsEXT
    enum class XrDebugUtilsTypeFlagBits : AxrXrDebugUtilsTypeFlags_T {
        GeneralBit = AXR_XR_EXTENSION_DEBUG_UTILS_TYPE_GENERAL_BIT,
        ValidationBit = AXR_XR_EXTENSION_DEBUG_UTILS_TYPE_VALIDATION_BIT,
        PerformanceBit = AXR_XR_EXTENSION_DEBUG_UTILS_TYPE_PERFORMANCE_BIT,
        ConformanceBit = AXR_XR_EXTENSION_DEBUG_UTILS_TYPE_CONFORMANCE_BIT,
    };

    /// Xr Extension Debug Utils Type Flags Type
    using XrDebugUtilsTypeFlags_T = axr::Flags<XrDebugUtilsTypeFlagBits>;

    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    template <>
    struct FlagTraits<axr::XrDebugUtilsSeverityFlagBits> {
        static constexpr bool isBitmask = true;
        static constexpr XrDebugUtilsSeverityFlags_T allFlags =
            XrDebugUtilsSeverityFlagBits::VerboseBit | XrDebugUtilsSeverityFlagBits::InfoBit |
            XrDebugUtilsSeverityFlagBits::WarningBit | XrDebugUtilsSeverityFlagBits::ErrorBit;
    };

    template <>
    struct FlagTraits<axr::XrDebugUtilsTypeFlagBits> {
        static constexpr bool isBitmask = true;
        static constexpr XrDebugUtilsTypeFlags_T allFlags =
            XrDebugUtilsTypeFlagBits::GeneralBit | XrDebugUtilsTypeFlagBits::ValidationBit |
            XrDebugUtilsTypeFlagBits::PerformanceBit | XrDebugUtilsTypeFlagBits::ConformanceBit;
    };

    // ---------------------------------------------------------------------------------- //
    //                                 Xr System Config                                   //
    // ---------------------------------------------------------------------------------- //

    // ----------------------------------------- //
    // Enums
    // ----------------------------------------- //

    /// Xr reference space Enum
    enum class XrReferenceSpaceEnum {
        Undefined = AXR_XR_REFERENCE_SPACE_UNDEFINED,
        Local = AXR_XR_REFERENCE_SPACE_LOCAL,
        Stage = AXR_XR_REFERENCE_SPACE_STAGE,
    };

    // ----------------------------------------- //
    // Structs
    // ----------------------------------------- //

    /// Xr System Config
    struct XrSystemConfig {
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        axr::XrReferenceSpaceEnum StageReferenceSpace;
        uint32_t ApiLayerCount;
        AxrXrApiLayer_T* ApiLayers;
        uint32_t ExtensionCount;
        AxrXrExtension_T* Extensions;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        XrSystemConfig():
            StageReferenceSpace(axr::XrReferenceSpaceEnum::Undefined),
            ApiLayerCount(0),
            // equal to the same number of available options in XrApiLayerTypeEnum
            ApiLayers(new AxrXrApiLayer_T[static_cast<uint32_t>(XrApiLayerTypeEnum::End) - 1]{}),
            ExtensionCount(0),
            // equal to the same number of available options in XrExtensionTypeEnum
            Extensions(new AxrXrExtension_T[static_cast<uint32_t>(XrExtensionTypeEnum::End) - 1]{}) {
        }

        /// Constructor
        XrSystemConfig(const axr::XrReferenceSpaceEnum stageReferenceSpace):
            StageReferenceSpace(stageReferenceSpace),
            ApiLayerCount(0),
            // equal to the same number of available options in XrApiLayerTypeEnum
            ApiLayers(new AxrXrApiLayer_T[static_cast<uint32_t>(XrApiLayerTypeEnum::End) - 1]{}),
            ExtensionCount(0),
            // equal to the same number of available options in XrExtensionTypeEnum
            Extensions(new AxrXrExtension_T[static_cast<uint32_t>(XrExtensionTypeEnum::End) - 1]{}) {
        }

        /// Copy Constructor
        /// @param src Source XrSystemConfig to copy from
        XrSystemConfig(const XrSystemConfig& src) = delete;
        /// Move Constructor
        /// @param src Source XrSystemConfig to move from
        XrSystemConfig(XrSystemConfig&& src) noexcept = delete;

        // ---- Destructor ----

        /// Destructor
        ~XrSystemConfig() {
            clearApiLayers();
            delete[] ApiLayers;
            ApiLayers = nullptr;

            clearExtensions();
            delete[] Extensions;
            Extensions = nullptr;
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source XrSystemConfig to copy from
        XrSystemConfig& operator=(const XrSystemConfig& src) = delete;
        /// Move Assignment Operator
        /// @param src Source XrSystemConfig to move from
        XrSystemConfig& operator=(XrSystemConfig&& src) noexcept = delete;

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Check if the given api layer exists
        /// @param apiLayerType The api layer type
        /// @returns True if the given api layer exists
        [[nodiscard]] bool apiLayerExists(axr::XrApiLayerTypeEnum apiLayerType) const {
            for (uint32_t i = 0; i < ApiLayerCount; ++i) {
                if (ApiLayers[i] != nullptr &&
                    ApiLayers[i]->Type == static_cast<AxrXrApiLayerTypeEnum>(apiLayerType)) {
                    return true;
                }
            }

            return false;
        }

        /// Check if the given extension exists
        /// @param extensionType The extension type
        /// @returns True if the given extension exists
        [[nodiscard]] bool extensionExists(axr::XrExtensionTypeEnum extensionType) const {
            for (uint32_t i = 0; i < ExtensionCount; ++i) {
                if (Extensions[i] != nullptr &&
                    Extensions[i]->Type == static_cast<AxrXrExtensionTypeEnum>(extensionType)) {
                    return true;
                }
            }

            return false;
        }

        /// Add the Core Validation api layer
        void addApiLayerCoreValidation() {
            const auto apiLayer = new AxrXrApiLayerCoreValidation{};
            addApiLayer(reinterpret_cast<AxrXrApiLayer_T>(apiLayer));
        }

        /// Add the Debug Utils extension
        /// @param isRequired True if this extension is required
        /// @param severityFlags The debug severity flags to log
        /// @param typeFlags The debug type flags to log
        void addExtensionDebugUtils(
            const bool isRequired,
            const axr::XrDebugUtilsSeverityFlags_T severityFlags,
            const axr::XrDebugUtilsTypeFlags_T typeFlags
        ) {
            const auto extension = new AxrXrExtensionDebugUtils{
                .IsRequired = isRequired,
                .SeverityFlags = static_cast<AxrXrDebugUtilsSeverityFlags_T>(severityFlags),
                .TypeFlags = static_cast<AxrXrDebugUtilsTypeFlags_T>(typeFlags)
            };
            addExtension(reinterpret_cast<AxrXrExtension_T>(extension));
        }

        /// Add the Vulkan Enable extension
        /// @param isRequired True if this extension is required
        void addExtensionVulkanEnable(const bool isRequired) {
            const auto extension = new AxrXrExtensionVulkanEnable{
                .IsRequired = isRequired,
            };
            addExtension(reinterpret_cast<AxrXrExtension_T>(extension));
        }

        /// Get a handle to the XrSystemConfig as an AxrXrSystemConfig
        /// @returns This as an AxrXrSystemConfig
        const AxrXrSystemConfig* toRaw() const {
            return reinterpret_cast<const AxrXrSystemConfig*>(this);
        }

        /// Get a handle to the XrSystemConfig as an AxrXrSystemConfig
        /// @returns This as an AxrXrSystemConfig
        AxrXrSystemConfig* toRaw() {
            return reinterpret_cast<AxrXrSystemConfig*>(this);
        }

    private:
        // ----------------------------------------- //
        // Private Functions
        // ----------------------------------------- //

        /// Add the given api layer
        /// @param apiLayer Api layer to add
        void addApiLayer(const AxrXrApiLayer_T apiLayer) {
            if (apiLayer == nullptr) {
                axr::logErrorLocation("Api Layer is null.");
                return;
            }

            if (apiLayerExists(static_cast<axr::XrApiLayerTypeEnum>(apiLayer->Type))) {
                axr::logErrorLocation("Api Layer already exists.");
                return;
            }

            if (ApiLayerCount + 1 > static_cast<uint32_t>(XrApiLayerTypeEnum::End) - 1) {
                axr::logErrorLocation("Api Layers array is full.");
                return;
            }

            if (ApiLayers[ApiLayerCount] != nullptr) {
                axr::logErrorLocation(
                    "This slot should be free. If this error triggered, something went really wrong."
                );
                return;
            }

            ApiLayers[ApiLayerCount] = apiLayer;
            ApiLayerCount++;
        }

        /// Add the given extension
        /// @param extension Extension to add
        void addExtension(const AxrXrExtension_T extension) {
            if (extension == nullptr) {
                axr::logErrorLocation("Extension is null.");
                return;
            }

            if (extensionExists(static_cast<axr::XrExtensionTypeEnum>(extension->Type))) {
                axr::logErrorLocation("Extension already exists.");
                return;
            }

            if (ExtensionCount + 1 > static_cast<uint32_t>(XrExtensionTypeEnum::End) - 1) {
                axr::logErrorLocation("Extensions array is full.");
                return;
            }

            if (Extensions[ExtensionCount] != nullptr) {
                axr::logErrorLocation(
                    "This slot should be free. If this error triggered, something went really wrong."
                );
                return;
            }

            Extensions[ExtensionCount] = extension;
            ExtensionCount++;
        }

        /// Clear all the api layers
        void clearApiLayers() {
            for (uint32_t i = 0; i < static_cast<uint32_t>(XrApiLayerTypeEnum::End) - 1; ++i) {
                if (ApiLayers[i] != nullptr) {
                    delete ApiLayers[i];
                    ApiLayers[i] = nullptr;
                }
            }

            ApiLayerCount = 0;
        }

        /// Clear all the extensions
        void clearExtensions() {
            for (uint32_t i = 0; i < static_cast<uint32_t>(XrExtensionTypeEnum::End) - 1; ++i) {
                if (Extensions[i] != nullptr) {
                    delete Extensions[i];
                    Extensions[i] = nullptr;
                }
            }

            ExtensionCount = 0;
        }
    };

    static_assert(
        sizeof(AxrXrSystemConfig) == sizeof(axr::XrSystemConfig),
        "Original type and wrapper have different size!"
    );

    // ----------------------------------------- //
    // Xr System Definition
    // ----------------------------------------- //

    /// Xr System
    class XrSystem {
    public:
        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Constructor
        /// @param xrSystem Xr system handle
        XrSystem(const AxrXrSystem_T xrSystem):
            m_XrSystem(xrSystem) {
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Check if the xr system is valid for use
        /// @returns True if the xr system is valid for use
        [[nodiscard]] bool isValid() const {
            return axrXrSystemIsValid(m_XrSystem);
        }

        /// Check if the xr session is running
        /// @returns True if the xr session is running
        [[nodiscard]] bool isXrSessionRunning() const {
            return axrXrSystemIsXrSessionRunning(m_XrSystem);
        }

        /// Set the xr rendering clipping planes
        /// @param zNear Near clipping plane
        /// @param zFar Far clipping plane
        void setClippingPlane(const float zNear, const float zFar) const {
            axrXrSystemSetClippingPlane(m_XrSystem, zNear, zFar);
        }

        /// Start the xr session
        /// @returns AXR_SUCCESS if the function succeeded
        [[nodiscard]] axr::Result startXrSession() const {
            return static_cast<axr::Result>(axrXrSystemStartXrSession(m_XrSystem));
        }

        /// Signal that we want to stop the xr session
        void stopXrSession() const {
            axrXrSystemStopXrSession(m_XrSystem);
        }

    private:
        // ----------------------------------------- //
        // Private Variables
        // ----------------------------------------- //
        AxrXrSystem_T m_XrSystem;
    };
}
