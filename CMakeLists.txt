cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(p2prng)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_library(p2prng
	./src/p2prng.h
	./src/p2prng.c
)

find_package(unofficial-sodium CONFIG QUIET)
find_package(sodium QUIET)
find_package(PkgConfig QUIET)
if(unofficial-sodium_FOUND) # vcpkg
	if(TARGET unofficial-sodium::sodium)
		target_link_libraries(p2prng unofficial-sodium::sodium)
	endif()
	if(TARGET unofficial-sodium::sodium_config_public)
		target_link_libraries(p2prng unofficial-sodium::sodium_config_public)
	endif()
elseif(sodium_FOUND)
	target_link_libraries(p2prng sodium)
else()
	if(PkgConfig_FOUND)
		pkg_check_modules(pkgconf_sodium IMPORTED_TARGET libsodium)
	endif()

	if (TARGET PkgConfig::pkgconf_sodium)
		target_link_libraries(p2prng PkgConfig::pkgconf_sodium)
	else()
		message(SEND_ERROR "missing libsodium")
	endif()
endif()

