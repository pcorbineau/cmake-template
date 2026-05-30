find_program(GCOVR_EXECUTABLE NAMES gcovr)
if(NOT GCOVR_EXECUTABLE)
    message(FATAL_ERROR "gcovr not found. Please install gcovr to use code coverage with GCC.")
    return()
endif()

# Derive the versioned gcov executable from the C++ compiler (e.g. g++-16 -> gcov-16).
# This ensures gcovr uses the same gcov version that produced the .gcno/.gcda files,
# avoiding "version mismatch" errors when a non-default GCC is used.
string(
    REGEX REPLACE "^.*g\\+\\+-([0-9]+)$"
    "gcov-\\1"
    GCOV_EXECUTABLE
    "${CMAKE_CXX_COMPILER}"
)
if(NOT GCOV_EXECUTABLE MATCHES "^gcov-[0-9]+$")
    set(GCOV_EXECUTABLE gcov)
endif()
find_program(
    GCOV_EXECUTABLE_PATH
    NAMES
        ${GCOV_EXECUTABLE}
        gcov
)
message(STATUS "Using gcov: ${GCOV_EXECUTABLE_PATH}")

set(COVERAGE_C_FLAGS
    --coverage
    -fkeep-inline-functions
)
set(COVERAGE_CXX_FLAGS
    --coverage
    -fkeep-inline-functions
)
set(COVERAGE_LD_FLAGS --coverage)

set(coverage_target mylib_tests)

add_custom_target(
    coverage_cleanup
    COMMAND
        ${CMAKE_COMMAND} -E rm -f -- ${CMAKE_BINARY_DIR}/*.gcda ${CMAKE_BINARY_DIR}/*.gcno
    COMMAND
        ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/coverage_report
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning up coverage data files"
)

add_custom_target(
    coverage
    COMMAND
        $<TARGET_FILE:${coverage_target}>
    COMMAND
        ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage_report
    COMMAND
        ${GCOVR_EXECUTABLE} --gcov-executable ${GCOV_EXECUTABLE_PATH} --html-details
        ${CMAKE_BINARY_DIR}/coverage_report/index.html --json-summary
        ${CMAKE_BINARY_DIR}/coverage_report/summary.json --exclude-throw-branches
        --exclude-noncode-lines --exclude ${CMAKE_SOURCE_DIR}/tests/.* --root ${CMAKE_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS
        ${coverage_target}
    DEPENDS
        coverage_cleanup
    COMMENT
        "Running ${coverage_target} with coverage instrumentation and generating gcovr HTML report"
    VERBATIM
)
