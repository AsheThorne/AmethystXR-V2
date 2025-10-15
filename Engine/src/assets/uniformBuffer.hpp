#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/assets.h"

// ----------------------------------------- //
// C/C++ Headers
// ----------------------------------------- //
#include <string>

/// Uniform buffer
class AxrUniformBuffer {
public:
    // ----------------------------------------- //
    // Special Functions
    // ----------------------------------------- //

    // ---- Constructors ----

    /// Default Constructor
    AxrUniformBuffer();
    /// Constructor
    /// @param config Uniform buffer config
    explicit AxrUniformBuffer(const AxrUniformBufferConfig& config);
    /// Constructor
    /// @param config Dynamic uniform buffer config
    explicit AxrUniformBuffer(const AxrDynamicUniformBufferConfig& config);
    /// Copy Constructor
    /// @param src Source AxrUniformBuffer to copy from
    AxrUniformBuffer(const AxrUniformBuffer& src);
    /// Move Constructor
    /// @param src Source AxrUniformBuffer to move from
    AxrUniformBuffer(AxrUniformBuffer&& src) noexcept;

    // ---- Destructor ----

    /// Destructor
    ~AxrUniformBuffer();

    // ---- Operator Overloads ----

    /// Copy Assignment Operator
    /// @param src Source AxrUniformBuffer to copy from
    AxrUniformBuffer& operator=(const AxrUniformBuffer& src);
    /// Move Assignment Operator
    /// @param src Source AxrUniformBuffer to move from
    AxrUniformBuffer& operator=(AxrUniformBuffer&& src) noexcept;

    // ----------------------------------------- //
    // Public Functions
    // ----------------------------------------- //

    /// Get the uniform buffer's name
    /// @returns The uniform buffer's name
    [[nodiscard]] const std::string& getName() const;

    /// Clone the given data
    /// @param size Size of the data
    /// @param data Data to clone
    /// @returns The cloned data
    [[nodiscard]] static void* cloneData(uint64_t size, const void* data);
    /// Destroy the given data
    /// @param size Size of the data
    /// @param data Data to destroy
    static void destroyData(uint64_t& size, void*& data);

    // ---- For Internal Use ----
    // These functions are only to be used internally in the AmethystXr engine.
    // They have not been given a publicly accessible function in the 'include headers' to be used by an application.

    /// Create new data of the given size
    /// @param size Size of the data
    /// @returns The data
    [[nodiscard]] static void* createData(uint64_t size);

    /// Get the buffer type
    /// @returns The buffer type
    [[nodiscard]] AxrUniformBufferTypeEnum getBufferType() const;
    /// Get the instance count
    /// @returns The instance count
    [[nodiscard]] const uint32_t& getInstanceCount() const;
    /// Get the instance size
    /// @returns The instance size
    [[nodiscard]] const uint64_t& getInstanceSize() const;
    /// Get the buffer size
    /// @returns The buffer size
    [[nodiscard]] const uint64_t& getDataSize() const;
    /// Get the buffer data
    /// @returns The buffer data
    [[nodiscard]] const void* getData() const;

    /// Cleanup the uniform buffer class
    void cleanup();

private:
    // ----------------------------------------- //
    // Private Variables
    // ----------------------------------------- //

    // ---- Config Variables ----
    std::string m_Name;
    AxrUniformBufferTypeEnum m_BufferType;
    uint32_t m_InstanceCount;
    uint64_t m_InstanceSize;
    uint64_t m_DataSize;
    void* m_Data;

    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //
};
