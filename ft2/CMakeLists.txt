cmake_minimum_required(VERSION 3.10)
project(ft2)

set(SRC
    ${PROJECT_SOURCE_DIR}/main.cpp
)

pkg_check_modules(GTK2 REQUIRED gtk+-2.0)
include_directories(${GTK2_INCLUDE_DIRS})
link_directories(${GTK2_LIBRARY_DIRS})
list(APPEND FC_DEP_LIBS ${GTK2_LIBRARIES})
pkg_check_modules(GTH2 REQUIRED gthread-2.0)
include_directories(${GTH2_INCLUDE_DIRS})
link_directories(${GTH2_LIBRARY_DIRS})
list(APPEND FC_DEP_LIBS ${GTH2_LIBRARIES})
	
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${FC_DEP_LIBS} spes png jpeg xcb pthread)
