#include "lexer/lexer.hpp"
#include "parser/parser.hpp"

/* for debug */
#include "debug.hpp"

int main() {
  auto token_stream = lex_file("../test/simple.c");
  std::cout << token_stream << std::endl;
  ProgramPtr program = parse(token_stream);
  std::cout << program << std::endl;
}
