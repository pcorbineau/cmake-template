find_package(Catch2 CONFIG REQUIRED)

add_executable(mylib_tests)
target_sources(mylib_tests
    PRIVATE
    mylib_test.cpp
)

target_link_libraries(mylib_tests
    PRIVATE
    mylib
    Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(mylib_tests)

add_custom_target(run_tests
    COMMAND $<TARGET_FILE:mylib_tests>
    DEPENDS mylib_tests
)

if(ENABLE_COVERAGE)
    add_coverage_target(mylib_tests)
endif()
