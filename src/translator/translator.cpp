#include "translator.hpp"

IRBuilderPtr Translator::translate(ProgramPtr &program) {
  visit(program);
  return ir_builder_;
}

void Translator::visit(ProgramPtr &program) {
  for (auto &item : program->func_decl_vec()) {
    if (std::holds_alternative<FunctionPtr>(item)) {
      visit(std::get<FunctionPtr>(item));
    } else if (std::holds_alternative<DeclarationPtr>(item)) {
      visit(std::get<DeclarationPtr>(item));
    } else { assert(false); }
  }
}

void Translator::visit(FunctionPtr &function) {
  symbol_table_.enter();
  auto new_function_entry = std::make_shared<FunctionEntry>(function);
  if (function->compound_statement()) { // 函数定义
    symbol_table_.add_function_definition(new_function_entry);
    ir_builder_->new_ir(IROp::FUNBEG,
                        new_ir_addr(function->name()),
                        new_ir_addr(static_cast<int>(function->parameter_list()->variables().size())));
    visit(function->parameter_list());
    visit(function->compound_statement());
    ir_builder_->new_ir(IROp::FUNEND);
  } else {  // 函数声明
    symbol_table_.add_function_declaration(new_function_entry);
  }
  symbol_table_.leave();
}

void Translator::visit(BaseType &type) {}

void Translator::visit(ParameterListPtr &parameter_list) {
  /* 将函数参数添加到符号表中 */
  for (auto &var : parameter_list->variables()) {
    symbol_table_.add_variable(var);
  }
}

