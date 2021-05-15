#include "../../include/parser/Parser.hpp"

using namespace std;

Parser::Parser(string path, bool isFile /*= true*/)
{
    lexer = new Lexer(path, isFile);
}

Parser::~Parser()
{
    delete lexer;
}

bool Parser::parse()
{
    token = lexer->getNextToken();

    bool program = parseProgram();
    if(program)
    {
        cout << "Przeparsowano program\n";
        return true;
    }
    else
        return false;
}

bool Parser::parseProgram()
{
    while(token.type != END_OF_FILE)
    {
        bool operation = parseOperation();
        if(operation)
            cout << "Przeparsowano operację\n";
        else
            return false;
    }
    return true;
}

bool Parser::parseOperation()
{
    //Operation operation;
    bool loop = parseLoop();
    if(loop)
    {
        //operation.addLoop(*loop);
        cout << "Przeparsowano operację - pętlę\n";
        return true;
        //return operation;
    }
    bool variable = parseVariable();
    if(variable)
    {
        if(token.type == INCREMENTAL_OPERATOR)
        {
            //operation.addPostIncrementation(*variable, token);
            token = lexer->getNextToken();
        }
        else
        {
            bool assigment = parseAssigment();
            if(assigment)
            {
                //operation.addAssigment(*variable, *assigment);
            }
            else
            {
                throw "oczekiwano inkrementacji lub przypisania";
            }
        }
        if(token.type != SEMICOLON)
            throw "oczekiwano średnika na koniec operacji";
        token = lexer->getNextToken();
        cout << "Przeparsowano przypisanie/post inkrementację\n";
        return true;
    }
    
    bool initioation = parseInitiation();
    if(initioation)
    {
        //operation.addInitiation(*initioation);
        if(token.type != SEMICOLON)
        {
            throw "oczekiwano średnika na koniec operacji";
        }
        token = lexer->getNextToken();
        //return operation;
        cout << "Przeparsowano inicjacę\n";
        return true;
    }

    bool preIncrementation = parsePreIncrementation();
    if(preIncrementation)
    {
        //operation.addPreIncrementation(*preIncrementation);
        if(token.type != SEMICOLON)
        {
            throw "oczekiwano średnika po poperacji";
        }
        cout << "przeparsowano operację - preinkrementację\n";
        //return operation;
        return true;
    }

    if(token.type == SEMICOLON)
    {
        token = lexer->getNextToken();
        //return operation;
        return true;
    }

    return false;
}

bool Parser::parseLoop()
{
    //Loop loop;
    if( !(token.type == KEY_WORD && std::get<std::string>(token.value) == "for") )
    {    
        cout << "ERROR oczekiwano 'for'\n";
        return false;
    }
    token = lexer->getNextToken();
    if( token.type != OPENING_ROUND_BRACKET )
    {
        throw "oczekiwano okrągłego nawiasu po 'for'";
    }
    token = lexer->getNextToken();
    bool initiation = parseInitiation();
    if( !initiation )
    {
        bool variable = parseVariable();
        if( ! variable )
        {
            if( token.type != SEMICOLON )
            {
                throw "oczekiwano inicjacji";
            }
        }
        else
        {
            bool assigment = parseAssigment();
            if( !assigment )
            {
                throw "po nazwie zmiennej oczekiwano przypisania";
            }
            else
            {
               // loop.addInitAssigment(*variable, *assigment);
            }
        }
    }
    else
    {
        //loop.addInitiation(*initiation);
    }

    if( token.type != SEMICOLON )
    {
        throw "oczekiwano średnik";
    }
    token = lexer->getNextToken();

    bool condition = parseCondition();
    if( !condition )
    {
        if( token.type != SEMICOLON )
        {
            throw "oczekiwano warunku";
        }
    }
    //loop.addCondition(*condition);
    if( token.type != SEMICOLON )
    {
        throw "oczekiwano średnika";
    }
    token = lexer->getNextToken();

    bool preIncrementation = parsePreIncrementation();
    if(!preIncrementation)
    {
        bool variable = parseVariable();
        if(!variable)
        {
            if( token.type != CLOSING_ROUND_BRACKET )
            {
                throw "oczekiwano aktualizacji zmiennej";
            }
        }
        else
        {
            if(token.type == INCREMENTAL_OPERATOR)
            {
                //loop.addPostIncrementation(*variable, token);
                token = lexer->getNextToken();
            }
            else
            {
                bool assigment = parseAssigment();
                if(!assigment)
                {
                    throw "oczekiwano aktualizacji zmiennych";
                }
                else
                {
                    //loop.addUpdateAssigment(*variable, *assigment);
                }
            }
        }
    }
    else
    {
        //loop.addPreIncrementation(*preIncrementation);
    }
    if( token.type != CLOSING_ROUND_BRACKET )
    {
        throw "oczekiwano )";
    }
    token = lexer->getNextToken();

    if( token.type != OPENING_BLOCK_BRACKET )
    {
        throw "oczekiwano {";
    }
    token = lexer->getNextToken();

    bool operation;
    //pobieranie operacji
    while(token.type != CLOSING_BLOCK_BRACKET && token.type != END_OF_FILE)
    {
        operation = parseOperation();
        if(operation)
        {
            //loop.addOperation(*operation);
        }
        else
        {
            throw "oczekiwano operację";
        }
    }
    if( !token.type == CLOSING_BLOCK_BRACKET )
    {
        throw "oczekiwano }";
    }
    token = lexer->getNextToken();
    //return loop;
    cout << "Przeparsowano pętlę\n";
    return true;
}

