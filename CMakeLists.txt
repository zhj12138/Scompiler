cmake_minimum_required(VERSION 3.16)
project(Scompiler)

set(CMAKE_CXX_STANDARD 17)

include_directories(src/base)

add_executable(Scompiler
        src/main.cpp
        src/base/token.cpp
        src/base/token_stream.cpp
        src/base/variable.cpp
        src/base/symbol_table.cpp
        src/base/ast.cpp
        src/lexer/handwritten_lexer.cpp
        src/lexer/spirit_lexer.cpp
        src/parser/recursive_descent_parser.cpp
        src/translator/translator.cpp
        src/optimizer/optimizer.cpp
        src/reg_allocator/reg_allocator.cpp
        src/asm_generator/asm_generator.cpp
        )
