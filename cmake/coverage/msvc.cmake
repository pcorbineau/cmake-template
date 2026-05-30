find_program(
    OPENCPPCOVERAGE_EXECUTABLE
    NAMES OpenCppCoverage
    HINTS "C:/Program Files/OpenCppCoverage"
)
if(NOT OPENCPPCOVERAGE_EXECUTABLE)
    message(FATAL_ERROR "OpenCppCoverage not found. Please install it from \
https://github.com/OpenCppCoverage/OpenCppCoverage to use code coverage with MSVC.")
    return()
endif()

# OpenCppCoverage instruments via Windows debug APIs at runtime — no compile/link flags needed.
set(COVERAGE_C_FLAGS "")
set(COVERAGE_CXX_FLAGS "")
set(COVERAGE_LD_FLAGS "")

# OpenCppCoverage requires Windows-style backslash paths.
file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/lib" _coverage_sources)
file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/coverage_report/coverage.xml" _coverage_xml)
file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/coverage_report/summary.json" _coverage_summary)
file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/coverage_report" _coverage_report_dir)

set(coverage_target mylib_tests)

add_custom_target(
    coverage_cleanup
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/coverage_report
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning up coverage report directory"
)

add_custom_target(
    coverage
    COMMAND ${OPENCPPCOVERAGE_EXECUTABLE}
        --sources "${_coverage_sources}"
        --export_type "html:${_coverage_report_dir}"
        --export_type "cobertura:${_coverage_xml}"
        -- $<TARGET_FILE:${coverage_target}>
    COMMAND ${CMAKE_COMMAND}
        -DCOBERTURA_XML=${_coverage_xml}
        -DOUTPUT=${_coverage_summary}
        -P ${CMAKE_CURRENT_LIST_DIR}/cobertura_to_summary.cmake
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS ${coverage_target}
    DEPENDS coverage_cleanup
    COMMENT "Running ${coverage_target} with OpenCppCoverage and generating HTML report"
    VERBATIM
)
