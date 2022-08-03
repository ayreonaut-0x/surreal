cmake_minimum_required(VERSION 3.23)

if(GENERATOR_IS_MULTI_CONFIG)
	set(CMAKE_DEFAULT_BUILD_TYPE RelWithDebInfo)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -ggdb" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fno-plt" CACHE STRING "" FORCE)

project(
	SURREAL
	VERSION 0.1.0
	DESCRIPTION "Framework for live-rendering applications."
	HOMEPAGE_URL https://github.com/ayreonaut-0x/surreal
	LANGUAGES CXX
)

## Project options

option(SURREAL_USE_CXX23 "Enable experimental C++23 features if available (Default: OFF)" OFF)
option(SURREAL_SHARED_BUILD "Build Surreal as a shared library object (Default: ON)" ON)
option(SURREAL_LTO_BUILD "Build Surreal with link-time optimization (Default: ON)" ON)

if(SURREAL_USE_CXX23)
	list(FIND CMAKE_CXX_KNOWN_FEATURES cxx_std_23 has_cxx_std_23)
	if(has_cxx_std_23)
		set(CMAKE_CXX_STANDARD 23)
	endif()
endif()

if(SURREAL_SHARED_BUILD)
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND SURREAL_LTO_BUILD)
	include(CheckIPOSupported)
	check_ipo_supported(RESULT has_ipo OUTPUT no_ipo_msg LANGUAGES CXX)
	if(has_ipo)
		set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
	else()
		message(WARNING "${no_ipo_msg}")
	endif()
endif()

set(__CSD__ "${CMAKE_CURRENT_SOURCE_DIR}")
set(__DEP_DIR__ "${__CSD__}/external")

include(FetchContent)
FetchContent_Declare(
	fmtlib
	GIT_REPOSITORY https://github.com/fmtlib/fmt.git
	GIT_TAG        c4ee726532178e556d923372f29163bd206d7732
	GIT_PROGRESS   ON
)

FetchContent_Declare(
	spdlog
	GIT_REPOSITORY https://github.com/gabime/spdlog.git
	GIT_TAG        76fb40d95455f249bd70824ecfcae7a8f0930fa3
	GIT_PROGRESS   ON
)

set(FMT_SYSTEM_HEADERS ON)
set(SPDLOG_FMT_EXTERNAL ON)
set(SPDLOG_DISABLE_DEFAULT_LOGGER ON)
FetchContent_MakeAvailable(fmtlib spdlog)

set(
	SURREAL_CXXFLAGS
	-pedantic -Wall -Wextra -Wshadow -Wconversion
)

## End Project options

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
	target_compile_definitions(surreal PUBLIC SURREAL_BUILD_DLL)
	target_compile_options(surreal PRIVATE -fvisibility=hidden)
else()
	add_library(
		surreal
		STATIC
	)
endif()
target_compile_definitions(surreal PRIVATE SURREAL_CPP_VERSION=${CMAKE_CXX_STANDARD})

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

add_dependencies(surreal fmt::fmt spdlog::spdlog)
target_compile_options(surreal BEFORE PUBLIC ${SURREAL_CXXFLAGS})
target_include_directories(surreal PRIVATE ${__CSD__}/include ${FMT_SOURCE_DIR}/${FMT_INC_DIR} ${spdlog_SOURCE_DIR}/include)
target_include_directories(surreal INTERFACE ${__CSD__}/include)
target_link_libraries(surreal PRIVATE ${surreal_XCB_DEPS} fmt::fmt spdlog::spdlog)

add_executable(test main.cpp)
add_dependencies(test surreal)
target_link_libraries(test surreal)
