// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "font.hpp"
#include "assetsUtils.hpp"
#include "axr/logger.h"
#include "engineAssets.hpp"

// ----------------------------------------- //
// C/C++ Headers
// ----------------------------------------- //
#include <fstream>

// ----------------------------------------- //
// External Functions
// ----------------------------------------- //

const char* axrFontGetName(const AxrFontConst_T font) {
    if (font == nullptr) {
        axrLogErrorLocation("`font` is null.");
        return "";
    }

    return font->getName().c_str();
}

uint16_t axrFontGetID(const AxrFontConst_T font) {
    if (font == nullptr) {
        axrLogErrorLocation("`font` is null.");
        return 0;
    }

    return font->getID();
}

// ----------------------------------------- //
// Internal Functions
// ----------------------------------------- //

// ---- Special Functions ----

AxrFont::AxrFont() = default;

AxrFont::AxrFont(const AxrFontConfig& config, const uint16_t id):
    m_Name(config.Name),
    m_AtlasImageName(config.AtlasImageName),
    m_AtlasImageSamplerName(config.AtlasImageSamplerName),
    m_AtlasLayoutFilePath(config.AtlasLayoutFilePath),
    m_ID(id) {
    if (!m_AtlasLayoutFilePath.empty() && !axrFileExists(m_AtlasLayoutFilePath)) {
        axrLogErrorLocation("File path is invalid.");
    }

    AxrMaterial material;
    std::vector<AxrEngineAssetEnum> requiredShaders;
    if (AXR_FAILED(
        axrEngineAssetCreateMaterial_UIText(
            m_AtlasImageName,
            m_AtlasImageSamplerName,
            m_ID,
            material,
            requiredShaders
        )
    )) {
        axrLogErrorLocation("Font material is invalid.");
    } else {
        m_Material = std::move(material);
    }

    AxrUniformBuffer fontDataUniformBuffer;
    if (AXR_FAILED(
        axrEngineAssetCreateUniformBuffer(AXR_ENGINE_ASSET_UNIFORM_BUFFER_FONT_DATA, fontDataUniformBuffer)
    )) {
        axrLogErrorLocation("Font data uniform buffer is invalid.");
    } else {
        m_FontDataUniformBuffer = std::move(fontDataUniformBuffer);
    }

    AxrUniformBuffer glyphUniformBuffer;
    if (AXR_FAILED(axrEngineAssetCreateUniformBuffer(AXR_ENGINE_ASSET_UNIFORM_BUFFER_UI_GLYPHS, glyphUniformBuffer))) {
        axrLogErrorLocation("Glyph uniform buffer is invalid.");
    } else {
        m_GlyphUniformBuffer = std::move(glyphUniformBuffer);
    }
}

AxrFont::AxrFont(const AxrFont& src) {
    m_Name = src.m_Name;
    m_AtlasImageName = src.m_AtlasImageName;
    m_AtlasImageSamplerName = src.m_AtlasImageSamplerName;
    m_AtlasLayoutFilePath = src.m_AtlasLayoutFilePath;
    m_ID = src.m_ID;
    m_Material = src.m_Material;
    m_FontDataUniformBuffer = src.m_FontDataUniformBuffer;
    m_GlyphUniformBuffer = src.m_GlyphUniformBuffer;
    m_Data = src.m_Data;
}

AxrFont::AxrFont(AxrFont&& src) noexcept {
    m_Name = std::move(src.m_Name);
    m_AtlasImageName = std::move(src.m_AtlasImageName);
    m_AtlasImageSamplerName = std::move(src.m_AtlasImageSamplerName);
    m_AtlasLayoutFilePath = std::move(src.m_AtlasLayoutFilePath);
    m_Material = std::move(src.m_Material);
    m_FontDataUniformBuffer = std::move(src.m_FontDataUniformBuffer);
    m_GlyphUniformBuffer = std::move(src.m_GlyphUniformBuffer);
    m_Data = std::move(src.m_Data);

    m_ID = src.m_ID;

    src.m_ID = 0;
}

AxrFont::~AxrFont() {
    cleanup();
}

AxrFont& AxrFont::operator=(const AxrFont& src) {
    if (this != &src) {
        cleanup();

        m_Name = src.m_Name;
        m_AtlasImageName = src.m_AtlasImageName;
        m_AtlasImageSamplerName = src.m_AtlasImageSamplerName;
        m_AtlasLayoutFilePath = src.m_AtlasLayoutFilePath;
        m_ID = src.m_ID;
        m_Material = src.m_Material;
        m_FontDataUniformBuffer = src.m_FontDataUniformBuffer;
        m_GlyphUniformBuffer = src.m_GlyphUniformBuffer;
        m_Data = src.m_Data;
    }

    return *this;
}

