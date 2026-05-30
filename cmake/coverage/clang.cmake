find_program(LLVM_COV_EXECUTABLE NAMES llvm-cov HINTS ${LLVM_ROOT})
if(NOT LLVM_COV_EXECUTABLE)
    message(
        FATAL_ERROR
        "llvm-cov not found. Please install llvm-cov to use code
    coverage with Clang."
    )
    return()
endif()
find_program(LLVM_PROFDATA_EXECUTABLE NAMES llvm-profdata HINTS ${LLVM_ROOT})
if(NOT LLVM_PROFDATA_EXECUTABLE)
    message(
        FATAL_ERROR
        "llvm-profdata not found. Please install llvm-profdata to use code
    coverage with Clang."
    )
    return()
endif()

set(COVERAGE_C_FLAGS -fprofile-instr-generate -fcoverage-mapping)
set(COVERAGE_CXX_FLAGS -fprofile-instr-generate -fcoverage-mapping)
set(COVERAGE_LD_FLAGS -fprofile-instr-generate)

set(coverage_target mylib_tests)
set(coverage_ignore_regex "tests/|${CMAKE_BINARY_DIR}")

# we want to glob all *.profraw, *.profdata, *.gcda, *.gcno files for cleanup, the cleanup should be recusive under ${CMAKE_BINARY_DIR}
add_custom_target(
    coverage_cleanup
    COMMAND
        ${CMAKE_COMMAND} -E rm -f -- ${CMAKE_BINARY_DIR}/*.profraw ${CMAKE_BINARY_DIR}/*.profdata
        ${CMAKE_BINARY_DIR}/*.gcda ${CMAKE_BINARY_DIR}/*.gcno
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/coverage_report
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning up coverage data files"
)

add_custom_target(
    coverage
    COMMAND
        ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/coverage.profraw
        $<TARGET_FILE:${coverage_target}>
    COMMAND
        ${LLVM_PROFDATA_EXECUTABLE} merge -sparse ${CMAKE_BINARY_DIR}/coverage.profraw -o
        ${CMAKE_BINARY_DIR}/coverage.profdata
    COMMAND
        ${LLVM_COV_EXECUTABLE} show -object $<TARGET_FILE:${coverage_target}> -format=html
        -output-dir=${CMAKE_BINARY_DIR}/coverage_report
        -instr-profile=${CMAKE_BINARY_DIR}/coverage.profdata
        -ignore-filename-regex=${coverage_ignore_regex}
    COMMAND
        ${CMAKE_COMMAND} -DLLVM_COV=${LLVM_COV_EXECUTABLE}
        -DTARGET_FILE=$<TARGET_FILE:${coverage_target}>
        -DPROFDATA=${CMAKE_BINARY_DIR}/coverage.profdata -DIGNORE_REGEX=${coverage_ignore_regex}
        -DOUTPUT=${CMAKE_BINARY_DIR}/coverage_report/summary.json -P
        ${CMAKE_CURRENT_LIST_DIR}/llvm_cov_export.cmake
    COMMAND
        ${LLVM_COV_EXECUTABLE} report --instr-profile=${CMAKE_BINARY_DIR}/coverage.profdata
        --ignore-filename-regex=${coverage_ignore_regex} $<TARGET_FILE:${coverage_target}>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS ${coverage_target}
    DEPENDS coverage_cleanup
    COMMENT "Running ${coverage_target} with coverage instrumentation and generating report"
    VERBATIM
)
