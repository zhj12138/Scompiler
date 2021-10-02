#include "config.hpp"

#include <boost/program_options.hpp>
#include <iostream>
#include <filesystem>

using namespace boost::program_options;
namespace fs = std::filesystem;

void Config::init(int argc, char **argv) {
  options_description desc{"Options"};
  desc.add_options()
      ("help,h", "print help message")
      ("input-file", value<std::string>(), "the source c file")
      ("token-file,t", value<std::string>(), "file to store token stream")
      ("ast-file,a", value<std::string>(), "file to store ast tree")
      ("ir-file,i", value<std::string>(), "file to store ir code")
      ("output-file,o", value<std::string>(), "file to store asm code")
      ("optimize,O", value<int>(), "optimize level");

  positional_options_description p;
  p.add("input-file", 1);

  variables_map vm;
  store(command_line_parser(argc, argv).options(desc).positional(p).allow_unregistered().run(), vm);
  if (vm.count("help")) {
    std::cout << "Usage: Scompiler <input-file>\n";
    std::cout << desc;
    exit(0);
  }
  if (vm.count("input-file")) {
    input_file = vm["input-file"].as<std::string>();
  } else {
    std::cout << "Usage: Scompiler <input-file>\n";
    std::cout << desc;
    exit(0);
  }
  if (vm.count("token-file")) {
    print_token = true;
    token_file = vm["token-file"].as<std::string>();
  }
  if (vm.count("ast-file")) {
    print_ast = true;
    ast_file = vm["ast-file"].as<std::string>();
  }
  if (vm.count("ir-file")) {
    print_ir = true;
    ir_file = vm["ir-file"].as<std::string>();
  }
  if (vm.count("output-file")) {
    output_file = vm["output-file"].as<std::string>();
  } else {
    output_file = fs::path(input_file).stem().concat(".asm").string();
  }
  if (vm.count("optimize")) {
    optimize_level = vm["optimize"].as<int>();
  }
//  std::cout << "input-file: " << input_file << "\n"
//            << "token-file: " << token_file << "\n"
//            << "ast-file: " << ast_file << "\n"
//            << "ir-file: " << ir_file << "\n"
//            << "ouput-file: " << output_file << "\n"
//            << "optimize-level: " << optimize_level << "\n";
}
