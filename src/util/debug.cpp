#include "debug.hpp"

#include "visitor.hpp"

#include <variant>
#include <iomanip>

std::string to_string(const TokenType &pt) {
  switch (pt) {
    case TokenType::Return: return "Return";
    case TokenType::Int: return "Int";
    case TokenType::If: return "If";
    case TokenType::Else: return "Else";
    case TokenType::For: return "For";
    case TokenType::Do: return "Do";
    case TokenType::While: return "While";
    case TokenType::Break: return "Break";
    case TokenType::Continue: return "Continue";
    case TokenType::Lparen: return "Lparen";
    case TokenType::Rparen: return "Rparen";
    case TokenType::LSbrace: return "LSbrace";
    case TokenType::RSbrace: return "RSbrace";
    case TokenType::LCbrace: return "LCbrace";
    case TokenType::RCbrace: return "RCbrace";
    case TokenType::Semicolon: return "Semicolon";
    case TokenType::Comma: return "Comma";
    case TokenType::Sub: return "Sub";
    case TokenType::Add: return "Add";
    case TokenType::Times: return "Times";
    case TokenType::Divide: return "Divide";
    case TokenType::Mod: return "Mod";
    case TokenType::Not: return "Not";
    case TokenType::Lnot: return "Lnot";
    case TokenType::Or: return "Or";
    case TokenType::Lor: return "Lor";
    case TokenType::And: return "And";
    case TokenType::Land: return "Land";
    case TokenType::Assign: return "Assign";
    case TokenType::Greater: return "Greater";
    case TokenType::Less: return "Less";
    case TokenType::Equal: return "Equal";
    case TokenType::Nequal: return "Nequal";
    case TokenType::Ge: return "Ge";
    case TokenType::Le: return "Le";
    case TokenType::Question: return "Question";
    case TokenType::Colon: return "Colon";
    case TokenType::Identifier:
    case TokenType::Number:
    case TokenType::String: break;
  }
  return "";
}

