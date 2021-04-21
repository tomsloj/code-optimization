#include "../../include/lexer/Source.hpp"

Source::~Source()
{
    if(sourceType == FILE_TYPE)
        fileReader.close();
}

char Source::getChar()
{
    if(sourceType == STRING_TYPE)
    {
        char ch = ' ';
        if(!source.empty())
        {
            ch = source.front();
            source.pop();
        }
        else
        {
            ch = EOF;
        }
        return ch;
    }
    else
    {
        char ch;
        if(fileReader.get(ch))
            return ch;
        else
            return EOF;

    }
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