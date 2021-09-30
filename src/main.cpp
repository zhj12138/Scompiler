#include "lexer/lexer.hpp"
#include "parser/parser.hpp"

#include <algorithm>

int main() {
  auto token_stream = lex_file("../test/lex.c");
  std::for_each(token_stream.begin(), token_stream.end(), [](auto token) {
    std::cout << token << "\n";
  });
}
