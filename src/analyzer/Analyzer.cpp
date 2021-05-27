#include "../../include/analyzer/Analyzer.hpp"

using namespace std;

Analyzer::Analyzer(string path, bool isFile /*= false*/)
{
    parser = new Parser(path, isFile);
}
Analyzer::~Analyzer()
{
    delete parser;
}

bool Analyzer::isMutable(ArithmeticExpression& arithmeticExpression)
{
    return false;
}

bool Analyzer::analyze()
{
    tree = parser->parse();
    if(!tree)
        return false;
    optional<AnalizeError> error = analyzeTree(*tree);
    if(!error)
    {
        return true;
    }
    else
    {
        writeError(*error);
        return false;
    }
}

optional<AnalizeError> Analyzer::analyzeTree(ParserTree& tree)
{
    Program p = *tree.getProgram();
    return analyzeProgram(p);
}

optional<AnalizeError> Analyzer::analyzeProgram(Program& program)
{
    for(Operation* operation : program.operations)
    {
        optional<AnalizeError> error = analyzeOperation(*operation, 0);
        if(error)
        {
            return error;
        }
    }
    return {};
}

optional<AnalizeError> Analyzer::analyzeOperation(Operation& operation, int level)
{
    switch (operation.getOper().index())
    {
        case 0:
        {
            // pętla
            return analyzeLoop(*get<Loop*>(operation.getOper()), level + 1);
            break;
        }
        case 1:
        {
            // przypisanie
            auto p = get<pair<Variable*, Assigment*> >(operation.getOper());
            optional<AnalizeError> error = checkVariable(*p.first);
            if(error)
                return error;
            return checkAssigment(*p.second);
            break;
        }
        case 2:
        {
            // post inkrementacja/dekrementacja
            auto p = get<pair<Variable*, Token> >(operation.getOper());
            return checkVariable(*p.first);
            break;
        }
        case 3:
        {
            // inicjacja zmiennych
            return analyzeInitiation(*get<Initiation*>(operation.getOper()), level);
            break;
        }
        case 4:
        {
            // preinkrementacja
            return checkPreinkrementation(*get<PreIncrementation*>(operation.getOper()));
            break;
        }
    }
    Token token;
    return createError(
        EXPECTED_EXPRESSION_AFTER_OPERATOR,
        "Unknown error",
        "",
        token
    );
}

optional<AnalizeError> Analyzer::analyzeLoop(Loop& loop, int level)
{
    optional<AnalizeError> error;
    if(loop.isInitiation())
    {

        switch(loop.getInitiation().index())
        {
            case 0:
                error = analyzeInitiation(*get<Initiation*>(loop.getInitiation()), level);
                if(error)
                {
                    return error;
                }
                break;
            case 1:
                auto initiation = get<pair<Variable*, Assigment*> >(loop.getInitiation());
                error = checkVariable(*initiation.first);
                if(error)
                {
                    return error;
                }
                error = checkAssigment(*initiation.second);
                if(error)
                {
                    return error;
                }
                break;
        }
    }
    error = checkCondition(*loop.getCondition());
    if(error)
    {
        return error;
    }
    if(loop.isBoolUpdate())
    {
        auto update = loop.getUpdate();
        switch(update.index())
        {
            case 0:
            {
                PreIncrementation pre = *get<PreIncrementation*>(update);
                error = checkPreinkrementation(pre);
                if(error)
                {
                    return error;
                }
                break;
            }
            case 1:
            {
                pair<Variable*, Assigment*> p = get<pair<Variable*, Assigment*> >(update);
                error = checkVariable(*p.first);
                if(error)
                {
                    return error;
                }
                error = checkAssigment(*p.second);
                if(error)
                {
                    return error;
                }
                break;
            }
            case 2:
            {
                pair<Variable*, Token> p = get<pair<Variable*, Token> > (update);
                error = checkVariable(*p.first);
                if(error)
                {
                    return error;
                }
                break;
            }
        }
    }
    for(auto operation : loop.operations)
    {
        error = analyzeOperation(*operation, level + 1);
        if(error)
        {
            return error;
        }
    }
    removeLevel(level);
    return {};
}

optional<AnalizeError> Analyzer::checkCondition(Condition& condition)
{
    auto expressions = condition.expressions;
    optional<AnalizeError> analizeError = checkArithmeticExpression(*expressions[0]);
    if(analizeError)
        return analizeError;
    return checkArithmeticExpression(*expressions[1]);
}

