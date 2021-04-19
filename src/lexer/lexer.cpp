#include "../../include/lexer/lexer.hpp"

#include <unistd.h>

using namespace std;

Lexer::Lexer(std::string source)
{
    key_word_map.insert(make_pair("int", DATA_TYPE));
    key_word_map.insert(make_pair("long", DATA_TYPE));
    key_word_map.insert(make_pair("double", DATA_TYPE));
    key_word_map.insert(make_pair("for", KEY_WORD));

    for (auto x : source)
    {
        this->source.push_back(x);
    }
    getChar();
    while(isspace(ch))
    {
        getChar();
    }
    
}

void Lexer::getChar()
{
    ch = ' ';
    if(!source.empty())
    {
        ch = source.front();
        source.pop_front();
    }
    else
    {
        ch = EOF;
    }
}

Token Lexer::getNextToken()
{
    int i, nextch;

    Token* token = NULL;
    try
    {
        token = buildIdentyfier();
    }
    catch(AnalizeError e)
    {
        writeError(e);
        while(isspace(ch))
            getChar();
        token = new Token;
        token->type = ERROR_TOKEN;
        return *token;
    }
    if( token != NULL )
    {
        while(isspace(ch))
            getChar();
        return *token;
    }

    try
    {
        token = buildNumber();
    }
    catch(AnalizeError e)
    {
        writeError(e);
        while(isspace(ch))
            getChar();
        token = new Token;
        token->type = ERROR_TOKEN;
        return *token;
    }
    if( token != NULL )
    {
        while(isspace(ch))
            getChar();
        return *token;
    }

    try
    {
        token = buildAssigmentAndEquality();
    }
    catch(AnalizeError e)
    {
        writeError(e);
        while(isspace(ch))
            getChar();
        token = new Token;
        token->type = ERROR_TOKEN;
        return *token;
    }
    if( token != NULL )
    {
        while(isspace(ch))
            getChar();
        return *token;
    }
    
    try
    {
        token = buildOperations();
    }
    catch(AnalizeError e)
    {
        writeError(e);
        while(isspace(ch))
            getChar();
        token = new Token;
        token->type = ERROR_TOKEN;
        return *token;
    }
    if( token != NULL )
    {
        while(isspace(ch))
            getChar();
        return *token;
    }
    
    try
    {
        token = buildLogicalOperator();
    }
    catch(AnalizeError e)
    {
        writeError(e);
        while(isspace(ch))
            getChar();
        token = new Token;
        token->type = ERROR_TOKEN;
        return *token;
    }
    if( token != NULL )
    {
        while(isspace(ch))
            getChar();
        return *token;
    }

    try
    {
        token = buildRoundBracket();
    }
    catch(AnalizeError e)
    {
        writeError(e);
        while(isspace(ch))
            getChar();
        token = new Token;
        token->type = ERROR_TOKEN;
        return *token;
    }
    if( token != NULL )
    {
        while(isspace(ch))
            getChar();
        return *token;
    } 
    
    try
    {
        token = buildBlockBracket();
    }
    catch(AnalizeError e)
    {
        writeError(e);
        while(isspace(ch))
            getChar();
        token = new Token;
        token->type = ERROR_TOKEN;
        return *token;
    }
    if( token != NULL )
    {
        while(isspace(ch))
            getChar();
        return *token;
    }

    try
    {
        token = buildSquareBracket();
    }
    catch(AnalizeError e)
    {
        writeError(e);
        while(isspace(ch))
            getChar();
        token = new Token;
        token->type = ERROR_TOKEN;
        return *token;
    }
    if( token != NULL )
    {
        while(isspace(ch))
            getChar();
        return *token;
    }

    try
    {
        token = buildEOF();
    }
    catch(AnalizeError e)
    {
        writeError(e);
        while(isspace(ch))
            getChar();
        token = new Token;
        token->type = ERROR_TOKEN;
        return *token;
    }
    if( token != NULL )
    {
        while(isspace(ch))
            getChar();
        return *token;
    }

    if(token == NULL)
    {
        string codePart = string(1, ch);;
        string message = "Unknow token";
        getChar();
        while(!isspace(ch) && ch != EOF)
        {
            codePart += ch;
            getChar();    
        }
        while(isspace(ch))
            getChar();
        AnalizeError error = 
        {
            UNKNOW_TOKEN,
            LEXER,
            message,
            codePart
        };
        writeError(error);
        token = new Token;
        token->type = UNKNOW;
    }
    return *token;
}

Token* Lexer::buildIdentyfier()
{
    if(isalpha(ch) || ch == '_')
    {
        string identyfier = "";
        while(isalpha(ch) || ch == '_' || isdigit(ch))
        {
            identyfier += ch;
            getChar();
        }
        if(!isspace(ch) && ch != EOF)
        {
            string message = "Not allowed sign: " + ch;
            string codePart = identyfier;
            while(!isspace(ch) && ch != EOF)
            {
                codePart += ch;
                getChar();
            }

            AnalizeError error = 
            {
                NOT_ALLOWED_SIGN,
                LEXER,
                message,
                codePart
            };
            throw error;
        }
        Token* token = new Token;
        token->value = identyfier;
        token->type = IDENTYFIER;
        
        return token;
    }
    return NULL;
}

