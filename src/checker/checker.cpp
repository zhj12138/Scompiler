#include "checker.hpp"

#include "compile_error.hpp"

void Checker::check(ProgramPtr &program) {
  visit(program);
}

void Checker::visit(ProgramPtr &program) {
  for (auto &item : program->func_decl_vec()) {
    if (std::holds_alternative<FunctionPtr>(item)) {
      visit(std::get<FunctionPtr>(item));
    } else if (std::holds_alternative<DeclarationPtr>(item)) {
      visit(std::get<DeclarationPtr>(item));
    } else { assert(false); }
  }
}

void Checker::visit(FunctionPtr &function) {
  symbol_table_.enter();
  if (!symbol_table_.can_use_function_name(function->name())) {
    throw check_error("already has a variable with name: " + function->name());
  }
  auto new_function_entry = std::make_shared<FunctionEntry>(function);
  if (function->compound_statement()) { // 函数定义
    if (symbol_table_.function_is_defined(function->name())) {
      throw check_error("multiple definition for function: " + function->name());
    }
    auto old_function_entry = symbol_table_.lookup_function(function->name());
    if (old_function_entry && *new_function_entry != *old_function_entry) {
      throw check_error("unmatched function definition: " + function->name());
    }
    symbol_table_.add_function_definition(new_function_entry);
    visit(function->parameter_list());
    visit(function->compound_statement());
  } else {  // 函数声明
    auto old_function_entry = symbol_table_.lookup_function(function->name());
    if (old_function_entry) {
      if (*new_function_entry != *old_function_entry) {
        throw check_error("unmatched function definition: " + function->name());
      }
    } else {
      symbol_table_.add_function_declaration(new_function_entry);
    }
  }
  symbol_table_.leave();
}

void Checker::visit(BaseType &type) {}

void Checker::visit(ParameterListPtr &parameter_list) {
  /* 将函数参数添加到符号表中 */
  for (auto &var : parameter_list->variables()) {
    if (!symbol_table_.can_use_var_name(var->name())) {
      throw check_error(var->name() + " already used");
    }
    symbol_table_.add_variable(var);
  }
}

