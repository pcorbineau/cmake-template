cmake_minimum_required(VERSION 3.28)

option(BUILD_TESTING "Build tests" ON)
option(USE_CONAN "Use Conan for dependency management" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(cmake-template)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(conan)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(lib)
if(BUILD_TESTING)
	enable_testing()
	add_subdirectory(tests)
endif()
