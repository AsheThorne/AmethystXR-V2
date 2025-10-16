#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "axr/assets.h"
#include "axr/common/eventHandler.h"

// ----------------------------------------- //
// C/C++ Headers
// ----------------------------------------- //
#include <string>

/// Uniform buffer
class AxrUniformBuffer {
public:
    // ----------------------------------------- //
    // Types
    // ----------------------------------------- //

    /// On 'data changed' event handler type
    using OnDataChangedEventHandler_T = AxrEventHandler<>;

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
    /// Set the buffer data.
    /// @param offset Offset of the data to set
    /// @param dataSize Size of the data to set
    /// @param data Data to set
    /// @returns AXR_SUCCESS if the function succeeded
    [[nodiscard]] AxrResult setData(uint64_t offset, uint64_t dataSize, const void* data) const;
    /// Clear uniform buffer data
    void clear();

    /// Add a new 'On Data Changed' callback function
    /// @tparam Candidate Function callback to add
    template <auto Candidate>
    void addOnDataChangedCallback() const {
        m_OnDataChangedEventHandler.addCallback<Candidate>();
    }

    /// Add a new 'On Data Changed' callback function
    /// @tparam Candidate Function callback to add
    /// @tparam Type Instance type
    template <auto Candidate, typename Type>
    void addOnDataChangedCallback(Type& instance) const {
        m_OnDataChangedEventHandler.addCallback<Candidate, Type>(instance);
    }

    /// Add a new 'On Data Changed' callback function
    /// @tparam Candidate Function callback to add
    /// @tparam Type Instance type
    template <auto Candidate, typename Type>
    void addOnDataChangedCallback(Type* instance) const {
        m_OnDataChangedEventHandler.addCallback<Candidate, Type>(instance);
    }

    /// Remove an 'On Data Changed' callback function
    /// @tparam Candidate Function callback to remove
    template <auto Candidate>
    void removeOnDataChangedCallback() const {
        m_OnDataChangedEventHandler.removeCallback<Candidate>();
    }

    /// Remove an 'On Data Changed' callback function
    /// @tparam Candidate Function callback to remove
    /// @tparam Type Instance type
    template <auto Candidate, typename Type>
    void removeOnDataChangedCallback(Type& instance) const {
        m_OnDataChangedEventHandler.removeCallback<Candidate, Type>(instance);
    }

    /// Remove an 'On Data Changed' callback function
    /// @tparam Candidate Function callback to remove
    /// @tparam Type Instance type
    template <auto Candidate, typename Type>
    void removeOnDataChangedCallback(Type* instance) const {
        m_OnDataChangedEventHandler.removeCallback<Candidate, Type>(instance);
    }

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

    mutable OnDataChangedEventHandler_T m_OnDataChangedEventHandler;

    // ----------------------------------------- //
    // Private Functions
    // ----------------------------------------- //
};
