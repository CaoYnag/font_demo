cmake_minimum_required(VERSION 3.10)
project(ft1)

if(WIN32)
	if($ENV{LIB_BASE})
	else()
		include_directories(
			$ENV{LIB_BASE}/include
		)
		link_directories(
			$ENV{LIB_BASE}/lib
		)
	endif()

	find_package(Boost 1.67 COMPONENTS)
	if(Boost_FOUND)
		include_directories(${Boost_INCLUDE_DIRS})
	endif()
endif()

set(SRC
    ${PROJECT_SOURCE_DIR}/main.cpp
)

add_executable(${PROJECT_NAME} ${SRC})
if(WIN32)
	target_link_libraries(curve OpenGL32 glu32 glew32 glut_mocks)
endif()
if(UNIX)
	target_link_libraries(curve GL GLU glut pthread xcb)
endif()