void Checker::visit(CompoundStatementPtr &compound_statement) {
  symbol_table_.enter();
  for (auto &block_item : compound_statement->block_item_vec()) {
    visit(block_item);
  }
  symbol_table_.leave();
}
void Checker::visit(BlockItemPtr &block_item) {
  if (std::holds_alternative<StatementPtr>(block_item->value())) {
    visit(std::get<StatementPtr>(block_item->value()));
  } else if (std::holds_alternative<DeclarationPtr>(block_item->value())) {
    visit(std::get<DeclarationPtr>(block_item->value()));
  } else { assert(false); }
}
void Checker::visit(StatementPtr &statement) {
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
void Checker::visit(DeclarationPtr &declaration) {
  if (!symbol_table_.can_use_var_name(declaration->var()->name())) {
    throw check_error(declaration->var()->name() + " already used");
  }
  symbol_table_.add_variable(declaration->var());
  if (declaration->init_exp()) {
    visit(declaration->init_exp());
  }
}
void Checker::visit(ExpressionListPtr &expression_list) {
  for (auto &exp : expression_list->expression_vec()) {
    visit(exp);
  }
}
void Checker::visit(ExpressionPtr &expression) {
  visit(expression->assignment());
}
void Checker::visit(AssignmentPtr &assignment) {
  if (std::holds_alternative<ConditionalPtr>(assignment->value())) {
    visit(std::get<ConditionalPtr>(assignment->value()));
  } else if (std::holds_alternative<AssignExpPtr>(assignment->value())) {
    visit(std::get<AssignExpPtr>(assignment->value()));
  } else { assert(false); }
}
void Checker::visit(ConditionalPtr &conditional) {
  visit(conditional->cond());
  if (conditional->cond_true()) {
    visit(conditional->cond_true());
    visit(conditional->cond_false());
  }
}
void Checker::visit(LogicalOrPtr &logical_or) {
  if (logical_or->left()) {
    visit(logical_or->left());
  }
  visit(logical_or->right());
}
void Checker::visit(LogicalAndPtr &logical_and) {
  if (logical_and->left()) {
    visit(logical_and->left());
  }
  visit(logical_and->right());
}
void Checker::visit(EqualityPtr &equality) {
  if (equality->left()) {
    visit(equality->left());
  }
  visit(equality->right());
}
void Checker::visit(RelationalPtr &relational) {
  if (relational->left()) {
    visit(relational->left());
  }
  visit(relational->right());
}
void Checker::visit(AdditivePtr &additive) {
  if (additive->left()) {
    visit(additive->left());
  }
  visit(additive->right());
}
void Checker::visit(MultiplicativePtr &multiplicative) {
  if (multiplicative->left()) {
    visit(multiplicative->left());
  }
  visit(multiplicative->right());
}
void Checker::visit(UnaryPtr &unary) {
  if (std::holds_alternative<PostfixPtr>(unary->value())) {
    visit(std::get<PostfixPtr>(unary->value()));
  } else if (std::holds_alternative<UnaryPtr>(unary->value())) {
    visit(std::get<UnaryPtr>(unary->value()));
  } else { assert(false); }
}
void Checker::visit(PostfixPtr &postfix) {
  if (std::holds_alternative<PrimaryPtr>(postfix->value())) {
    visit(std::get<PrimaryPtr>(postfix->value()));
  } else if (std::holds_alternative<FuncCallPtr>(postfix->value())) {
    visit(std::get<FuncCallPtr>(postfix->value()));
  } else if (std::holds_alternative<ArrayPtr>(postfix->value())) {
    visit(std::get<ArrayPtr>(postfix->value()));
  } else { assert(false); }
}
void Checker::visit(PrimaryPtr &primary) {
  if (std::holds_alternative<ExpressionPtr>(primary->value())) {
    visit(std::get<ExpressionPtr>(primary->value()));
  } else if (std::holds_alternative<int>(primary->value())) {
  } else if (std::holds_alternative<std::string>(primary->value())) {
    auto name = std::get<std::string>(primary->value());
    auto result = symbol_table_.lookup_variable(name);
    if (!result) {
      throw check_error("use unknown variable: " + name);
    }
  } else { assert(false); }
}
void Checker::visit(ReturnStatementPtr &return_statement) {
  visit(return_statement->exp());
}
void Checker::visit(ExpStatementPtr &exp_statement) {
  if (exp_statement->exp()) {
    visit(exp_statement->exp());
  }
}
void Checker::visit(IfStatementPtr &if_statement) {
  visit(if_statement->cond_exp());
  visit(if_statement->if_stmt());
  if (if_statement->else_stmt()) {  // has else branch
    visit(if_statement->else_stmt());
  }
}
void Checker::visit(ForExpStatementPtr &for_exp_statement) {
  symbol_table_.enter_loop();
  if (for_exp_statement->init_exp()) {
    visit(for_exp_statement->init_exp());
  }
  if (for_exp_statement->cond_exp()) {
    visit(for_exp_statement->cond_exp());
  }
  if (for_exp_statement->update_exp()) {
    visit(for_exp_statement->update_exp());
  }
  visit(for_exp_statement->statement());
  symbol_table_.leave_loop();
}
void Checker::visit(ForDecStatementPtr &for_dec_statement) {
  symbol_table_.enter_loop();
  if (for_dec_statement->init_decl()) {
    visit(for_dec_statement->init_decl());
  }
  if (for_dec_statement->cond_exp()) {
    visit(for_dec_statement->cond_exp());
  }
  if (for_dec_statement->update_exp()) {
    visit(for_dec_statement->update_exp());
  }
  visit(for_dec_statement->statement());
  symbol_table_.leave_loop();
}
void Checker::visit(WhileStatementPtr &while_statement) {
  symbol_table_.enter_loop();
  visit(while_statement->cond_exp());
  visit(while_statement->statement());
  symbol_table_.leave_loop();
}
void Checker::visit(DoStatementPtr &do_statement) {
  symbol_table_.enter_loop();
  visit(do_statement->statement());
  visit(do_statement->cond_exp());
  symbol_table_.leave_loop();
}
void Checker::visit(BreakStatementPtr &break_statement) {
  if (!symbol_table_.in_loop()) {
    throw check_error("use break out of loop");
  }
}
void Checker::visit(ContinueStatementPtr &continue_statement) {
  if (!symbol_table_.in_loop()) {
    throw check_error("use continue out of loop");
  }
}
void Checker::visit(AssignExpPtr &assign_exp) {
  visit(assign_exp->left());
  visit(assign_exp->right());
}
void Checker::visit(FuncCallPtr &func_call) {
  auto result = symbol_table_.lookup_function(func_call->func_name());
  if (!result) {
    throw check_error("use unknown function: " + func_call->func_name());
  }
  // 只检查参数个数，不检查类型
  if (result->param_type_vec().size() != func_call->expression_list()->expression_vec().size()) {
    throw check_error("function param num didn't match: " + func_call->func_name());
  }
  visit(func_call->expression_list());
}
void Checker::visit(ArrayPtr &array) {
  if (std::holds_alternative<PrimaryPtr>(array->name())) {
    // 只检查变量是否为数组类型，暂时不检查维度
    auto primary = std::get<PrimaryPtr>(array->name());
    if (!std::holds_alternative<std::string>(primary->value())) {
      throw check_error("array name should be string");
    }
    auto name = std::get<std::string>(primary->value());
    auto result = symbol_table_.lookup_variable(name);
    if (!result) {  // 其实已经检查过了
      throw check_error("use unknown variable: " + name);
    }
    if (!result->type().is_array()) {
      throw check_error("variable " + name + " is not array");
    }
    for (auto &exp : array->expression_vec()) {
      visit(exp);
    }
  } else if (std::holds_alternative<FuncCallPtr>(array->name())) {
    assert(false);  // 暂不支持该语法
  } else {
    assert(false);
  }
}
