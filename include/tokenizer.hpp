#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include "common.hpp"
#include "token.hpp"
#include "linked_list.hpp"

class Tokenizer { 
public:
    LinkedList<Token> tokenize(const std::string& s);
};

#endif // TOKENIZER_HPP
