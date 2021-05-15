#ifndef NODE_HPP
#define NODE_HPP

#include <variant>
#include <string>
#include <vector>

#include <memory>

using namespace std;

// enum TypeOfNode {VAR, PROGRAM};

// class Node
// {
//     private:
//         std::weak_ptr<Node> parent;
        
//     public:
//         enum class Type
//         {
//             Program,
//             Operation,
//             Loop,
//             Initiation,
//             Assigment,
//             Condition,
//             ArithmeticExpression,
//             MulitplicativeExpression,
//             PrimaryExpression,
//             PreIncrementation,
//             Variable,
//             Number,
//         };

//         virtual TypeOfNode getType() = 0;
// };

#include "../structures/Token.hpp"

// #include "Variable.hpp"
// #include "Program.hpp"
class Variable;
class ArithmeticExpression;
class PreIncrementation;
class Operation;
class PrimaryExpression
{
    public:
        variant<Variable*, pair<Variable*, Token>, Token, PreIncrementation*> pExpression;
        void addVariable(Variable* variable)
        {
            pExpression = variable;
        }
        void addIncrementOperator(Token& token)
        {
            Variable* variable = std::get<Variable*>(pExpression);
            pExpression = std::make_pair(variable, token);
        }
        void addNumber(Token& token)
        {
            pExpression = token;
        }
        void addPreIncrementation(PreIncrementation* preIncrementation)
        {
            pExpression = preIncrementation;
        }
};

class Variable
{
    public:
        Token variableName;
        ArithmeticExpression* index;
        void addVariableName(Token& token)
        {
            variableName = token;
        }
        void addIndex(ArithmeticExpression* arithmeticExpression)
        {
            index = arithmeticExpression;
        }

};

class ArithmeticExpression
{
    public:
        vector<PrimaryExpression>primaryExpressions;
        vector<Token> operators;

        void addPrimaryExpression(PrimaryExpression& primaryExpression)
        {
            primaryExpressions.push_back(primaryExpression);
        }
        void addOperator(Token& token)
        {
            operators.push_back(token);
        }
};

class PreIncrementation
{
    public:
        Token incrementation;
        Variable* variable;

        void addToken(Token& token)
        {
            incrementation = token;
        }
        void addVariable(Variable* variable)
        {
            this->variable = variable;
        }
};

class Assigment
{
    public:
        ArithmeticExpression* arithmeticExpression;
        void addArithmeticExpression(ArithmeticExpression* arithmeticExpression)
        {
            this->arithmeticExpression = arithmeticExpression;
        }

};

class Condition
{
    public:
        vector<ArithmeticExpression>expressions;
        Token logicalOperator;
        void addExpression(ArithmeticExpression& arithmeticExpression)
        {
            expressions.push_back(arithmeticExpression);
        }
        void addOperator(Token& token)
        {
            logicalOperator = token;
        }
};

class Initiation
{
    public:
        Token dataType;
        Variable* variable;
        Assigment* assigment;
        void addDataType(Token& token)
        {
            dataType = token;
        }
        void addVariable(Variable* variable)
        {
            this->variable = variable;
        }
        void addAssigment(Assigment* assigment)
        {
            this->assigment = assigment;
        }
};

class Loop
{
    public:
        variant<Initiation*, pair<Variable*, Assigment*> >initiation;
        Condition* condition;
        variant<PreIncrementation*, pair<Variable*, Assigment*>, pair<Variable*, Token> >update;
        vector<Operation>operations;

        void addInitAssigment(Variable* variable, Assigment* assigment)
        {
            initiation = std::make_pair(variable, assigment);
        }
        void addInitiation(Initiation* initiation)
        {
            this->initiation = initiation;
        }
        void addCondition(Condition* condition)
        {
            this->condition = condition;
        }
        void addPostIncrementation(Variable* variable, Token token)
        {
            update = std::make_pair(variable, token);
        }
        void addUpdateAssigment(Variable* variable, Assigment* assigment)
        {
            update = std::make_pair(variable, assigment);
        }
        void addPreIncrementation(PreIncrementation* preIncrementation)
        {
            update = preIncrementation;
        }
        void addOperation(Operation& operation)
        {
            operations.push_back(operation);
        }
};

class Operation
{
    public:
        variant<Loop*, pair<Variable*, Assigment*>, pair<Variable*, Token>, Initiation*, PreIncrementation*> oper;
        
        void addLoop(Loop* loop)
        {
            oper = loop;
        }
        void addPostIncrementation(Variable* variable, Token& token)
        {
           oper = std::make_pair(variable, token);
        }
        void addAssigment(Variable* variable, Assigment* assigment)
        {
            oper = std::make_pair(variable, assigment);
        }
        void addInitiation(Initiation* initiation)
        {
            oper = initiation;
        }
        void addPreIncrementation(PreIncrementation* preIncrementation)
        {
            oper = preIncrementation;
        }
};

class Program
{
    public:
        std::vector<Operation>operations;

        void addOperation(Operation &oper)
        {
            operations.push_back(oper);
        }
};

class ParserTree
{
    private:
        
    public:
        Program program;
        ParserTree(Program root)
        {
            program = root;
        };

};


#endif