AxrFont& AxrFont::operator=(AxrFont&& src) noexcept {
    if (this != &src) {
        cleanup();

        m_Name = std::move(src.m_Name);
        m_AtlasImageName = std::move(src.m_AtlasImageName);
        m_AtlasImageSamplerName = std::move(src.m_AtlasImageSamplerName);
        m_AtlasLayoutFilePath = std::move(src.m_AtlasLayoutFilePath);
        m_Material = std::move(src.m_Material);
        m_FontDataUniformBuffer = std::move(src.m_FontDataUniformBuffer);
        m_GlyphUniformBuffer = std::move(src.m_GlyphUniformBuffer);
        m_Data = std::move(src.m_Data);

        m_ID = src.m_ID;

        src.m_ID = 0;
    }

    return *this;
}

// ---- Public Functions ----

const std::string& AxrFont::getName() const {
    return m_Name;
}

uint16_t AxrFont::getID() const {
    return m_ID;
}

const AxrMaterial& AxrFont::getMaterial() const {
    return m_Material;
}

const AxrUniformBuffer& AxrFont::getFontDataUniformBuffer() const {
    return m_FontDataUniformBuffer;
}

const AxrUniformBuffer& AxrFont::getGlyphUniformBuffer() const {
    return m_GlyphUniformBuffer;
}

const AxrFont::Glyph* AxrFont::getGlyph(const uint32_t unicode) const {
    const auto foundGlyph = m_Data.Glyphs.find(unicode);
    if (foundGlyph == m_Data.Glyphs.end()) {
        return nullptr;
    }

    return &foundGlyph->second;
}

bool AxrFont::isLoaded() const {
    return m_Data.isValid();
}

AxrResult AxrFont::loadFile() const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (isLoaded()) {
        return AXR_SUCCESS;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    axrResult = loadAtlasLayoutData();
    if (AXR_FAILED(axrResult)) {
        return axrResult;
    }

    axrResult = setFontUniformBufferData();
    if (AXR_FAILED(axrResult)) {
        return axrResult;
    }

    axrResult = setGlyphUniformBufferData();
    if (AXR_FAILED(axrResult)) {
        return axrResult;
    }

    return AXR_SUCCESS;
}

void AxrFont::unloadFile() const {
    m_GlyphUniformBuffer.clear();
    m_FontDataUniformBuffer.clear();
    m_Data.cleanup();
}

// ---- Private Functions ----

void AxrFont::cleanup() {
    unloadFile();

    m_Name.clear();
    m_AtlasImageName.clear();
    m_AtlasImageSamplerName.clear();
    m_AtlasLayoutFilePath.clear();
    m_ID = 0;
    m_Material.cleanup();
    m_FontDataUniformBuffer.cleanup();
    m_GlyphUniformBuffer.cleanup();
}

