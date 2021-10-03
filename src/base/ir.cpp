#include "ir.hpp"

IRBuilder::iterator IRBuilder::new_func_beg(const std::string &func_name) {
  return new_func_beg(ircode_list_.end(), func_name);
}
IRBuilder::iterator IRBuilder::new_func_beg(IRBuilder::iterator pos, const std::string &func_name) {
  return ircode_list_.insert(pos, std::make_shared<IRCode>(IRCode::Op::FUNCBEG,
                                                           std::make_shared<IRAddr>(func_name),
                                                           nullptr, nullptr));
}

IRBuilder::iterator IRBuilder::new_func_end() {
  return new_func_end(ircode_list_.end());
}
IRBuilder::iterator IRBuilder::new_func_end(IRBuilder::iterator pos) {
  return ircode_list_.insert(pos, std::make_shared<IRCode>(IRCode::Op::FUNCEND,
                                                           nullptr, nullptr, nullptr));
}

IRBuilder::iterator IRBuilder::new_mov(const IRVar &var0, const IRVar &var1) {
  return new_mov(ircode_list_.end(), var0, var1);
}
IRBuilder::iterator IRBuilder::new_mov(IRBuilder::iterator pos, const IRVar &var0, const IRVar &var1) {
  return ircode_list_.insert(pos, std::make_shared<IRCode>(IRCode::Op::MOV,
                                                           std::make_shared<IRAddr>(var0),
                                                           std::make_shared<IRAddr>(var1),
                                                           nullptr));
}
IRBuilder::iterator IRBuilder::new_mov(const IRVar &var0, int imm) {
  return new_mov(ircode_list_.end(), var0, imm);
}
IRBuilder::iterator IRBuilder::new_mov(IRBuilder::iterator pos, const IRVar &var0, int imm) {
  return ircode_list_.insert(pos, std::make_shared<IRCode>(IRCode::Op::MOV,
                                                           std::make_shared<IRAddr>(var0),
                                                           std::make_shared<IRAddr>(imm),
                                                           nullptr));
}
IRBuilder::iterator IRBuilder::new_ret(const IRVar &var0) {
  return new_ret(ircode_list_.end(), var0);
}
IRBuilder::iterator IRBuilder::new_ret(IRBuilder::iterator pos, const IRVar &var0) {
  return ircode_list_.insert(pos, std::make_shared<IRCode>(IRCode::Op::RET,
                                                           std::make_shared<IRAddr>(var0),
                                                           nullptr, nullptr));
}
IRBuilder::iterator IRBuilder::new_ret(int imm) {
  return new_ret(ircode_list_.end(), imm);
}
IRBuilder::iterator IRBuilder::new_ret(IRBuilder::iterator pos, int imm) {
  return ircode_list_.insert(pos, std::make_shared<IRCode>(IRCode::Op::RET,
                                                           std::make_shared<IRAddr>(imm),
                                                           nullptr, nullptr));
}
