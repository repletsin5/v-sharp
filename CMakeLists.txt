cmake_minimum_required(VERSION 3.20)
project(
    VSharp 
    VERSION 0.1.0
    LANGUAGES CXX
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/source/include/config.in.hxx
    ${CMAKE_CURRENT_BINARY_DIR}/source/include/config.hxx
    @ONLY
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

foreach(OUTPUTCONFIG IN ITEMS RUNTIME LIBRARY ARCHIVE)
    set(CMAKE_${OUTPUTCONFIG}_OUTPUT_DIRECTORY
        ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}
    )
endforeach()

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build." FORCE
    )
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(VSHARP_SOURCES
    source/main.cxx
    source/lexer.cxx
    source/parser.cxx
    source/ast.cxx
    source/lsp.cxx
    source/server.cxx
    source/cli.cxx
)

add_executable(vsharp ${VSHARP_SOURCES})

target_include_directories(vsharp
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/include
        ${CMAKE_CURRENT_BINARY_DIR}/source/include
)

target_compile_options(vsharp PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4
        /EHsc
    >
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:
        -Wall
        -Wextra
        -Wpedantic
    >
)

target_compile_options(vsharp PRIVATE
    $<$<CONFIG:Release>:
        $<$<CXX_COMPILER_ID:MSVC>:/O2>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O2>
    >
    $<$<CONFIG:Debug>:
        $<$<CXX_COMPILER_ID:MSVC>:/Od>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O0 -g>
    >
)

option(ENABLE_SANITIZERS "Enable ASan/UBSan in Debug builds" ON)

if(ENABLE_SANITIZERS AND NOT MSVC)
    target_compile_options(vsharp PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address,undefined>
    )
    target_link_options(vsharp PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address,undefined>
    )
endif()

enable_testing()

add_executable(lexer_tests
    tests/lexer_tests.cxx
    source/lexer.cxx
)

target_include_directories(lexer_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/include
)

target_compile_options(lexer_tests PRIVATE
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

add_test(
    NAME LexerTests
    COMMAND lexer_tests
)
