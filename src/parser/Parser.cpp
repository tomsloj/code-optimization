#include "../../include/parser/Parser.hpp"

using namespace std;

Parser::Parser(string path, bool isFile /*= false*/)
{
    lexer = new Lexer(path, isFile);
}

Parser::~Parser()
{
    delete lexer;
}

optional<ParserTree> Parser::parse()
{
    token = lexer->getNextToken();

    optional<Program> program = parseProgram();
    if(program)
    {
        cout << "Przeparsowano program\n";
        return ParserTree(*program);
    }
    else
        return {};
}

optional<Program> Parser::parseProgram()
{
    Program program;
    while(token.type != END_OF_FILE)
    {
        optional<Operation> operation = parseOperation();
        if(operation)
        {
            cout << "Przeparsowano operację\n";
            program.addOperation(*operation);
        }
        else
            return {};
    }
    return program;
}

optional<Operation> Parser::parseOperation()
{
    Operation operation;
    optional<Loop> loop = parseLoop();
    if(loop)
    {
        operation.addLoop(&*loop);
        cout << "Przeparsowano operację - pętlę\n";
        return operation;
    }
    optional<Variable> variable = parseVariable();
    if(variable)
    {
        if(token.type == INCREMENTAL_OPERATOR)
        {
            operation.addPostIncrementation(&*variable, token);
            token = lexer->getNextToken();
        }
        else
        {
            optional<Assigment> assigment = parseAssigment();
            if(assigment)
            {
                operation.addAssigment(&*variable, &*assigment);
            }
            else
            {
                throw createError(
                    EXPECTED_EXPRESSION_AFTER_OPERATOR,
                    "expected inkrementation or assigment",
                    "",
                    token
                );
            }
        }
        if(token.type != SEMICOLON)
        {
            throw createError(
                EXPECTED_EXPRESSION_AFTER_OPERATOR,
                "expected semicolon after operation",
                "",
                token
            );
        }
        token = lexer->getNextToken();
        cout << "Przeparsowano przypisanie/post inkrementację\n";
        return operation;
    }
    
    optional<Initiation> initioation = parseInitiation();
    if(initioation)
    {
        operation.addInitiation(&*initioation);
        if(token.type != SEMICOLON)
        {
            throw createError(
                EXPECTED_EXPRESSION_AFTER_OPERATOR,
                "expected semicolon after operation",
                "",
                token
            );
        }
        token = lexer->getNextToken();
        cout << "Przeparsowano inicjacę\n";
        return operation;
    }

    optional<PreIncrementation> preIncrementation = parsePreIncrementation();
    if(preIncrementation)
    {
        operation.addPreIncrementation(&*preIncrementation);
        if(token.type != SEMICOLON)
        {
            throw createError(
                EXPECTED_EXPRESSION_AFTER_OPERATOR,
                "expected semicolon after operation",
                "",
                token
            );
        }
        cout << "przeparsowano operację - preinkrementację\n";
        return operation;
    }

    if(token.type == SEMICOLON)
    {
        token = lexer->getNextToken();
        return operation;
    }
    return {};
}

