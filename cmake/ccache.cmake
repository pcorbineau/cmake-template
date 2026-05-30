if(NOT USE_CCACHE)
    message(STATUS "Not using ccache")
    return()
endif()

find_program(CCACHE_PROGRAM ccache)
if(NOT CCACHE_PROGRAM)
    message(STATUS "ccache not found, not using ccache")
    return()
endif()

message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
