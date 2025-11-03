#include "tokenizer.hpp"

using std::string;
using std::unordered_set;

static bool isFunc(const string& name){
    static const unordered_set<string> F = {"sqrt","sin","cos","tan","log","ln"};
    return F.count(name) > 0;
}

LinkedList<Token> Tokenizer::tokenize(const string& s){
    LinkedList<Token> out;
    size_t i=0, n=s.size();

    while(i<n){
        if(isSpace(s[i])){ ++i; continue; }

        if(isDigitC(s[i]) || (s[i]=='.')){
            size_t j=i; bool dot=(s[i]=='.'); ++i;
            while(i<n && (isDigitC(s[i]) || (!dot && s[i]=='.'))){ dot = dot || (s[i]=='.'); ++i; }
            double val = std::stod(s.substr(j, i-j));
            Token t; t.type=TokenType::Number; t.text=""; t.value=val; t.precedence=-1; t.rightAssoc=false; t.unary=false;
            out.push_back(t);
            continue;
        }

        if(isAlphaC(s[i])){
            size_t j=i; ++i; while(i<n && (isAlphaC(s[i])||isDigitC(s[i]))) ++i;
            string name = s.substr(j, i-j);
            if(isFunc(name)){
                Token t; t.type=TokenType::Operator; t.text=name; t.value=0; t.precedence=4; t.rightAssoc=true; t.unary=true;
                out.push_back(t);
            } else {
                Token t; t.type=TokenType::Identifier; t.text=name; t.value=0; t.precedence=-1; t.rightAssoc=false; t.unary=false;
                out.push_back(t);
            }
            continue;
        }

        // operadores y paréntesis
        char c=s[i++];
        if(c=='('){ Token t; t.type=TokenType::LParen; t.text="("; t.value=0; t.precedence=-1; t.rightAssoc=false; t.unary=false; out.push_back(t); continue; }
        if(c==')'){ Token t; t.type=TokenType::RParen; t.text=")"; t.value=0; t.precedence=-1; t.rightAssoc=false; t.unary=false; out.push_back(t); continue; }
        if(string("+-*/^").find(c)!=string::npos){
            Token t; t.type=TokenType::Operator; t.text=string(1,c); t.value=0; t.precedence=-1; t.rightAssoc=false; t.unary=false;
            if(c=='+'||c=='-') t.precedence=1;
            if(c=='*'||c=='/') t.precedence=2;
            if(c=='^'){ t.precedence=3; t.rightAssoc=true; }
            out.push_back(t); continue;
        }

        throw std::runtime_error(string("Caracter no reconocido: ")+c);
    }
    return out;
}
