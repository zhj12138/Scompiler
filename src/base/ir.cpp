#include "ir.hpp"

void IRBuilder::new_func_beg(const std::string &func_name) {
  ircode_list_.push_back(std::make_shared<IRCode>(IRCode::Op::FUNCBEG,
                                                  std::make_shared<IRAddr>(func_name),
                                                  nullptr, nullptr));
}
void IRBuilder::new_func_end() {
  ircode_list_.push_back(std::make_shared<IRCode>(IRCode::Op::FUNCEND,
                                                  nullptr, nullptr, nullptr));
}
void IRBuilder::new_mov(const IRVar &var0, const IRVar &var1) {
  ircode_list_.push_back(std::make_shared<IRCode>(IRCode::Op::MOV,
                                                  std::make_shared<IRAddr>(var0),
                                                  std::make_shared<IRAddr>(var1),
                                                  nullptr));
}
void IRBuilder::new_mov(const IRVar &var0, int imm) {
  ircode_list_.push_back(std::make_shared<IRCode>(IRCode::Op::MOV,
                                                  std::make_shared<IRAddr>(var0),
                                                  std::make_shared<IRAddr>(imm),
                                                  nullptr));
}
void IRBuilder::new_ret(const IRVar &var0) {
  ircode_list_.push_back(std::make_shared<IRCode>(IRCode::Op::RET,
                                                  std::make_shared<IRAddr>(var0),
                                                  nullptr, nullptr));
}
void IRBuilder::new_ret(int imm) {
  ircode_list_.push_back(std::make_shared<IRCode>(IRCode::Op::RET,
                                                  std::make_shared<IRAddr>(imm),
                                                  nullptr, nullptr));
}
