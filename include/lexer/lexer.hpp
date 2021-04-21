#ifndef LEXER_HPP
#define LEXER_HPP
#include "../structures/Token.hpp"
#include "../structures/AnalizeError.hpp"
#include "../lexer/Source.hpp"

#include <iostream>
#include <string>
#include <deque>
#include <map>


class Lexer
{
    private:
        const int ZERO = '0';
        const char POINT = '.';
        const int MAX_IDENTYFIER_SIZE = 64;
        char ch;
        Source source;
        std::map<std::string, TokenType>key_word_map;

        void getChar();

        Token* buildIdentyfier();
        Token* buildNumber();
        Token* buildAssigmentAndEquality();
        Token* buildOperations();
        Token* buildLogicalOperator();
        Token* buildRoundBracket();
        Token* buildBlockBracket();
        Token* buildSquareBracket();
        Token* buildSemicolon();
        Token* buildEOF();

        void writeError(AnalizeError error);

    public:
        Lexer(std::string source, bool isFile = false);
        Token getNextToken();
};

#endif