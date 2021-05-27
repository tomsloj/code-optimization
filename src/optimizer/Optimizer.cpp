#include "../../include/optimizer/Optimizer.hpp"

using namespace std;

Optimizer::Optimizer(string path, bool isFile /*= false*/)
{
    analyzer = new Analyzer(path, isFile);
}

Optimizer::~Optimizer()
{
    delete analyzer;
}

ParserTree* Optimizer::getTree()
{
    return tree;
}

bool Optimizer::optimize()
{
    if(!analyzer->analyze())
    {
        cout << "Cannot optimize - error in code\n";
        return false;
    }

    if(!analyzer->getTree())
        return false;
    
    tree = new ParserTree(*analyzer->getTree());
    optimizeTree(tree);
    return true;
}

void Optimizer::optimizeTree(ParserTree* tree)
{
    Program* p = tree->getProgram();
    return optimizeProgram(*p);
}

void Optimizer::optimizeProgram(Program& program)
{
    for(int i = 0; i < program.operations.size(); ++i)
    {
        Operation* operation = program.operations[i];
        variant< bool, vector<Operation*> > var = optimizeOperation(*operation, 0);
        if(var.index() == 1)
        {
            vector<Operation*> vec = get< vector<Operation*> >(var);
            program.operations.insert(program.operations.begin() + i, vec.begin(), vec.end());
            i += vec.size();
        }
    }
}

variant< bool, vector<Operation*> > Optimizer::optimizeOperation(Operation& operation, int level)
{
    switch (operation.getOper().index())
    {
        case 0:
        {
            // pętla
            vector<struct VarDetails> varsCopy = varMap;
            for(auto var = varMap.begin(); var != varMap.end(); ++var)
            {
                var->alreadyUsed = false;
            }
            vector<Operation*> operations = optimizeLoop(*get<Loop*>(operation.getOper()), level + 1);
            for(auto i = 0; i < varsCopy.size(); ++i)
            {
                varsCopy[i].alreadyUsed = varsCopy[i].alreadyUsed || varMap[i].alreadyUsed;
            }
            varMap = varsCopy;
            return operations;
            break;
        }
        case 1:
        {
            // przypisanie
            auto p = get<pair<Variable*, Assigment*> >(operation.getOper());
            bool used = isUsed(*p.first);
            int varLevel = getVariableLevel(*p.first);
            if(signAssigment(*p.second) && !used && varLevel < level - 1)
            {
                return true;
            }
            else
            {
                signVariable(*p.first);
                return false;
            }
            break;
        }
        case 2:
        {
            // post inkrementacja/dekrementacja
            auto p = get<pair<Variable*, Token> >(operation.getOper());
            signVariable(*p.first);
            return false;
            break;
        }
        case 3:
        {
            // inicjacja zmiennych
            return signInitiation(*get<Initiation*>(operation.getOper()), level);
            break;
        }
        case 4:
        {
            // preinkrementacja
            signPreinkrementation(*get<PreIncrementation*>(operation.getOper()));
            return false;
            break;
        }
        default:
            return false;
            break;
    }
}

std::vector<Operation*> Optimizer::optimizeLoop(Loop& loop, int level)
{
    if(loop.isInitiation())
    {
        switch(loop.getInitiation().index())
        {
            case 0:
                signInitiation(*get<Initiation*>(loop.getInitiation()), level);
                break;
            case 1:
                auto initiation = get<pair<Variable*, Assigment*> >(loop.getInitiation());
                signVariable(*initiation.first);
                signAssigment(*initiation.second);
                break;
        }
    }
    signCondition(*loop.getCondition());
    if(loop.isBoolUpdate())
    {
        auto update = loop.getUpdate();
        switch(update.index())
        {
            case 0:
            {
                PreIncrementation pre = *get<PreIncrementation*>(update);
                signPreinkrementation(pre);
                break;
            }
            case 1:
            {
                pair<Variable*, Assigment*> p = get<pair<Variable*, Assigment*> >(update);
                signVariable(*p.first);
                signAssigment(*p.second);
                break;
            }
            case 2:
            {
                pair<Variable*, Token> p = get<pair<Variable*, Token> > (update);
                signVariable(*p.first);
                break;
            }
        }
    }
    std::vector<Operation*> operations;
    
    for(int i = 0; i < loop.operations.size(); ++i)
    {
        auto operation = loop.operations.begin() + i;
        variant< bool, vector<Operation*> > vec = optimizeOperation(**operation, level + 1);
        if(vec.index() == 0 && get<bool>(vec))
        {
            operations.push_back(*operation);
            loop.operations.erase(operation, operation + 1);
            operation--;
            i--;
        }
        // analizujemy operacje z wyższego poziomu
        else if(vec.index() == 1)
        {
            vector<Operation*> otherLevelOperations = get< vector<Operation*> >(vec);
            for(auto oper = otherLevelOperations.begin(); oper != otherLevelOperations.end(); ++oper)
            {
                variant< bool, vector<Operation*> > var = optimizeOperation(**oper, level + 1);
                if(var.index() == 0 && get<bool>(var))
                {
                    operations.push_back(*oper);
                    otherLevelOperations.erase(oper, oper + 1);
                    oper--;
                }
            }
            //dokładamy operacje
            i += otherLevelOperations.size();
            loop.operations.insert(operation, otherLevelOperations.begin(), otherLevelOperations.end());
        }
    }
    removeLevel(level);
    return operations;
}

