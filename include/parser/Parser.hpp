#ifndef PARSER_HPP
#define PARSER_HPP

#include "../lexer/Lexer.hpp"
#include "../structures/Token.hpp"
#include "../structures/AnalizeError.hpp"
#include "../structures/ParserTree/ParserTree.hpp"


class Parser
{
    private:
        Lexer* lexer;
    public:
        Parser();
        ~Parser();
        ParserTree parseProgram();

};

#endif