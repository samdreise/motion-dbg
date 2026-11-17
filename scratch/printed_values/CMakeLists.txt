cmake_minimum_required(VERSION 3.20)

# The project and its program take the folder's name, so build.sh knows the program is build/debug/<folder>
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME name)
project(${name} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# every .cpp in this folder; CONFIGURE_DEPENDS picks up new files without re-running cmake by hand
file(GLOB SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${name} ${SOURCES})
target_compile_options(${name} PRIVATE -Wall -Wextra -Wpedantic)
