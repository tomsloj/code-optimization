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

    optional<Program*> program;
    try
    {
        program = parseProgram();
    }
    catch(AnalizeError e)
    {
        writeError(e);
    }
    if(program)
    {
        return ParserTree(**program);
    }
    else
        return {};
}

optional<Program*> Parser::parseProgram()
{
    Program* program = new Program();
    while(token.type != END_OF_FILE)
    {
        optional<Operation*> operation;
        try
        {
            operation = parseOperation();
        }
        catch(AnalizeError e)
        {
            throw e;
        }
        if(operation)
        {
            program->addOperation(*operation);
        }
        else
            return {};
    }
    return program;
}

optional<Operation*> Parser::parseOperation()
{
    Operation* operation = new Operation();
    optional<Loop*> loop;
    try
    {
        loop = parseLoop();
    }
    catch(AnalizeError e)
    {
        throw e;
    }
    if(loop)
    {
        operation->addLoop(*loop);
        return operation;
    }
    optional<Variable*> variable;
    try
    {
        variable = parseVariable();
    }
    catch(AnalizeError e)
    {
        throw e;
    }
    if(variable)
    {
        if(token.type == INCREMENTAL_OPERATOR || token.type == DECREMENTAL_OPERATOR)
        {
            operation->addPostIncrementation(*variable, token);
            token = lexer->getNextToken();
        }
        else
        {
            optional<Assigment*> assigment;
            try
            {
                assigment = parseAssigment();
            }
            catch(AnalizeError e)
            {
                throw e;
            }
            if(assigment)
            {
                operation->addAssigment(*variable, *assigment);
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
        return operation;
    }
    
    optional<Initiation*> initioation;
    try
    {
        initioation = parseInitiation();
    }
    catch(AnalizeError e)
    {
        throw e;
    }
    if(initioation)
    {
        operation->addInitiation(*initioation);
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
        return operation;
    }

    optional<PreIncrementation*> preIncrementation;
    try
    {
        preIncrementation = parsePreIncrementation();
    }
    catch(AnalizeError e)
    {
        throw e;
    }
    if(preIncrementation)
    {
        operation->addPreIncrementation(*preIncrementation);
        if(token.type != SEMICOLON)
        {
            throw createError(
                EXPECTED_EXPRESSION_AFTER_OPERATOR,
                "expected semicolon after operation",
                "",
                token
            );
        }
        return operation;
    }

    if(token.type == SEMICOLON)
    {
        token = lexer->getNextToken();
        return operation;
    }
    return {};
}

optional<Loop*> Parser::parseLoop()
{
    Loop* loop = new Loop();
    if( !(token.type == KEY_WORD && std::get<std::string>(token.value) == "for") )
    {    
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
    optional<Initiation*> initiation;
    try
    {
        initiation = parseInitiation();
    }
    catch(AnalizeError e)
    {
        throw e;
    }
    if( !initiation )
    {
        optional<Variable*> variable;
        try
        {
            variable = parseVariable();
        }
        catch(AnalizeError e)
        {
            throw e;
        }
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
            optional<Assigment*> assigment;
            try
            {
                assigment = parseAssigment();
            }
            catch(AnalizeError e)
            {
                throw e;
            }
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
               loop->addInitAssigment(*variable, *assigment);
            }
        }
    }
    else
    {
        loop->addInitiation(*initiation);
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

    optional<Condition*> condition;
    try
    {
        condition = parseCondition();
    }
    catch(AnalizeError e)
    {
        throw e;
    }
    
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
    loop->addCondition(*condition);
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

    optional<PreIncrementation*> preIncrementation;
    try
    {
        preIncrementation = parsePreIncrementation();
    }
    catch(AnalizeError e)
    {
        throw e;
    }
    if(!preIncrementation)
    {
        optional<Variable*> variable;
        try
        {
            variable = parseVariable();
        }
        catch(AnalizeError e)
        {
            throw e;
        }
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
            if(token.type == INCREMENTAL_OPERATOR || token.type == DECREMENTAL_OPERATOR)
            {
                loop->addPostIncrementation(*variable, token);
                token = lexer->getNextToken();
            }
            else
            {
                optional<Assigment*> assigment;
                try
                {
                    assigment = parseAssigment();
                }
                catch(AnalizeError e)
                {
                    throw e;
                }
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
                    loop->addUpdateAssigment(*variable, *assigment);
                }
            }
        }
    }
    else
    {
        loop->addPreIncrementation(*preIncrementation);
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

    optional<Operation*> operation;
    //pobieranie operacji
    while(token.type != CLOSING_BLOCK_BRACKET && token.type != END_OF_FILE)
    {
        try
        {
            operation = parseOperation();
        }
        catch(AnalizeError e)
        {
            throw e;
        }
        if(operation)
        {
            loop->addOperation(*operation);
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
    return loop;
}

optional<Variable*> Parser::parseVariable()
{
    Variable* variable = new Variable();
    if( token.type != IDENTYFIER )
    {
        return {};
    }
    variable->addVariableName(token);
    token = lexer->getNextToken();


    if(token.type == OPENING_SQUARE_BRACKET)
    {
        token = lexer->getNextToken();
        optional<ArithmeticExpression*> arithmeticExpression;
        try
        {
            arithmeticExpression = parseArithmeticExpression();
        }
        catch(AnalizeError e)
        {
            throw e;
        }
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
            if(token.type == CLOSING_SQUARE_BRACKET)
            {
                variable->addIndex(*arithmeticExpression);
                token = lexer->getNextToken();
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
        return variable;
    }
    
}

optional<Assigment*> Parser::parseAssigment()
{
    Assigment* assigment = new Assigment();
    if( token.type != ASSIGMENT_OPERATOR )
    {
        return {};
    }

    token = lexer->getNextToken();
    optional<ArithmeticExpression*> arithmeticExpression;
    try
    {
        arithmeticExpression = parseArithmeticExpression();
    }
    catch(AnalizeError e)
    {
        throw e;
    }
    if( !arithmeticExpression )
    {
        throw createError(
        EXPECTED_EXPRESSION_AFTER_OPERATOR,
        "expected expression after assign operator",
        "",
        token
        );
    }
    assigment->addArithmeticExpression(*arithmeticExpression);
    return assigment;
}

optional<Initiation*> Parser::parseInitiation()
{
    Initiation* initiation = new Initiation();
    if(token.type != DATA_TYPE)
    {
        return {};
    }
    initiation->addDataType(token);
    token = lexer->getNextToken();

    optional<Variable*> variable;
    try
    {
        variable = parseVariable();
    }
    catch(AnalizeError e)
    {
        throw e;
    }
    if( !variable )
    {
        throw createError(
        EXPECTED_EXPRESSION_AFTER_OPERATOR,
        "expected variable in initiation",
        "",
        token
        );
    }
    initiation->addVariable(*variable);

    optional<Assigment*> assigment;
    try
    {
        assigment = parseAssigment();
    }
    catch(AnalizeError e)
    {
        throw e;
    }
    if(assigment)
    {
        initiation->addAssigment(*assigment);
    }
    return initiation;
}

optional<PreIncrementation*> Parser::parsePreIncrementation()
{
    PreIncrementation* preIncrementation = new PreIncrementation();
    if( token.type != INCREMENTAL_OPERATOR && token.type != DECREMENTAL_OPERATOR )
    {
        return {};
    }
    preIncrementation->addToken(token);
    token = lexer->getNextToken();

    optional<Variable*> variable;
    try
    {
        variable = parseVariable();
    }
    catch(AnalizeError e)
    {
        throw e;
    }
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
    preIncrementation->addVariable(*variable);
    return preIncrementation;
}

optional<Condition*> Parser::parseCondition()
{
    Condition* condition = new Condition();
    optional<ArithmeticExpression*> arithmeticExpression;
    try
    {
        arithmeticExpression = parseArithmeticExpression();
    }
    catch(AnalizeError e)
    {
        throw e;
    }
    
    if(!arithmeticExpression)
    {
        return {};
    }
    condition->addExpression(*arithmeticExpression);
    if( token.type != RELATIONAL_OPERATOR )
    {
        throw createError(
        EXPECTED_EXPRESSION_AFTER_OPERATOR,
        "expected logical operator in condition",
        "",
        token
        );
    }
    condition->addOperator(token);
    token = lexer->getNextToken();
    try
    {
        arithmeticExpression = parseArithmeticExpression();
    }
    catch(AnalizeError e)
    {
        throw e;
    }
    if(!arithmeticExpression)
    {
        throw createError(
        EXPECTED_EXPRESSION_AFTER_OPERATOR,
        "expected arithmetic expression after logical operator",
        "",
        token
        );
    }
    condition->addExpression(*arithmeticExpression);
    return condition;
}

optional<ArithmeticExpression*> Parser::parseArithmeticExpression()
{
    ArithmeticExpression* arithmeticExpression = new ArithmeticExpression();
    optional<PrimaryExpression*> primaryExpression;
    try
    {
        primaryExpression = parsePrimaryExpression();
    }
    catch(AnalizeError e)
    {
        throw e;
    }
    
    if(!primaryExpression)
    {
        return {};
    }
    arithmeticExpression->addPrimaryExpression(*primaryExpression);
    while(token.type == ADDITIVE_OPERATOR || token.type == MULTIPLICATIVE_OPERATOR)
    {
        arithmeticExpression->addOperator(token);
        token = lexer->getNextToken();
        try
        {
            primaryExpression = parsePrimaryExpression();
        }
        catch(AnalizeError e)
        {
            throw e;
        }
        

        if(!primaryExpression)
        {

            throw createError(
                EXPECTED_EXPRESSION_AFTER_OPERATOR,
                "expected expression after operator",
                "",
                token
                );
        }
        arithmeticExpression->addPrimaryExpression(*primaryExpression);
    }
    return arithmeticExpression;
}

optional<PrimaryExpression*> Parser::parsePrimaryExpression()
{
    PrimaryExpression* primaryExpression = new PrimaryExpression();
    optional<Variable*> variable;
    try
    {
        variable = parseVariable();
    }
    catch(AnalizeError e)   
    {
        throw e;
    }    
    if(variable)
    {
        primaryExpression->addVariable(*variable);
        if(token.type == INCREMENTAL_OPERATOR || token.type == DECREMENTAL_OPERATOR)
        {
            primaryExpression->addIncrementOperator(token);
            token = lexer->getNextToken();
            return primaryExpression;
        }
    }
    else if(token.type == NUMBER)
    {
        primaryExpression->addNumber(token);
        token = lexer->getNextToken();
        return primaryExpression;
    }
    else
    {
        optional<PreIncrementation*> preIncrementation;
        try
        {
            preIncrementation = parsePreIncrementation();
        }
        catch(AnalizeError e)   
        {
            throw e;
        }
        if(!preIncrementation)
        {
            return {};
        }
        primaryExpression->addPreIncrementation(*preIncrementation);
    }
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

void Parser::writeError(AnalizeError error)
{
    cout << "PARSER ERROR: line " + to_string(error.line) + 
    "; sign " + to_string(error.signNumber) + "\n";
    cout << error.message << "\n";
}