void Translator::visit(CompoundStatementPtr &compound_statement) {
  symbol_table_.enter();
  for (auto &block_item : compound_statement->block_item_vec()) {
    visit(block_item);
  }
  symbol_table_.leave();
}
void Translator::visit(BlockItemPtr &block_item) {
  if (std::holds_alternative<StatementPtr>(block_item->value())) {
    visit(std::get<StatementPtr>(block_item->value()));
  } else if (std::holds_alternative<DeclarationPtr>(block_item->value())) {
    visit(std::get<DeclarationPtr>(block_item->value()));
  } else { assert(false); }
}
void Translator::visit(StatementPtr &statement) {
  if (std::holds_alternative<ReturnStatementPtr>(statement->value())) {
    visit(std::get<ReturnStatementPtr>(statement->value()));
  } else if (std::holds_alternative<ExpStatementPtr>(statement->value())) {
    visit(std::get<ExpStatementPtr>(statement->value()));
  } else if (std::holds_alternative<IfStatementPtr>(statement->value())) {
    visit(std::get<IfStatementPtr>(statement->value()));
  } else if (std::holds_alternative<CompoundStatementPtr>(statement->value())) {
    visit(std::get<CompoundStatementPtr>(statement->value()));
  } else if (std::holds_alternative<ForExpStatementPtr>(statement->value())) {
    visit(std::get<ForExpStatementPtr>(statement->value()));
  } else if (std::holds_alternative<ForDecStatementPtr>(statement->value())) {
    visit(std::get<ForDecStatementPtr>(statement->value()));
  } else if (std::holds_alternative<WhileStatementPtr>(statement->value())) {
    visit(std::get<WhileStatementPtr>(statement->value()));
  } else if (std::holds_alternative<DoStatementPtr>(statement->value())) {
    visit(std::get<DoStatementPtr>(statement->value()));
  } else if (std::holds_alternative<BreakStatementPtr>(statement->value())) {
    visit(std::get<BreakStatementPtr>(statement->value()));
  } else if (std::holds_alternative<ContinueStatementPtr>(statement->value())) {
    visit(std::get<ContinueStatementPtr>(statement->value()));
  } else { assert(false); }
}
void Translator::visit(DeclarationPtr &declaration) {
  symbol_table_.add_variable(declaration->var());
  auto[var, is_global] = symbol_table_.lookup_variable(declaration->var()->name());
  auto is_array = var->type().is_array();
  auto get_number_of_init_exp = [](const ExpressionPtr &exp) {
    auto cond = std::get<ConditionalPtr>(exp->assignment()->value());
    auto postfix = std::get<PostfixPtr>(cond->cond()->right()->right()->right()->right()->right()->right()->value());
    auto primary = std::get<PrimaryPtr>(postfix->value());
    return std::get<int>(primary->value());
  };
  if (is_global) {  // 全局变量
    if (is_array) {
      ir_builder_->new_ir(IROp::GBSS, new_ir_addr(var->name()), new_ir_addr(var->type().array_size() * 4));
    } else {
      if (declaration->init_exp()) {  // 初始化
        int number = get_number_of_init_exp(declaration->init_exp()); // 在checker阶段进行检查
        ir_builder_->new_ir(IROp::GINI, new_ir_addr(var->name()), new_ir_addr(number));
      } else {
        ir_builder_->new_ir(IROp::GBSS, new_ir_addr(var->name()), new_ir_addr(4));
      }
    }
  } else {
    if (is_array) {
      tmp_var_ = symbol_table_.lookup_ir_var(var->name());  // 为局部数组的基址分配一个IR变量
      ir_builder_->new_ir(IROp::ALLOC, new_ir_addr(tmp_var_), new_ir_addr(var->type().array_size() * 4));
    } else {
      if (declaration->init_exp()) {
        visit(declaration->init_exp());
        auto right_var1 = tmp_var_;
        auto left_var0 = symbol_table_.lookup_ir_var(declaration->var()->name());
        // 没有必要修改tmp_var_，因为声明语句不能再赋给其他的值
        ir_builder_->new_ir(IROp::MOV, new_ir_addr(left_var0), new_ir_addr(right_var1));
      }
    }
  }
}
// expression_list只用于传递函数参数
void Translator::visit(ExpressionListPtr &expression_list) {
  for (auto &exp : expression_list->expression_vec()) {
    visit(exp);
    ir_builder_->new_ir(IROp::PARAM, new_ir_addr(tmp_var_));
  }
}
void Translator::visit(ExpressionPtr &expression) {
  visit(expression->assignment());
}
void Translator::visit(AssignmentPtr &assignment) {
  if (std::holds_alternative<ConditionalPtr>(assignment->value())) {
    visit(std::get<ConditionalPtr>(assignment->value()));
  } else if (std::holds_alternative<AssignExpPtr>(assignment->value())) {
    visit(std::get<AssignExpPtr>(assignment->value()));
  } else { assert(false); }
}
void Translator::visit(ConditionalPtr &conditional) {
  visit(conditional->cond());
  if (conditional->cond_true()) {
    visit(conditional->cond_true());
    visit(conditional->cond_false());
  }
}
void Translator::visit(LogicalOrPtr &logical_or) {
  if (logical_or->left()) {
    visit(logical_or->left());
    auto right_var1 = tmp_var_;
    visit(logical_or->right());
    auto right_var2 = tmp_var_;
    tmp_var_ = IRVar(symbol_table_.alloc_var());
    ir_builder_->new_ir(IROp::LOR, new_ir_addr(tmp_var_), new_ir_addr(right_var1), new_ir_addr(right_var2));
  } else {
    visit(logical_or->right());
  }
}
void Translator::visit(LogicalAndPtr &logical_and) {
  if (logical_and->left()) {
    visit(logical_and->left());
    auto right_var1 = tmp_var_;
    visit(logical_and->right());
    auto right_var2 = tmp_var_;
    tmp_var_ = IRVar(symbol_table_.alloc_var());
    ir_builder_->new_ir(IROp::LAND, new_ir_addr(tmp_var_), new_ir_addr(right_var1), new_ir_addr(right_var2));
  } else {
    visit(logical_and->right());
  }
}
void Translator::visit(EqualityPtr &equality) {
  if (equality->left()) {
    visit(equality->left());
    auto right_var1 = tmp_var_;
    visit(equality->right());
    auto right_var2 = tmp_var_;
    ir_builder_->new_ir(to_ir_op(equality->op()),
                        new_ir_addr(tmp_var_),
                        new_ir_addr(right_var1),
                        new_ir_addr(right_var2));
  } else {
    visit(equality->right());
  }
}
void Translator::visit(RelationalPtr &relational) {
  if (relational->left()) {
    visit(relational->left());
    auto right_var1 = tmp_var_;
    visit(relational->right());
    auto right_var2 = tmp_var_;
    tmp_var_ = IRVar(symbol_table_.alloc_var());
    ir_builder_->new_ir(to_ir_op(relational->op()),
                        new_ir_addr(tmp_var_),
                        new_ir_addr(right_var1),
                        new_ir_addr(right_var2));
  } else {
    visit(relational->right());
  }
}
void Translator::visit(AdditivePtr &additive) {
  if (additive->left()) {
    visit(additive->left());
    auto right_var1 = tmp_var_;
    visit(additive->right());
    auto right_var2 = tmp_var_;
    tmp_var_ = IRVar(symbol_table_.alloc_var());
    ir_builder_->new_ir(to_ir_op(additive->op()),
                        new_ir_addr(tmp_var_),
                        new_ir_addr(right_var1),
                        new_ir_addr(right_var2));
  } else {
    visit(additive->right());
  }
}
void Translator::visit(MultiplicativePtr &multiplicative) {
  if (multiplicative->left()) {
    visit(multiplicative->left());
    auto right_var1 = tmp_var_;
    visit(multiplicative->right());
    auto right_var2 = tmp_var_;
    tmp_var_ = IRVar(symbol_table_.alloc_var());
    ir_builder_->new_ir(to_ir_op(multiplicative->op()),
                        new_ir_addr(tmp_var_),
                        new_ir_addr(right_var1),
                        new_ir_addr(right_var2));
  } else {
    visit(multiplicative->right());
  }
}
void Translator::visit(UnaryPtr &unary) {
  if (std::holds_alternative<PostfixPtr>(unary->value())) {
    visit(std::get<PostfixPtr>(unary->value()));
  } else if (std::holds_alternative<UnaryPtr>(unary->value())) {
    visit(std::get<UnaryPtr>(unary->value()));
    auto right_var = tmp_var_;
    tmp_var_ = IRVar(symbol_table_.alloc_var());
    ir_builder_->new_ir(to_ir_op(unary->op()), new_ir_addr(tmp_var_), new_ir_addr(right_var));
  } else { assert(false); }
}
void Translator::visit(PostfixPtr &postfix) {
  if (std::holds_alternative<PrimaryPtr>(postfix->value())) {
    visit(std::get<PrimaryPtr>(postfix->value()));
  } else if (std::holds_alternative<FuncCallPtr>(postfix->value())) {
    visit(std::get<FuncCallPtr>(postfix->value()));
  } else if (std::holds_alternative<ArrayPtr>(postfix->value())) {
    visit(std::get<ArrayPtr>(postfix->value()));
    // 暂时不处理
  } else { assert(false); }
}
void Translator::visit(PrimaryPtr &primary) {
  if (std::holds_alternative<ExpressionPtr>(primary->value())) {
    visit(std::get<ExpressionPtr>(primary->value()));
  } else if (std::holds_alternative<int>(primary->value())) {
    int imm = std::get<int>(primary->value());
    tmp_var_ = IRVar(symbol_table_.alloc_var());
    ir_builder_->new_ir(IROp::MOV, new_ir_addr(tmp_var_), new_ir_addr(imm));
  } else if (std::holds_alternative<std::string>(primary->value())) {
    auto name = std::get<std::string>(primary->value());
    tmp_var_ = symbol_table_.lookup_ir_var(name);
    auto[variable, is_global] = symbol_table_.lookup_variable(name);
    bool is_array = variable->type().is_array();
    if (tmp_var_.is_global()) { // 全局变量
      tmp_var_ = IRVar(symbol_table_.alloc_var());
      ir_builder_->new_ir(IROp::LA, new_ir_addr(tmp_var_), new_ir_addr(name));
      if (!is_array) {  // 数组类型只需加载到地址，而变量类型需要加载到值
        auto addr_var = tmp_var_;
        tmp_var_ = IRVar(symbol_table_.alloc_var());
        ir_builder_->new_ir(IROp::LOAD, new_ir_addr(tmp_var_), new_ir_addr(addr_var), new_ir_addr(0));
      }
    } // 局部变量无需做额外处理
    name_ = variable->name();
  } else { assert(false); }
}
void Translator::visit(ReturnStatementPtr &return_statement) {
  visit(return_statement->exp());
  ir_builder_->new_ir(IROp::RET, new_ir_addr(tmp_var_));
}
void Translator::visit(ExpStatementPtr &exp_statement) {
  if (exp_statement->exp()) {
    visit(exp_statement->exp());
  }
}
void Translator::visit(IfStatementPtr &if_statement) {
  visit(if_statement->cond_exp());
  int false_label_number = symbol_table_.alloc_label();
  ir_builder_->new_ir(IROp::BEQZ, new_ir_addr(tmp_var_), new_ir_addr(false_label_number));
  visit(if_statement->if_stmt());
  if (if_statement->else_stmt()) {  // has else branch
    int end_label_number = symbol_table_.alloc_label();
    ir_builder_->new_ir(IROp::JMP, new_ir_addr(end_label_number));
    ir_builder_->new_ir(IROp::LABEL, new_ir_addr(false_label_number));
    visit(if_statement->else_stmt());
    ir_builder_->new_ir(IROp::LABEL, new_ir_addr(end_label_number));
  } else {  // no else branch, false_label就是end_label
    ir_builder_->new_ir(IROp::LABEL, new_ir_addr(false_label_number));
  }
}
void Translator::visit(ForExpStatementPtr &for_exp_statement) {
  symbol_table_.enter_loop();
  if (for_exp_statement->init_exp()) {
    visit(for_exp_statement->init_exp());
  }
  ir_builder_->new_ir(IROp::LABEL, new_ir_addr(symbol_table_.loop_begin_label()));
  if (for_exp_statement->cond_exp()) {
    visit(for_exp_statement->cond_exp());
  }
  ir_builder_->new_ir(IROp::BEQZ, new_ir_addr(tmp_var_), new_ir_addr(symbol_table_.loop_break_label()));
  visit(for_exp_statement->statement());
  ir_builder_->new_ir(IROp::LABEL, new_ir_addr(symbol_table_.loop_continue_label()));
  if (for_exp_statement->update_exp()) {
    visit(for_exp_statement->update_exp());
  }
  ir_builder_->new_ir(IROp::JMP, new_ir_addr(symbol_table_.loop_begin_label()));
  ir_builder_->new_ir(IROp::LABEL, new_ir_addr(symbol_table_.loop_break_label()));
  symbol_table_.leave_loop();
}
void Translator::visit(ForDecStatementPtr &for_dec_statement) {
  symbol_table_.enter_loop();
  if (for_dec_statement->init_decl()) {
    visit(for_dec_statement->init_decl());
  }
  ir_builder_->new_ir(IROp::LABEL, new_ir_addr(symbol_table_.loop_begin_label()));
  if (for_dec_statement->cond_exp()) {
    visit(for_dec_statement->cond_exp());
  }
  ir_builder_->new_ir(IROp::BEQZ, new_ir_addr(tmp_var_), new_ir_addr(symbol_table_.loop_break_label()));
  visit(for_dec_statement->statement());
  ir_builder_->new_ir(IROp::LABEL, new_ir_addr(symbol_table_.loop_continue_label()));
  if (for_dec_statement->update_exp()) {
    visit(for_dec_statement->update_exp());
  }
  ir_builder_->new_ir(IROp::JMP, new_ir_addr(symbol_table_.loop_begin_label()));
  ir_builder_->new_ir(IROp::LABEL, new_ir_addr(symbol_table_.loop_break_label()));
  symbol_table_.leave_loop();
}
void Translator::visit(WhileStatementPtr &while_statement) {
  symbol_table_.enter_loop();
  ir_builder_->new_ir(IROp::LABEL, new_ir_addr(symbol_table_.loop_begin_label()));
  ir_builder_->new_ir(IROp::LABEL, new_ir_addr(symbol_table_.loop_continue_label()));
  visit(while_statement->cond_exp());
  ir_builder_->new_ir(IROp::BEQZ, new_ir_addr(tmp_var_), new_ir_addr(symbol_table_.loop_break_label()));
  visit(while_statement->statement());
  ir_builder_->new_ir(IROp::JMP, new_ir_addr(symbol_table_.loop_begin_label()));
  ir_builder_->new_ir(IROp::LABEL, new_ir_addr(symbol_table_.loop_break_label()));
  symbol_table_.leave_loop();
}
void Translator::visit(DoStatementPtr &do_statement) {
  symbol_table_.enter_loop();
  ir_builder_->new_ir(IROp::LABEL, new_ir_addr(symbol_table_.loop_begin_label()));
  visit(do_statement->statement());
  ir_builder_->new_ir(IROp::LABEL, new_ir_addr(symbol_table_.loop_continue_label()));
  visit(do_statement->cond_exp());
  ir_builder_->new_ir(IROp::BEQZ, new_ir_addr(tmp_var_), new_ir_addr(symbol_table_.loop_break_label()));
  ir_builder_->new_ir(IROp::JMP, new_ir_addr(symbol_table_.loop_begin_label()));
  ir_builder_->new_ir(IROp::LABEL, new_ir_addr(symbol_table_.loop_break_label()));
  symbol_table_.leave_loop();
}
void Translator::visit(BreakStatementPtr &break_statement) {
  ir_builder_->new_ir(IROp::JMP, new_ir_addr(symbol_table_.loop_break_label()));
}
void Translator::visit(ContinueStatementPtr &continue_statement) {
  ir_builder_->new_ir(IROp::JMP, new_ir_addr(symbol_table_.loop_continue_label()));
}
void Translator::visit(AssignExpPtr &assign_exp) {
  visit(assign_exp->right()); // 先访问右边的表达式
  auto right_var1 = tmp_var_;
  visit(assign_exp->left());
  auto unary_name = name_;
  auto[var, is_global] = symbol_table_.lookup_variable(unary_name);
  if (is_global || var->type().is_array()) {  // 全局变量或全局数组或局部数组，都要用STORE指令存入内存中
    // 直接把最后一条LOAD指令改成STORE指令
    ir_builder_->last_ir()->op() = IROp::STORE;
    ir_builder_->last_ir()->a0() = new_ir_addr(right_var1);
  } else {
    ir_builder_->new_ir(IROp::MOV, new_ir_addr(tmp_var_), new_ir_addr(right_var1));
  }
  // 不修改tmp_var_，这样assign_exp的返回值就是左边的值
}
void Translator::visit(FuncCallPtr &func_call) {
  visit(func_call->expression_list());
  tmp_var_ = IRVar(symbol_table_.alloc_var());
  ir_builder_->new_ir(IROp::CALL, new_ir_addr(tmp_var_), new_ir_addr(func_call->func_name()));
}
void Translator::visit(ArrayPtr &array) {
  if (std::holds_alternative<PrimaryPtr>(array->name())) {
    auto primary = std::get<PrimaryPtr>(array->name());
    visit(primary);
    auto base_var = tmp_var_;
    auto array_name = name_;
    auto[result, is_global] = symbol_table_.lookup_variable(array_name);
    auto dimension_vec = result->type().dimension_vec();
    int sz = dimension_vec.size();
    std::vector<int> offset_vec(sz);
    int ini = 1;
    for (int i = sz - 1; i >= 0; --i) {
      if (i == sz - 1) {
        offset_vec[i] = ini;
      } else {
        offset_vec[i] = offset_vec[i - 1] * dimension_vec[i - 1];
      }
    }
    auto offset_var = IRVar(symbol_table_.alloc_var());
    ir_builder_->new_ir(IROp::MOV, new_ir_addr(offset_var), new_ir_addr(0));  // 初始化为0
    for (int i = 0; i < sz; ++i) {
      visit(array->expression_vec()[i]);
      auto t_var = IRVar(symbol_table_.alloc_var());
      ir_builder_->new_ir(IROp::MUL, new_ir_addr(t_var), new_ir_addr(tmp_var_), new_ir_addr(offset_vec[i]));
      ir_builder_->new_ir(IROp::ADD, new_ir_addr(offset_var), new_ir_addr(offset_var), new_ir_addr(t_var));
    }
    ir_builder_->new_ir(IROp::MUL, new_ir_addr(offset_var), new_ir_addr(offset_var), new_ir_addr(4)); // 偏移量乘以4
    auto addr_var = IRVar(symbol_table_.alloc_var());
    ir_builder_->new_ir(IROp::SUB, new_ir_addr(addr_var), new_ir_addr(base_var), new_ir_addr(offset_var));
    tmp_var_ = IRVar(symbol_table_.alloc_var());
    ir_builder_->new_ir(IROp::LOAD, new_ir_addr(tmp_var_), new_ir_addr(addr_var), new_ir_addr(0));
    name_ = array_name;
  } else if (std::holds_alternative<FuncCallPtr>(array->name())) {
    assert(false);  // 不支持该语法
  } else {
    assert(false);
  }
}
