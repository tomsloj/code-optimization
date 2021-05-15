#ifndef PARSER_HPP
#define PARSER_HPP

#pragma once

#include "../lexer/Lexer.hpp"
#include "../structures/Token.hpp"
#include "../structures/AnalizeError.hpp"
#include "../ParserTree/ParserTree.hpp"

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
        Parser(std::string path, bool isFile = true);
        ~Parser();

        void getNextToken();

        bool parse();
        bool parseProgram();
        bool parseOperation();
        bool parseLoop();
        bool parseVariable();
        bool parseAssigment();
        bool parseInitiation();
        bool parsePreIncrementation();
        bool parseCondition();
        bool parseArithmeticExpression();
        bool parsePrimaryExpression();

};

#endif