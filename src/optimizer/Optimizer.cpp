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
        tree = NULL;
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
    multiset<string> usedNames;
    multiset<string> usedNamesDownLevel;
    for(int i = 0; i < program.operations.size(); ++i)
    {
        Operation* operation = program.operations[i];
        multiset<string> nextLevelUsed = getUsed(*operation);
        for(auto name : nextLevelUsed)
        {
            usedNames.insert(name);
        }
    }

    for(int i = 0; i < program.operations.size(); ++i)
    {
        Operation* operation = program.operations[i];
        variant< bool, vector<Operation*> > var = optimizeOperation(*operation, 0, usedNamesDownLevel, usedNames);
        if(var.index() == 1)
        {
            vector<Operation*> vec = get< vector<Operation*> >(var);
            program.operations.insert(program.operations.begin() + i, vec.begin(), vec.end());
            i += vec.size();
        }
    }
}

variant< bool, vector<Operation*> > Optimizer::optimizeOperation(Operation& operation, int level, multiset<string> usedNamesDownLevel, multiset<string> usedThisLevel)
{
    switch (operation.getOper().index())
    {
        case 0:
        {
            // pętla
            vector<Operation*> operations = optimizeLoop(*get<Loop*>(operation.getOper()), level + 1, usedThisLevel);
            return operations;
            break;
        }
        case 1:
        {
            // przypisanie
            auto p = get<pair<Variable*, Assigment*> >(operation.getOper());
            bool used = isUsed(*p.first);
            bool table = isTable(*p.first);
            int varLevel = getVariableLevel(*p.first);
            if(signAssigment(*p.second) && !used && varLevel < level - 1 && !table)
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
            // auto tmp = *get<Initiation*>(operation.getOper();
            // Variable variable = *tmp.getVariable();
            return signInitiation(*get<Initiation*>(operation.getOper()), level, usedNamesDownLevel, usedThisLevel);
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

std::vector<Operation*> Optimizer::optimizeLoop(Loop& loop, int level, multiset<string> usedNamesDownLevel)
{
    multiset<string> empty1;
    multiset<string> usedNames;
    if(loop.isInitiation())
    {
        switch(loop.getInitiation().index())
        {
            case 0:
            {
                signInitiation(*get<Initiation*>(loop.getInitiation()), level, empty1, empty1);
                multiset<string> tmp = getUsed(*get<Initiation*>(loop.getInitiation()));
                for(auto name : tmp)
                {
                    usedNames.insert(name);
                }
                break;
            }
            case 1:
                auto initiation = get<pair<Variable*, Assigment*> >(loop.getInitiation());
                signVariable(*initiation.first);
                signAssigment(*initiation.second);
                usedNames.insert(get<string>(initiation.first->getVariableName().value));
                for(auto x : initiation.second->getArithmeticExpression()->primaryExpressions)
                {
                    multiset<string> tmp = getUsed(*x);
                    for(auto name : tmp)
                    {
                        usedNames.insert(name);
                    }
                }
                break;
        }
    }
    signCondition(*loop.getCondition());

    auto expression1 = loop.getCondition()->expressions[0];
    auto expression2 = loop.getCondition()->expressions[1];
    for(auto pExpression : expression1->primaryExpressions)
    {
        multiset<string> tmp = getUsed(*pExpression);
        for(auto name : tmp)
            usedNames.insert(name);
    }
    for(auto pExpression : expression2->primaryExpressions)
    {
        multiset<string> tmp = getUsed(*pExpression);
        for(auto name : tmp)
            usedNames.insert(name);
    }

    if(loop.isBoolUpdate())
    {
        auto update = loop.getUpdate();
        switch(update.index())
        {
            case 0:
            {
                PreIncrementation pre = *get<PreIncrementation*>(update);
                signPreinkrementation(pre);
                usedNames.insert(get<string>(pre.getVariable()->getVariableName().value));
                break;
            }
            case 1:
            {
                pair<Variable*, Assigment*> p = get<pair<Variable*, Assigment*> >(update);
                signVariable(*p.first);
                signAssigment(*p.second);
                usedNames.insert(get<string>(p.first->getVariableName().value));
                for(auto x : p.second->getArithmeticExpression()->primaryExpressions)
                {
                    multiset<string> tmp = getUsed(*x);
                    for(auto name : tmp)
                    {
                        usedNames.insert(name);
                    }
                }
                break;
            }
            case 2:
            {
                pair<Variable*, Token> p = get<pair<Variable*, Token> > (update);
                signVariable(*p.first);
                usedNames.insert(get<string>(p.first->getVariableName().value));
                break;
            }
        }
    }
    
    for(int i = 0; i < loop.operations.size(); ++i)
    {
        Operation* operation = loop.operations[i];
        multiset<string> nextLevelUsed = getUsed(*operation);
        for(auto name : nextLevelUsed)
        {
            usedNames.insert(name);
        }
    }

    std::vector<Operation*> operations;

    for(int i = 0; i < loop.operations.size(); ++i)
    {
        auto operation = loop.operations.begin() + i;
        vector<struct VarDetails> varsCopy = varMap;
        bool removed;
        if((*operation)->getOper().index() == 0)
        {
            // varsCopy = varMap;
            for(auto var = varMap.begin(); var != varMap.end(); ++var)
            {
                var->alreadyUsed = false;
            }
        }
        variant< bool, vector<Operation*> > vec = optimizeOperation(**operation, level + 1, usedNamesDownLevel, usedNames);
        if(vec.index() == 0 && get<bool>(vec))
        {
            if((*operation)->getOper().index() != 3)
            {
                vector<Operation*> operationsToCheck;
                for(int j = i + 1; j < loop.operations.size(); ++j )
                {
                    operationsToCheck.push_back(*(loop.operations.begin() + j));
                }
                auto p = get<pair<Variable*, Assigment*> >((*operation)->getOper());
                string name = get<string>(p.first->getVariableName().value);
                int varLevel = getVariableLevel(*p.first);
                if(findUsageAndChange(operationsToCheck, name, varLevel, level))
                {
                    for(auto i = 0; i < varsCopy.size(); ++i)
                    {
                        varsCopy[i].alreadyUsed = varsCopy[i].alreadyUsed || varMap[i].alreadyUsed;
                    }
                    for(auto i = varsCopy.size(); i < varMap.size(); ++i)
                    {
                        varsCopy.push_back(varMap[i]);
                    }
                    varMap = varsCopy;
                    continue;
                }
            }

            if((*operation)->getOper().index() == 3)
            {
                Initiation* initiation = get<Initiation*>((*operation)->getOper());
                Variable* variable = initiation->getVariable();
                changeLevel(*variable);
                usedNames.erase(get<string>(variable->getVariableName().value));
            }
            else
            {
                pair<Variable*, Assigment*> p = get<pair<Variable*, Assigment*>>((*operation)->getOper());
                changeLevel(*p.first);
            }
            operations.push_back(*operation);
            loop.operations.erase(operation, operation + 1);
            
            operation--;
            i--;
        }
        // analizujemy operacje z wyższego poziomu
        else if(vec.index() == 1)
        {
            swap(varsCopy, varMap);
            vector<Operation*> otherLevelOperations = get< vector<Operation*> >(vec);
            for(auto oper = otherLevelOperations.begin(); oper != otherLevelOperations.end(); ++oper)
            {
                variant< bool, vector<Operation*> > var = optimizeOperation(**oper, level + 1, usedNamesDownLevel, usedNames);
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
            swap(varsCopy, varMap);
        }
        // if((*operation)->getOper().index() == 0)
        {
            for(auto i = 0; i < varsCopy.size(); ++i)
            {
                varsCopy[i].alreadyUsed = varsCopy[i].alreadyUsed || varMap[i].alreadyUsed;
            }
            for(auto i = varsCopy.size(); i < varMap.size(); ++i)
            {
                varsCopy.push_back(varMap[i]);
            }
            varMap = varsCopy;
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

bool Optimizer::signInitiation(Initiation& initiation, int level,  multiset<string> usedNamesDownLevel, multiset<string> usedThisLevel)
{
    int varLevel = getVariableLevel(*initiation.getVariable());
    addVariable(initiation.getVariable()->getVariableName(), bool(initiation.getVariable()->getIndex()), level);
    if(initiation.hasAssigment())
        if(!signAssigment(*initiation.getAssigment()))
            return false;
    string name = get<string>(initiation.getVariable()->getVariableName().value);
    if(usedNamesDownLevel.count(name) >= 1 || usedThisLevel.count(name) > 1)
    {
        return false;
    }
    if(level <= varLevel + 2)
        return false;
    if(bool(initiation.getVariable()->getIndex()))
        return false;
    return true;
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
        false,
        isTable
    };
    varMap.push_back(varInfo);
}

void Optimizer::addVariable(Token token, bool isTable, int level, vector<struct VarDetails>* varMap)
{
    string name = get<string>(token.value);
    struct VarDetails varInfo
    {
        name,
        level,
        false,
        isTable
    };
    varMap->push_back(varInfo);
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

bool Optimizer::isTable(Variable& variable)
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
    return it->isTable;
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
    vector<struct VarDetails>::iterator it = varMap.end();
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
    if(it == varMap.end())
        return -2;
    return it->level;
}

int Optimizer::getVariableLevel(Variable& variable, std::vector<struct VarDetails>* varMap)
{
    string name = get<string>(variable.getVariableName().value);
    struct VarDetails varInfo
    {
        "",
        -1,
        false
    }; 
    vector<struct VarDetails>::iterator it = varMap->end();
    for(auto var = varMap->begin(); var != varMap->end(); ++var)
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
    if(it == varMap->end())
        return -2;
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

std::multiset<string> Optimizer::getUsed(Operation& operation)
{
    multiset<string> usedNames;
    switch (operation.getOper().index())
    {
        case 0:
        {
            // pętla
            break;
        }
        case 1:
        {
            // przypisanie
            auto p = get<pair<Variable*, Assigment*> >(operation.getOper());
            usedNames.insert(get<string>(p.first->getVariableName().value));
            for(auto x : p.second->getArithmeticExpression()->primaryExpressions)
            {
                multiset<string> tmp = getUsed(*x);
                for(auto name : tmp)
                {
                    usedNames.insert(name);
                }
            }
            break;
        }
        case 2:
        {
            // post inkrementacja/dekrementacja
            auto p = get<pair<Variable*, Token> >(operation.getOper());
            usedNames.insert(get<string>(p.first->getVariableName().value));
            break;
        }
        case 3:
        {
            // inicjacja zmiennych
            Initiation* initiation = get<Initiation*>(operation.getOper());
            return getUsed(*initiation);
            break;
        }
        case 4:
        {
            // preinkrementacja
            usedNames.insert(get<string>(get<PreIncrementation*>(operation.getOper())->getVariable()->getVariableName().value));
            break;
        }
    }
    return usedNames;
}

std::multiset<string> Optimizer::getUsed(PrimaryExpression& primaryExpression)
{
    multiset<string> name;
    switch(primaryExpression.getPExpression().index())
    {
        case 0:
        {
            // variable
            Variable* variable = get<Variable*>(primaryExpression.getPExpression());
            name.insert(get<string>(variable->getVariableName().value));
            break;
        }
        case 1:
        {
            // post inkrementacja/dekrementacja
            auto p = get<pair<Variable*, Token> >(primaryExpression.getPExpression());
            name.insert(get<string>(p.first->getVariableName().value));
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
            PreIncrementation* preIncrementation = get<PreIncrementation*>(primaryExpression.getPExpression());
            name.insert(get<string>(preIncrementation->getVariable()->getVariableName().value));
            break;
        }
    }
    return name;
}

multiset<string> Optimizer::getUsed(Initiation& initiation)
{
    multiset<string>usedNames;
    usedNames.insert(get<string>(initiation.getVariable()->getVariableName().value));
    if(initiation.hasAssigment())
    {
        for(auto x : initiation.getAssigment()->getArithmeticExpression()->primaryExpressions)
        {
            multiset<string> tmp = getUsed(*x);
            for(auto name : tmp)
            {
                usedNames.insert(name);
            }
        }
    }
    return usedNames;
}


bool Optimizer::findUsageAndChange(vector<Operation*> operations, string varName, int varLevel, int currentLevel)
{
    int state = 0;
    varMapToCondition = varMap;
    for(auto operation : operations)
    {
        state = findUsageAndChange(*operation, state, varName, varLevel, currentLevel);
        if(state == 2)
            return true;
    }
    return false;
}

int Optimizer::findUsageAndChange(Operation& operation, int currentState, string varName, int varLevel, int currentLevel)
{
    multiset<string> usedNames;
    switch (operation.getOper().index())
    {
        case 0:
        {
            int state = currentState;
            for(auto oper : get<Loop*>(operation.getOper())->operations)
            {
                state = findUsageAndChange(*oper, state, varName, varLevel, currentLevel + 2);
                if(state == 2)
                    return 2;
            }
            break;
        }
        case 1:
        {
            // przypisanie
            auto p = get<pair<Variable*, Assigment*> >(operation.getOper());
            if(currentState == 1)
            {
                if(varName == get<string>(p.first->getVariableName().value) && varLevel == getVariableLevel(*p.first, &varMapToCondition))
                {
                    return 2;
                }
            }
            for(auto x : p.second->getArithmeticExpression()->primaryExpressions)
            {
                if(currentState == 0)
                {
                    if(findUsageAndChange(*x, currentState, varName, varLevel, currentLevel))
                    {
                        currentState = 1;
                    }
                }
            }
            break;
        }
        case 2:
        {
            // post inkrementacja/dekrementacja
            return 2;
            break;
        }
        case 3:
        {
            // inicjacja zmiennych
            Initiation* initiation = get<Initiation*>(operation.getOper());
            addVariable(initiation->getVariable()->getVariableName(), false, currentLevel, &varMapToCondition);
            if(initiation->hasAssigment() && currentState == 0)
                for(auto pExpression : initiation->getAssigment()->getArithmeticExpression()->primaryExpressions)
                {
                    if(findUsageAndChange(*pExpression, currentState, varName, varLevel, currentLevel))
                    {
                        currentState = 1;
                    }
                }
            return currentState;
            break;
        }
        case 4:
        {
            // preinkrementacja
            return 2;
            break;
        }
    }
    return currentState;
}

bool Optimizer::findUsageAndChange(PrimaryExpression& primaryExpression, int currentState, string varName, int varLevel, int currentLevel)
{
    switch(primaryExpression.getPExpression().index())
    {
        case 0:
        {
            // variable
            Variable* variable = get<Variable*>(primaryExpression.getPExpression());
            if(get<string>(variable->getVariableName().value) == varName && getVariableLevel(*variable, &varMapToCondition) == varLevel)
            {
                return true;
            }
            break;
        }
        case 1:
        {
            // post inkrementacja/dekrementacja
            auto p = get<pair<Variable*, Token> >(primaryExpression.getPExpression());
            if(get<string>(p.first->getVariableName().value) == varName && getVariableLevel(*p.first, &varMapToCondition) == varLevel)
            {
                return true;
            }
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
            PreIncrementation* preIncrementation = get<PreIncrementation*>(primaryExpression.getPExpression());
            if(get<string>(preIncrementation->getVariable()->getVariableName().value) == varName && getVariableLevel(*preIncrementation->getVariable(), &varMapToCondition) == varLevel)
            {
                return true;
            }
            break;
        }
    }
    return false;
}


void Optimizer::changeLevel(Variable& variable)
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
    it->level = it->level - 2;
}