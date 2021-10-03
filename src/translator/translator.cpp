#include "translator.hpp"

IRBuilderPtr Translator::translate(ProgramPtr &program) {
  visit(program);
  return ir_builder_;
}

void Translator::visit(ProgramPtr &program) {

}
void Translator::visit(FunctionPtr &function) {

}
void Translator::visit(BaseType &type) {

}
void Translator::visit(ParameterListPtr &parameter_list) {

}
void Translator::visit(CompoundStatementPtr &compound_statement) {

}
void Translator::visit(BlockItemPtr &block_item) {

}
void Translator::visit(StatementPtr &statement) {

}
void Translator::visit(DeclarationPtr &declaration) {

}
void Translator::visit(ExpressionListPtr &expression_list) {

}
void Translator::visit(ExpressionPtr &expression) {

}
void Translator::visit(AssignmentPtr &assignment) {

}
void Translator::visit(ConditionalPtr &conditional) {

}
void Translator::visit(LogicalOrPtr &logical_or) {

}
void Translator::visit(LogicalAndPtr &logical_and) {

}
void Translator::visit(EqualityPtr &equality) {

}
void Translator::visit(RelationalPtr &relational) {

}
void Translator::visit(AdditivePtr &additive) {

}
void Translator::visit(MultiplicativePtr &multiplicative) {

}
void Translator::visit(UnaryPtr &unary) {

}
void Translator::visit(PostfixPtr &postfix) {

}
void Translator::visit(PrimaryPtr &primary) {

}
void Translator::visit(ReturnStatementPtr &return_statement) {

}
void Translator::visit(ExpStatementPtr &exp_statement) {

}
void Translator::visit(IfStatementPtr &if_statement) {

}
void Translator::visit(ForExpStatementPtr &for_exp_statement) {

}
void Translator::visit(ForDecStatementPtr &for_dec_statement) {

}
void Translator::visit(WhileStatementPtr &while_statement) {

}
void Translator::visit(DoStatementPtr &do_statement) {

}
void Translator::visit(BreakStatementPtr &break_statement) {

}
void Translator::visit(ContinueStatementPtr &continue_statement) {

}
void Translator::visit(AssignExpPtr &assign_exp) {

}
void Translator::visit(FuncCallPtr &func_call) {

}
void Translator::visit(ArrayPtr &array) {

}
