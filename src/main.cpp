#include <iostream>
#include "../include/lexer/lexer.hpp"

int main()
{
    Lexer lexer("+-");
    auto x = lexer.getNextToken();
    std::cout << x.type << "\n";
    std::cout << std::get<std::string>(x.value) << "\n";
    
    x = lexer.getNextToken();
    std::cout << x.type << "\n";
    std::cout << std::get<std::string>(x.value) << "\n";
    return 0;
    x = lexer.getNextToken();
    std::cout << x.type << "\n";
}