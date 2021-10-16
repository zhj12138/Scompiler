#include "recursive_descent_parser.hpp"

ProgramPtr RecursiveDescentParser::parse() {
  ProgramPtr program;
  visit(program);
  return program;
}

void RecursiveDescentParser::visit(ProgramPtr &program) {
  std::vector<Program::value_type> func_decl_vec;

  while (!token_stream_.eof()) {  // still has tokens
    if (token_stream_.look_ahead(2, TokenType::Lparen)) { // function
      FunctionPtr function;
      visit(function);
      func_decl_vec.emplace_back(function);
    } else { // declaration
      DeclarationPtr declaration;
      visit(declaration);
      func_decl_vec.emplace_back(declaration);
    }
  }

  program = std::make_shared<Program>(func_decl_vec);
}

void RecursiveDescentParser::visit(FunctionPtr &function) {
  BaseType ret_type;
  std::string func_name;
  ParameterListPtr parameter_list;
  CompoundStatementPtr compound_statement;

  visit(ret_type);
  func_name = token_stream_.consume_identifier();
  token_stream_.consume(TokenType::Lparen);
  visit(parameter_list);
  token_stream_.consume(TokenType::Rparen);
  if (token_stream_.is(TokenType::Semicolon)) {
    token_stream_.advance();
  } else {  // compound_statement
    visit(compound_statement);
  }

  function = std::make_shared<Function>(ret_type, func_name, parameter_list, compound_statement);
}

void RecursiveDescentParser::visit(BaseType &type) {
  if (token_stream_.is(TokenType::Int)) {
    type.set_type(SupportType::Int);
    token_stream_.advance();
  } else {
    assert(false);  // 只支持int类型
  }
}

void RecursiveDescentParser::visit(ParameterListPtr &parameter_list) {
  if (token_stream_.is(TokenType::Rparen)) {  // empty list
    parameter_list = std::make_shared<ParameterList>();
    return;
  }
  BaseType var_type;
  std::string id;
  std::vector<VariablePtr> variable_vec;
  int param_num = 0;

  while (true) {
    visit(var_type);
    id = token_stream_.consume_identifier();
    variable_vec.push_back(std::make_shared<Variable>(var_type.type(), id));
    variable_vec.back()->set_param_num(++param_num);
    if (!token_stream_.is(TokenType::Comma)) break; // end
    token_stream_.advance();
  }

  parameter_list = std::make_shared<ParameterList>(variable_vec);
}

void RecursiveDescentParser::visit(CompoundStatementPtr &compound_statement) {
  std::vector<BlockItemPtr> blockitem_vec;

  token_stream_.consume(TokenType::LCbrace);
  while (!token_stream_.is(TokenType::RCbrace)) {
    BlockItemPtr block_item;
    visit(block_item);
    blockitem_vec.push_back(block_item);
  }
  token_stream_.advance();

  compound_statement = std::make_shared<CompoundStatement>(blockitem_vec);
}

void RecursiveDescentParser::visit(BlockItemPtr &block_item) {
  if (token_stream_.is_type()) {  // declaration
    DeclarationPtr declaration;
    visit(declaration);
    block_item = std::make_shared<BlockItem>(declaration);
  } else {  // statement
    StatementPtr statement;
    visit(statement);
    block_item = std::make_shared<BlockItem>(statement);
  }
}

