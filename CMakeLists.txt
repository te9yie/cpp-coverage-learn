cmake_minimum_required(VERSION 3.14)
project(t9)

option(T9_BUILD_TESTS "Builds tests" OFF)

file(GLOB_RECURSE sources src/*.cpp)

add_library(t9 STATIC ${sources})
add_library(t9::t9 ALIAS t9)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /utf-8>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-W -Wall>)
target_include_directories(t9 PUBLIC include)

if(T9_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()