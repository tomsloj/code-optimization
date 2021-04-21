#ifndef SOURCE_HPP
#define SOURCE_HPP

#include <queue>
#include <string>
#include <fstream>
#include <iostream>

enum SourceType {STRING_TYPE, FILE_TYPE};

class Source
{
    private:
        std::queue<char>source;
        std::ifstream fileReader;
        SourceType sourceType;

    public:
        ~Source();
        char getChar();
        void loadFile(std::string fileName);
        void loadString(std::string source);

};



#endif