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
            operation = parseOperation(program);
        }
        catch(AnalizeError e)
        {
            delete program;
            throw e;
        }
        if(operation)
        {
            program->addOperation(*operation);
        }
        else
        {
            delete program;
            return {};
        }
    }
    return program;
}

optional<Operation*> Parser::parseOperation(Node* parent)
{
    Operation* operation = new Operation();
    operation->setParent(parent);
    optional<Loop*> loop;
    try
    {
        loop = parseLoop(operation);
    }
    catch(AnalizeError e)
    {
        delete operation;
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
        variable = parseVariable(operation);
    }
    catch(AnalizeError e)
    {
        delete operation;
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
                assigment = parseAssigment(operation);
            }
            catch(AnalizeError e)
            {
                delete operation;
                throw e;
            }
            if(assigment)
            {
                operation->addAssigment(*variable, *assigment);
            }
            else
            {
                delete operation;
                throw createError(
                    EXPECTED_EXPRESSION_AFTER_OPERATOR,
                    "expected operation",
                    "",
                    token
                );
            }
        }
        if(token.type != SEMICOLON)
        {
            delete operation;
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
        initioation = parseInitiation(operation);
    }
    catch(AnalizeError e)
    {
        delete operation;
        throw e;
    }
    if(initioation)
    {
        operation->addInitiation(*initioation);
        if(token.type != SEMICOLON)
        {
            delete operation;
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
        preIncrementation = parsePreIncrementation(operation);
    }
    catch(AnalizeError e)
    {
        delete operation;
        throw e;
    }
    if(preIncrementation)
    {
        operation->addPreIncrementation(*preIncrementation);
        if(token.type != SEMICOLON)
        {
            delete operation;
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
    delete operation;
    return {};
}

optional<Loop*> Parser::parseLoop(Node* parent)
{
    Loop* loop = new Loop();
    loop->setParent(parent);
    if( !(token.type == KEY_WORD && std::get<std::string>(token.value) == "for") )
    {    
        delete loop;
        return {};
    }
    token = lexer->getNextToken();
    if( token.type != OPENING_ROUND_BRACKET )
    {
        delete loop;
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
        initiation = parseInitiation(loop);
    }
    catch(AnalizeError e)
    {
        delete loop;
        throw e;
    }
    if( !initiation )
    {
        optional<Variable*> variable;
        try
        {
            variable = parseVariable(loop);
        }
        catch(AnalizeError e)
        {
            delete loop;
            throw e;
        }
        if( ! variable )
        {
            if( token.type != SEMICOLON )
            {
                delete loop;
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
                assigment = parseAssigment(loop);
            }
            catch(AnalizeError e)
            {
                delete loop;
                throw e;
            }
            if( !assigment )
            {
                delete loop;
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
        delete loop;
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
        condition = parseCondition(loop);
    }
    catch(AnalizeError e)
    {
        delete loop;
        throw e;
    }
    
    if( !condition )
    {
        if( token.type != SEMICOLON )
        {
            delete loop;
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
        delete loop;
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
        preIncrementation = parsePreIncrementation(loop);
    }
    catch(AnalizeError e)
    {
        delete loop;
        throw e;
    }
    if(!preIncrementation)
    {
        optional<Variable*> variable;
        try
        {
            variable = parseVariable(loop);
        }
        catch(AnalizeError e)
        {
            delete loop;
            throw e;
        }
        if(!variable)
        {
            if( token.type != CLOSING_ROUND_BRACKET )
            {
                delete loop;
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
                    assigment = parseAssigment(loop);
                }
                catch(AnalizeError e)
                {
                    delete loop;
                    throw e;
                }
                if(!assigment)
                {
                    delete loop;
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
        delete loop;
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
        delete loop;
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
            operation = parseOperation(loop);
        }
        catch(AnalizeError e)
        {
            delete loop;
            throw e;
        }
        if(operation)
        {
            loop->addOperation(*operation);
        }
        else
        {
            delete loop;
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
        delete loop;
        throw createError(
            EXPECTED_EXPRESSION_AFTER_OPERATOR,
            "expected } at and of block",
            "",
            token
        );
    }
    token = lexer->getNextToken();
    return loop;
}

optional<Variable*> Parser::parseVariable(Node* parent)
{
    Variable* variable = new Variable();
    variable->setParent(parent);
    if( token.type != IDENTYFIER )
    {
        delete variable;
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
            arithmeticExpression = parseArithmeticExpression(variable);
        }
        catch(AnalizeError e)
        {
            delete variable;
            throw e;
        }
        if( !arithmeticExpression )
        {
            delete variable;
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
                delete variable;
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

optional<Assigment*> Parser::parseAssigment(Node* parent)
{
    Assigment* assigment = new Assigment();
    assigment->setParent(parent);
    if( token.type != ASSIGMENT_OPERATOR )
    {
        delete assigment;
        return {};
    }

    token = lexer->getNextToken();
    optional<ArithmeticExpression*> arithmeticExpression;
    try
    {
        arithmeticExpression = parseArithmeticExpression(assigment);
    }
    catch(AnalizeError e)
    {
        delete assigment;
        throw e;
    }
    if( !arithmeticExpression )
    {
        delete assigment;
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

optional<Initiation*> Parser::parseInitiation(Node* parent)
{
    Initiation* initiation = new Initiation();
    initiation->setParent(parent);
    if(token.type != DATA_TYPE)
    {
        delete initiation;
        return {};
    }
    initiation->addDataType(token);
    token = lexer->getNextToken();

    optional<Variable*> variable;
    try
    {
        variable = parseVariable(initiation);
    }
    catch(AnalizeError e)
    {
        delete initiation;
        throw e;
    }
    if( !variable )
    {
        delete initiation;
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
        assigment = parseAssigment(initiation);
    }
    catch(AnalizeError e)
    {
        delete initiation;
        throw e;
    }
    if(assigment)
    {
        initiation->addAssigment(*assigment);
    }
    return initiation;
}

optional<PreIncrementation*> Parser::parsePreIncrementation(Node* parent)
{
    PreIncrementation* preIncrementation = new PreIncrementation();
    preIncrementation->setParent(parent);
    if( token.type != INCREMENTAL_OPERATOR && token.type != DECREMENTAL_OPERATOR )
    {
        delete preIncrementation;
        return {};
    }
    preIncrementation->addToken(token);
    token = lexer->getNextToken();

    optional<Variable*> variable;
    try
    {
        variable = parseVariable(preIncrementation);
    }
    catch(AnalizeError e)
    {
        delete preIncrementation;
        throw e;
    }
    if(!variable)
    {
        delete preIncrementation;
        throw createError(
        EXPECTED_EXPRESSION_AFTER_OPERATOR,
        "expected variable in preincrementation",
        "",
        token
        );
    }
    preIncrementation->addVariable(*variable);
    return preIncrementation;
}

optional<Condition*> Parser::parseCondition(Node* parent)
{
    Condition* condition = new Condition();
    condition->setParent(parent);
    optional<ArithmeticExpression*> arithmeticExpression;
    try
    {
        arithmeticExpression = parseArithmeticExpression(condition);
    }
    catch(AnalizeError e)
    {
        delete condition;
        throw e;
    }
    
    if(!arithmeticExpression)
    {
        delete condition;
        return {};
    }
    condition->addExpression(*arithmeticExpression);
    if( token.type != RELATIONAL_OPERATOR )
    {
        delete condition;
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
        arithmeticExpression = parseArithmeticExpression(condition);
    }
    catch(AnalizeError e)
    {
        delete condition;
        throw e;
    }
    if(!arithmeticExpression)
    {
        delete condition;
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

optional<ArithmeticExpression*> Parser::parseArithmeticExpression(Node* parent)
{
    ArithmeticExpression* arithmeticExpression = new ArithmeticExpression();
    arithmeticExpression->setParent(parent);
    optional<PrimaryExpression*> primaryExpression;
    try
    {
        primaryExpression = parsePrimaryExpression(arithmeticExpression);
    }
    catch(AnalizeError e)
    {
        delete arithmeticExpression;
        throw e;
    }
    
    if(!primaryExpression)
    {
        delete arithmeticExpression;
        return {};
    }
    arithmeticExpression->addPrimaryExpression(*primaryExpression);
    while(token.type == ADDITIVE_OPERATOR || token.type == MULTIPLICATIVE_OPERATOR)
    {
        arithmeticExpression->addOperator(token);
        token = lexer->getNextToken();
        try
        {
            primaryExpression = parsePrimaryExpression(arithmeticExpression);
        }
        catch(AnalizeError e)
        {
            delete arithmeticExpression;
            throw e;
        }
        

        if(!primaryExpression)
        {
            delete arithmeticExpression;
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

optional<PrimaryExpression*> Parser::parsePrimaryExpression(Node* parent)
{
    PrimaryExpression* primaryExpression = new PrimaryExpression();
    primaryExpression->setParent(parent);
    optional<Variable*> variable;
    try
    {
        variable = parseVariable(primaryExpression);
    }
    catch(AnalizeError e)   
    {
        delete primaryExpression;
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
            preIncrementation = parsePreIncrementation(primaryExpression);
        }
        catch(AnalizeError e)   
        {
            delete primaryExpression;
            throw e;
        }
        if(!preIncrementation)
        {
            delete primaryExpression;
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
    "; sign " + to_string(error.signNumber - 1) + "\n";
    cout << error.message << "\n";
}