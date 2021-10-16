cmake_minimum_required(VERSION 3.16)
project(Scompiler)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

include_directories(src/base)
include_directories(src/util)

include_directories(third_party)

add_executable(Scompiler
        src/main.cpp
        src/util/config.cpp
        src/base/token.cpp
        src/base/ast.cpp
        src/util/debug.cpp
        src/lexer/handwritten_lexer.cpp
        src/lexer/spirit_lexer.cpp
        src/parser/recursive_descent_parser.cpp
        src/checker/checker.cpp
        src/translator/translator.cpp
        src/optimizer/basic_block.cpp
        src/optimizer/function_block.cpp
        src/optimizer/module.cpp
        src/asm_generator/asm_generator.cpp
        src/optimizer/detail_debug.cpp src/optimizer/detail_debug.hpp)
target_link_libraries(Scompiler ${Boost_LIBRARIES})