void RecursiveDescentParser::visit(StatementPtr &statement) {
  if (token_stream_.is(TokenType::Return)) {
    ExpressionPtr expression;

    token_stream_.advance();
    visit(expression);
    token_stream_.consume(TokenType::Semicolon);

    statement = std::make_shared<Statement>(std::make_shared<ReturnStatement>(expression));
  } else if (token_stream_.is(TokenType::If)) {
    ExpressionPtr cond_exp;
    StatementPtr if_stmt;
    StatementPtr else_stmt;

    token_stream_.advance();
    token_stream_.consume(TokenType::Lparen);
    visit(cond_exp);
    token_stream_.consume(TokenType::Rparen);
    visit(if_stmt);
    if (token_stream_.is(TokenType::Else)) {  // has else branch
      token_stream_.advance();
      visit(else_stmt);
    }

    statement = std::make_shared<Statement>(std::make_shared<IfStatement>(cond_exp, if_stmt, else_stmt));
  } else if (token_stream_.is(TokenType::LCbrace)) {  // compound statement
    CompoundStatementPtr compound_statement;
    visit(compound_statement);
    statement = std::make_shared<Statement>(compound_statement);
  } else if (token_stream_.is(TokenType::For)) {
    ExpressionPtr cond_exp;
    ExpressionPtr update_exp;
    StatementPtr loop_statement;

    token_stream_.advance();
    token_stream_.consume(TokenType::Lparen);
    if (token_stream_.is_type()) {  // declaration
      DeclarationPtr init_decl;

      visit(init_decl);
      if (!token_stream_.is(TokenType::Semicolon)) { // non-empty cond_exp
        visit(cond_exp);
      }
      token_stream_.consume(TokenType::Semicolon);
      if (!token_stream_.is(TokenType::Rparen)) { // non-empty update_exp
        visit(update_exp);
      }
      token_stream_.consume(TokenType::Rparen);
      visit(loop_statement);

      statement = std::make_shared<Statement>(std::make_shared<ForDecStatement>(init_decl,
                                                                                cond_exp,
                                                                                update_exp,
                                                                                loop_statement));
    } else {  // expression
      ExpressionPtr init_exp;

      if (!token_stream_.is(TokenType::Semicolon)) { // non-empty init_exp
        visit(init_exp);
      }
      token_stream_.consume(TokenType::Semicolon);
      if (!token_stream_.is(TokenType::Semicolon)) { // non-empty cond_exp
        visit(cond_exp);
      }
      token_stream_.consume(TokenType::Semicolon);
      if (!token_stream_.is(TokenType::Rparen)) { // non-empty update_exp
        visit(update_exp);
      }
      token_stream_.consume(TokenType::Rparen);
      visit(loop_statement);

      statement = std::make_shared<Statement>(std::make_shared<ForExpStatement>(init_exp,
                                                                                cond_exp,
                                                                                update_exp,
                                                                                loop_statement));
    }
  } else if (token_stream_.is(TokenType::While)) {
    ExpressionPtr cond_exp;
    StatementPtr loop_statement;

    token_stream_.advance();
    token_stream_.consume(TokenType::Lparen);
    visit(cond_exp);
    token_stream_.consume(TokenType::Rparen);
    visit(loop_statement);

    statement = std::make_shared<Statement>(std::make_shared<WhileStatement>(cond_exp, loop_statement));
  } else if (token_stream_.is(TokenType::Do)) {
    StatementPtr loop_statement;
    ExpressionPtr cond_exp;

    token_stream_.advance();
    visit(loop_statement);
    token_stream_.consume(TokenType::While);
    token_stream_.consume(TokenType::Lparen);
    visit(cond_exp);
    token_stream_.consume(TokenType::Rparen);
    token_stream_.consume(TokenType::Semicolon);

    statement = std::make_shared<Statement>(std::make_shared<DoStatement>(loop_statement, cond_exp));
  } else if (token_stream_.is(TokenType::Break)) {
    token_stream_.advance();
    token_stream_.consume(TokenType::Semicolon);

    statement = std::make_shared<Statement>(std::make_shared<BreakStatement>());
  } else if (token_stream_.is(TokenType::Continue)) {
    token_stream_.advance();
    token_stream_.consume(TokenType::Semicolon);

    statement = std::make_shared<Statement>(std::make_shared<ContinueStatement>());
  } else {  // ExpStatement
    ExpressionPtr exp;

    if (!token_stream_.is(TokenType::Semicolon)) {  // non-empty expression
      visit(exp);
    }
    token_stream_.consume(TokenType::Semicolon);

    statement = std::make_shared<Statement>(std::make_shared<ExpStatement>(exp));
  }
}

void RecursiveDescentParser::visit(DeclarationPtr &declaration) {
  BaseType var_type;
  std::string var_name;
  std::vector<int> dimension_vec;
  VariablePtr variable;
  ExpressionPtr init_exp;

  visit(var_type);
  var_name = token_stream_.consume_identifier();
  while (token_stream_.is(TokenType::LSbrace)) {  // ArrayType
    token_stream_.advance();
    dimension_vec.push_back(token_stream_.consume_number());
    token_stream_.consume(TokenType::RSbrace);
  }
  if (dimension_vec.empty()) {  // Base Type
    variable = std::make_shared<Variable>(var_type.type(), var_name);
  } else {  // Array Type
    variable = std::make_shared<Variable>(var_type.type(), dimension_vec, var_name);
  }
  if (token_stream_.is(TokenType::Assign)) {  // has init expression
    token_stream_.advance();
    visit(init_exp);
  }
  token_stream_.consume(TokenType::Semicolon);

  declaration = std::make_shared<Declaration>(variable, init_exp);
}

