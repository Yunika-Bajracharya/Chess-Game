cmake_minimum_required (VERSION 3.5)
set(PROJECT_NAME Chess)

project(${PROJECT_NAME})

set (CMAKE_BUID_TYPE Debug)
if(UNIX) 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -g -std=c++14 -lSDL2 -lGL -lSDL2_image")
set (source_dir "${PROJECT_SOURCE_DIR}/src/")

file (GLOB source_files "${source_dir}/*.cpp")

add_executable(${PROJECT_NAME} ${source_files})

elseif(WIN32)

# find_file(SDL2_INCLUDE_DIR NAME SDL.h HINTS SDL2)
# find_library(SDL2_LIBRARY NAME SDL2)

# target_include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIR})
# target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})    
set (source_dir "${PROJECT_SOURCE_DIR}/src/")

file (GLOB source_files "${source_dir}/*.cpp")

add_executable(${PROJECT_NAME} ${source_files})

find_package(SDL2 PATHS . REQUIRED)
include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES}) 

endif()

