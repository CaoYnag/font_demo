cmake_minimum_required(VERSION 3.10)
project(g2n)

set(SRC
    ${PROJECT_SOURCE_DIR}/main.cpp
)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${FC_DEP_LIBS})
