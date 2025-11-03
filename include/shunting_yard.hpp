#ifndef SHUNTING_YARD_HPP
#define SHUNTING_YARD_HPP

#include "common.hpp"
#include "token.hpp"
#include "linked_list.hpp"
#include "stack.hpp"

class ShuntingYard {
public:
    LinkedList<Token> toPostfix(LinkedList<Token>& infix);
};

#endif // SHUNTING_YARD_HPP
