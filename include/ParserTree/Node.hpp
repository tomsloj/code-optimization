#ifndef NODE_HPP
#define NODE_HPP

#include <variant>
#include <string>
#include <vector>

#include <memory>

// #include "Program.hpp"
// #include "Operation.hpp"
// #include "Loop.hpp"
// #include "Initiation.hpp"
// #include "Assigment.hpp"
// #include "Condition.hpp"
// #include "PreIncrementation.hpp"
// #include "ArithmeticExpression.hpp"
// #include "PrimaryExpression.hpp"

using namespace std;

// enum TypeOfNode {VAR, PROGRAM};

// class Node
// {
//     private:
//         std::weak_ptr<Node> parent;
        
//     public:
//         enum class Type
//         {
//             Program,
//             Operation,
//             Loop,
//             Initiation,
//             Assigment,
//             Condition,
//             ArithmeticExpression,
//             MulitplicativeExpression,
//             PrimaryExpression,
//             PreIncrementation,
//             Variable,
//             Number,
//         };

//         virtual TypeOfNode getType() = 0;
// };

#include "../structures/Token.hpp"

// #include "Variable.hpp"
// #include "Program.hpp"



#endif