cmake_minimum_required(VERSION 3.16)
project(Scompiler)

set(CMAKE_CXX_STANDARD 17)

add_executable(Scompiler
        src/main.cpp
        src/lexer/Token.cpp
        src/lexer/handwritten_lexer.cpp
        src/lexer/spirit_lexer.cpp
        src/parser/recursive_descent_parser.cpp)
