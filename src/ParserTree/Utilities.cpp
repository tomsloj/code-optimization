#include "../../include/ParserTree/Utilities.hpp"

using namespace std;

std::string conditionToString(Condition* condtion)
{
    auto expression1 = condtion->expressions[0];
    string s = arithmeticExpressionToString(expression1);
    s += get<string>(condtion->getLogicalOperator().value);
    auto expression2 = condtion->expressions[1]; 
    s += arithmeticExpressionToString(expression2);
    return s;
}

std::string primaryExpressionToString(PrimaryExpression* primaryExpression)
{
    switch(primaryExpression->getPExpression().index())
    {
        case 0:
        {
            return variableToString(get<Variable*>(primaryExpression->getPExpression()));
            break;
        }
        case 1:
        {
            auto p = get<pair<Variable*, Token> >(primaryExpression->getPExpression());
            string s = variableToString(p.first);
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
            string s = preIncrementationToString(get<PreIncrementation*>(primaryExpression->getPExpression()));
            return s;
            break;
        }
        default:
            return "";
            break;
    }
}

std::string arithmeticExpressionToString(ArithmeticExpression* arithmeticExpression)
{
    string s = primaryExpressionToString(arithmeticExpression->primaryExpressions[0]);
    for(int i = 0; i < arithmeticExpression->operators.size(); ++i)
    {
        s += get<string>(arithmeticExpression->operators[i].value);
        s += primaryExpressionToString(arithmeticExpression->primaryExpressions[i + 1]);
    }
    return s;
}

std::string preIncrementationToString(PreIncrementation* preIncrementation)
{
    string s = ""; 
    auto token = preIncrementation->getIncrementation();
    if( token.type == DECREMENTAL_OPERATOR )
        s += "--";
    else if ( token.type == INCREMENTAL_OPERATOR)
    {
        s += "++";
    }
    s += variableToString(preIncrementation->getVariable());
    return s;
}

std::string initiationToString(Initiation* initiation)
{
    string s = "";
    s += get<string>(initiation->getDataType().value);
    s += " ";
    s += variableToString(initiation->getVariable());
    s += assigmentToString(initiation->getAssigment());
    return s;
}

std::string assigmentToString(Assigment* assigment)
{
    string s = "=";
    s += arithmeticExpressionToString(assigment->getArithmeticExpression());
    return s;
}

std::string variableToString(Variable* variable)
{
    string s = get<string>(variable->getVariableName().value);
    if( variable->getIndex() )
    {
        s += "[";
        s += arithmeticExpressionToString(variable->getIndex());
        s += "]";
    }
    return s;
}

std::string operationToString(Operation* oper)
{
    switch(oper->getOper().index())
    {
        case 0:
        {
            return loopToString(get<Loop*>(oper->getOper()));
            break;
        }
        case 1:
        {
            auto p = get<pair<Variable*, Assigment*> >(oper->getOper());
            string s = variableToString(p.first);
            s += assigmentToString(p.second);
            s += ";";
            return  s;
            break;
        }
        case 2:
        {
            auto p = get<pair<Variable*, Token> >(oper->getOper());
            string s = variableToString(p.first);
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
            string s = initiationToString(get<Initiation*>(oper->getOper()));
            s += ";";
            return s;
            break;
        }
        case 4:
        {
            string s = preIncrementationToString(get<PreIncrementation*>(oper->getOper()));
            s += ";";
            return s;
            break;
        }
        default:
            return "";
            break;
    }
}

std::string loopToString(Loop* loop)
{
    string s = "for(";
    if(loop->isInitiation())
    {
        auto initiation = loop->getInitiation();
        if(initiation.index() == 0)
        {
            Initiation* i = get<Initiation*>(initiation);
            s += initiationToString(i);
        }
        else
        {
            auto p = get<pair<Variable*, Assigment*>>(initiation);
            Variable* var = p.first;
            Assigment* ass = p.second;
            s += variableToString(var);
            s += assigmentToString(ass);
        }
    }
    s += ";";
    Condition* condition = loop->getCondition();
    if(condition)
    {
        s += conditionToString(condition);
    }
    s += ";";
    if( loop->isBoolUpdate())
    {
        auto update = loop->getUpdate();
        switch (update.index())
        {
            case 0:
            {
                PreIncrementation* pre = get<PreIncrementation*>(update);
                s += preIncrementationToString(pre);
                break;
            }
            case 1:
            {
                pair<Variable*, Assigment*> p = get<pair<Variable*, Assigment*> >(update);
                s += variableToString(p.first);
                s += assigmentToString(p.second);
                break;
            }
            case 2:
            {
                pair<Variable*, Token> p = get<pair<Variable*, Token> > (update);
                s += variableToString(p.first);
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
    for(auto o : loop->operations)  
    {
        s += operationToString(o);
    }
    s += "}";
    return s;
}

std::string programToString(Program* program)
{
    string s = "";
    for( int i = 0; i < program->operations.size(); ++i)
    {
        Operation* oper = program->operations[i];
        s += operationToString(oper);
    }
    return s;
}

std::string treeToString(ParserTree tree)
{
    Program p = tree.getProgram();
    return programToString(&p);
}