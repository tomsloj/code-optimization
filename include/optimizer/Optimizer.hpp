#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

#include <string>
#include <variant>

#include "../analyzer/Analyzer.hpp"
#include "../ParserTree/Node.hpp"

struct VarDetails
{
    std::string name;
    int level;
    bool alreadyUsed = false;
};

class Optimizer
{
    private:
        Analyzer* analyzer;
        std::vector<struct VarDetails>varMap;
        ParserTree* tree;

        void optimizeTree(ParserTree* tree);
        void optimizeProgram(Program& program);
        variant< bool, vector<Operation*> > optimizeOperation(Operation& operation, int level);
        std::vector<Operation*> optimizeLoop(Loop& loop, int level);
        bool signInitiation(Initiation& initiation, int level);

        void signPreinkrementation(PreIncrementation& preincrementation);
        void signPostinkrementation(Variable* variable, Token token);
        bool signAssigment(Assigment& assigment);
        void signArithmeticExpression(ArithmeticExpression& arithmeticExpression);
        void signCondition(Condition& condition);
        void signPrimaryExpression(PrimaryExpression& primaryExpression);
        void signVariable(Variable& variable);
        bool isMutable(ArithmeticExpression& arithmeticExpression);
        bool isMutable(PrimaryExpression& primaryExpression);
        bool isUsed(Variable& variable);
        int getVariableLevel(Variable& variable);

        void addVariable(Token token, bool isTable, int level);
        void removeLevel(int level);
        AnalizeError createError(ErrorType type, string message, string codePart, Token token);
        void writeError(AnalizeError error);
    
    public:
        Optimizer(string path, bool isFile = false);
        ~Optimizer();
        bool optimize();
        ParserTree* getTree();
};

#endif