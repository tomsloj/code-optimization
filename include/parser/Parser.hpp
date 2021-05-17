#ifndef PARSER_HPP
#define PARSER_HPP

#pragma once

#include "../lexer/Lexer.hpp"
#include "../structures/Token.hpp"
#include "../structures/AnalizeError.hpp"
// #include "../ParserTree/ParserTree.hpp"

#include "../ParserTree/Node.hpp"

#include <optional>
#include <string>

class Parser
{
    private:
        Lexer* lexer;
        Token token;

        AnalizeError createError(ErrorType type, string message, string codePart, Token token);
        void writeError(AnalizeError e);
    
        optional<Program*> parseProgram();
        optional<Operation*> parseOperation(Node* parent);
        optional<Loop*> parseLoop(Node* parent);
        optional<Variable*> parseVariable(Node* parent);
        optional<Assigment*> parseAssigment(Node* parent);
        optional<Initiation*> parseInitiation(Node* parent);
        optional<PreIncrementation*> parsePreIncrementation(Node* parent);
        optional<Condition*> parseCondition(Node* parent);
        optional<ArithmeticExpression*> parseArithmeticExpression(Node* parent);
        optional<PrimaryExpression*> parsePrimaryExpression(Node* parent);
    public:
        Parser(std::string path, bool isFile = false);
        ~Parser();
        optional<ParserTree> parse();
};

#endif