optional<AnalizeError> Analyzer::analyzeInitiation(Initiation& initiation, int level)
{
    optional<AnalizeError> analizeError = addVariable(initiation.getVariable()->getVariableName(), bool(initiation.getVariable()->getIndex()), level);
    if(analizeError)
    {
        return analizeError;
    }
    if(initiation.getVariable()->getIndex())
    {
        analizeError = checkArithmeticExpression(*initiation.getVariable()->getIndex());
        if(analizeError)
        {
            return analizeError;
        }
    }
    if(initiation.hasAssigment() && initiation.getVariable()->getIndex())
    {
        return createError(
            EXPECTED_EXPRESSION_AFTER_OPERATOR,
            "Cannot cast to array type",
            "",
            initiation.getVariable()->getVariableName()
        );
    }
    if(initiation.hasAssigment())
        return checkAssigment(*initiation.getAssigment());
    return {};
}

optional<AnalizeError> Analyzer::checkAssigment(Assigment& assigment)
{
    return checkArithmeticExpression(*assigment.getArithmeticExpression());
}

optional<AnalizeError> Analyzer::checkArithmeticExpression(ArithmeticExpression& arithmeticExpression)
{
    for(auto primaryExpression : arithmeticExpression.primaryExpressions)
    {
        optional<AnalizeError> analyzeError = checkPrimaryExpression(*primaryExpression);
        if(analyzeError)
        {
            return analyzeError;
        }
    }
    return {};
}

optional<AnalizeError> Analyzer::checkPrimaryExpression(PrimaryExpression& primaryExpression)
{
    switch(primaryExpression.getPExpression().index())
    {
        case 0:
        {
            // variable
            return checkVariable(*get<Variable*>(primaryExpression.getPExpression()));
            break;
        }
        case 1:
        {
            // post inkrementacja/dekrementacja
            auto p = get<pair<Variable*, Token> >(primaryExpression.getPExpression());
            return checkVariable(*p.first);
            break;
        }
        case 2:
        {
            // stala (numer)
            return {};
            break;
        }
        case 3:
        {
            // preinkrementacja/dekrementacja
            return checkPreinkrementation(*get<PreIncrementation*>(primaryExpression.getPExpression()));
            break;
        }
    }
    Token token;
    return createError(
        EXPECTED_EXPRESSION_AFTER_OPERATOR,
        "Unknown error",
        "",
        token
    );
}

optional<AnalizeError> Analyzer::checkPreinkrementation(PreIncrementation& preincrementation)
{
    auto var = preincrementation.getVariable();
    return checkVariable(*var);
}


optional<AnalizeError> Analyzer::checkVariable(Variable& variable)
{
    string name = get<string>(variable.getVariableName().value);
    bool isTable = bool(variable.getIndex());
    struct VarInfo varInfo
    {
        "",
        -1,
        false
    };
    for(auto var : varMap)
    {
        if(var.name == name)
        {
            if(var.level > varInfo.level)
            {
                varInfo = var;
            }
        }
    }
    if( varInfo.name == name && varInfo.isTable == isTable )
    {
        if(isTable)
            return checkArithmeticExpression(*variable.getIndex());
        return {};
    }
    string message = "Undefined variable name " + name;
    if(isTable)
        message += "[]"; 

    return createError(
        EXPECTED_EXPRESSION_AFTER_OPERATOR,
        message,
        "",
        variable.getVariableName()
    );
}

optional<AnalizeError> Analyzer::addVariable(Token token, bool isTable, int level)
{
    string name = get<string>(token.value);
    for(auto var : varMap)
    {
        if(var.name == name && var.level >= level)
        {
            return createError(
                EXPECTED_EXPRESSION_AFTER_OPERATOR,
                "Variable " + name + " already exist",
                "",
                token
            );

        }
    }
    struct VarInfo varInfo
    {
        name,
        level,
        isTable
    };
    varMap.push_back(varInfo);
    return {};
}

void Analyzer::removeLevel(int level)
{
    for(auto it = varMap.begin(); it != varMap.end(); ++it)
    {
        if((*it).level >= level)
        {
            varMap.erase(it, it + 1);
            --it;
        }
    }
}

AnalizeError Analyzer::createError(ErrorType type, string message, string codePart, Token token)
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

void Analyzer::writeError(AnalizeError error)
{
    cout << "ANALYZER ERROR: line " + to_string(error.line) + 
    "; sign " + to_string(error.signNumber) + "\n";
    cout << error.message << "\n";
}

optional<ParserTree> Analyzer::getTree()
{
    return tree;
}