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
    public:
        Parser(std::string path, bool isFile = false);
        ~Parser();

        optional<ParserTree> parse();
        optional<Program> parseProgram();
        optional<Operation> parseOperation();
        optional<Loop> parseLoop();
        optional<Variable> parseVariable();
        optional<Assigment> parseAssigment();
        optional<Initiation> parseInitiation();
        optional<PreIncrementation> parsePreIncrementation();
        optional<Condition> parseCondition();
        optional<ArithmeticExpression> parseArithmeticExpression();
        optional<PrimaryExpression> parsePrimaryExpression();

};

#endif