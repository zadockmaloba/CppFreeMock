cmake_minimum_required(VERSION 3.20)
project(CppFreeMockTest)

find_package(GTest REQUIRED)

add_executable(CppFreeMockTest hello_world.cpp)

include_directories(
    cpp11
    posix
    x86
    arm
    win32
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(CppFreeMockTest
PRIVATE GTest::gtest
PRIVATE GTest::gtest_main
PRIVATE GTest::gmock
)