std::ostream &operator<<(std::ostream &os, const Token &token) {
  if (token.is_keyword()) {
    os << "( " << token.get_string() << " , " << to_string(token.type()) << " )";
  } else if (token.is_punctuation()) {
    os << "( " << token.get_string() << " , " << to_string(token.type()) << " )";
  } else if (token.is_identifier()) {
    os << "( " << token.get_string() << " , Identifier )";
  } else if (token.is_number()) {
    os << "( " << token.get_number() << " , Number )";
  } else if (token.is_string()) {
    os << "( " << token.get_string() << " , String )";
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, TokenStream &token_stream) {
  std::for_each(token_stream.begin(), token_stream.end(), [&os](auto token) {
    os << token << "\n";
  });
  return os;
}

std::string to_string(SupportType type) {
  if (type == SupportType::Int) {
    return "int";
  }
  return "";
}

std::ostream &operator<<(std::ostream &os, VariableType &variable) {
  os << to_string(variable.type());
  for (auto &dim : variable.dimension_vec()) {
    os << "[" << dim << "]";
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, Variable &variable) {
  os << "name: " << variable.name() << ", type: " << variable.type();
  return os;
}

std::string to_string(const Equality::Op &op) {
  switch (op) {
    case Equality::Op::Equal: return "==";
    case Equality::Op::Nequal: return "!=";
  }
  return "";
}
std::string to_string(const Relational::Op &op) {
  switch (op) {
    case Relational::Op::Less: return "<";
    case Relational::Op::Greater: return ">";
    case Relational::Op::Le: return "<=";
    case Relational::Op::Ge: return ">=";
  }
  return "";
}
std::string to_string(const Additive::Op &op) {
  switch (op) {
    case Additive::Op::Add: return "+";
    case Additive::Op::Sub: return "-";
  }
  return "";
}
std::string to_string(const Multiplicative::Op &op) {
  switch (op) {
    case Multiplicative::Op::Times: return "*";
    case Multiplicative::Op::Divide: return "/";
    case Multiplicative::Op::Mod: return "%";
  }
  return "";
}
std::string to_string(const Unary::Op &op) {
  switch (op) {
    case Unary::Op::Sub: return "-";
    case Unary::Op::Not: return "~";
    case Unary::Op::Lnot: return "!";
  }
  return "";
}

class ASTPrinter : public DetailedASTVisitor {
 public:
  explicit ASTPrinter(std::ostream &ostream = std::cout)
      : os(ostream) {}

  void visit(ProgramPtr &program) override {
    os << "program\n";
    auto total_num = program->func_decl_vec().size();
    int cur_num = 0;
    auto stored_prefix = append_prefix(prefix_, true);

    for (auto &item : program->func_decl_vec()) {
      ++cur_num;
      is_end_ = (cur_num == total_num);
      prefix_ = stored_prefix;
      if (std::holds_alternative<FunctionPtr>(item)) {
        visit(std::get<FunctionPtr>(item));
      } else if (std::holds_alternative<DeclarationPtr>(item)) {
        visit(std::get<DeclarationPtr>(item));
      } else {
        assert(false);
      }
    }
  }
  void visit(FunctionPtr &function) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "function(" << function->name() << ")\n";
    is_end_ = false;
    prefix_ = stored_prefix;
    visit(function->ret_type());
    if (function->compound_statement()) { // 函数定义
      is_end_ = false;
      prefix_ = stored_prefix;
      visit(function->parameter_list());
      is_end_ = true;
      prefix_ = stored_prefix;
      visit(function->compound_statement());
    } else {  // 函数声明
      is_end_ = true;
      prefix_ = stored_prefix;
      visit(function->parameter_list());
    }
  }
  void visit(BaseType &type) override {
    print_front();
    os << "type: " << to_string(type.type()) << "\n";
  }
  void visit(ParameterListPtr &parameter_list) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "parameterList\n";
    int total_num = parameter_list->variables().size();
    int cur_num = 0;
    for (auto &param : parameter_list->variables()) {
      ++cur_num;
      is_end_ = (cur_num == total_num);
      prefix_ = stored_prefix;
      visit(param);
    }
  }
  void visit(VariablePtr &variable) {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "variable(" << *variable << ")\n";
  }
  void visit(CompoundStatementPtr &compound_statement) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "compoundStatement\n";
    int total_num = compound_statement->block_item_vec().size();
    int cur_num = 0;
    for (auto &block_item : compound_statement->block_item_vec()) {
      ++cur_num;
      is_end_ = (cur_num == total_num);
      prefix_ = stored_prefix;
      visit(block_item);
    }
  }
  void visit(BlockItemPtr &block_item) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "blockItem\n";
    is_end_ = true;
    prefix_ = stored_prefix;
    if (std::holds_alternative<StatementPtr>(block_item->value())) {
      visit(std::get<StatementPtr>(block_item->value()));
    } else if (std::holds_alternative<DeclarationPtr>(block_item->value())) {
      visit(std::get<DeclarationPtr>(block_item->value()));
    } else {
      assert(false);
    }
  }
  void visit(StatementPtr &statement) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "statement\n";
    is_end_ = true;
    prefix_ = stored_prefix;
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
    } else {
      assert(false);
    }
  }
  void visit(DeclarationPtr &declaration) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "declaration\n";
    if (declaration->init_exp()) {  // initialized
      is_end_ = false;
      prefix_ = stored_prefix;
      visit(declaration->var());
      is_end_ = true;
      prefix_ = stored_prefix;
      visit(declaration->init_exp());
    } else {
      is_end_ = true;
      prefix_ = stored_prefix;
      visit(declaration->var());
    }
  }
  void visit(ExpressionListPtr &expression_list) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "expressionList\n";
    int total_num = expression_list->expression_vec().size();
    int cur_num = 0;
    for (auto &exp : expression_list->expression_vec()) {
      ++cur_num;
      is_end_ = (cur_num == total_num);
      prefix_ = stored_prefix;
      visit(exp);
    }
  }
  void visit(ExpressionPtr &expression) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "expression\n";
    is_end_ = true;
    prefix_ = stored_prefix;
    visit(expression->assignment());
  }
  void visit(AssignmentPtr &assignment) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "assignment\n";
    is_end_ = true;
    prefix_ = stored_prefix;
    if (std::holds_alternative<ConditionalPtr>(assignment->value())) {
      visit(std::get<ConditionalPtr>(assignment->value()));
    } else if (std::holds_alternative<AssignExpPtr>(assignment->value())) {
      visit(std::get<AssignExpPtr>(assignment->value()));
    } else {
      assert(false);
    }
  }
  void visit(ConditionalPtr &conditional) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "conditional\n";
    if (conditional->cond_true()) { // cond '?' cond_true ':' cond_false
      is_end_ = false;
      prefix_ = stored_prefix;
      visit(conditional->cond());
      is_end_ = false;
      prefix_ = stored_prefix;
      visit(conditional->cond_true());
      is_end_ = true;
      prefix_ = stored_prefix;
      visit(conditional->cond_false());
    } else {  // logical_or
      is_end_ = true;
      prefix_ = stored_prefix;
      visit(conditional->cond());
    }
  }
  void visit(LogicalOrPtr &logical_or) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "logicalOr\n";
    if (logical_or->left()) { // or || and
      is_end_ = false;
      prefix_ = stored_prefix;
      visit(logical_or->left());
    } // else :  logical_and
    is_end_ = true;
    prefix_ = stored_prefix;
    visit(logical_or->right());
  }
  void visit(LogicalAndPtr &logical_and) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "logicalAnd\n";
    if (logical_and->left()) { // and && equality
      is_end_ = false;
      prefix_ = stored_prefix;
      visit(logical_and->left());
    } // else : equality
    is_end_ = true;
    prefix_ = stored_prefix;
    visit(logical_and->right());
  }
  void visit(EqualityPtr &equality) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "equality\n";
    if (equality->left()) {
      is_end_ = false;
      prefix_ = stored_prefix;
      visit(equality->left());
      is_end_ = false;
      prefix_ = stored_prefix;
      visit < Equality > (equality->op());
    }
    is_end_ = true;
    prefix_ = stored_prefix;
    visit(equality->right());
  }
  void visit(RelationalPtr &relational) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "relational\n";
    if (relational->left()) {
      is_end_ = false;
      prefix_ = stored_prefix;
      visit(relational->left());
      is_end_ = false;
      prefix_ = stored_prefix;
      visit < Relational > (relational->op());
    }
    is_end_ = true;
    prefix_ = stored_prefix;
    visit(relational->right());
  }
  void visit(AdditivePtr &additive) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "additive\n";
    if (additive->left()) {
      is_end_ = false;
      prefix_ = stored_prefix;
      visit(additive->left());
      is_end_ = false;
      prefix_ = stored_prefix;
      visit < Additive > (additive->op());
    }
    is_end_ = true;
    prefix_ = stored_prefix;
    visit(additive->right());
  }
  void visit(MultiplicativePtr &multiplicative) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "multiplicative\n";
    if (multiplicative->left()) {
      is_end_ = false;
      prefix_ = stored_prefix;
      visit(multiplicative->left());
      is_end_ = false;
      prefix_ = stored_prefix;
      visit < Multiplicative > (multiplicative->op());
    }
    is_end_ = true;
    prefix_ = stored_prefix;
    visit(multiplicative->right());
  }
  void visit(UnaryPtr &unary) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "unary\n";
    if (std::holds_alternative<PostfixPtr>(unary->value())) {
      is_end_ = true;
      prefix_ = stored_prefix;
      visit(std::get<PostfixPtr>(unary->value()));
    } else if (std::holds_alternative<UnaryPtr>(unary->value())) {
      is_end_ = false;
      prefix_ = stored_prefix;
      visit < Unary > (unary->op());
      is_end_ = true;
      prefix_ = stored_prefix;
      visit(std::get<UnaryPtr>(unary->value()));
    }
  }
  void visit(PostfixPtr &postfix) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "postfix\n";
    is_end_ = true;
    prefix_ = stored_prefix;
    if (std::holds_alternative<PrimaryPtr>(postfix->value())) {
      visit(std::get<PrimaryPtr>(postfix->value()));
    } else if (std::holds_alternative<FuncCallPtr>(postfix->value())) {
      visit(std::get<FuncCallPtr>(postfix->value()));
    } else if (std::holds_alternative<ArrayPtr>(postfix->value())) {
      visit(std::get<ArrayPtr>(postfix->value()));
    } else {
      assert(false);
    }
  }
  void visit(PrimaryPtr &primary) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    if (std::holds_alternative<ExpressionPtr>(primary->value())) {
      os << "primary\n";
      is_end_ = true;
      prefix_ = stored_prefix;
      visit(std::get<ExpressionPtr>(primary->value()));
    } else if (std::holds_alternative<int>(primary->value())) {
      os << "primary(Interger: " << std::get<int>(primary->value()) << ")\n";
    } else if (std::holds_alternative<std::string>(primary->value())) {
      os << "primary(Identifier: " << std::get<std::string>(primary->value()) << ")\n";
    } else {
      assert(false);
    }
  }
  void visit(ReturnStatementPtr &return_statement) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "returnStatement\n";
    is_end_ = true;
    prefix_ = stored_prefix;
    visit(return_statement->exp());
  }
  void visit(ExpStatementPtr &exp_statement) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "expStatement\n";
    if (exp_statement->exp()) {
      is_end_ = true;
      prefix_ = stored_prefix;
      visit(exp_statement->exp());
    }
  }
  void visit(IfStatementPtr &if_statement) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "ifStatement\n";
    is_end_ = false;
    prefix_ = stored_prefix;
    visit(if_statement->cond_exp());
    if (if_statement->else_stmt()) {  // has else branch
      is_end_ = false;
      prefix_ = stored_prefix;
      visit(if_statement->if_stmt());
      is_end_ = true;
      prefix_ = stored_prefix;
      visit(if_statement->else_stmt());
    } else {
      is_end_ = true;
      prefix_ = stored_prefix;
      visit(if_statement->if_stmt());
    }
  }
  void visit(ForExpStatementPtr &for_exp_statement) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "forExpStatement\n";
    if (for_exp_statement->init_exp()) {
      is_end_ = false;
      prefix_ = stored_prefix;
      visit(for_exp_statement->init_exp());
    }
    if (for_exp_statement->cond_exp()) {
      is_end_ = false;
      prefix_ = stored_prefix;
      visit(for_exp_statement->cond_exp());
    }
    if (for_exp_statement->update_exp()) {
      is_end_ = false;
      prefix_ = stored_prefix;
      visit(for_exp_statement->update_exp());
    }
    is_end_ = true;
    prefix_ = stored_prefix;
    visit(for_exp_statement->statement());
  }
  void visit(ForDecStatementPtr &for_dec_statement) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "forDecStatement\n";
    if (for_dec_statement->init_decl()) {
      is_end_ = false;
      prefix_ = stored_prefix;
      visit(for_dec_statement->init_decl());
    }
    if (for_dec_statement->cond_exp()) {
      is_end_ = false;
      prefix_ = stored_prefix;
      visit(for_dec_statement->cond_exp());
    }
    if (for_dec_statement->update_exp()) {
      is_end_ = false;
      prefix_ = stored_prefix;
      visit(for_dec_statement->update_exp());
    }
    is_end_ = true;
    prefix_ = stored_prefix;
    visit(for_dec_statement->statement());
  }
  void visit(WhileStatementPtr &while_statement) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "whileStatement\n";
    is_end_ = false;
    prefix_ = stored_prefix;
    visit(while_statement->cond_exp());
    is_end_ = false;
    prefix_ = stored_prefix;
    visit(while_statement->statement());
  }
  void visit(DoStatementPtr &do_statement) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "doStatement\n";
    is_end_ = false;
    prefix_ = stored_prefix;
    visit(do_statement->statement());
    is_end_ = false;
    prefix_ = stored_prefix;
    visit(do_statement->cond_exp());
  }
  void visit(BreakStatementPtr &break_statement) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "breakStatement\n";
  }
  void visit(ContinueStatementPtr &continue_statement) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "continueStatement\n";
  }
  void visit(AssignExpPtr &assign_exp) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "assignExp\n";
    is_end_ = false;
    prefix_ = stored_prefix;
    visit(assign_exp->left());
    is_end_ = true;
    prefix_ = stored_prefix;
    visit(assign_exp->right());
  }
  void visit(FuncCallPtr &func_call) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "funcCall(func_name: " << func_call->func_name() << ")\n";
    is_end_ = true;
    prefix_ = stored_prefix;
    visit(func_call->expression_list());
  }
  void visit(ArrayPtr &array) override {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "array\n";
    auto total_num = 1 + array->expression_vec().size();
    int cur_num = 1;
    is_end_ = (cur_num == total_num);
    prefix_ = stored_prefix;
    if (std::holds_alternative<PrimaryPtr>(array->name())) {
      visit(std::get<PrimaryPtr>(array->name()));
    } else if (std::holds_alternative<FuncCallPtr>(array->name())) {
      visit(std::get<FuncCallPtr>(array->name()));
    } else {
      assert(false);
    }
    for (auto &exp : array->expression_vec()) {
      ++cur_num;
      is_end_ = (cur_num == total_num);
      prefix_ = stored_prefix;
      visit(exp);
    }
  }
  template<typename T>
  void visit(typename T::Op &op) {
    auto stored_prefix = append_prefix(prefix_);
    print_front();
    os << "op: " << to_string(op) << "\n";
  }
 private:
  void print_front() {
    os << prefix_;
    os << (is_end_ ? "└── " : "├── ");
  }
  [[nodiscard]] std::string append_prefix(const std::string &prefix) const {
    return prefix + (is_end_ ? "    " : "│    ");
  }
  [[nodiscard]] static std::string append_prefix(const std::string &prefix, bool is_end) {
    return prefix + (is_end ? "    " : "│    ");
  }
  std::string prefix_{};
  bool is_end_{};
  std::ostream &os;
};

