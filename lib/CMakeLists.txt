add_library(coolgui)

target_sources(
    coolgui
    PUBLIC
        include/coolgui/coolgui.hpp
        include/coolgui/events.hpp
        include/coolgui/log.hpp
        include/coolgui/platform.hpp
        include/coolgui/strong_type.hpp
        include/coolgui/window.hpp
        include/coolgui/window_config.hpp
        include/coolgui/window_traits.hpp
        include/coolgui/window_types.hpp
    PRIVATE
        $<$<PLATFORM_ID:Darwin>:src/platform_macos.mm>
        $<$<PLATFORM_ID:Windows>:src/platform_windows.cpp>
)

target_include_directories(
    coolgui
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(coolgui PUBLIC spdlog::spdlog)

# Platform-specific system frameworks / libraries
if(APPLE)
    target_link_libraries(coolgui PRIVATE "-framework Cocoa")
elseif(WIN32)
    target_compile_definitions(
        coolgui
        PUBLIC
            WIN32_LEAN_AND_MEAN
            NOMINMAX
            UNICODE
    )
elseif(UNIX)
    if(USE_WAYLAND)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)
        pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)

        # Locate wayland-scanner executable
        find_program(WAYLAND_SCANNER wayland-scanner REQUIRED)

        # Locate xdg-shell.xml from wayland-protocols
        pkg_get_variable(WAYLAND_PROTOCOLS_DATADIR wayland-protocols pkgdatadir)
        set(XDG_SHELL_XML "${WAYLAND_PROTOCOLS_DATADIR}/stable/xdg-shell/xdg-shell.xml")

        # Generate xdg-shell protocol sources
        set(XDG_SHELL_HEADER "${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-client-protocol.h")
        set(XDG_SHELL_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-protocol.c")

        add_custom_command(
            OUTPUT
                "${XDG_SHELL_HEADER}"
            COMMAND
                "${WAYLAND_SCANNER}" client-header "${XDG_SHELL_XML}" "${XDG_SHELL_HEADER}"
            DEPENDS
                "${XDG_SHELL_XML}"
            COMMENT "Generating xdg-shell-client-protocol.h"
        )
        add_custom_command(
            OUTPUT
                "${XDG_SHELL_SOURCE}"
            COMMAND
                "${WAYLAND_SCANNER}" private-code "${XDG_SHELL_XML}" "${XDG_SHELL_SOURCE}"
            DEPENDS
                "${XDG_SHELL_XML}"
            COMMENT "Generating xdg-shell-protocol.c"
        )

        target_sources(
            coolgui
            PRIVATE
                src/platform_wayland.cpp
                "${XDG_SHELL_SOURCE}"
                "${XDG_SHELL_HEADER}"
        )

        target_include_directories(
            coolgui
            PRIVATE
                "${CMAKE_CURRENT_BINARY_DIR}"
                ${WAYLAND_CLIENT_INCLUDE_DIRS}
        )

        target_compile_definitions(coolgui PUBLIC COOLGUI_USE_WAYLAND)
        target_link_libraries(coolgui PRIVATE ${WAYLAND_CLIENT_LIBRARIES})

        message(STATUS "coolgui: Linux backend = Wayland")
    else()
        target_sources(coolgui PRIVATE src/platform_linux.cpp)
        target_link_libraries(coolgui PRIVATE X11)
        message(STATUS "coolgui: Linux backend = X11")
    endif()
endif()

if(ENABLE_COVERAGE)
    add_coverage_target(coolgui)
endif()
