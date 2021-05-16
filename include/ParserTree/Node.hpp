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
    private: 
        variant<Variable*, pair<Variable*, Token>, Token, PreIncrementation*> pExpression;
    public:
        variant<Variable*, pair<Variable*, Token>, Token, PreIncrementation*> getPExpression() { return pExpression; }
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
    private:
        Token variableName;
        ArithmeticExpression* index;
    public:
        Token getVariableName() { return variableName; }
        ArithmeticExpression* getIndex() { return index; }
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
        vector<PrimaryExpression*>primaryExpressions;
        vector<Token> operators;

        void addPrimaryExpression(PrimaryExpression* primaryExpression)
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
    private:
        Token incrementation;
        Variable* variable;
    public:
        Token getIncrementation() { return incrementation; };
        Variable* getVariable() { return variable; };

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
    private:
        ArithmeticExpression* arithmeticExpression;
    public:
        ArithmeticExpression* getArithmeticExpression() { return arithmeticExpression; }
        void addArithmeticExpression(ArithmeticExpression* arithmeticExpression)
        {
            this->arithmeticExpression = arithmeticExpression;
        }

};

class Condition
{
    private:
        Token logicalOperator;
    public:
        vector<ArithmeticExpression*>expressions;
        Token getLogicalOperator() { return logicalOperator; }
        void addExpression(ArithmeticExpression* arithmeticExpression)
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
    private:
        Token dataType;
        Variable* variable;
        Assigment* assigment;
    public:
        Token getDataType() { return dataType; }
        Variable* getVariable() { return variable; }
        Assigment* getAssigment() { return assigment; }
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
    private:
        variant<Initiation*, pair<Variable*, Assigment*> >initiation;
        bool boolInitiation = false;
        Condition* condition;
        variant<PreIncrementation*, pair<Variable*, Assigment*>, pair<Variable*, Token> >update;
        bool boolUpdate = false;
    public:
        variant<Initiation*, pair<Variable*, Assigment*> >getInitiation() { return initiation; }
        Condition* getCondition() { return condition; };
        variant<PreIncrementation*, pair<Variable*, Assigment*>, pair<Variable*, Token> > getUpdate() { return update; };

        bool isInitiation() { return boolInitiation; }
        bool isBoolUpdate() { return boolUpdate; }

        vector<Operation*>operations;

        void addInitAssigment(Variable* variable, Assigment* assigment)
        {
            boolInitiation = true;
            initiation = std::make_pair(variable, assigment);
        }
        void addInitiation(Initiation* initiation)
        {
            boolInitiation = true;
            this->initiation = initiation;
        }
        void addCondition(Condition* condition)
        {
            this->condition = condition;
        }
        void addPostIncrementation(Variable* variable, Token token)
        {
            boolUpdate = true;
            update = std::make_pair(variable, token);
        }
        void addUpdateAssigment(Variable* variable, Assigment* assigment)
        {
            boolUpdate = true;
            update = std::make_pair(variable, assigment);
        }
        void addPreIncrementation(PreIncrementation* preIncrementation)
        {
            boolUpdate = true;
            update = preIncrementation;
        }
        void addOperation(Operation* operation)
        {
            operations.push_back(operation);
        }
};

class Operation
{
    private:
        variant<Loop*, pair<Variable*, Assigment*>, pair<Variable*, Token>, Initiation*, PreIncrementation*> oper;
    public:
        variant<Loop*, pair<Variable*, Assigment*>, pair<Variable*, Token>, Initiation*, PreIncrementation*> getOper()
        {
            return oper;
        }
        
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
        std::vector<Operation*>operations;

        void addOperation(Operation* oper)
        {
            operations.push_back(oper);
        }
};

class ParserTree
{
    private:
        Program program;
    public:
        ParserTree(Program root)
        {
            program = root;
        };
        Program getProgram()
        {
            return program;
        }

};


#endif