Token* Lexer::buildNumber()
{
    // TODO sprawdzać czy nie przekroczyliśmy typu
    if(isdigit(ch))
    {
        unsigned long long value = 0;
        double valueAfterPoint = 0.0;
        double multiplier = 0.1;
        while(isdigit(ch))
        {
            value = value * 10 + (ch - ZERO);
            getChar();
        }
        if(ch == POINT)
        {
            getChar();
            if(!isdigit(ch))
            {
                // error, expected number after point
                string message = "Expected number after point but get";
                if(isspace(ch))
                    message += " nothing";
                else
                    message += ": ";
                    message += ch;
                string codePart = to_string(value) + ".";
                // read characters till white space
                while(!isspace(ch) && ch != EOF)
                {
                    codePart += ch;
                    getChar();
                }
                AnalizeError error = 
                {
                    WRONG_NUMBER,
                    LEXER,
                    message,
                    codePart
                };
                throw error;
            }

            while(isdigit(ch))
            {
                valueAfterPoint = valueAfterPoint + static_cast<double>(ch - ZERO) * multiplier;
                multiplier *= 0.1;
                getChar();
            }
        }
        // if we have other sign than whitespace
        if(!isspace(ch) && ch != EOF)
        {
            string message = "Expected whitespace after number but get: ";
            message += ch;
            string codePart;
            if(valueAfterPoint == 0.0)
                codePart = to_string(value);
            else
                codePart = to_string(double(value) + valueAfterPoint);
            // read characters till white space
            while(!isspace(ch) && ch != EOF)
            {
                codePart += ch;
                getChar();
            }
            
            AnalizeError error = 
            {
                WRONG_NUMBER,
                LEXER,
                message,
                codePart
            };
            throw error;
        }
        Token* token = new Token;
        token->type = NUMBER;
        if(valueAfterPoint == 0.0)
            token->value = value;
        else
            token->value = double(value) + valueAfterPoint;
        return token;   
    }
    return NULL;
}

Token* Lexer::buildAssigmentAndEquality()
{
    if(ch == '=')
    {
        getChar();
        if(ch == '=')
        {
            Token* token = new Token;
            token->type = RELATIONAL_OPERATOR;
            token->value = "==";
            getChar();
            return token;
        }
        else
        if(isspace(ch) || ch == EOF)
        {
            Token* token = new Token;
            token->type = ASSIGMENT_OPERATOR;
            return token;
        }
    }
    return NULL;
}

Token* Lexer::buildOperations()
{
    if(ch == '+')
    {
        getChar();
        if(ch == '+')
        {
            Token* token = new Token;
            token->type = INCREMENTAL_OPERATOR;
            getChar();
            return token;
        }
        else
        {
            Token* token = new Token;
            token->type = ADDITIVE_OPERATOR;
            token->value = "+";
            return token;
        }
    }
    else
    if(ch == '-')
    {
        getChar();
        if(ch == '-')
        {
            Token* token = new Token;
            token->type = DECREMENTAL_OPERATOR;
            getChar();
            return token;
        }
        else
        {
            Token* token = new Token;
            token->type = ADDITIVE_OPERATOR;
            token->value = "-";
            return token;
        }
    }
    return NULL;
}

Token* Lexer::buildLogicalOperator()
{
    if(ch == '<')
    {
        Token* token = new Token;
        token->type = RELATIONAL_OPERATOR;
        token->value = "<";
        getChar();
        return token;
    }
    else
    if(ch == '>')
    {
        Token* token = new Token;
        token->type = RELATIONAL_OPERATOR;
        token->value = ">";
        getChar();
        return token;
    }
    return NULL;
}

Token* Lexer::buildRoundBracket()
{
    if(ch == '(')
    {
        Token* token = new Token;
        token->type = OPENING_ROUND_BRACKET;
        getChar();
        return token;
    }
    else
    if(ch == ')')
    {
        Token* token = new Token;
        token->type = CLOSING_ROUND_BRACKET;
        getChar();
        return token;
    }
    return NULL;
}

Token* Lexer::buildBlockBracket()
{
    if(ch == '{')
    {
        Token* token = new Token;
        token->type = OPENING_BLOCK_BRACKET;
        getChar();
        return token;
    }
    else
    if(ch == '}')
    {
        Token* token = new Token;
        token->type = CLOSING_BLOCK_BRACKET;
        getChar();
        return token;
    }
    return NULL;
}

Token* Lexer::buildSquareBracket()
{
    if(ch == '[')
    {
        Token* token = new Token;
        token->type = OPENING_SQUARE_BRACKET;
        getChar();
        return token;
    }
    else
    if(ch == ']')
    {
        Token* token = new Token;
        token->type = CLOSING_SQUARE_BRACKET;
        getChar();
        return token;
    }
    return NULL;
}

Token* Lexer::buildEOF()
{
    if(ch == EOF)
    {
        Token* token = new Token;
        token->type = END_OF_FILE;
        return token;
    }
    return NULL;
}

void Lexer::writeError(AnalizeError error)
{
    cout << "LEXER ERROR: line " + to_string(error.line + 1) + 
    "; sign " + to_string(error.signNumber + 1) + "\n";
    cout << error.codePart << "\n";
    cout << error.message << "\n";
}