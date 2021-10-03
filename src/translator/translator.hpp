#ifndef SCOMPILER_SRC_TRANSLATOR_TRANSLATOR_HPP_
#define SCOMPILER_SRC_TRANSLATOR_TRANSLATOR_HPP_

#include "visitor.hpp"
#include "ir.hpp"

class Translator : DetailedASTVisitor {
 public:
  Translator() = default;
  ~Translator() = default;
  IRBuilderPtr translate(ProgramPtr &program);
 private:
  void visit(ProgramPtr &program) override;
  void visit(FunctionPtr &function) override;
  void visit(BaseType &type) override;
  void visit(ParameterListPtr &parameter_list) override;
  void visit(CompoundStatementPtr &compound_statement) override;
  void visit(BlockItemPtr &block_item) override;
  void visit(StatementPtr &statement) override;
  void visit(DeclarationPtr &declaration) override;
  void visit(ExpressionListPtr &expression_list) override;
  void visit(ExpressionPtr &expression) override;
  void visit(AssignmentPtr &assignment) override;
  void visit(ConditionalPtr &conditional) override;
  void visit(LogicalOrPtr &logical_or) override;
  void visit(LogicalAndPtr &logical_and) override;
  void visit(EqualityPtr &equality) override;
  void visit(RelationalPtr &relational) override;
  void visit(AdditivePtr &additive) override;
  void visit(MultiplicativePtr &multiplicative) override;
  void visit(UnaryPtr &unary) override;
  void visit(PostfixPtr &postfix) override;
  void visit(PrimaryPtr &primary) override;
  void visit(ReturnStatementPtr &return_statement) override;
  void visit(ExpStatementPtr &exp_statement) override;
  void visit(IfStatementPtr &if_statement) override;
  void visit(ForExpStatementPtr &for_exp_statement) override;
  void visit(ForDecStatementPtr &for_dec_statement) override;
  void visit(WhileStatementPtr &while_statement) override;
  void visit(DoStatementPtr &do_statement) override;
  void visit(BreakStatementPtr &break_statement) override;
  void visit(ContinueStatementPtr &continue_statement) override;
  void visit(AssignExpPtr &assign_exp) override;
  void visit(FuncCallPtr &func_call) override;
  void visit(ArrayPtr &array) override;

  IRBuilderPtr ir_builder_;
};

IRBuilderPtr translate(ProgramPtr &program) {
  Translator translator;
  return translator.translate(program);
}

#endif //SCOMPILER_SRC_TRANSLATOR_TRANSLATOR_HPP_