AxrResult AxrFont::loadAtlasLayoutData() const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (m_AtlasLayoutFilePath.empty()) {
        axrLogErrorLocation("File path is empty.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

    AxrResult axrResult = AXR_SUCCESS;

    const auto readDataError = [this](std::ifstream& file) -> AxrResult {
        axrLogErrorLocation("Json data is invalid.");
        file.close();
        unloadFile();
        return AXR_ERROR;
    };

    std::ifstream file(m_AtlasLayoutFilePath.c_str());
    if (!file.is_open()) {
        axrLogErrorLocation("Failed to open file");
        return AXR_ERROR;
    }

    const json data = json::parse(file, nullptr, false, false);
    if (!data.is_object()) {
        return readDataError(file);
    }

    const json& atlas = data["atlas"];
    if (!atlas.is_object()) {
        return readDataError(file);
    }

    const json& metrics = data["metrics"];
    if (!metrics.is_object()) {
        return readDataError(file);
    }

    const json& glyphs = data["glyphs"];
    if (!glyphs.is_array()) {
        return readDataError(file);
    }

    if (atlas.value("yOrigin", "") != "bottom") {
        axrLogErrorLocation("Only 'bottom' yOrigin is currently supported.");
        return readDataError(file);
    }

    Data fontData;

    const std::string& atlasType = atlas.value("type", "");
    if (atlasType == "msdf") {
        fontData.Type = Type::MSDF;
    } else if (atlasType == "mtsdf") {
        fontData.Type = Type::MTSDF;
    } else {
        return readDataError(file);
    }

    fontData.DistanceRange = atlas.value("distanceRange", 0.0f);
    fontData.AtlasWidth = atlas.value("width", 0);
    fontData.AtlasHeight = atlas.value("height", 0);
    fontData.Size = atlas.value("size", 0.0f);

    fontData.LineHeight = metrics.value("lineHeight", 0.0f);
    fontData.UnderlineY = metrics.value("underlineY", 0.0f);
    fontData.UnderlineThickness = metrics.value("underlineThickness", 0.0f);

    for (const json& glyph : glyphs) {
        if (!glyph.is_object()) {
            axrResult = AXR_ERROR;
            break;
        }

        const float advance = glyph.value("advance", 0.0f);

        Bounds atlasBoundsData{};
        if (glyph.contains("atlasBounds")) {
            const json& atlasBounds = glyph["atlasBounds"];
            if (!atlasBounds.is_object()) {
                axrResult = AXR_ERROR;
                break;
            }
            atlasBoundsData = Bounds{
                .Left = atlasBounds.value("left", 0.0f),
                .Right = atlasBounds.value("right", 0.0f),
                .Top = atlasBounds.value("top", 0.0f),
                .Bottom = atlasBounds.value("bottom", 0.0f),
            };
        }

        Bounds planeBoundsData{};
        if (glyph.contains("planeBounds")) {
            const json& planeBounds = glyph["planeBounds"];
            if (!planeBounds.is_object()) {
                axrResult = AXR_ERROR;
                break;
            }
            planeBoundsData = Bounds{
                .Left = planeBounds.value("left", 0.0f),
                .Right = planeBounds.value("right", 0.0f),
                .Top = planeBounds.value("top", 0.0f),
                .Bottom = planeBounds.value("bottom", 0.0f),
            };
        }

        uint32_t unicode = glyph.value("unicode", 0);
        fontData.Glyphs.emplace(
            std::pair(
                unicode,
                Glyph{
                    .Unicode = unicode,
                    .Advance = advance,
                    .PlaneBounds = planeBoundsData,
                    .AtlasPixelBounds = atlasBoundsData,
                    .AtlasUVBounds = {
                        .Left = atlasBoundsData.Left / static_cast<float>(fontData.AtlasWidth),
                        .Right = atlasBoundsData.Right / static_cast<float>(fontData.AtlasWidth),
                        .Top = atlasBoundsData.Top / static_cast<float>(fontData.AtlasHeight),
                        .Bottom = atlasBoundsData.Bottom / static_cast<float>(fontData.AtlasHeight),
                    },
                }
            )
        );
    }
    if (AXR_FAILED(axrResult)) {
        return readDataError(file);
    }

    file.close();
    m_Data = std::move(fontData);

    return AXR_SUCCESS;
}

AxrResult AxrFont::setFontUniformBufferData() const {
    AxrResult axrResult = AXR_SUCCESS;

    const AxrEngineAssetUniformBuffer_FontData fontData{
        .SdfDistanceRange = m_Data.DistanceRange,
        .SdfUnitRange = glm::vec2(m_Data.DistanceRange, m_Data.DistanceRange) /
        glm::vec2(m_Data.AtlasWidth, m_Data.AtlasHeight),
    };

    axrResult = m_FontDataUniformBuffer.setData(
        0,
        sizeof(fontData),
        &fontData
    );
    if (AXR_FAILED(axrResult)) {
        axrLogErrorLocation("Failed to set glyph uniform buffer data.");
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}

AxrResult AxrFont::setGlyphUniformBufferData() const {
    AxrResult axrResult = AXR_SUCCESS;

    std::vector<AxrEngineAssetUniformBuffer_UIGlyph> uiGlyphUniformBufferData;
    uiGlyphUniformBufferData.reserve(m_Data.Glyphs.size());
    uint32_t index = 0;
    for (Glyph& glyph : m_Data.Glyphs | std::views::values) {
        glyph.UniformBufferIndex = index++;

        uiGlyphUniformBufferData.push_back(
            AxrEngineAssetUniformBuffer_UIGlyph{
                .Size = glm::vec2(
                    glyph.AtlasUVBounds.Right - glyph.AtlasUVBounds.Left,
                    glyph.AtlasUVBounds.Top - glyph.AtlasUVBounds.Bottom
                ),
                .AtlasOffset = glm::vec2(
                    glyph.AtlasUVBounds.Left,
                    1.0f - glyph.AtlasUVBounds.Top
                ),
            }
        );
    }

    axrResult = m_GlyphUniformBuffer.setData(
        0,
        uiGlyphUniformBufferData.size() * sizeof(AxrEngineAssetUniformBuffer_UIGlyph),
        uiGlyphUniformBufferData.data()
    );
    if (AXR_FAILED(axrResult)) {
        axrLogErrorLocation("Failed to set glyph uniform buffer data.");
        return AXR_ERROR;
    }

    return AXR_SUCCESS;
}
