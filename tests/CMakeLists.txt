cmake_minimum_required(VERSION 3.14)
project(t9-tests)

include(FetchContent)

# doctest
FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest
    GIT_TAG master
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(doctest)

file(GLOB sources *.cpp)

add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} PRIVATE
    t9::t9
    doctest::doctest_with_main)

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(${PROJECT_NAME})