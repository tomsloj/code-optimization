#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include <string>
#include <variant>

#include "../lexer/Lexer.hpp"
#include "../parser/Parser.hpp"
#include "../ParserTree/Node.hpp"

struct VarInfo
{
    std::string name;
    int level;
    bool isTable = false;
};

class Analyzer
{
    private:
        Parser* parser;
        std::vector<struct VarInfo>varMap;

        optional<ParserTree> tree;

        optional<AnalizeError> analyzeTree(ParserTree& tree);
        optional<AnalizeError> analyzeProgram(Program& program);
        optional<AnalizeError> analyzeOperation(Operation& operation, int level);
        optional<AnalizeError> analyzeLoop(Loop& loop, int level);
        optional<AnalizeError> analyzeInitiation(Initiation& initiation, int level);


        optional<AnalizeError> checkPreinkrementation(PreIncrementation& preincrementation);
        optional<AnalizeError> checkPostinkrementation(Variable* variable, Token token);
        // bool checkInitiation(Initiation& initiation, int level);
        optional<AnalizeError> checkAssigment(Assigment& assigment);
        optional<AnalizeError> checkArithmeticExpression(ArithmeticExpression& arithmeticExpression);
        optional<AnalizeError> checkCondition(Condition& condition);
        optional<AnalizeError> checkPrimaryExpression(PrimaryExpression& primaryExpression);
        optional<AnalizeError> checkVariable(Variable& variable);
        bool isMutable(ArithmeticExpression& arithmeticExpression);

        optional<AnalizeError> addVariable(Token token, bool isTable, int level);
        void removeLevel(int level);
        AnalizeError createError(ErrorType type, string message, string codePart, Token token);
        void writeError(AnalizeError error);
    
    public:
        Analyzer(string path, bool isFile = false);
        ~Analyzer();
        bool analyze();
        optional<ParserTree> getTree();
};

#endif