std::ostream &operator<<(std::ostream &os, ProgramPtr &program) {
  ASTPrinter printer(os);
  printer.visit(program);
  return os;
}

std::ostream &operator<<(std::ostream &os, const IRVar &var) {
  if (var.is_global()) {
    os << var.name();
  } else {
    os << "%" << var.num();
  }
  return os;
}
std::ostream &operator<<(std::ostream &os, IRAddr &addr) {
  if (addr.is_imm()) {
    os << addr.imm();
  } else if (addr.is_var()) {
    os << addr.var();
  } else {
    os << addr.name();
  }
  return os;
}
std::string to_string(IROp op) {
  switch (op) {
    case IROp::FUNBEG: return "FUNBEG";
    case IROp::FUNEND: return "FUNEND";
    case IROp::LABEL: return "LABEL";
    case IROp::RET: return "RET";
    case IROp::MOV: return "MOV";
    case IROp::NEG: return "NEG";
    case IROp::NOT: return "NOT";
    case IROp::LNOT: return "LNOT";
    case IROp::MUL: return "MUL";
    case IROp::DIV: return "DIV";
    case IROp::REM: return "REM";
    case IROp::ADD: return "ADD";
    case IROp::SUB: return "SUB";
    case IROp::LT: return "LT";
    case IROp::GT: return "GT";
    case IROp::LE: return "LE";
    case IROp::GE: return "GE";
    case IROp::EQ: return "EQ";
    case IROp::NE: return "NE";
    case IROp::LAND: return "LAND";
    case IROp::LOR: return "LOR";
    case IROp::JMP: return "JMP";
    case IROp::BEQZ: return "BEQZ";
    case IROp::PARAM: return "PARAM";
    case IROp::CALL: return "CALL";
    case IROp::LA: return "LA";
    case IROp::LOAD: return "LOAD";
    case IROp::STORE: return "STORE";
    case IROp::ALLOC: return "ALLOC";
    case IROp::GBSS: return "GBSS";
    case IROp::GINI: return "GINI";
    case IROp::LOADFP: return "LOADFP";
    case IROp::STOREFP: return "STOREFP";
    case IROp::LARRAY: return "LARRAY";
  }
  assert(false);
}
std::ostream &operator<<(std::ostream &os, IRCode &code) {
  os << std::setw(8) << std::left << to_string(code.op());
  if (code.a0()) {
    os << *(code.a0());
  }
  if (code.a1()) {
    os << "    " << *(code.a1());
  }
  if (code.a2()) {
    os << "    " << *(code.a2());
  }
  return os;
}
std::ostream &operator<<(std::ostream &os, IRBuilderPtr &ir_builder) {
  for (auto &ir : ir_builder->ircode_list()) {
    os << *ir << "\n";
  }
  return os;
}
