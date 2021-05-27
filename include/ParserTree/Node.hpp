#ifndef NODE_HPP
#define NODE_HPP

#include <variant>
#include <string>
#include <vector>

#include <memory>

using namespace std;

#include "../structures/Token.hpp"

class Variable;
class ArithmeticExpression;
class PreIncrementation;
class Operation;
class Loop;
class Program;

class Node
{
    private:
        variant<Loop*, Program*> parent;
        
    public:
        void setParent(variant<Loop*, Program*> paernt)
        { 
            this->parent = parent;
        }
        enum class Type
        {
            Program,
            Operation,
            Loop,
            Initiation,
            Assigment,
            Condition,
            ArithmeticExpression,
            PrimaryExpression,
            PreIncrementation,
            Variable,
        };

        virtual Type getType() = 0;
};

class PrimaryExpression : public Node
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
        virtual Type getType(){ return Node::Type::PrimaryExpression;}
};

class Variable : public Node
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
        virtual Type getType(){ return Node::Type::Variable;}
};

class ArithmeticExpression : public Node
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
        virtual Type getType(){ return Node::Type::ArithmeticExpression;}
};

class PreIncrementation : public Node
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
        virtual Type getType(){ return Node::Type::PreIncrementation;}
};

class Assigment : public Node
{
    private:
        ArithmeticExpression* arithmeticExpression;
    public:
        ArithmeticExpression* getArithmeticExpression() { return arithmeticExpression; }
        void addArithmeticExpression(ArithmeticExpression* arithmeticExpression)
        {
            this->arithmeticExpression = arithmeticExpression;
        }
        virtual Type getType(){ return Node::Type::Assigment;}
};

class Condition : public Node
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
        virtual Type getType(){ return Node::Type::Condition;}
};

class Initiation : public Node
{
    private:
        Token dataType;
        Variable* variable;
        bool assigmentFlag = false;
        Assigment* assigment;
    public:
        Token getDataType() { return dataType; }
        Variable* getVariable() { return variable; }
        bool hasAssigment() { return assigmentFlag; }
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
            assigmentFlag = true;
            this->assigment = assigment;
        }
        virtual Type getType(){ return Node::Type::Initiation;}
};

class Loop : public Node
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
        virtual Type getType(){ return Node::Type::Loop;}
};

class Operation : public Node
{
    private:
        bool empty = true;
        variant<Loop*, pair<Variable*, Assigment*>, pair<Variable*, Token>, Initiation*, PreIncrementation*> oper;
    public:
        bool isEmpty(){ return empty; };
        variant<Loop*, pair<Variable*, Assigment*>, pair<Variable*, Token>, Initiation*, PreIncrementation*> getOper()
        {
            return oper;
        }
        
        void addLoop(Loop* loop)
        {
            empty = false;
            oper = loop;
        }
        void addPostIncrementation(Variable* variable, Token& token)
        {
            empty = false;
           oper = std::make_pair(variable, token);
        }
        void addAssigment(Variable* variable, Assigment* assigment)
        {
            empty = false;
            oper = std::make_pair(variable, assigment);
        }
        void addInitiation(Initiation* initiation)
        {
            empty = false;
            oper = initiation;
        }
        void addPreIncrementation(PreIncrementation* preIncrementation)
        {
            empty = false;
            oper = preIncrementation;
        }
        virtual Type getType(){ return Node::Type::Operation;}
};

class Program : public Node
{
    public:
        std::vector<Operation*>operations;

        void addOperation(Operation* oper)
        {
            operations.push_back(oper);
        }
        virtual Type getType(){ return Node::Type::Program;}
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
        Program* getProgram()
        {
            return &program;
        }

};


#endif