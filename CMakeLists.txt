cmake_minimum_required(VERSION 3.20)
project(MotionDBG LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(WARNING_FLAGS -Wall -Wextra -Wpedantic)

# include headers as "mi_records/<header>.h" via PUBLIC src/ dir.
file(GLOB_RECURSE MI_RECORDS_SOURCES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/mi_records/*.cpp
)
add_library(mi_records STATIC ${MI_RECORDS_SOURCES})
target_include_directories(mi_records PUBLIC ${PROJECT_SOURCE_DIR}/src/mi_records)
target_compile_options(mi_records PRIVATE ${WARNING_FLAGS})

# --- main executable ------------------------------------------------------
add_executable(motiondbg ${PROJECT_SOURCE_DIR}/src/main.cpp )
target_link_libraries(motiondbg PRIVATE mi_records)
target_compile_options(motiondbg PRIVATE ${WARNING_FLAGS})

# --- tests (google test) --------------------------------------------------
enable_testing()
find_package(GTest REQUIRED)

file(GLOB_RECURSE MI_RECORDS_TEST_SOURCES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/tests/mi_records/*.cpp
)
add_executable(unit_tests ${MI_RECORDS_TEST_SOURCES})
target_link_libraries(unit_tests PRIVATE mi_records GTest::gtest_main)
target_include_directories(unit_tests PRIVATE ${PROJECT_SOURCE_DIR}/tests)
target_compile_options(unit_tests PRIVATE ${WARNING_FLAGS})

include(GoogleTest)
gtest_discover_tests(unit_tests)
