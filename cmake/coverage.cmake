if(NOT ENABLE_COVERAGE)
    message(STATUS "Code coverage is disabled.")
    return()
endif()
message(STATUS "Code coverage is enabled.")

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    include(coverage/clang)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    include(coverage/gcc)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    include(coverage/msvc)
else()
    message(WARNING "Unsupported compiler for code coverage: ${CMAKE_CXX_COMPILER_ID}")
endif()

# Creates the 'coverage' custom target that builds, runs, and reports coverage
# for the given test executable target. Must be called after the target is defined.
function(add_coverage_target target)
    target_compile_options(
        ${target}
        PRIVATE
            ${COVERAGE_C_FLAGS}
            ${COVERAGE_CXX_FLAGS}
    )
    target_link_libraries(${target} PRIVATE ${COVERAGE_LD_FLAGS})
endfunction()
