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

        int numberOfLine = 1;
        int signNumber = 0;

    public:
        ~Source();
        char getChar();
        void loadFile(std::string fileName);
        void loadString(std::string source);
        std::pair<int, int>getPlace();

};



#endif