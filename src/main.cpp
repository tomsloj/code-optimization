#include <iostream>

#include <variant>
#include <string>
#include <optional>

// #include "../include/lexer/Lexer.hpp"
// #include "../include/parser/Parser.hpp"
// #include "../include/analyzer/Analyzer.hpp"
#include "../include/optimizer/Optimizer.hpp"
#include "../include/ParserTree/Utilities.hpp"


int main(int argc, char *argv[])
{
    if(argc == 1)
    {
        std::cout << "Need path to file with code\n";
    }
    else if(argc == 2)
    {
        Optimizer optimizer(argv[1], true);
        cout << optimizer.optimize() << "\n";
        cout << treeToString(*optimizer.getTree()) << "\n";

        // Analyzer analyzer(argv[1], true);
        // cout << analyzer.analyze();
        // cout << " analyze finished\n";

        // Parser parser(argv[1], true);
        // optional<ParserTree> tree = parser.parse();
        // if( tree )
        //     cout << treeToString(*tree);

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