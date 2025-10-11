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
}

AxrFont::AxrFont(const AxrFont& src) {
    m_Name = src.m_Name;
    m_AtlasImageName = src.m_AtlasImageName;
    m_AtlasImageSamplerName = src.m_AtlasImageSamplerName;
    m_AtlasLayoutFilePath = src.m_AtlasLayoutFilePath;
    m_ID = src.m_ID;
    m_Material = src.m_Material;
    m_Type = src.m_Type;
    m_AtlasWidth = src.m_AtlasWidth;
    m_AtlasHeight = src.m_AtlasHeight;
    m_LineHeight = src.m_LineHeight;
    m_UnderlineY = src.m_UnderlineY;
    m_UnderlineThickness = src.m_UnderlineThickness;
    m_Glyphs = src.m_Glyphs;
}

AxrFont::AxrFont(AxrFont&& src) noexcept {
    m_Name = std::move(src.m_Name);
    m_AtlasImageName = std::move(src.m_AtlasImageName);
    m_AtlasImageSamplerName = std::move(src.m_AtlasImageSamplerName);
    m_AtlasLayoutFilePath = std::move(src.m_AtlasLayoutFilePath);
    m_Material = std::move(src.m_Material);
    m_Glyphs = std::move(src.m_Glyphs);

    m_ID = src.m_ID;
    m_Type = src.m_Type;
    m_AtlasWidth = src.m_AtlasWidth;
    m_AtlasHeight = src.m_AtlasHeight;
    m_LineHeight = src.m_LineHeight;
    m_UnderlineY = src.m_UnderlineY;
    m_UnderlineThickness = src.m_UnderlineThickness;

    src.m_ID = 0;
    src.m_Type = Type::Undefined;
    src.m_AtlasWidth = 0;
    src.m_AtlasHeight = 0;
    src.m_LineHeight = 0.0f;
    src.m_UnderlineY = 0.0f;
    src.m_UnderlineThickness = 0.0f;
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
        m_Type = src.m_Type;
        m_AtlasWidth = src.m_AtlasWidth;
        m_AtlasHeight = src.m_AtlasHeight;
        m_LineHeight = src.m_LineHeight;
        m_UnderlineY = src.m_UnderlineY;
        m_UnderlineThickness = src.m_UnderlineThickness;
        m_Glyphs = src.m_Glyphs;
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
        m_Glyphs = std::move(src.m_Glyphs);

        m_ID = src.m_ID;
        m_Type = src.m_Type;
        m_AtlasWidth = src.m_AtlasWidth;
        m_AtlasHeight = src.m_AtlasHeight;
        m_LineHeight = src.m_LineHeight;
        m_UnderlineY = src.m_UnderlineY;
        m_UnderlineThickness = src.m_UnderlineThickness;

        src.m_ID = 0;
        src.m_Type = Type::Undefined;
        src.m_AtlasWidth = 0;
        src.m_AtlasHeight = 0;
        src.m_LineHeight = 0.0f;
        src.m_UnderlineY = 0.0f;
        src.m_UnderlineThickness = 0.0f;
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

bool AxrFont::isLoaded() const {
    return m_Type != Type::Undefined &&
        m_AtlasWidth != 0 &&
        m_AtlasHeight != 0 &&
        m_LineHeight != 0.0f &&
        !m_Glyphs.empty();
}

AxrResult AxrFont::loadFile() const {
    // ----------------------------------------- //
    // Validation
    // ----------------------------------------- //

    if (isLoaded()) {
        return AXR_SUCCESS;
    }

    if (m_AtlasLayoutFilePath.empty()) {
        axrLogErrorLocation("File path is empty.");
        return AXR_ERROR;
    }

    // ----------------------------------------- //
    // Process
    // ----------------------------------------- //

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

    const std::string& atlasType = atlas.value("type", "");
    if (atlasType == "msdf") {
        m_Type = Type::MSDF;
    } else if (atlasType == "mtsdf") {
        m_Type = Type::MTSDF;
    } else {
        return readDataError(file);
    }

    m_AtlasWidth = atlas.value("width", 0);
    m_AtlasHeight = atlas.value("height", 0);

    m_LineHeight = metrics.value("lineHeight", 0.0f);
    m_UnderlineY = metrics.value("underlineY", 0.0f);
    m_UnderlineThickness = metrics.value("underlineThickness", 0.0f);

    AxrResult axrResult = AXR_SUCCESS;
    for (const json& glyph : glyphs) {
        if (!glyph.is_object()) {
            axrResult = AXR_ERROR;
            break;
        }
        const json& atlasBounds = glyph["atlasBounds"];
        if (!atlasBounds.is_object()) {
            axrResult = AXR_ERROR;
            break;
        }
        const Bounds atlasBoundsData{
            .Left = atlasBounds.value("left", 0.0f),
            .Right = atlasBounds.value("right", 0.0f),
            .Top = atlasBounds.value("top", 0.0f),
            .Bottom = atlasBounds.value("bottom", 0.0f),
        };

        uint32_t unicode = glyph.value("unicode", 0);
        m_Glyphs.emplace(
            std::pair(
                unicode,
                Glyph{
                    .Unicode = unicode,
                    .AtlasPixelBounds = Bounds{
                        .Left = atlasBoundsData.Left,
                        .Right = atlasBoundsData.Right,
                        .Top = atlasBoundsData.Top,
                        .Bottom = atlasBoundsData.Bottom,
                    },
                    .AtlasUVBounds = {
                        .Left = atlasBoundsData.Left / static_cast<float>(m_AtlasWidth),
                        .Right = atlasBoundsData.Right / static_cast<float>(m_AtlasWidth),
                        .Top = atlasBoundsData.Top / static_cast<float>(m_AtlasHeight),
                        .Bottom = atlasBoundsData.Bottom / static_cast<float>(m_AtlasHeight),
                    },
                }
            )
        );
    }
    if (AXR_FAILED(axrResult)) {
        return readDataError(file);
    }

    file.close();

    return AXR_SUCCESS;
}

void AxrFont::unloadFile() const {
    m_Type = Type::Undefined;
    m_AtlasWidth = 0;
    m_AtlasHeight = 0;
    m_LineHeight = 0.0f;
    m_UnderlineY = 0.0f;
    m_UnderlineThickness = 0.0f;
    m_Glyphs.clear();
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
}
