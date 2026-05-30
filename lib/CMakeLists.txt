add_library(mylib)

target_sources(
    mylib
    PUBLIC include/mylib/mylib.hpp include/mylib/platform.hpp include/mylib/log.hpp
    PRIVATE
        src/mylib.cpp
        $<$<PLATFORM_ID:Linux>:src/platform_linux.cpp>
        $<$<PLATFORM_ID:Darwin>:src/platform_macos.cpp>
        $<$<PLATFORM_ID:Windows>:src/platform_windows.cpp>
)
target_include_directories(
    mylib
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>
)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(mylib PRIVATE spdlog::spdlog)

# GCC 14 declares std::nextafter(_Float16, _Float16) overloads in <cmath> which
# reference nextafterf16 from libm. On Ubuntu 24.04 (glibc 2.39), this symbol
# exists in libm but is not linked by default due to --as-needed. With coverage
# instrumentation (-fprofile-arcs), dead-code elimination is suppressed and the
# reference survives to link time. Explicit -lm resolves it.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND UNIX AND NOT APPLE)
    target_link_libraries(mylib PUBLIC m)
endif()

if(ENABLE_COVERAGE)
    add_coverage_target(mylib)
endif()
