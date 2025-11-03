#include "shunting_yard.hpp"

static inline int precedence(const Token& t){ return t.precedence; }
static inline bool rightAssoc(const Token& t){ return t.rightAssoc; }

LinkedList<Token> ShuntingYard::toPostfix(LinkedList<Token>& infix){
    LinkedList<Token> out; Stack<Token> ops;
    Token prev; bool hasPrev=false;

    for(auto it=infix.begin(); it!=infix.end(); ++it){
        Token t = *it;

        if(t.type==TokenType::Number || t.type==TokenType::Identifier){
            out.push_back(t); hasPrev=true; prev=t; continue;
        }

        if(t.type==TokenType::LParen){ ops.push(t); hasPrev=false; continue; }

        if(t.type==TokenType::RParen){
            while(!ops.empty() && ops.top().type!=TokenType::LParen){ out.push_back(ops.top()); ops.pop(); }
            if(ops.empty()) throw std::runtime_error("Parentesis desbalanceados");
            ops.pop(); // saca '('
            hasPrev=true; continue;
        }

        if(t.type==TokenType::Operator){
            // +/− unario al inicio o tras '(' u otro operador
            if((!hasPrev) || prev.type==TokenType::LParen || prev.type==TokenType::Operator){
                if(t.text=="+"||t.text=="-") { t.unary=true; t.precedence=4; t.rightAssoc=true; }
            }
            while(!ops.empty() && ops.top().type!=TokenType::LParen && (
                  (!rightAssoc(t) && precedence(t) <= precedence(ops.top())) ||
                  ( rightAssoc(t) && precedence(t) <  precedence(ops.top())))){
                out.push_back(ops.top()); ops.pop();
            }
            ops.push(t); hasPrev=true; prev=t; continue;
        }
    }

    while(!ops.empty()){
        if(ops.top().type==TokenType::LParen||ops.top().type==TokenType::RParen) throw std::runtime_error("Parentesis desbalanceados");
        out.push_back(ops.top()); ops.pop();
    }
    return out;
}
