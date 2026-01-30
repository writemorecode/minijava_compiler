cmake_minimum_required(VERSION 3.20)

project(minijava_compiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/gen)

# Link compile_commands.json to the project root for clang-tidy and tools.
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    file(CREATE_LINK
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
        SYMBOLIC
    )
endif()

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(Parser
    ${SRC_DIR}/parsing/parser.yy
    ${GEN_DIR}/parser.tab.cc
    COMPILE_FLAGS "-d"
    DEFINES_FILE ${GEN_DIR}/parser.tab.hh
)

FLEX_TARGET(Lexer
    ${SRC_DIR}/lexing/lexer.flex
    ${GEN_DIR}/lex.yy.cc
)

ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

file(GLOB_RECURSE ALL_CPP CONFIGURE_DEPENDS ${SRC_DIR}/*.cpp)
list(REMOVE_ITEM ALL_CPP ${SRC_DIR}/vm/vm.cpp)

add_executable(compiler
    ${ALL_CPP}
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)

add_executable(vm
    ${SRC_DIR}/vm/vm.cpp
    ${SRC_DIR}/util/serialize.cpp
)

foreach(target compiler vm)
    target_include_directories(${target} PRIVATE ${SRC_DIR} ${GEN_DIR})
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target} PRIVATE -g -Wall -Wextra -Wpedantic)
    endif()
endforeach()

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
