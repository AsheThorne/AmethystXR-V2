// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "material.hpp"
#include "axr/logger.h"
#include "axr/common/utils.h"

// ----------------------------------------- //
// External Functions
// ----------------------------------------- //

bool axrMaterialConfigIsValid(const AxrMaterialConfig* materialConfig) {
    if (materialConfig == nullptr) {
        axrLogErrorLocation("`materialConfig` is null.");
        return false;
    }

    return !axrStringIsEmpty(materialConfig->Name) &&
        !axrStringIsEmpty(materialConfig->VertexShaderName) &&
        !axrStringIsEmpty(materialConfig->FragmentShaderName) &&
        AxrShaderValuesRAII::isValid(materialConfig->VertexShaderValues) &&
        AxrShaderValuesRAII::isValid(materialConfig->FragmentShaderValues);
}

const char* axrMaterialGetName(const AxrMaterialConst_T material) {
    if (material == nullptr) {
        axrLogErrorLocation("`material` is null.");
        return "";
    }

    return material->getName().c_str();
}

// ----------------------------------------- //
// Internal Functions
// ----------------------------------------- //

// ---- Special Functions ----

AxrMaterial::AxrMaterial():
    m_BackfaceCullMode(AXR_MATERIAL_BACKFACE_CULL_MODE_NONE),
    m_AlphaRenderMode(AXR_MATERIAL_ALPHA_RENDER_MODE_OPAQUE),
    m_EnableDepthTest(false),
    m_EnableDepthWrite(false) {
}

AxrMaterial::AxrMaterial(const AxrMaterialConfig& config):
    m_Name(config.Name),
    m_VertexShaderName(config.VertexShaderName),
    m_FragmentShaderName(config.FragmentShaderName),
    m_VertexShaderValues(config.VertexShaderValues),
    m_FragmentShaderValues(config.FragmentShaderValues),
    m_BackfaceCullMode(config.BackfaceCullMode),
    m_AlphaRenderMode(config.AlphaRenderMode),
    m_EnableDepthTest(config.EnableDepthTest),
    m_EnableDepthWrite(config.EnableDepthWrite) {
    if (!axrMaterialConfigIsValid(&config)) {
        axrLogErrorLocation("Material config is invalid.");
    }

    if (config.DynamicUniformBufferOffsetCount > 0 && config.DynamicUniformBufferOffsets != nullptr) {
        m_DynamicUniformBufferOffsets.resize(config.DynamicUniformBufferOffsetCount);
        for (uint32_t i = 0; i < config.DynamicUniformBufferOffsetCount; ++i) {
            m_DynamicUniformBufferOffsets[i] = config.DynamicUniformBufferOffsets[i];
        }
    }
}

AxrMaterial::AxrMaterial(const AxrMaterial& src) {
    m_Name = src.m_Name;
    m_VertexShaderName = src.m_VertexShaderName;
    m_FragmentShaderName = src.m_FragmentShaderName;
    m_VertexShaderValues = src.m_VertexShaderValues;
    m_FragmentShaderValues = src.m_FragmentShaderValues;
    m_BackfaceCullMode = src.m_BackfaceCullMode;
    m_AlphaRenderMode = src.m_AlphaRenderMode;
    m_EnableDepthTest = src.m_EnableDepthTest;
    m_EnableDepthWrite = src.m_EnableDepthWrite;
    m_DynamicUniformBufferOffsets = src.m_DynamicUniformBufferOffsets;
}

