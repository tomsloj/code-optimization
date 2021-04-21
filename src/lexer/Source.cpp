#include "../../include/lexer/Source.hpp"

Source::~Source()
{
    if(sourceType == FILE_TYPE)
        fileReader.close();
}

char Source::getChar()
{
    char ch = ' ';
    if(sourceType == STRING_TYPE)
    {
        if(!source.empty())
        {
            ch = source.front();
            source.pop();
        }
        else
        {
            ch = EOF;
        }
    }
    else
    {
        if(!fileReader.get(ch))
            ch = EOF;
    }
    if(ch == '\n')
    {
        numberOfLine ++;
        signNumber = 0;
    }
    else
        signNumber ++;
    return ch;
}

void Source::loadFile(std::string fileName)
{
    sourceType = FILE_TYPE;
    fileReader.open(fileName);
}

void Source::loadString(std::string source)
{
    sourceType = STRING_TYPE;
    for (auto x : source)
    {
        this->source.push(x);
    }
}

std::pair<int, int>Source::getPlace()
{
    return std::make_pair(numberOfLine, signNumber);
}