bool Parser::parseVariable()
{
    //Variable variable;
    if( token.type != IDENTYFIER )
    {
        cout << "ERRoR oczekiwano identyfiaktora\n";
        return false;
    }
    //variable.addVariableName(token);
    token = lexer->getNextToken();


    if(token.type == OPENING_SQUARE_BRACKET)
    {
        token = lexer->getNextToken();
        bool arithmeticExpression = parseArithmeticExpression();
        if( !arithmeticExpression )
        {
            throw "oczekiwano indeksu";
        }
        else
        {
            cout << token.type << "\n";
            if(token.type == CLOSING_SQUARE_BRACKET)
            {
                //variable.addIndex(*arithmeticExpression);
                token = lexer->getNextToken();
                cout << "Przeparsowano tablicę\n";
                return true;
            }
            else
            {
                throw "oczekiwano zamknięcia kwadratowego nawiasu";
                return false;
            }
        }
    }
    else
    {
        cout << "Przeparsowano zmienną\n";
        return true;
        //return variable;
    }
    
}

bool Parser::parseAssigment()
{
    cout << "P Assigment\n";
    if( token.type != ASSIGMENT_OPERATOR )
    {
        cout << "ERROR oczekiwano znaku równa się\n";
        return false;
    }

    token = lexer->getNextToken();
    bool arithmeticExpression = parseArithmeticExpression();
    if( !arithmeticExpression )
    {
        throw "oczekiwano wyrażenia po znaku równa się";
    }
    cout << "Przeparsowano przypisanie\n";
    return true;
}

bool Parser::parseInitiation()
{
    //Initiation initiation;
    if(token.type != DATA_TYPE)
    {
        cout << "ERROR oczekiwoano typu danych\n";
        return false;
    }
    //initiation.addDataType(token);
    token = lexer->getNextToken();

    bool variable = parseVariable();
    if( !variable )
    {
        throw "oczekiwano nazwy zmiennej";
    }
    //initiation.addVariable(*variable);

    bool assigment = parseAssigment();
    if(assigment)
    {
        cout << token.type << "\n";
        //initiation.addAssigment(*assigment);
    }
    cout << "Przeparsowano inicjację\n" << token.type << "\n";
    // return initiation;
    return true;
}

bool Parser::parsePreIncrementation()
{
    //PreIncrementation preIncrementation;
    if( token.type != INCREMENTAL_OPERATOR )
    {
        cout << "ERROR oczekiwano operatora inkrementacji\n";
        return false;
    }
    //preIncrementation.addToken(token);
    token = lexer->getNextToken();

    bool variable = parseVariable();
    if(!variable)
    {
        throw "oczekiwano nazwy zmiennej";
        return false;
    }
    //preIncrementation.addVariable(*variable);
    cout << "Przeparsowano preinkrementację\n";
    //return preIncrementation;
    return true;
}

bool Parser::parseCondition()
{
    //Condition condition;
    bool arithmeticExpression = parseArithmeticExpression();
    if(!arithmeticExpression)
    {
        cout << "ERROR oczekiwano wyrażenia arytmetycznego\n";
        return false;
    }
    // condition.addExpression(*arithmeticExpression);
    if( token.type != RELATIONAL_OPERATOR )
    {
        throw "oczekiwano operatora logicznego";
    }
    // condition.addOperator(token);
    token = lexer->getNextToken();
    arithmeticExpression = parseArithmeticExpression();
    if(!arithmeticExpression)
    {
        throw "oczekiwano wyrażenia arytmetycznego";
    }
    //condition.addExpression(*arithmeticExpression);
    //return condition;
    cout << "Przeparsowano warunek\n";
    return true;
}

bool Parser::parseArithmeticExpression()
{
    // ArithmeticExpression arithmeticExpression;
    bool primaryExpression = parsePrimaryExpression();
    if(!primaryExpression)
    {
        cout << "ERROR oczekiwano primary expression\n";
        return false;
    }
    //arithmeticExpression.addPrimaryExpression(*primaryExpression);
    while(token.type == ADDITIVE_OPERATOR || token.type == MULTIPLICATIVE_OPERATOR)
    {
        //arithmeticExpression.addOperator(token);
        token = lexer->getNextToken();
        primaryExpression = parsePrimaryExpression();
        if(!primaryExpression)
        {

            throw createError(
                EXPECTED_EXPRESSION_AFTER_OPERATOR,
                "expected expression after operator",
                "",
                token
                );
        }
        //arithmeticExpression.addPrimaryExpression(*primaryExpression);
    }
    // return arithmeticExpression;
    cout << "Przeparsowano wyrażenie arytmetyczne\n";
    return true;
}

bool Parser::parsePrimaryExpression()
{
    // PrimaryExpression primaryExpression;
    bool variable = parseVariable();
    if(variable)
    {
        // primaryExpression.addVariable(*variable);
        if(token.type == INCREMENTAL_OPERATOR)
        {
            cout << "Primary expression - inkrementacja\n";
            //primaryExpression.addIncrementOperator(token);
            return true;
        }
    }
    else if(token.type == NUMBER)
    {
        cout << "Primary expression - numer\n";
        // primaryExpression.addNumber(token);
        token = lexer->getNextToken();
        return true;
    }
    else
    {
        bool preIncrementation = parsePreIncrementation();
        if(!preIncrementation)
        {
            cout << "ERROR oczekiwano zmiennej, numeru lub inkrementacji\n";
            return false;
        }
        //primaryExpression.addPreIncrementation(*preIncrementation);
    }
    cout << "Przeparsowano podstawowe wyrażenie\n";
    return true;
    // return primaryExpression;
}

AnalizeError Parser::createError(ErrorType type, string message, string codePart, Token token)
{
    AnalizeError error 
    {
        type, 
        PARSER,
        message,
        codePart,
        token.line_number,
        token.position_in_line
    };
    return error;
}