AxrMaterial::AxrMaterial(AxrMaterial&& src) noexcept {
    m_VertexShaderValues = std::move(src.m_VertexShaderValues);
    m_FragmentShaderValues = std::move(src.m_FragmentShaderValues);
    m_DynamicUniformBufferOffsets = std::move(src.m_DynamicUniformBufferOffsets);

    m_Name = src.m_Name;
    m_VertexShaderName = src.m_VertexShaderName;
    m_FragmentShaderName = src.m_FragmentShaderName;
    m_BackfaceCullMode = src.m_BackfaceCullMode;
    m_AlphaRenderMode = src.m_AlphaRenderMode;
    m_EnableDepthTest = src.m_EnableDepthTest;
    m_EnableDepthWrite = src.m_EnableDepthWrite;

    src.m_Name = "";
    src.m_VertexShaderName = "";
    src.m_FragmentShaderName = "";
    src.m_BackfaceCullMode = AXR_MATERIAL_BACKFACE_CULL_MODE_NONE;
    src.m_AlphaRenderMode = AXR_MATERIAL_ALPHA_RENDER_MODE_OPAQUE;
    src.m_EnableDepthTest = false;
    src.m_EnableDepthWrite = false;
}

AxrMaterial::~AxrMaterial() {
    cleanup();
}

AxrMaterial& AxrMaterial::operator=(const AxrMaterial& src) {
    if (this != &src) {
        cleanup();

        m_Name = src.m_Name;
        m_VertexShaderName = src.m_VertexShaderName;
        m_FragmentShaderName = src.m_FragmentShaderName;
        m_VertexShaderValues = src.m_VertexShaderValues;
        m_FragmentShaderValues = src.m_FragmentShaderValues;
        m_BackfaceCullMode = src.m_BackfaceCullMode;
        m_AlphaRenderMode = src.m_AlphaRenderMode;
        m_EnableDepthTest = src.m_EnableDepthTest;
        m_EnableDepthWrite = src.m_EnableDepthWrite;
        m_DynamicUniformBufferOffsets = src.m_DynamicUniformBufferOffsets;
    }

    return *this;
}

AxrMaterial& AxrMaterial::operator=(AxrMaterial&& src) noexcept {
    if (this != &src) {
        cleanup();

        m_VertexShaderValues = std::move(src.m_VertexShaderValues);
        m_FragmentShaderValues = std::move(src.m_FragmentShaderValues);
        m_DynamicUniformBufferOffsets = std::move(src.m_DynamicUniformBufferOffsets);

        m_Name = src.m_Name;
        m_VertexShaderName = src.m_VertexShaderName;
        m_FragmentShaderName = src.m_FragmentShaderName;
        m_BackfaceCullMode = src.m_BackfaceCullMode;
        m_AlphaRenderMode = src.m_AlphaRenderMode;
        m_EnableDepthTest = src.m_EnableDepthTest;
        m_EnableDepthWrite = src.m_EnableDepthWrite;

        src.m_Name = "";
        src.m_VertexShaderName = "";
        src.m_FragmentShaderName = "";
        src.m_BackfaceCullMode = AXR_MATERIAL_BACKFACE_CULL_MODE_NONE;
        src.m_AlphaRenderMode = AXR_MATERIAL_ALPHA_RENDER_MODE_OPAQUE;
        src.m_EnableDepthTest = false;
        src.m_EnableDepthWrite = false;
    }

    return *this;
}

// ---- Public Functions ----

const std::string& AxrMaterial::getName() const {
    return m_Name;
}

const std::string& AxrMaterial::getVertexShaderName() const {
    return m_VertexShaderName;
}

const std::string& AxrMaterial::getFragmentShaderName() const {
    return m_FragmentShaderName;
}

const AxrShaderValuesRAII& AxrMaterial::getVertexShaderValues() const {
    return m_VertexShaderValues;
}

const AxrShaderValuesRAII& AxrMaterial::getFragmentShaderValues() const {
    return m_FragmentShaderValues;
}

AxrMaterialBackfaceCullModeEnum AxrMaterial::getBackfaceCullMode() const {
    return m_BackfaceCullMode;
}

AxrMaterialAlphaRenderModeEnum AxrMaterial::getAlphaRenderMode() const {
    return m_AlphaRenderMode;
}

bool AxrMaterial::enableDepthTest() const {
    return m_EnableDepthTest;
}