void RecursiveDescentParser::visit(ExpressionListPtr &expression_list) {
  if (token_stream_.is(TokenType::Rparen)) {  // empty expression list
    expression_list = std::make_shared<ExpressionList>();
    return;
  }
  std::vector<ExpressionPtr> expression_vec;
  while (true) {
    ExpressionPtr expression;
    visit(expression);
    expression_vec.push_back(expression);
    if (token_stream_.is(TokenType::Rparen)) break; // end
    token_stream_.consume(TokenType::Comma);
  }
  expression_list = std::make_shared<ExpressionList>(expression_vec);
}

void RecursiveDescentParser::visit(ExpressionPtr &expression) {
  AssignmentPtr assignment;
  visit(assignment);
  expression = std::make_shared<Expression>(assignment);
}

void RecursiveDescentParser::visit(AssignmentPtr &assignment) {
  UnaryPtr left;
  ExpressionPtr right;
  ConditionalPtr conditional;

  auto status = token_stream_.store();
  visit(left);
  if (token_stream_.is(TokenType::Assign)) {  // AssignExp
    token_stream_.advance();
    visit(right);

    assignment = std::make_shared<Assignment>(std::make_shared<AssignExp>(left, right));
  } else {  // conditional
    token_stream_.restore(status);  // restore to old status
    visit(conditional);

    assignment = std::make_shared<Assignment>(conditional);
  }
}

void RecursiveDescentParser::visit(ConditionalPtr &conditional) {
  LogicalOrPtr cond;
  ExpressionPtr cond_true;
  ConditionalPtr cond_false;

  visit(cond);
  if (token_stream_.is(TokenType::Question)) {  // cond ? cond_true : cond_false
    token_stream_.advance();
    visit(cond_true);
    token_stream_.consume(TokenType::Colon);
    visit(cond_false);
  }

  conditional = std::make_shared<Conditional>(cond, cond_true, cond_false);
}

void RecursiveDescentParser::visit(LogicalOrPtr &logical_or) {
  LogicalOrPtr left;
  LogicalAndPtr right;
  LogicalAndPtr tmp;

  visit(tmp);
  left = std::make_shared<LogicalOr>(tmp);
  while (token_stream_.is(TokenType::Lor)) {
    token_stream_.advance();
    visit(right);
    left = std::make_shared<LogicalOr>(left, right);
  }

  logical_or = left;
}

void RecursiveDescentParser::visit(LogicalAndPtr &logical_and) {
  LogicalAndPtr left;
  EqualityPtr right;
  EqualityPtr tmp;

  visit(tmp);
  left = std::make_shared<LogicalAnd>(tmp);
  while (token_stream_.is(TokenType::Land)) {
    token_stream_.advance();
    visit(right);
    left = std::make_shared<LogicalAnd>(left, right);
  }

  logical_and = left;
}

void RecursiveDescentParser::visit(EqualityPtr &equality) {
  EqualityPtr left;
  Equality::Op op;
  RelationalPtr right;
  RelationalPtr tmp;

  visit(tmp);
  left = std::make_shared<Equality>(tmp);
  while (token_stream_.is_equality_op()) {
    if (token_stream_.is(TokenType::Equal)) {
      op = Equality::Op::Equal;
    } else if (token_stream_.is(TokenType::Nequal)) {
      op = Equality::Op::Nequal;
    } else {
      assert(false);
    }
    token_stream_.advance();
    visit(right);
    left = std::make_shared<Equality>(left, op, right);
  }

  equality = left;
}

void RecursiveDescentParser::visit(RelationalPtr &relational) {
  RelationalPtr left;
  Relational::Op op;
  AdditivePtr right;
  AdditivePtr tmp;

  visit(tmp);
  left = std::make_shared<Relational>(tmp);
  while (token_stream_.is_relational_op()) {
    if (token_stream_.is(TokenType::Less)) {
      op = Relational::Op::Less;
    } else if (token_stream_.is(TokenType::Greater)) {
      op = Relational::Op::Greater;
    } else if (token_stream_.is(TokenType::Le)) {
      op = Relational::Op::Le;
    } else if (token_stream_.is(TokenType::Ge)) {
      op = Relational::Op::Ge;
    } else {
      assert(false);
    }
    token_stream_.advance();
    visit(right);
    left = std::make_shared<Relational>(left, op, right);
  }

  relational = left;
}