void Optimizer::signCondition(Condition& condition)
{
    auto expressions = condition.expressions;
    signArithmeticExpression(*expressions[0]);
    return signArithmeticExpression(*expressions[1]);
}

bool Optimizer::signInitiation(Initiation& initiation, int level)
{
    addVariable(initiation.getVariable()->getVariableName(), bool(initiation.getVariable()->getIndex()), level);
    if(initiation.hasAssigment())
        signAssigment(*initiation.getAssigment());
    return false;
    // TODO sprawdzenie czy można przenieść inicjację
}

bool Optimizer::signAssigment(Assigment& assigment)
{
    signArithmeticExpression(*assigment.getArithmeticExpression());
    return !isMutable(*assigment.getArithmeticExpression());
}

void Optimizer::signArithmeticExpression(ArithmeticExpression& arithmeticExpression)
{
    for(auto primaryExpression : arithmeticExpression.primaryExpressions)
    {
        signPrimaryExpression(*primaryExpression);
    }
}

void Optimizer::signPrimaryExpression(PrimaryExpression& primaryExpression)
{
    switch(primaryExpression.getPExpression().index())
    {
        case 0:
        {
            // variable
            return signVariable(*get<Variable*>(primaryExpression.getPExpression()));
            break;
        }
        case 1:
        {
            // post inkrementacja/dekrementacja
            auto p = get<pair<Variable*, Token> >(primaryExpression.getPExpression());
            return signVariable(*p.first);
            break;
        }
        case 2:
        {
            // stala (numer)
            break;
        }
        case 3:
        {
            // preinkrementacja/dekrementacja
            return signPreinkrementation(*get<PreIncrementation*>(primaryExpression.getPExpression()));
            break;
        }
    }
}

void Optimizer::signPreinkrementation(PreIncrementation& preincrementation)
{
    auto var = preincrementation.getVariable();
    return signVariable(*var);
}

void Optimizer::signVariable(Variable& variable)
{
    string name = get<string>(variable.getVariableName().value);
    bool isTable = bool(variable.getIndex());
    struct VarDetails varInfo
    {
        "",
        -1,
        false
    }; 
    vector<struct VarDetails>::iterator it;
    for(auto var = varMap.begin(); var != varMap.end(); ++var)
    {
        if(var->name == name)
        {
            if(var->level > varInfo.level)
            {
                varInfo = *var;
                it = var;
            }
        }
    }
    it->alreadyUsed = true;
    if(variable.getIndex())
    {
        signArithmeticExpression(*variable.getIndex());
    }
}

void Optimizer::addVariable(Token token, bool isTable, int level)
{
    string name = get<string>(token.value);
    struct VarDetails varInfo
    {
        name,
        level,
        false
    };
    varMap.push_back(varInfo);
}

void Optimizer::removeLevel(int level)
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

bool Optimizer::isMutable(ArithmeticExpression& arithmeticExpression)
{
    for( auto pExpression : arithmeticExpression.primaryExpressions)
    {
        if( isMutable(*pExpression) )
            return true;
    }
    return false;
}

bool Optimizer::isMutable(PrimaryExpression& primaryExpression)
{
    variant<Variable*, pair<Variable*, Token>, Token, PreIncrementation*> pExpression = primaryExpression.getPExpression();
    if(pExpression.index() == 2)
        if(get<Token>(pExpression).type == NUMBER)
            return false;
    return true;
}

bool Optimizer::isUsed(Variable& variable)
{
    string name = get<string>(variable.getVariableName().value);
    struct VarDetails varInfo
    {
        "",
        -1,
        false
    }; 
    vector<struct VarDetails>::iterator it;
    for(auto var = varMap.begin(); var != varMap.end(); ++var)
    {
        if(var->name == name)
        {
            if(var->level > varInfo.level)
            {
                varInfo = *var;
                it = var;
            }
        }
    }
    return it->alreadyUsed;
}

int Optimizer::getVariableLevel(Variable& variable)
{
    string name = get<string>(variable.getVariableName().value);
    struct VarDetails varInfo
    {
        "",
        -1,
        false
    }; 
    vector<struct VarDetails>::iterator it;
    for(auto var = varMap.begin(); var != varMap.end(); ++var)
    {
        if(var->name == name)
        {
            if(var->level > varInfo.level)
            {
                varInfo = *var;
                it = var;
            }
        }
    }
    return it->level;
}

AnalizeError Optimizer::createError(ErrorType type, string message, string codePart, Token token)
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

void Optimizer::writeError(AnalizeError error)
{
    cout << "ANALYZER ERROR: line " + to_string(error.line) + 
    "; sign " + to_string(error.signNumber) + "\n";
    cout << error.message << "\n";
}

