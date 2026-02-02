cmake_minimum_required(VERSION 3.20)

project(minijava_compiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Link compile_commands.json to the project root for clang-tidy and tools.
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    file(CREATE_LINK
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
        SYMBOLIC
    )
endif()

file(GLOB_RECURSE ALL_CPP CONFIGURE_DEPENDS ${SRC_DIR}/*.cpp)
list(REMOVE_ITEM ALL_CPP ${SRC_DIR}/vm/vm.cpp ${SRC_DIR}/main.cpp)

add_library(minijava_core
    ${ALL_CPP}
)
target_include_directories(minijava_core PUBLIC ${SRC_DIR})

add_executable(compiler
    ${SRC_DIR}/main.cpp
)
target_link_libraries(compiler PRIVATE minijava_core)

add_executable(vm
    ${SRC_DIR}/vm/vm.cpp
    ${SRC_DIR}/util/serialize.cpp
)

foreach(target minijava_core compiler vm)
    target_include_directories(${target} PRIVATE ${SRC_DIR})
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target} PRIVATE -g -Wall -Wextra -Wpedantic)
    endif()
endforeach()

include(CTest)
if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    include(GoogleTest)

    add_executable(minijava_tests
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/lexer_valid_tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/lexer_exact_test.cpp
    )
    target_link_libraries(minijava_tests PRIVATE GTest::gtest_main minijava_core)
    target_include_directories(minijava_tests PRIVATE ${SRC_DIR})
    target_compile_definitions(minijava_tests PRIVATE
        TEST_FILES_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/test_files"
    )
    gtest_discover_tests(minijava_tests)
endif()

file(GLOB_RECURSE FORMAT_SOURCES CONFIGURE_DEPENDS
    ${SRC_DIR}/*.cpp
    ${SRC_DIR}/*.hpp
    ${SRC_DIR}/*.h
)

add_custom_target(fmt
    COMMAND clang-format -i ${FORMAT_SOURCES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(tree
    COMMAND dot -Tpdf ${CMAKE_CURRENT_SOURCE_DIR}/output/tree.dot -o ${CMAKE_CURRENT_SOURCE_DIR}/tree.pdf
)

add_custom_target(st
    COMMAND dot -Tpdf ${CMAKE_CURRENT_SOURCE_DIR}/output/st.dot -o ${CMAKE_CURRENT_SOURCE_DIR}/st.pdf
)

add_custom_target(cfg
    COMMAND dot -Tpdf ${CMAKE_CURRENT_SOURCE_DIR}/output/cfg.dot -o ${CMAKE_CURRENT_SOURCE_DIR}/cfg.pdf
)
