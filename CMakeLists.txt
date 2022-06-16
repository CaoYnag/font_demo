cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0074 NEW)

project(font_demo)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY .)
set(LIBRARY_OUTPUT_PATH .)

if(WIN32 AND NOT MINGW)
	error("linux only")
endif()
set( CMAKE_CXX_FLAGS "-std=c++1z")
if(CMAKE_BUILD_TYPE MATCHES "Debug")
	add_compile_options("-g")
	add_definitions(-D__debug__)
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(FT REQUIRED freetype2)
include_directories(${FT_INCLUDE_DIRS})
link_directories(${FT_LIBRARY_DIRS})
list(APPEND FC_DEP_LIBS ${FT_LIBRARIES})

add_subdirectory(ft1)
add_subdirectory(ft2)
