#include <iostream>
#include "../include/lexer/lexer.hpp"

int main(int argc, char *argv[])
{
    if(argc == 1)
    {
        std::cout << "Need path to file with code\n";
    }
    else if(argc == 2)
    {
        Lexer lexer(argv[1], true);
        auto x = lexer.getNextToken();
        std::cout << x.type << " - number of token type \n";
        std::cout << std::get<std::string>(x.value) << " - value\n";
    }
    
}