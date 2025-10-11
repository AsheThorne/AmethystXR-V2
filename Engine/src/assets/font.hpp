#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/assets.h"

// ----------------------------------------- //
// C/C++ Headers
// ----------------------------------------- //
#include <string>

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
        float Left;
        float Right;
        float Top;
        float Bottom;
    };

    /// Font Glyph
    struct Glyph {
        uint32_t Unicode;
        Bounds AtlasPixelBounds;
        Bounds AtlasUVBounds;
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
    mutable Type m_Type = Type::Undefined;
    mutable uint32_t m_AtlasWidth = 0;
    mutable uint32_t m_AtlasHeight = 0;
    mutable float m_LineHeight = 0.0f;
    mutable float m_UnderlineY = 0.0f;
    mutable float m_UnderlineThickness = 0.0f;
    /// Key is the Unicode character, value is the glyph data
    mutable std::unordered_map<uint32_t, Glyph> m_Glyphs;


    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //

    /// Clean up this class
    void cleanup();
};
