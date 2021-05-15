#include "../../include/ParserTree/Variable.hpp"

using namespace std;

void Variable::addVariableName(Token& token)
{
    variableName = token;
}
// void Variable::addIndex(ArithmeticExpression arithmeticExpression)
// {
//     index = arithmeticExpression;
// }

TypeOfNode Variable::getType()
{
 
    return VAR;
}