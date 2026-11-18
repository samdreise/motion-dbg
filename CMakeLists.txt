cmake_minimum_required(VERSION 3.20)
project(MotionDBG LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# using google's test suite
find_package(GTest REQUIRED)

set(WARNING_FLAGS -Wall -Wextra -Wpedantic)

file(GLOB_RECURSE MI_RECORDS_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/mi_records/*.cpp)
add_library(mi_records STATIC ${MI_RECORDS_SOURCES})
target_include_directories(mi_records PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_compile_options(mi_records PRIVATE ${WARNING_FLAGS})

enable_testing()

file(GLOB_RECURSE MI_RECORDS_TEST_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/tests/mi_records/*.cpp
)

add_executable(unit_tests ${MI_RECORDS_TEST_SOURCES})
target_link_libraries(unit_tests PRIVATE mi_records GTest::gtest_main)
target_include_directories(unit_tests PRIVATE ${CMAKE_SOURCE_DIR}/tests)
target_compile_options(unit_tests PRIVATE ${WARNING_FLAGS})

include(GoogleTest)
gtest_discover_tests(unit_tests)
