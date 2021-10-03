#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "checker/checker.hpp"
#include "translator/translator.hpp"
#include "optimizer/optimizer.hpp"
// no reg alloc now
#include "asm_generator/asm_generator.hpp"

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
  check(program);
  IRBuilderPtr ir_builder = translate(program);
  if (config.print_ir) {
    std::ofstream ofs(config.ir_file);
    ofs << ir_builder << std::endl;
  }
  if (config.optimize_level > 0) {
    optimize(ir_builder); // do nothing now
  }
  // maybe print optimized ir here?
  std::vector<std::string> asm_vec = generate(ir_builder);
  std::ofstream ofs(config.output_file);
  for (auto &code : asm_vec) {
    ofs << code << "\n";
  }
  ofs << std::endl;
}
