#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

#include <string>
#include <variant>
#include <vector>
#include <set>

#include "../analyzer/Analyzer.hpp"
#include "../ParserTree/Node.hpp"

struct VarDetails
{
    std::string name;
    int level;
    bool alreadyUsed = false;
    bool isTable = false;
};

class Optimizer
{
    private:
        Analyzer* analyzer;
        std::vector<struct VarDetails>varMap;
        ParserTree* tree;

        void optimizeTree(ParserTree* tree);
        void optimizeProgram(Program& program);
        variant< bool, vector<Operation*> > optimizeOperation(Operation& operation, int level, multiset<string> usedNamesDownLevel, multiset<string> usedThisLevel);
        std::vector<Operation*> optimizeLoop(Loop& loop, int level, multiset<string> usedNamesDownLevel);
        bool signInitiation(Initiation& initiation, int level,  multiset<string> usedNamesDownLevel, multiset<string> usedThisLevel);

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
        bool isTable(Variable& variable);
        int getVariableLevel(Variable& variable);
        int getVariableLevel(Variable& variable, std::vector<struct VarDetails>* varMap);

        void addVariable(Token token, bool isTable, int level);
        void addVariable(Token token, bool isTable, int level, std::vector<struct VarDetails>* varMap);
        void removeLevel(int level);
        AnalizeError createError(ErrorType type, string message, string codePart, Token token);

        std::multiset<string> getUsed(Operation& operation);
        std::multiset<string> getUsed(PrimaryExpression& primaryExpression);
        std::multiset<string> getUsed(Initiation& initiation);

        std::vector<struct VarDetails>varMapToCondition;

        bool findUsageAndChange(vector<Operation*> operations, string varName, int varLevel, int currentLevel);
        int findUsageAndChange(Operation& operation, int currentState, string varName, int varLevel, int currentLevel);
        bool findUsageAndChange(PrimaryExpression& primaryExpression, int currentState, string varName, int varLevel, int currentLevel);

        void changeLevel(Variable& variable);

    public:
        Optimizer(string path, bool isFile = false);
        ~Optimizer();
        bool optimize();
        ParserTree* getTree();
};

#endif