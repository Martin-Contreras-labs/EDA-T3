#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "common.hpp"

enum class TokenType { Number, Identifier, Operator, LParen, RParen, End };

struct Token {
    TokenType type;
    string text;     // para operadores e identificadores
    double value;    // para numeros
    int precedence;  // solo si es operador
    bool rightAssoc; // ^ es asociativo a derecha
    bool unary;      // +/− unario o funciones tipo sqrt/sin/etc.

    Token()
    : type(TokenType::End), text(), value(0.0), precedence(-1), rightAssoc(false), unary(false) {}
};

#endif // TOKEN_HPP
