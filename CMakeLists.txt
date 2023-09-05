cmake_minimum_required(VERSION 3.20)
project(CppFreeMockTest)

add_subdirectory(inline-hook)

find_package(GTest REQUIRED)

add_executable(CppFreeMockTest hello_world.cpp And64InlineHook/And64InlineHook.cpp)

include_directories(
    cpp11
    posix
    x86
    arm
    win32
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/And64InlineHook
    ${CMAKE_CURRENT_SOURCE_DIR}/inline-hook/hooker
    ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(CppFreeMockTest
PRIVATE GTest::gtest
PRIVATE GTest::gtest_main
PRIVATE GTest::gmock
PRIVATE inline-hook
)