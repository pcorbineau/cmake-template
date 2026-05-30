if(NOT USE_CLANG_TIDY)
    return()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(STATUS "clang-tidy disabled for GCC (module flags incompatible with clang-tidy)")
    return()
endif()
find_program(
    CLANG_TIDY_EXE
    NAMES clang-tidy
    HINTS
    ${LLVM_ROOT}
)
if(NOT CLANG_TIDY_EXE)
    message(WARNING "clang-tidy not found, skipping clang-tidy checks")
    return()
endif()

set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
