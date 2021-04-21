#ifndef ANALIZE_ERROR_HPP
#define ANALIZE_ERROR_HPP

#include <string>

enum ErrorType 
{
    UNCORRECT_NAME,
    WRONG_NUMBER,
    NOT_ALLOWED_SIGN,
    UNKNOW_TOKEN
};

enum ModuleType
{
    LEXER
};

struct AnalizeError
{
    ErrorType type;
    ModuleType module;
    std::string message;
    std::string codePart;
    int line;
    int signNumber;
};

#endif