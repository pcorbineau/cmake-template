#pragma once

#include <optional>

#include "coolgui/events.hpp"
#include "coolgui/window_config.hpp"
#include "coolgui/window_types.hpp"

namespace mock {

struct Handle {
    int id{ 0 };
};

enum class Scenario {
    Empty,      // poll_event always returns nullopt
    ThenClose,  // poll_event returns CloseEvent once, then nullopt
    ThenResize, // poll_event returns ResizeEvent once, then nullopt
};

struct Traits {
    using Handle = mock::Handle;

    Scenario scenario{ Scenario::Empty };
    int      poll_count{ 0 };
    int      destroy_count{ 0 };

    constexpr auto create(coolgui::WindowConfig /*cfg*/) -> Handle { return Handle{ 42 }; }

    auto destroy(Handle& /*handle*/) -> void { ++destroy_count; }

    auto poll_event(Handle& /*handle*/) -> std::optional<coolgui::Event>
    {
        ++poll_count;
        switch (scenario) {
        case Scenario::ThenClose:
            scenario = Scenario::Empty;
            return coolgui::Event{ coolgui::CloseEvent{} };
        case Scenario::ThenResize:
            scenario = Scenario::Empty;
            return coolgui::Event{ coolgui::ResizeEvent{
                coolgui::Width{ 1280 },
                coolgui::Height{ 720 },
            } };
        case Scenario::Empty:
        default:
            return std::nullopt;
        }
    }
};

} // namespace mock
