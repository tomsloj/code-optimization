#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <string>
#include "Node.hpp"

std::string conditionToString(Condition* condtion);
std::string primaryExpressionToString(PrimaryExpression* primaryExpression);
std::string arithmeticExpressionToString(ArithmeticExpression* arithmeticExpression);
std::string preIncrementationToString(PreIncrementation* preIncrementation);
std::string initiationToString(Initiation* initiation);
std::string assigmentToString(Assigment* assigment);
std::string variableToString(Variable* variable);
std::string loopToString(Loop* loop);
std::string operationToString(Operation* oper);
std::string programToString(Program* tree);
std::string treeToString(ParserTree tree);



#endif