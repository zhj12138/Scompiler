#include "lexer/lexer.hpp"
#include "parser/parser.hpp"

#include "config.hpp"

/* for debug */
#include "debug.hpp"
#include <fstream>

int main(int argc, char *argv[]) {
  config.init(argc, argv);
  auto token_stream = lex_file(config.input_file);
  if (config.print_token) {
    std::ofstream ofs(config.token_file);
    ofs << token_stream << std::endl;
  }
  ProgramPtr program = parse(token_stream);
  if (config.print_ast) {
    std::ofstream ofs(config.ast_file);
    ofs << program << std::endl;
  }
}
