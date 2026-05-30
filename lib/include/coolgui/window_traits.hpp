#pragma once

#include <optional>

#include "coolgui/events.hpp"
#include "coolgui/window_config.hpp"

namespace coolgui {

// A platform traits type must provide:
//   - a Handle type (default-constructible)
//   - create(WindowConfig)   -> Handle
//   - destroy(Handle&)       -> void
//   - poll_event(Handle&)    -> std::optional<Event>
template<typename T>
concept WindowTraits = requires(T t, typename T::Handle& h, WindowConfig cfg) {
    { t.create(cfg) }      -> std::same_as<typename T::Handle>;
    { t.destroy(h) }       -> std::same_as<void>;
    { t.poll_event(h) }    -> std::same_as<std::optional<Event>>;
};

} // namespace coolgui
