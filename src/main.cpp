#include <iostream>
// #include "../include/lexer/Lexer.hpp"
#include "../include/parser/Parser.hpp"


int main(int argc, char *argv[])
{
    if(argc == 1)
    {
        std::cout << "Need path to file with code\n";
    }
    else if(argc == 2)
    {
        Parser parser(argv[1], true);
        parser.parse();
        // Lexer lexer(argv[1], true);
        // auto x = lexer.getNextToken();
        // std::cout << x.type << " - number of token type \n";
        // std::cout << std::get<std::string>(x.value) << " - value\n";
        // while(x.type != END_OF_FILE)
        // {
        //     x = lexer.getNextToken();
        //     std::cout << x.type << " - number of token type \n";
        // }
    }
    
}