if(MSVC)
    add_compile_options(
        /W4
        /WX
    )
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Wshadow
        -Wconversion
        -Wsign-conversion
    )
endif()

add_compile_options($<$<PLATFORM_ID:Windows>:/EHsc>)
