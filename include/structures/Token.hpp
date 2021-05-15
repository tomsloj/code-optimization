#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <variant>
#include <string>

enum TokenType 
{
    KEY_WORD,
    IDENTYFIER,
    ASSIGMENT_OPERATOR,
    ADDITIVE_OPERATOR,
    MULTIPLICATIVE_OPERATOR,
    INCREMENTAL_OPERATOR, //5
    DECREMENTAL_OPERATOR,
    DATA_TYPE,
    RELATIONAL_OPERATOR,
    OPENING_ROUND_BRACKET,
    CLOSING_ROUND_BRACKET, //10
    OPENING_BLOCK_BRACKET,
    CLOSING_BLOCK_BRACKET,
    OPENING_SQUARE_BRACKET,
    CLOSING_SQUARE_BRACKET,
    NUMBER,
    SEMICOLON, // 16
    END_OF_FILE,
    UNKNOW,
    ERROR_TOKEN
};

struct Token 
{
    TokenType type;
    std::variant<unsigned long long, double, std::string> value;
    int line_number = 1;
    int position_in_line = 1;
};

#endif