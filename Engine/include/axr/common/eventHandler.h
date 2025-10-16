#pragma once

// ----------------------------------------- //
// AXR Headers
// ----------------------------------------- //
#include "callback.h"

// ----------------------------------------- //
// C/C++ Headers
// ----------------------------------------- //
#include <vector>

/// Generic event handler
/// @tparam Args Callback arguments
template <typename... Args>
class AxrEventHandler {
public:
    // ----------------------------------------- //
    // Types
    // ----------------------------------------- //

    /// Event handler callback function signature
    using Callback_T = AxrCallback<void(Args...)>;

    // ----------------------------------------- //
    // Public Functions
    // ----------------------------------------- //

    /// Add a new callback function
    /// @tparam Candidate Function callback to add
    template <auto Candidate>
    void addCallback() {
        Callback_T callback;
        callback.template connect<Candidate>();

        for (const Callback_T& cb : m_Callbacks) {
            // Don't add a callback that already exists
            if (cb == callback) {
                return;
            }
        }

        m_Callbacks.push_back(std::move(callback));
    }

    /// Add a new callback function
    /// @tparam Candidate Function callback to add
    /// @tparam Type Instance type
    template <auto Candidate, typename Type>
    void addCallback(Type& instance) {
        Callback_T callback;
        callback.template connect<Candidate, Type>(instance);

        for (const Callback_T& cb : m_Callbacks) {
            // Don't add a callback that already exists
            if (cb == callback) {
                return;
            }
        }

        m_Callbacks.push_back(std::move(callback));
    }

    /// Add a new callback function
    /// @tparam Candidate Function callback to add
    /// @tparam Type Instance type
    template <auto Candidate, typename Type>
    void addCallback(Type* instance) {
        Callback_T callback;
        callback.template connect<Candidate, Type>(instance);

        for (const Callback_T& cb : m_Callbacks) {
            // Don't add a callback that already exists
            if (cb == callback) {
                return;
            }
        }

        m_Callbacks.push_back(std::move(callback));
    }

    /// Remove a callback function
    /// @tparam Candidate Function callback to remove
    template <auto Candidate>
    void removeCallback() {
        Callback_T callback;
        callback.template connect<Candidate>();

        for (auto it = m_Callbacks.begin(); it != m_Callbacks.end(); ++it) {
            if (*it == callback) {
                m_Callbacks.erase(it);
                return;
            }
        }
    }

    /// Remove a callback function
    /// @tparam Candidate Function callback to remove
    /// @tparam Type Instance type
    template <auto Candidate, typename Type>
    void removeCallback(Type& instance) {
        Callback_T callback;
        callback.template connect<Candidate, Type>(instance);

        for (auto it = m_Callbacks.begin(); it != m_Callbacks.end(); ++it) {
            if (*it == callback) {
                m_Callbacks.erase(it);
                return;
            }
        }
    }

    /// Remove a callback function
    /// @tparam Candidate Function callback to remove
    /// @tparam Type Instance type
    template <auto Candidate, typename Type>
    void removeCallback(Type* instance) {
        Callback_T callback;
        callback.template connect<Candidate, Type>(instance);

        for (auto it = m_Callbacks.begin(); it != m_Callbacks.end(); ++it) {
            if (*it == callback) {
                m_Callbacks.erase(it);
                return;
            }
        }
    }

    /// Invoke all callback functions
    /// @param params Parameters to use for the callback functions
    void invoke(Args... params) const {
        for (const Callback_T& callback : m_Callbacks) {
            callback(std::forward<Args>(params)...);
        }
    }

    /// Clear all callback functions
    void clear() {
        m_Callbacks.clear();
    }

private:
    // ----------------------------------------- //
    // Private Variables
    // ----------------------------------------- //
    std::vector<Callback_T> m_Callbacks;
};
