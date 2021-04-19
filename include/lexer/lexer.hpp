#include "../structures/Token.hpp"
#include "../structures/AnalizeError.hpp"

#include <iostream>
#include <string>
#include <deque>
#include <map>


class Lexer
{
    private:
        const int ZERO = '0';
        const char POINT = '.';
        std::deque<char>source;
        char ch;

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
        Token* buildEOF();

        void writeError(AnalizeError error);

    public:
        Lexer(std::string source);
        Token getNextToken();
};