optional<Loop> Parser::parseLoop()
{
    Loop loop;
    if( !(token.type == KEY_WORD && std::get<std::string>(token.value) == "for") )
    {    
        cout << "ERROR oczekiwano 'for'\n";
        return {};
    }
    token = lexer->getNextToken();
    if( token.type != OPENING_ROUND_BRACKET )
    {
        throw createError(
            EXPECTED_EXPRESSION_AFTER_OPERATOR,
            "expected ( in for loop",
            "",
            token
        );
    }
    token = lexer->getNextToken();
    optional<Initiation> initiation = parseInitiation();
    if( !initiation )
    {
        optional<Variable> variable = parseVariable();
        if( ! variable )
        {
            if( token.type != SEMICOLON )
            {
                throw createError(
                    EXPECTED_EXPRESSION_AFTER_OPERATOR,
                    "expected initiation",
                    "",
                    token
                );
            }
        }
        else
        {
            optional<Assigment> assigment = parseAssigment();
            if( !assigment )
            {
                throw createError(
                    EXPECTED_EXPRESSION_AFTER_OPERATOR,
                    "expected assigment after variable",
                    "",
                    token
                );
            }
            else
            {
               loop.addInitAssigment(&*variable, &*assigment);
            }
        }
    }
    else
    {
        loop.addInitiation(&*initiation);
    }

    if( token.type != SEMICOLON )
    {
        throw createError(
            EXPECTED_EXPRESSION_AFTER_OPERATOR,
            "expected semicolon",
            "",
            token
        );
    }
    token = lexer->getNextToken();

    optional<Condition> condition = parseCondition();
    if( !condition )
    {
        if( token.type != SEMICOLON )
        {
            throw createError(
                EXPECTED_EXPRESSION_AFTER_OPERATOR,
                "expected condition",
                "",
                token
            );
        }
    }
    loop.addCondition(&*condition);
    if( token.type != SEMICOLON )
    {
        throw createError(
            EXPECTED_EXPRESSION_AFTER_OPERATOR,
            "expected semicolon",
            "",
            token
        );
    }
    token = lexer->getNextToken();

    optional<PreIncrementation> preIncrementation = parsePreIncrementation();
    if(!preIncrementation)
    {
        optional<Variable> variable = parseVariable();
        if(!variable)
        {
            if( token.type != CLOSING_ROUND_BRACKET )
            {
                throw createError(
                    EXPECTED_EXPRESSION_AFTER_OPERATOR,
                    "expected update of variable",
                    "",
                    token
                );
            }
        }
        else
        {
            if(token.type == INCREMENTAL_OPERATOR)
            {
                loop.addPostIncrementation(&*variable, token);
                token = lexer->getNextToken();
            }
            else
            {
                optional<Assigment> assigment = parseAssigment();
                if(!assigment)
                {
                    throw createError(
                        EXPECTED_EXPRESSION_AFTER_OPERATOR,
                        "expected update of variable",
                        "",
                        token
                    );
                }
                else
                {
                    loop.addUpdateAssigment(&*variable, &*assigment);
                }
            }
        }
    }
    else
    {
        loop.addPreIncrementation(&*preIncrementation);
    }
    if( token.type != CLOSING_ROUND_BRACKET )
    {
        throw createError(
            EXPECTED_EXPRESSION_AFTER_OPERATOR,
            "expected )",
            "",
            token
        );
    }
    token = lexer->getNextToken();

    if( token.type != OPENING_BLOCK_BRACKET )
    {
        throw createError(
            EXPECTED_EXPRESSION_AFTER_OPERATOR,
            "expected { at the begining of block of code",
            "",
            token
        );
    }
    token = lexer->getNextToken();

    optional<Operation> operation;
    //pobieranie operacji
    while(token.type != CLOSING_BLOCK_BRACKET && token.type != END_OF_FILE)
    {
        operation = parseOperation();
        if(operation)
        {
            loop.addOperation(*operation);
        }
        else
        {
            throw createError(
                EXPECTED_EXPRESSION_AFTER_OPERATOR,
                "expected statement",
                "",
                token
            );
        }
    }
    if( !token.type == CLOSING_BLOCK_BRACKET )
    {
        throw createError(
            EXPECTED_EXPRESSION_AFTER_OPERATOR,
            "expected } at and of block",
            "",
            token
        );
    }
    token = lexer->getNextToken();
    //return loop;
    cout << "Przeparsowano pętlę\n";
    return loop;
}

optional<Variable> Parser::parseVariable()
{
    Variable variable;
    if( token.type != IDENTYFIER )
    {
        cout << "ERRoR oczekiwano identyfiaktora\n";
        return {};
    }
    variable.addVariableName(token);
    token = lexer->getNextToken();


    if(token.type == OPENING_SQUARE_BRACKET)
    {
        token = lexer->getNextToken();
        optional<ArithmeticExpression> arithmeticExpression = parseArithmeticExpression();
        if( !arithmeticExpression )
        {
            throw createError(
                EXPECTED_EXPRESSION_AFTER_OPERATOR,
                "expected index of array",
                "",
                token
            );
        }
        else
        {
            cout << token.type << "\n";
            if(token.type == CLOSING_SQUARE_BRACKET)
            {
                variable.addIndex(&*arithmeticExpression);
                token = lexer->getNextToken();
                cout << "Przeparsowano tablicę\n";
                return variable;
            }
            else
            {
                throw createError(
                    EXPECTED_EXPRESSION_AFTER_OPERATOR,
                    "expected ]",
                    "",
                    token
                );
            }
        }
    }
    else
    {
        cout << "Przeparsowano zmienną\n";
        return variable;
    }
    
}

optional<Assigment> Parser::parseAssigment()
{
    Assigment assigment;
    if( token.type != ASSIGMENT_OPERATOR )
    {
        cout << "ERROR oczekiwano znaku równa się\n";
        return {};
    }

    token = lexer->getNextToken();
    optional<ArithmeticExpression> arithmeticExpression = parseArithmeticExpression();
    if( !arithmeticExpression )
    {
        throw createError(
        EXPECTED_EXPRESSION_AFTER_OPERATOR,
        "expected expression after assign operator",
        "",
        token
        );
    }
    cout << "Przeparsowano przypisanie\n";
    return assigment;
}