void RecursiveDescentParser::visit(AdditivePtr &additive) {
  AdditivePtr left;
  Additive::Op op;
  MultiplicativePtr right;
  MultiplicativePtr tmp;

  visit(tmp);
  left = std::make_shared<Additive>(tmp);
  while (token_stream_.is_additive_op()) {
    if (token_stream_.is(TokenType::Add)) {
      op = Additive::Op::Add;
    } else if (token_stream_.is(TokenType::Sub)) {
      op = Additive::Op::Sub;
    } else {
      assert(false);
    }
    token_stream_.advance();
    visit(right);
    left = std::make_shared<Additive>(left, op, right);
  }

  additive = left;
}

void RecursiveDescentParser::visit(MultiplicativePtr &multiplicative) {
  MultiplicativePtr left;
  Multiplicative::Op op;
  UnaryPtr right;
  UnaryPtr tmp;

  visit(tmp);
  left = std::make_shared<Multiplicative>(tmp);
  while (token_stream_.is_multiplicative_op()) {
    if (token_stream_.is(TokenType::Times)) {
      op = Multiplicative::Op::Times;
    } else if (token_stream_.is(TokenType::Divide)) {
      op = Multiplicative::Op::Divide;
    } else if (token_stream_.is(TokenType::Mod)) {
      op = Multiplicative::Op::Mod;
    } else {
      assert(false);
    }
    token_stream_.advance();
    visit(right);
    left = std::make_shared<Multiplicative>(left, op, right);
  }

  multiplicative = left;
}

void RecursiveDescentParser::visit(UnaryPtr &unary) {
  PostfixPtr p;
  Unary::Op op;
  UnaryPtr u;

  if (token_stream_.is_unary_op()) {  // op unary
    if (token_stream_.is(TokenType::Sub)) {
      op = Unary::Op::Sub;
    } else if (token_stream_.is(TokenType::Not)) {
      op = Unary::Op::Not;
    } else if (token_stream_.is(TokenType::Lnot)) {
      op = Unary::Op::Lnot;
    } else {
      assert(false);
    }
    token_stream_.advance();
    visit(u);
    unary = std::make_shared<Unary>(op, u);
  } else {  // postfix
    visit(p);
    unary = std::make_shared<Unary>(p);
  }
}

void RecursiveDescentParser::visit(PostfixPtr &postfix) {
  FuncCallPtr func_call;
  PrimaryPtr primary;
  std::vector<ExpressionPtr> expression_vec;
  bool is_primary = false;

  if (token_stream_.is(TokenType::Identifier) &&
      token_stream_.look_ahead(1, TokenType::Lparen)) { // func call
    ExpressionListPtr expression_list;
    std::string func_name;

    func_name = token_stream_.consume_identifier();
    token_stream_.advance();
    visit(expression_list);
    token_stream_.consume(TokenType::Rparen);

    func_call = std::make_shared<FuncCall>(func_name, expression_list);
  } else {  // primary
    is_primary = true;
    visit(primary);
  }
  while (token_stream_.is(TokenType::LSbrace)) { // Array
    token_stream_.advance();
    ExpressionPtr exp;
    visit(exp);
    token_stream_.consume(TokenType::RSbrace);
    expression_vec.push_back(exp);
  }
  if (expression_vec.empty()) { // primary or func call
    if (is_primary) { // primary
      postfix = std::make_shared<Postfix>(primary);
    } else {  // func call
      postfix = std::make_shared<Postfix>(func_call);
    }
  } else {
    if (is_primary) { // primary
      postfix = std::make_shared<Postfix>(std::make_shared<Array>(primary, expression_vec));
    } else {  // func call
      postfix = std::make_shared<Postfix>(std::make_shared<Array>(func_call, expression_vec));
    }
  }
}

void RecursiveDescentParser::visit(PrimaryPtr &primary) {
  if (token_stream_.is(TokenType::Number)) {
    primary = std::make_shared<Primary>(token_stream_.consume_number());
  } else if (token_stream_.is(TokenType::Lparen)) {
    ExpressionPtr exp;
    token_stream_.advance();
    visit(exp);
    token_stream_.consume(TokenType::Rparen);
    primary = std::make_shared<Primary>(exp);
  } else if (token_stream_.is(TokenType::Identifier)) {
    primary = std::make_shared<Primary>(token_stream_.consume_identifier());
  } else {
    assert(false);
  }
}
