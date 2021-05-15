#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include "Node.hpp"


class Variable: public Node
{
    public:
        Token variableName;
        //ArithmeticExpression index;
        void addVariableName(Token& token);
        //void addIndex(ArithmeticExpression arithmeticExpression);

        virtual TypeOfNode getType();
};

#endif