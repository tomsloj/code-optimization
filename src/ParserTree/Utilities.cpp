#include "../../include/ParserTree/Utilities.hpp"

using namespace std;

std::string conditionToString(Condition* condtion, bool whitespaces)
{
    auto expression1 = condtion->expressions[0];
    string s = arithmeticExpressionToString(expression1, whitespaces);
    if(whitespaces)
        s += " ";
    s += get<string>(condtion->getLogicalOperator().value);
    if(whitespaces)
        s += " ";
    auto expression2 = condtion->expressions[1]; 
    s += arithmeticExpressionToString(expression2, whitespaces);
    return s;
}

std::string primaryExpressionToString(PrimaryExpression* primaryExpression, bool whitespaces)
{
    switch(primaryExpression->getPExpression().index())
    {
        case 0:
        {
            return variableToString(get<Variable*>(primaryExpression->getPExpression()), whitespaces);
            break;
        }
        case 1:
        {
            auto p = get<pair<Variable*, Token> >(primaryExpression->getPExpression());
            string s = variableToString(p.first, whitespaces);
            if( p.second.type == DECREMENTAL_OPERATOR )
                s += "--";
            else if ( p.second.type == INCREMENTAL_OPERATOR)
            {
                s += "++";
            }
            return s;
            break;
        }
        case 2:
        {
            if(get<Token>(primaryExpression->getPExpression()).value.index() == 0)
                return to_string(get<unsigned long long>(get<Token>(primaryExpression->getPExpression()).value));
            else
            {
                string s = to_string(get<double>(get<Token>(primaryExpression->getPExpression()).value));
                while(s[s.size() - 1] == '0')
                    s = s.substr(0, s.size()-1);
                return s;
            }
            break;
        }
        case 3:
        {
            string s = preIncrementationToString(get<PreIncrementation*>(primaryExpression->getPExpression()), whitespaces);
            return s;
            break;
        }
        default:
            return "";
            break;
    }
}

std::string arithmeticExpressionToString(ArithmeticExpression* arithmeticExpression, bool whitespaces)
{
    string s = primaryExpressionToString(arithmeticExpression->primaryExpressions[0], whitespaces);
    for(int i = 0; i < arithmeticExpression->operators.size(); ++i)
    {
        if(whitespaces)
            s += " ";
        s += get<string>(arithmeticExpression->operators[i].value);
        if(whitespaces)
            s += " ";
        s += primaryExpressionToString(arithmeticExpression->primaryExpressions[i + 1], whitespaces);
    }
    return s;
}

std::string preIncrementationToString(PreIncrementation* preIncrementation, bool whitespaces)
{
    string s = ""; 
    auto token = preIncrementation->getIncrementation();
    if( token.type == DECREMENTAL_OPERATOR )
        s += "--";
    else if ( token.type == INCREMENTAL_OPERATOR)
    {
        s += "++";
    }
    s += variableToString(preIncrementation->getVariable(), whitespaces);
    return s;
}

std::string initiationToString(Initiation* initiation, bool whitespaces)
{
    string s = "";
    s += get<string>(initiation->getDataType().value);
    s += " ";
    s += variableToString(initiation->getVariable(), whitespaces);
    if( initiation->hasAssigment())
    {
        if(whitespaces)
         s += " ";
        s += assigmentToString(initiation->getAssigment(), whitespaces);
    }
    return s;
}

std::string assigmentToString(Assigment* assigment, bool whitespaces)
{
    string s = "=";
    if(whitespaces)
        s += " ";
    s += arithmeticExpressionToString(assigment->getArithmeticExpression(), whitespaces);
    return s;
}

std::string variableToString(Variable* variable, bool whitespaces)
{
    string s = get<string>(variable->getVariableName().value);
    if( variable->getIndex() )
    {
        s += "[";
        s += arithmeticExpressionToString(variable->getIndex(), whitespaces);
        s += "]";
    }
    return s;
}