optional<Initiation> Parser::parseInitiation()
{
    Initiation initiation;
    if(token.type != DATA_TYPE)
    {
        cout << "ERROR oczekiwoano typu danych\n";
        return {};
    }
    initiation.addDataType(token);
    token = lexer->getNextToken();

    optional<Variable> variable = parseVariable();
    if( !variable )
    {
        throw createError(
        EXPECTED_EXPRESSION_AFTER_OPERATOR,
        "expected variable in initiation",
        "",
        token
        );
    }
    initiation.addVariable(&*variable);

    optional<Assigment> assigment = parseAssigment();
    if(assigment)
    {
        cout << token.type << "\n";
        initiation.addAssigment(&*assigment);
    }
    cout << "Przeparsowano inicjację\n" << token.type << "\n";
    return initiation;
}

optional<PreIncrementation> Parser::parsePreIncrementation()
{
    PreIncrementation preIncrementation;
    if( token.type != INCREMENTAL_OPERATOR )
    {
        cout << "ERROR oczekiwano operatora inkrementacji\n";
        return {};
    }
    preIncrementation.addToken(token);
    token = lexer->getNextToken();

    optional<Variable> variable = parseVariable();
    if(!variable)
    {
        throw createError(
        EXPECTED_EXPRESSION_AFTER_OPERATOR,
        "expected variable in preincrementation",
        "",
        token
        );
        throw "oczekiwano nazwy zmiennej";
    }
    preIncrementation.addVariable(&*variable);
    cout << "Przeparsowano preinkrementację\n";
    return preIncrementation;
}

optional<Condition> Parser::parseCondition()
{
    Condition condition;
    optional<ArithmeticExpression> arithmeticExpression = parseArithmeticExpression();
    if(!arithmeticExpression)
    {
        cout << "ERROR oczekiwano wyrażenia arytmetycznego\n";
        return {};
    }
    condition.addExpression(*arithmeticExpression);
    if( token.type != RELATIONAL_OPERATOR )
    {
        throw createError(
        EXPECTED_EXPRESSION_AFTER_OPERATOR,
        "expected logical operator in condition",
        "",
        token
        );
    }
    condition.addOperator(token);
    token = lexer->getNextToken();
    arithmeticExpression = parseArithmeticExpression();
    if(!arithmeticExpression)
    {
        throw createError(
        EXPECTED_EXPRESSION_AFTER_OPERATOR,
        "expected arithmetic expression after logical operator",
        "",
        token
        );
    }
    condition.addExpression(*arithmeticExpression);
    cout << "Przeparsowano warunek\n";
    return condition;
}

optional<ArithmeticExpression> Parser::parseArithmeticExpression()
{
    ArithmeticExpression arithmeticExpression;
    optional<PrimaryExpression> primaryExpression = parsePrimaryExpression();
    if(!primaryExpression)
    {
        cout << "ERROR oczekiwano primary expression\n";
        return {};
    }
    arithmeticExpression.addPrimaryExpression(*primaryExpression);
    while(token.type == ADDITIVE_OPERATOR || token.type == MULTIPLICATIVE_OPERATOR)
    {
        arithmeticExpression.addOperator(token);
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
        arithmeticExpression.addPrimaryExpression(*primaryExpression);
    }
    cout << "Przeparsowano wyrażenie arytmetyczne\n";
    return arithmeticExpression;
}

optional<PrimaryExpression> Parser::parsePrimaryExpression()
{
    unique_ptr<PrimaryExpression> primaryExpression;
    optional<Variable> variable = parseVariable();
    if(variable)
    {
        primaryExpression.addVariable(&*variable);
        if(token.type == INCREMENTAL_OPERATOR)
        {
            cout << "Primary expression - inkrementacja\n";
            primaryExpression.addIncrementOperator(token);
            token = lexer->getNextToken();
            return primaryExpression;
        }
    }
    else if(token.type == NUMBER)
    {
        cout << "Primary expression - numer\n";
        primaryExpression.addNumber(token);
        token = lexer->getNextToken();
        return primaryExpression;
    }
    else
    {
        optional<PreIncrementation> preIncrementation = parsePreIncrementation();
        if(!preIncrementation)
        {
            cout << "ERROR oczekiwano zmiennej, numeru lub inkrementacji\n";
            return {};
        }
        primaryExpression.addPreIncrementation(&*preIncrementation);
    }
    cout << "Przeparsowano podstawowe wyrażenie\n";
    return primaryExpression;
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