cmake_minimum_required(VERSION 3.20)
project(VSharp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/source/*.cxx)

add_executable(vsharp ${SOURCES})

if(MSVC)
    target_compile_options(vsharp PRIVATE /W4 /EHsc)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(vsharp PRIVATE /Zi /Od)
    else()
        target_compile_options(vsharp PRIVATE /O2)
    endif()
else()
    target_compile_options(vsharp PRIVATE -Wall -Wextra -pedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(vsharp PRIVATE -g)
    else()
        target_compile_options(vsharp PRIVATE -O2)
    endif()
endif()

target_include_directories(vsharp PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_executable(lexer_tests
    tests/lexer_tests.cxx
    source/lexer.cxx
)

target_include_directories(lexer_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)

enable_testing()
add_test(NAME LexerTests COMMAND lexer_tests)