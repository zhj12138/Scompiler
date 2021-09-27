#include "lexer/lexer.hpp"

#include <algorithm>

int main() {
  Lexer lexer;
  auto token_vec = lexer.lex_file("../test/input.c");
  std::for_each(token_vec.begin(), token_vec.end(), [](auto token) {
    std::cout << token << "\n";
  });
}