std::string operationToString(Operation* oper, bool whitespaces, int level)
{
    string s = "";
    if(whitespaces)
    {
        for(int i = 0; i < level; ++i)
        {
            s += "\t";
        }
    }
    if( oper->isEmpty() )
    {
        s += ";";
        return s;
    }
    switch(oper->getOper().index())
    {
        case 0:
        {
            s += loopToString(get<Loop*>(oper->getOper()), whitespaces, level);
            return s;
            break;
        }
        case 1:
        {
            auto p = get<pair<Variable*, Assigment*> >(oper->getOper());
            s += variableToString(p.first, whitespaces);
            if(whitespaces)
                s += " ";
            s += assigmentToString(p.second, whitespaces);
            s += ";";
            return  s;
            break;
        }
        case 2:
        {
            auto p = get<pair<Variable*, Token> >(oper->getOper());
            s += variableToString(p.first, whitespaces);
            if( p.second.type == DECREMENTAL_OPERATOR )
                s += "--";
            else if ( p.second.type == INCREMENTAL_OPERATOR)
            {
                s += "++";
            }
            s += ";";
            return s;
            break;
        }
        case 3:
        {
            s += initiationToString(get<Initiation*>(oper->getOper()), whitespaces);
            s += ";";
            return s;
            break;
        }
        case 4:
        {
            s += preIncrementationToString(get<PreIncrementation*>(oper->getOper()), whitespaces);
            s += ";";
            return s;
            break;
        }
        default:
            return s;
            break;
    }
}

std::string loopToString(Loop* loop, bool whitespaces, int level)
{
    string s = "for(";
    if(loop->isInitiation())
    {
        auto initiation = loop->getInitiation();
        if(initiation.index() == 0)
        {
            Initiation* i = get<Initiation*>(initiation);
            s += initiationToString(i, whitespaces);
        }
        else
        {
            auto p = get<pair<Variable*, Assigment*>>(initiation);
            Variable* var = p.first;
            Assigment* ass = p.second;
            s += variableToString(var, whitespaces);
            if(whitespaces)
                s += " ";
            s += assigmentToString(ass, whitespaces);
        }
    }
    s += ";";
    if(whitespaces)
        s += " ";
    Condition* condition = loop->getCondition();
    if(condition)
    {
        s += conditionToString(condition, whitespaces);
    }
    s += ";";
    if(whitespaces)
        s += " ";
    if( loop->isBoolUpdate())
    {
        auto update = loop->getUpdate();
        switch (update.index())
        {
            case 0:
            {
                PreIncrementation* pre = get<PreIncrementation*>(update);
                s += preIncrementationToString(pre, whitespaces);
                break;
            }
            case 1:
            {
                pair<Variable*, Assigment*> p = get<pair<Variable*, Assigment*> >(update);
                s += variableToString(p.first, whitespaces);
                s += assigmentToString(p.second, whitespaces);
                break;
            }
            case 2:
            {
                pair<Variable*, Token> p = get<pair<Variable*, Token> > (update);
                s += variableToString(p.first, whitespaces);
                if( p.second.type == DECREMENTAL_OPERATOR )
                    s += "--";
                else if ( p.second.type == INCREMENTAL_OPERATOR)
                {
                    s += "++";
                }
                break;
            }
        }
    } 
    s += "){";
    if(whitespaces)
        s += "\n";
    for(auto o : loop->operations)  
    {
        s += operationToString(o, whitespaces, level + 1);
        if(whitespaces)
        s += "\n";
    }
    if(whitespaces)
        for(int a = 0; a < level; ++a)
            s += "\t";
    s += "}";
    return s;
}

std::string programToString(Program* program, bool whitespaces)
{
    string s = "";
    for( int i = 0; i < program->operations.size(); ++i)
    {
        Operation* oper = program->operations[i];
        s += operationToString(oper, whitespaces, 0);
        if(whitespaces)
            s += "\n";
    }
    return s;
}

std::string treeToString(ParserTree tree, bool whitespaces /*= false*/)
{
    Program p = *tree.getProgram();
    return programToString(&p, whitespaces);
}