#ifndef SCOMPILER_SRC_BASE_VISITOR_HPP_
#define SCOMPILER_SRC_BASE_VISITOR_HPP_

#include "ast.hpp"

class Visitor {
 public:
  virtual void visit(ProgramPtr &program) = 0;
  virtual void visit(FunctionPtr &function) = 0;
  virtual void visit(BaseType &type) = 0;
  virtual void visit(ParameterListPtr &parameter_list) = 0;
  virtual void visit(CompoundStatementPtr &compound_statement) = 0;
  virtual void visit(BlockItemPtr &block_item) = 0;
  virtual void visit(StatementPtr &statement) = 0;
  virtual void visit(DeclarationPtr &declaration) = 0;
  virtual void visit(ExpressionListPtr &expression_list) = 0;
  virtual void visit(ExpressionPtr &expression) = 0;
  virtual void visit(AssignmentPtr &assignment) = 0;
  virtual void visit(ConditionalPtr &conditional) = 0;
  virtual void visit(LogicalOrPtr &logical_or) = 0;
  virtual void visit(LogicalAndPtr &logical_and) = 0;
  virtual void visit(EqualityPtr &equality) = 0;
  virtual void visit(RelationalPtr &relational) = 0;
  virtual void visit(AdditivePtr &additive) = 0;
  virtual void visit(MultiplicativePtr &multiplicative) = 0;
  virtual void visit(UnaryPtr &unary) = 0;
  virtual void visit(PostfixPtr &postfix) = 0;
  virtual void visit(PrimaryPtr &primary) = 0;
};

#endif //SCOMPILER_SRC_BASE_VISITOR_HPP_
