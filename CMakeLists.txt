cmake_minimum_required(VERSION 3.23)

project(
	SURREAL
	VERSION 0.1.0
	DESCRIPTION "Framework for live-rendering applications."
)

## Project options

option(SURREAL_USE_CXX23 "Enable experimental C++23 features if available (Default: OFF)" OFF)
option(SURREAL_SHARED_BUILD "Build Surreal as a shared library object (Default: OFF)" ON)
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
	option(SURREAL_LTO_BUILD "Build Surreal with link-time optimization (Default: ON)" ON)
	if(SURREAL_LTO_BUILD)
		if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
			set(SURREAL_LTO_FLAGS -flto -fno-fat-lto-objects)
		elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
			set(SURREAL_LTO_FLAGS -flto=full)
		endif()
	endif()
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

if(SURREAL_USE_CXX23)
message(STATUS "SURREAL_USE_CXX23 is set. Testing for cxx_std_23.")
	list(FIND CMAKE_CXX_KNOWN_FEATURES cxx_std_23 __HAS_CXX23__)
	if(__HAS_CXX23__)
		set(CMAKE_CXX_STANDARD 23)
	else()
		message(STATUS "C++23 unsupported.")
	endif()
endif()
message(STATUS "SURREAL_CPP_VERSION is set to ${CMAKE_CXX_STANDARD}.")

set(__CSD__ "${CMAKE_CURRENT_SOURCE_DIR}")
set(__DEP_DIR__ "${__CSD__}/external")

include(FetchContent)
FetchContent_Declare(
	fmtlib
	GIT_REPOSITORY https://github.com/fmtlib/fmt.git
	GIT_TAG        c4ee726532178e556d923372f29163bd206d7732
	GIT_PROGRESS   ON
)

set(__old_build_type ${CMAKE_BUILD_TYPE})
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
FetchContent_MakeAvailable(fmtlib)
set(CMAKE_BUILD_TYPE ${__old_build_type})
unset(__old_build_type)

if(GENERATOR_IS_MULTI_CONFIG)
	set(CMAKE_CONFIGURATION_TYPES Debug OptimizedDebug Release)
	set(CMAKE_DEFAULT_BUILD_TYPE OptimizedDebug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(SURREAL_CXXFLAGS -ggdb -Wall -Wextra -Wpedantic -Wshadow -Wconversion)
elseif(CMAKE_BUILD_TYPE STREQUAL "OptimizedDebug")
	set(SURREAL_CXXFLAGS -ggdb -O2 -Wall -Wextra -Wpedantic -Wshadow -Wconversion)
	if(SURREAL_LTO_BUILD)
		set(SURREAL_CXXFLAGS ${SURREAL_CXXFLAGS} ${SURREAL_LTO_FLAGS})
	endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(SURREAL_CXXFLAGS -O3 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -fgraphite -fgraphite-identity -floop-interchange -floop-nest-optimize -fno-plt -fdevirtualize-at-ltrans -fipa-pta)
	add_compile_definitions(NDEBUG)
	if(SURREAL_LTO_BUILD)
		set(SURREAL_CXXFLAGS ${SURREAL_CXXFLAGS} ${SURREAL_LTO_FLAGS})
	endif()
else()
	message(FATAL_ERROR "Invalid build type specified. Expected \"Debug\", \"OptimizedDebug\" or \"Release\", got ${CMAKE_BUILD_TYPE}.")
endif()

## End Project options

add_compile_definitions(SURREAL_CPP_VERSION=${CMAKE_CXX_STANDARD})

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	add_compile_definitions(SURREAL_PLATFORM_LINUX=1)
	set(__PLATFORM_SRC_DIR__ "${__CSD__}/src/platform/linux")

	find_package(PkgConfig REQUIRED)
	set(surreal_XCB_DEPS xcb xcb-util xcb-keysyms)
	foreach(dep ${surreal_XCB_DEPS})
		pkg_search_module(${dep} REQUIRED IMPORTED_TARGET ${dep})
	endforeach()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_compile_definitions(SURREAL_PLATFORM_WINDOWS=1)
	set(__PLATFORM_SRC_DIR__ "${__CSD__}/src/platform/windows")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	add_compile_definitions(SURREAL_CXX_IS_GCC=1)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_compile_definitions(SURREAL_CXX_IS_CLANG=1)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_compile_definitions(SURREAL_CXX_IS_MSVC=1)
endif()

## Configure useful variables
string(FIND CMAKE_GENERATOR "Visual Studio" __USING_VISUAL_STUDIO)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${__CSD__}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${__CSD__}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${__CSD__}/bin")

## Core library
if(SURREAL_SHARED_BUILD)
	add_library(
		surreal
		SHARED
	)
else()
	add_library(
		surreal
		STATIC
	)
endif()

file(GLOB_RECURSE surreal_SOURCES "${__CSD__}/src/core/*.cpp")
file(GLOB_RECURSE surreal_platform_SOURCES "${__PLATFORM_SRC_DIR__}/*.cpp")
target_sources(
	surreal
	PRIVATE
	${surreal_SOURCES}
	${surreal_platform_SOURCES}
)

if(GENERATOR_IS_MULTI_CONFIG)
	file(GLOB_RECURSE surreal_HEADERS "${__CSD__}/include/core/*.hpp")
	target_sources(
		surreal
		PRIVATE
		${surreal_HEADERS}
	)
endif()

add_dependencies(surreal fmt::fmt)
target_compile_options(surreal BEFORE PUBLIC ${SURREAL_CXXFLAGS})
target_include_directories(surreal PRIVATE ${__CSD__}/include ${FMT_SOURCE_DIR}/${FMT_INC_DIR})
target_include_directories(surreal INTERFACE ${__CSD__}/include)
target_link_libraries(surreal PRIVATE ${surreal_XCB_DEPS} fmt::fmt)

add_executable(test main.cpp)
add_dependencies(test surreal)
target_link_libraries(test surreal)
