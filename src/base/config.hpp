#ifndef SCOMPILER_SRC_BASE_CONFIG_HPP_
#define SCOMPILER_SRC_BASE_CONFIG_HPP_

#include <string>

class Config {
 public:
  Config() = default;
  ~Config() = default;

  void init(int argc, char *argv[]);

  std::string input_file;
  std::string token_file;
  std::string ast_file;
  std::string ir_file;
  std::string output_file;
  bool print_token{false};
  bool print_ast{false};
  bool print_ir{false};
  int optimize_level{0};
};

inline Config config;

#endif //SCOMPILER_SRC_BASE_CONFIG_HPP_