bool AxrMaterial::enableDepthWrite() const {
    return m_EnableDepthWrite;
}

std::vector<AxrShaderUniformBufferLinkConst_T> AxrMaterial::getUniformBufferLinks() const {
    std::vector<AxrShaderUniformBufferLinkConst_T> uniformBuffers = m_VertexShaderValues.getUniformBufferLinks();
    std::vector<AxrShaderUniformBufferLinkConst_T> fragmentUniformBuffers = m_FragmentShaderValues
        .getUniformBufferLinks();

    uniformBuffers.insert(
        uniformBuffers.end(),
        fragmentUniformBuffers.begin(),
        fragmentUniformBuffers.end()
    );
    return uniformBuffers;
}

std::vector<AxrShaderImageSamplerBufferLinkConst_T> AxrMaterial::getImageSamplerBufferLinks() const {
    std::vector<AxrShaderImageSamplerBufferLinkConst_T> imageSamplerBuffers = m_VertexShaderValues
        .getImageSamplerBufferLinks();
    std::vector<AxrShaderImageSamplerBufferLinkConst_T> fragmentImageSamplerBuffers = m_FragmentShaderValues
        .getImageSamplerBufferLinks();

    imageSamplerBuffers.insert(
        imageSamplerBuffers.end(),
        fragmentImageSamplerBuffers.begin(),
        fragmentImageSamplerBuffers.end()
    );
    return imageSamplerBuffers;
}

AxrShaderUniformBufferLinkConst_T AxrMaterial::findShaderUniformBuffer(const uint32_t binding) const {
    AxrShaderUniformBufferLinkConst_T uniformBuffer = m_VertexShaderValues.findShaderUniformBuffer(binding);
    if (uniformBuffer != nullptr) {
        return uniformBuffer;
    }

    uniformBuffer = m_FragmentShaderValues.findShaderUniformBuffer(binding);
    if (uniformBuffer != nullptr) {
        return uniformBuffer;
    }

    return nullptr;
}

AxrShaderImageSamplerBufferLinkConst_T AxrMaterial::findShaderImageSamplerBuffer(const uint32_t binding) const {
    AxrShaderImageSamplerBufferLinkConst_T imageSamplerBuffer = m_VertexShaderValues.findShaderImageSamplerBuffer(
        binding
    );
    if (imageSamplerBuffer != nullptr) {
        return imageSamplerBuffer;
    }

    imageSamplerBuffer = m_FragmentShaderValues.findShaderImageSamplerBuffer(binding);
    if (imageSamplerBuffer != nullptr) {
        return imageSamplerBuffer;
    }

    return nullptr;
}

std::string AxrMaterial::getMaterialLayoutName() const {
    return std::string(std::string(m_VertexShaderName) + '_' + m_FragmentShaderName);
}

const std::vector<AxrDynamicUniformBufferOffsetConfig>& AxrMaterial::getDynamicUniformBufferOffsets() const {
    return m_DynamicUniformBufferOffsets;
}

bool AxrMaterial::isValid() const {
    return !m_Name.empty() &&
        !m_VertexShaderName.empty() &&
        !m_FragmentShaderName.empty() &&
        m_VertexShaderValues.isValid() &&
        m_FragmentShaderValues.isValid();
}

void AxrMaterial::cleanup() {
    m_Name.clear();
    m_VertexShaderName.clear();
    m_FragmentShaderName.clear();
    m_DynamicUniformBufferOffsets.clear();

    m_VertexShaderValues.cleanup();
    m_FragmentShaderValues.cleanup();

    m_BackfaceCullMode = AXR_MATERIAL_BACKFACE_CULL_MODE_NONE;
    m_AlphaRenderMode = AXR_MATERIAL_ALPHA_RENDER_MODE_OPAQUE;
    m_EnableDepthTest = false;
    m_EnableDepthWrite = false;
}

// ---- Private Functions ----
