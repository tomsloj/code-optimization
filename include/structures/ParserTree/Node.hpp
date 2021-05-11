#ifndef NODE_HPP
#define NODE_HPP

class Node
{
    private:
        
    public:
        enum class Type
        {
            Program,
            Operation,
            Loop,
            Block,
            Initiation,
            Assigment,
            Condition,
            ArithmeticExpression,
            Incrementation,
            Variable,
            VariableName,
            Number,
        };

        virtual Type getType() = 0;


};

class Number: public Node
{
    public:

        virtual Type getType()
        {
            return Node::Type::Number;
        }
};

class VariableName: public Node
{
    public:

        virtual Type getType()
        {
            return Node::Type::VariableName;
        }
};

class Variable: public Node
{
    public:

        virtual Type getType()
        {
            return Node::Type::Variable;
        }
};

class Incrementation: public Node
{
    public:

        virtual Type getType()
        {
            return Node::Type::Incrementation;
        }
};

class ArithmeticExpression: public Node
{
    public:

        virtual Type getType()
        {
            return Node::Type::ArithmeticExpression;
        }
};

class Condition: public Node
{
    public:

        virtual Type getType()
        {
            return Node::Type::Condition;
        }
};

class Assigment: public Node
{
    public:

        virtual Type getType()
        {
            return Node::Type::Assigment;
        }
};

class Initiation: public Node
{
    public:

        virtual Type getType()
        {
            return Node::Type::Initiation;
        }
};

class Block: public Node
{
    public:

        virtual Type getType()
        {
            return Node::Type::Loop;
        }
};

class Loop: public Node
{
    public:

        virtual Type getType()
        {
            return Node::Type::Loop;
        }
};

class Operation: public Node
{
    public:

        virtual Type getType()
        {
            return Node::Type::Operation;
        }
};

class Program: public Node
{
    public:
        std::vector<Operation>operations;

        void addOperation(Operation &operation)
        {
            operations.push_back(operation);
        }

        virtual Type getType()
        {
            return Node::Type::Program;
        }
};

#endif