if(NOT USE_CONAN)
	message(STATUS "Conan is disabled, skipping...")
	return()
endif()

file(
	GLOB_RECURSE
	conan_config_files
	*cmakedeps_paths.cmake
)
list(FILTER conan_config_files INCLUDE REGEX ".*generators.*")
list(TRANSFORM conan_config_files
	REPLACE "^(.*)(\/|\\\\)(.*)cmakedeps_paths.cmake$" "\\1"
		OUTPUT_VARIABLE conan_config_dirs
)
message(STATUS "Adding Conan generators directories to CMAKE_PREFIX_PATH: ${conan_config_dirs}")
list(APPEND CMAKE_PREFIX_PATH ${conan_config_dirs})
foreach(conan_config_file IN LISTS conan_config_files)
	message(STATUS "Including Conan configuration file: ${conan_config_file}")
	include(${conan_config_file})
endforeach()
