#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <string>
#include "Node.hpp"

std::string conditionToString(Condition* condtion, bool whitespaces);
std::string primaryExpressionToString(PrimaryExpression* primaryExpression, bool whitespaces);
std::string arithmeticExpressionToString(ArithmeticExpression* arithmeticExpression, bool whitespaces);
std::string preIncrementationToString(PreIncrementation* preIncrementation, bool whitespaces);
std::string initiationToString(Initiation* initiation, bool whitespaces);
std::string assigmentToString(Assigment* assigment, bool whitespaces);
std::string variableToString(Variable* variable, bool whitespaces);
std::string loopToString(Loop* loop, bool whitespaces, int level);
std::string operationToString(Operation* oper, bool whitespaces, int level);
std::string programToString(Program* tree, bool whitespaces);
std::string treeToString(ParserTree tree, bool whitespaces = false);



#endif