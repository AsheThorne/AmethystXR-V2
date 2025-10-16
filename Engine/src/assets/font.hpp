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

    /// Font Data
    class Data {
    public:
        // ----------------------------------------- //
        // Public Variables
        // ----------------------------------------- //
        Type Type = Type::Undefined;
        uint32_t AtlasWidth = 0;
        uint32_t AtlasHeight = 0;
        float LineHeight = 0.0f;
        float UnderlineY = 0.0f;
        float UnderlineThickness = 0.0f;
        /// Key is the Unicode character, value is the glyph data
        std::unordered_map<uint32_t, Glyph> Glyphs;

        // ----------------------------------------- //
        // Special Functions
        // ----------------------------------------- //

        // ---- Constructors ----

        /// Default Constructor
        Data() = default;

        /// Copy Constructor
        /// @param src Source Data to copy from
        Data(const Data& src) {
            Type = src.Type;
            AtlasWidth = src.AtlasWidth;
            AtlasHeight = src.AtlasHeight;
            LineHeight = src.LineHeight;
            UnderlineY = src.UnderlineY;
            UnderlineThickness = src.UnderlineThickness;
            Glyphs = src.Glyphs;
        }

        /// Move Constructor
        /// @param src Source Data to move from
        Data(Data&& src) noexcept {
            Glyphs = std::move(src.Glyphs);

            Type = src.Type;
            AtlasWidth = src.AtlasWidth;
            AtlasHeight = src.AtlasHeight;
            LineHeight = src.LineHeight;
            UnderlineY = src.UnderlineY;
            UnderlineThickness = src.UnderlineThickness;

            src.Type = Type::Undefined;
            src.AtlasWidth = 0;
            src.AtlasHeight = 0;
            src.LineHeight = 0.0f;
            src.UnderlineY = 0.0f;
            src.UnderlineThickness = 0.0f;
        }

        // ---- Destructor ----

        /// Destructor
        ~Data() {
            cleanup();
        }

        // ---- Operator Overloads ----

        /// Copy Assignment Operator
        /// @param src Source Data to copy from
        Data& operator=(const Data& src) {
            if (this != &src) {
                cleanup();

                Type = src.Type;
                AtlasWidth = src.AtlasWidth;
                AtlasHeight = src.AtlasHeight;
                LineHeight = src.LineHeight;
                UnderlineY = src.UnderlineY;
                UnderlineThickness = src.UnderlineThickness;
                Glyphs = src.Glyphs;
            }
            return *this;
        }

        /// Move Assignment Operator
        /// @param src Source Data to move from
        Data& operator=(Data&& src) noexcept {
            if (this != &src) {
                cleanup();

                Glyphs = std::move(src.Glyphs);

                Type = src.Type;
                AtlasWidth = src.AtlasWidth;
                AtlasHeight = src.AtlasHeight;
                LineHeight = src.LineHeight;
                UnderlineY = src.UnderlineY;
                UnderlineThickness = src.UnderlineThickness;

                src.Type = Type::Undefined;
                src.AtlasWidth = 0;
                src.AtlasHeight = 0;
                src.LineHeight = 0.0f;
                src.UnderlineY = 0.0f;
                src.UnderlineThickness = 0.0f;
            }
            return *this;
        }

        // ----------------------------------------- //
        // Public Functions
        // ----------------------------------------- //

        /// Clean up this class
        void cleanup() {
            Type = Type::Undefined;
            AtlasWidth = 0;
            AtlasHeight = 0;
            LineHeight = 0.0f;
            UnderlineY = 0.0f;
            UnderlineThickness = 0.0f;
            Glyphs.clear();
        }

        /// Check if this data is valid
        /// @returns True if this data is valid
        [[nodiscard]] bool isValid() const {
            return Type != Type::Undefined &&
                AtlasWidth != 0 &&
                AtlasHeight != 0 &&
                LineHeight != 0.0f &&
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
    /// Get the font's glyph uniform buffer
    /// @returns The font's glyph uniform buffer
    [[nodiscard]] const AxrUniformBuffer& getGlyphUniformBuffer() const;

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
    mutable AxrUniformBuffer m_GlyphUniformBuffer;

    mutable Data m_Data;


    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //

    /// Clean up this class
    void cleanup();
};
