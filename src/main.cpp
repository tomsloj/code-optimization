#include <iostream>
#include <fstream>

#include <variant>
#include <string>
#include <optional>

#include "../include/optimizer/Optimizer.hpp"
#include "../include/ParserTree/Utilities.hpp"


int main(int argc, char *argv[])
{
    if(argc == 1)
    {
        std::cout << "Need path to file with code\n";
    }
    else if(argc == 2 || argc == 3)
    {
        Optimizer optimizer(argv[1], true);
        optimizer.optimize();
        ParserTree* tree = optimizer.getTree();
        if(tree)
        {
            if(argc == 2)
                cout << treeToString(*tree, true) << "\n";
            if(argc == 3)
            {
                ofstream outFile (argv[2]);
                if (outFile.is_open())
                {
                    outFile << treeToString(*tree, true);
                    outFile.close();
                }
                else
                {
                    cout << "Cannot open output file\n";
                }
            }

        }
    }
    else
        cout << "Too much parameters\n";
    
}