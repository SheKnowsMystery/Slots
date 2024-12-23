cmake_minimum_required(VERSION 3.28)

set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

project(
	Slots
	LANGUAGES CXX
	VERSION 1.0
)

add_executable(${PROJECT_NAME})

file(GLOB SRC "./source/*.cxx")
file(GLOB HPP "./include/*.h")
set(INC "./include/")

source_group(source FILES ${SRC})
source_group(headers FILES ${HPP})

target_sources(${PROJECT_NAME} PRIVATE ${SRC})
target_include_directories(${PROJECT_NAME} PRIVATE ${INC})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

set(msvc $<CXX_COMPILER_ID:MSVC>)
set(clang $<CXX_COMPILER_ID:Clang>)

if (${WIN32})
	target_link_options(
		${PROJECT_NAME}
		PRIVATE
		$<${msvc}:/SUBSYSTEM:WINDOWS>
		$<${clang}:-Wl,/SUBSYSTEM:WINDOWS>
	)
endif()

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	$<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
	$<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

find_package(SDL2_image CONFIG REQUIRED)
target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	$<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/templates/.clangd.in
	${CMAKE_CURRENT_SOURCE_DIR}/.clangd
	@ONLY
)

set(VCPKG_PACKAGES_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET})

set(VCPKG_BINARY_DIR ${VCPKG_PACKAGES_DIR}/bin/)

if(WIN32)
	set(LIBRARY_EXTENSION ".dll")
elseif(APPLE)
	set(LIBRARY_EXTENSION ".dylib")
elseif(UNIX)
	set(LIBRARY_EXTENSION ".so")
endif()

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})

install(
	TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	CONFIGURATIONS Release
)

# install(
# 	FILES $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
# 	DESTINATION bin
# )
# ^^^^^^^^^^^
# Doesn't work

install(
	DIRECTORY ${VCPKG_BINARY_DIR}
	DESTINATION bin
	CONFIGURATIONS Release
	FILES_MATCHING PATTERN "*${LIBRARY_EXTENSION}"
)

install(
	DIRECTORY assets/
	DESTINATION bin/assets
	CONFIGURATIONS Release
)