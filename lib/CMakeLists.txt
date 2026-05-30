add_library(mylib)

target_sources(mylib
	PUBLIC
		include/mylib/mylib.hpp
		include/mylib/platform.hpp
		include/mylib/log.hpp
	PRIVATE
		src/mylib.cpp
		$<$<PLATFORM_ID:Linux>:src/platform_linux.cpp>
		$<$<PLATFORM_ID:Darwin>:src/platform_macos.cpp>
		$<$<PLATFORM_ID:Windows>:src/platform_windows.cpp>
)
target_include_directories(mylib
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(mylib PRIVATE spdlog::spdlog)
