cmake_minimum_required(VERSION 3.28)

option(BUILD_TESTING "Build tests" ON)
option(USE_CONAN "Use Conan for dependency management" ON)
option(USE_CCACHE "Use ccache for faster builds" ON)
option(USE_CLANG_TIDY "Use clang-tidy for static analysis" ON)
option(ENABLE_COVERAGE "Enable code coverage analysis" OFF)

if(USE_CLANG_TIDY AND ENABLE_COVERAGE)
    message(FATAL_ERROR "USE_CLANG_TIDY and ENABLE_COVERAGE are mutually exclusive. Enable only one at a time.")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(cmake-template)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(conan)
include(ccache)
include(clang-tidy)
include(coverage)
include(compile-options)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(lib)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
