cmake_minimum_required (VERSION 3.5)
set(PROJECT_NAME Chess)

project(${PROJECT_NAME})

set (CMAKE_BUID_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -g -std=c++14 -lSDL2 -lGL -lSDL2_image")

set (source_dir "${PROJECT_SOURCE_DIR}/src/")

file (GLOB source_files "${source_dir}/*.cpp")

add_executable(${PROJECT_NAME} ${source_files})


