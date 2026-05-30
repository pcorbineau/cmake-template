find_package(Catch2 CONFIG REQUIRED)

add_executable(coolgui_tests)
target_sources(
    coolgui_tests
    PRIVATE
        test_color_panel.cpp
        test_strong_type.cpp
        test_widget_types.cpp
        test_window_config.cpp
        test_window.cpp
)

target_link_libraries(
    coolgui_tests
    PRIVATE
        coolgui
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(coolgui_tests)

add_custom_target(run_tests COMMAND $<TARGET_FILE:coolgui_tests> DEPENDS coolgui_tests)

if(ENABLE_COVERAGE)
    add_coverage_target(coolgui_tests)
endif()
