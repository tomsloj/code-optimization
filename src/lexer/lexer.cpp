#include "../../include/lexer/lexer.hpp"

#include <unistd.h>

using namespace std;

Lexer::Lexer(std::string source, bool isFile /*= false*/)
{
    key_word_map.insert(make_pair("int", DATA_TYPE));
    key_word_map.insert(make_pair("long", DATA_TYPE));
    key_word_map.insert(make_pair("double", DATA_TYPE));
    key_word_map.insert(make_pair("for", KEY_WORD));

    if(isFile)
        this->source.loadFile(source);
    else
        this->source.loadString(source);

    // for (auto x : source)
    // {
    //     this->source.push_back(x);
    // }
    getChar();
    while(isspace(ch))
    {
        getChar();
    }
    
}

void Lexer::getChar()
{
    ch = source.getChar();
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
        token = buildSemicolon();
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
            if(identyfier.size() <= MAX_IDENTYFIER_SIZE)
                identyfier += ch;
            getChar();
        }
        if(identyfier.size() > MAX_IDENTYFIER_SIZE)
        {
            // error, expected number after point
            string message = "Too long identyfier; Max size is 64 signs";
            // read characters till white space
            AnalizeError error = 
            {
                TOO_LONG_IDENTYFIER,
                LEXER,
                message,
                identyfier + "..."
            };
            throw error;
        }
        Token* token = new Token;
        token->value = identyfier;
        if( key_word_map.find(identyfier) == key_word_map.end() )
            token->type = IDENTYFIER;
        else
            token->type = key_word_map[identyfier];
        return token;
    }
    return NULL;
}

Token* Lexer::buildNumber()
{
    string codePart = "";
    if(isdigit(ch))
    {
        codePart += ch;
        bool outOfRange = false;
        unsigned long long value = 0;
        double valueAfterPoint = 0.0;
        double multiplier = 0.1;
        while(isdigit(ch))
        {
            value = value * 10 + (ch - ZERO);
            codePart += ch;
            getChar();
            if(value >= 1000000000000)
            {
                value = 0;
                outOfRange = true;
            }
        }
        if(ch == POINT)
        {
            if(value >= 10000)
            {
                value = 0;
                outOfRange = true;
            }
            codePart += ch;
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
                //string codePart = to_string(value) + ".";
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
                codePart += ch;
                getChar();
                if(multiplier < 0.000000000000001)
                {
                    multiplier = 0.1;
                    outOfRange = true;
                }
            }
        }
        if(outOfRange)
        {
            // error, expected number after point
            string message = "Number out of range";
            // read characters till white space
            AnalizeError error = 
            {
                VALUE_OUT_OF_RANGE,
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

Token* Lexer::buildSemicolon()
{
    if(ch == ';')
    {
        Token* token = new Token;
        token->type = SEMICOLON;
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