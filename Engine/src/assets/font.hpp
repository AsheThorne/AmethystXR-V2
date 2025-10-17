#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/assets.h"
#include "material.hpp"
#include "uniformBuffer.hpp"

// ----------------------------------------- //
// C/C++ Headers
// ----------------------------------------- //
#include <string>
#include <map>

// ----------------------------------------- //
// Json Headers
// ----------------------------------------- //
#include <json.hpp>
using json = nlohmann::json;

/// Axr Font
class AxrFont {
public:
    // ----------------------------------------- //
    // Public Enums
    // ----------------------------------------- //

    /// Font Type
    enum class Type {
        Undefined = 0,
        MSDF,
        MTSDF,
    };

    // ----------------------------------------- //
    // Public Structs
    // ----------------------------------------- //

    /// Font Atlas Bounds
    struct Bounds {
        float Left = 0.0f;
        float Right = 0.0f;
        float Top = 0.0f;
        float Bottom = 0.0f;
    };

    /// Font Glyph Data
    struct Glyph {
        uint32_t Unicode = 0;
        uint32_t UniformBufferIndex = 0;
        /// The horizontal advance in em's
        float Advance = 0.0f;
        /// The glyph quad's bounds in em's relative to the baseline and horizontal cursor position
        Bounds PlaneBounds;
        /// The glyph's bounds in the atlas in pixels
        Bounds AtlasPixelBounds;
        /// The normalized glyph's bounds in the atlas (ranged from 1-0)
        Bounds AtlasUVBounds;
    };

    /// Font Atlas Layout Data
    class AtlasLayout {
    public:
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        Type Type = Type::Undefined;
        float DistanceRange = 0.0f;
        uint32_t AtlasWidth = 0;
        uint32_t AtlasHeight = 0;
        float Size = 0.0f;
        /// Line height in ems
        float LineHeight = 0.0f;
        /// Underline y offset in ems
        float UnderlineY = 0.0f;
        /// Underline thickness in ems
        float UnderlineThickness = 0.0f;

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            Type = Type::Undefined;
            DistanceRange = 0.0f;
            AtlasWidth = 0;
            AtlasHeight = 0;
            Size = 0.0f;
            LineHeight = 0.0f;
            UnderlineY = 0.0f;
            UnderlineThickness = 0.0f;
        }

        /// Check if this data is valid
        /// @returns True if this data is valid
        [[nodiscard]] bool isValid() const {
            return Type != Type::Undefined &&
                DistanceRange != 0.0f &&
                AtlasWidth != 0 &&
                AtlasHeight != 0 &&
                Size != 0 &&
                LineHeight != 0.0f;
        }
    };

    /// Font Data
    class Data {
    public:
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        AtlasLayout Atlas;
        /// Key is the Unicode character, value is the glyph data
        std::map<uint32_t, Glyph> Glyphs;

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            Atlas.cleanup();
            Glyphs.clear();
        }

        /// Check if this data is valid
        /// @returns True if this data is valid
        [[nodiscard]] bool isValid() const {
            return Atlas.isValid() &&
                !Glyphs.empty();
        }
    };

    // ----------------------------------------- //
    // Special Functions
    // ----------------------------------------- //

    // ---- Constructors ----

    /// Default Constructor
    AxrFont();
    /// Constructor
    /// @param config Font config
    /// @param id Font ID
    AxrFont(const AxrFontConfig& config, uint16_t id);
    /// Copy Constructor
    /// @param src Source AxrFont to copy from
    AxrFont(const AxrFont& src);
    /// Move Constructor
    /// @param src Source AxrFont to move from
    AxrFont(AxrFont&& src) noexcept;

    // ---- Destructor ----

    /// Destructor
    ~AxrFont();

    // ---- Operator Overloads ----

    /// Copy Assignment Operator
    /// @param src Source AxrFont to copy from
    AxrFont& operator=(const AxrFont& src);
    /// Move Assignment Operator
    /// @param src Source AxrFont to move from
    AxrFont& operator=(AxrFont&& src) noexcept;

    // ----------------------------------------- //
    // Public Functions
    // ----------------------------------------- //

    /// Get the font name
    /// @returns The name of the font
    [[nodiscard]] const std::string& getName() const;
    /// Get the font's ID
    /// @returns The font's ID
    [[nodiscard]] uint16_t getID() const;
    /// Get the font's material
    /// @returns The font's material
    [[nodiscard]] const AxrMaterial& getMaterial() const;
    /// Get the font data uniform buffer
    /// @returns The font data uniform buffer
    [[nodiscard]] const AxrUniformBuffer& getFontDataUniformBuffer() const;
    /// Get the font's glyph uniform buffer
    /// @returns The font's glyph uniform buffer
    [[nodiscard]] const AxrUniformBuffer& getGlyphUniformBuffer() const;
    /// Get the atlas layout data
    /// @returns The atlas layout data
    [[nodiscard]] const AxrFont::AtlasLayout& getLayoutData() const;
    /// Get the glyph data for the given Unicode
    /// @param unicode Unicode
    /// @returns A handle to the glyph data. Or nullptr if it wasn't found
    [[nodiscard]] const AxrFont::Glyph* getGlyph(uint32_t unicode) const;

    // ---- For Internal Use ----
    // These functions are only to be used internally in the AmethystXr engine.
    // They have not been given a publicly accessible function in the 'include headers' to be used by an application.

    /// Check if this font is loaded
    /// @returns True if this font is loaded
    [[nodiscard]] bool isLoaded() const;
    /// Load this font
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult loadFile() const;
    /// Unload this font
    void unloadFile() const;

private:
    // ----------------------------------------- //
    // Private Variables
    // ----------------------------------------- //

    // ---- Config Variables ----
    std::string m_Name;
    std::string m_AtlasImageName;
    std::string m_AtlasImageSamplerName;
    std::string m_AtlasLayoutFilePath;
    uint16_t m_ID = 0;

    // ---- Data ----
    AxrMaterial m_Material;
    mutable AxrUniformBuffer m_FontDataUniformBuffer;
    mutable AxrUniformBuffer m_GlyphUniformBuffer;

    mutable Data m_Data;


    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //

    /// Clean up this class
    void cleanup();

    /// Load atlas layout data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult loadAtlasLayoutData() const;
    /// Set the 'font data' uniform buffer data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setFontUniformBufferData() const;
    /// Set the 'glyph' uniform buffer data
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setGlyphUniformBufferData() const;
};
