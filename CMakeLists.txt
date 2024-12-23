# Minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Project name
project(GameEngine3D)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler options
add_compile_options(-Wall -Wextra -DCL_TARGET_OPENCL_VERSION=100)

# Include directories
include_directories(src)

# Source files
file(GLOB_RECURSE CORE_SOURCES src/core/*.cpp)
file(GLOB_RECURSE UTILS_SOURCES src/utils/*.cpp)
set(SOURCES ${CORE_SOURCES} ${UTILS_SOURCES} src/main.cpp)

# Output object files in the /obj/ directory
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/obj)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/obj)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Executable target
add_executable(main ${SOURCES})

# Link libraries
find_package(OpenCL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

target_link_libraries(main OpenCL m SDL2 SDL2_ttf)
