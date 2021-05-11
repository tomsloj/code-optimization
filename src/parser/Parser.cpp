#include "../../include/parser/Parser.hpp"

Parser::Parser()
{
    lexer = new Lexer("");
}

Parser::~Parser()
{
    delete lexer;
}

