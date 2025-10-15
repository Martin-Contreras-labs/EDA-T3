// ================================
// EdaCal — Skeleton v1 (C++11)
// Estructuras: LinkedList, Stack, Árbol de Expresión
// Flujo: Tokenizar (lista) -> Infija→Posfija (pila) -> Árbol -> Evaluar
// TODO próximos pasos: variables, comandos (show, tree, prefix, posfix), sqrt, potencia
// ================================

#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;

// -------------------- Utilidad --------------------
static inline bool isSpace(char c){ return c==' '||c=='\t'||c=='\n' || c=='\r'; }
static inline bool isDigit(char c){ return c>='0' && c<='9'; }
static inline bool isAlpha(char c){ return (c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='_'; }

// -------------------- Token --------------------
enum class TokenType { Number, Identifier, Operator, LParen, RParen, End };

struct Token {
    TokenType type;
    string text;   // para operadores e identificadores
    double value;  // para números
    int precedence{ -1 }; // solo si es operador
    bool rightAssoc{ false }; // ^ es asociativo a derecha
    bool unary{ false }; // para +/− unario y funciones tipo sqrt
};

// -------------------- LinkedList<T> (simple) --------------------
template<typename T>
class LinkedList {
    struct Node { T data; Node* next; Node(const T& d):data(d),next(nullptr){} };
    Node* head{nullptr};
    Node* tail{nullptr};
    size_t n{0};
public:
    struct iterator {
        Node* p; iterator(Node* p=nullptr):p(p){}
        T& operator*(){ return p->data; }
        iterator& operator++(){ if(p) p=p->next; return *this; }
        bool operator!=(const iterator& o) const { return p!=o.p; }
    };
    ~LinkedList(){ clear(); }
    void push_back(const T& x){ Node* nn=new Node(x); if(!head) head=tail=nn; else { tail->next=nn; tail=nn; } ++n; }
    bool empty() const { return head==nullptr; }
    size_t size() const { return n; }
    void clear(){ Node* p=head; while(p){ Node* q=p->next; delete p; p=q; } head=tail=nullptr; n=0; }
    iterator begin() { return iterator(head); }
    iterator end() { return iterator(nullptr); }
};

// -------------------- Stack<T> usando lista enlazada --------------------
template<typename T>
class Stack {
    struct Node { T data; Node* next; Node(const T& d, Node* nx):data(d),next(nx){} };
    Node* topNode{nullptr};
    size_t n{0};
public:
    ~Stack(){ while(!empty()) pop(); }
    void push(const T& x){ topNode=new Node(x, topNode); ++n; }
    void pop(){ if(!topNode) return; Node* t=topNode; topNode=topNode->next; delete t; --n; }
    T& top(){ if(!topNode) throw runtime_error("stack empty"); return topNode->data; }
    const T& top() const { if(!topNode) throw runtime_error("stack empty"); return topNode->data; }
    bool empty() const { return topNode==nullptr; }
    size_t size() const { return n; }
};

// -------------------- Tokenizer: string -> LinkedList<Token> (infija) --------------------
class Tokenizer {
public:
    LinkedList<Token> tokenize(const string& s){
        LinkedList<Token> out;
        size_t i=0; size_t n=s.size();
        while(i<n){
            if(isSpace(s[i])){ ++i; continue; }
            if(isDigit(s[i]) || (s[i]=='.')){
                size_t j=i; bool dot=(s[i]=='.'); ++i;
                while(i<n && (isDigit(s[i]) || (!dot && s[i]=='.'))){ dot = dot || (s[i]=='.'); ++i; }
                double val = stod(s.substr(j, i-j));
                Token t; t.type=TokenType::Number; t.text=""; t.value=val; t.precedence=-1; t.rightAssoc=false; t.unary=false;
                out.push_back(t);
                continue;
            }
            if(isAlpha(s[i])){
                size_t j=i; ++i; while(i<n && (isAlpha(s[i])||isDigit(s[i]))) ++i; // identificador o función (p.ej., sqrt)
                string name = s.substr(j, i-j);
                Token t; t.type=TokenType::Identifier; t.text=name; t.value=0; t.precedence=-1; t.rightAssoc=false; t.unary=false;
                out.push_back(t); continue;
            }
            // operadores y paréntesis
            char c=s[i++];
            if(c=='('){ Token t; t.type=TokenType::LParen; t.text="("; t.value=0; t.precedence=-1; t.rightAssoc=false; t.unary=false; out.push_back(t); continue; }
            if(c==')'){ Token t; t.type=TokenType::RParen; t.text=")"; t.value=0; t.precedence=-1; t.rightAssoc=false; t.unary=false; out.push_back(t); continue; }
            if(string("+-*/^").find(c)!=string::npos){
                Token t; t.type=TokenType::Operator; t.text=string(1,c); t.value=0; t.precedence=-1; t.rightAssoc=false; t.unary=false;
                if(c=='+'||c=='-') t.precedence=1; // se ajusta para unarios luego
                if(c=='*'||c=='/') t.precedence=2;
                if(c=='^'){ t.precedence=3; t.rightAssoc=true; }
                out.push_back(t); continue;
            }
            throw runtime_error(string("Caracter no reconocido: ")+c);
        }
        return out;
    }
};

// -------------------- Helpers de operadores --------------------
static inline bool isOperator(const Token& t){ return t.type==TokenType::Operator || (t.type==TokenType::Identifier && (t.text=="sqrt")); }
static inline int precedence(const Token& t){
    if(t.type==TokenType::Identifier && t.text=="sqrt") return 4; // mayor que ^ (unario)
    return t.precedence;
}
static inline bool rightAssoc(const Token& t){
    if(t.type==TokenType::Identifier && t.text=="sqrt") return true; // unario derecha
    return t.rightAssoc;
}

// -------------------- Shunting Yard: infija -> posfija (lista) --------------------
class ShuntingYard {
public:
    LinkedList<Token> toPostfix(LinkedList<Token>& infix){
        LinkedList<Token> out; Stack<Token> ops;
        Token prev; bool hasPrev=false;
        for(auto it=infix.begin(); it!=infix.end(); ++it){ Token t=*it;
            if(t.type==TokenType::Number || t.type==TokenType::Identifier){
                // Si identifier es función conocida (sqrt), trátala como operador unario; si no, como variable (por ahora a salida)
                if(t.type==TokenType::Identifier && t.text=="sqrt"){ t.type=TokenType::Operator; t.unary=true; }
                if(t.type==TokenType::Operator && t.unary){
                    // caerá en lógica de operador abajo
                } else {
                    out.push_back(t); hasPrev=true; prev=t; continue;
                }
            }
            if(t.type==TokenType::LParen){ ops.push(t); hasPrev=false; continue; }
            if(t.type==TokenType::RParen){
                while(!ops.empty() && ops.top().type!=TokenType::LParen){ out.push_back(ops.top()); ops.pop(); }
                if(ops.empty()) throw runtime_error("Paréntesis desbalanceados");
                ops.pop(); // saca '('
                hasPrev=true; continue;
            }
            if(t.type==TokenType::Operator){
                // detectar unario para +/− al inicio o después de '(' u otro operador
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
            if(ops.top().type==TokenType::LParen||ops.top().type==TokenType::RParen) throw runtime_error("Paréntesis desbalanceados");
            out.push_back(ops.top()); ops.pop();
        }
        return out;
    }
};

// -------------------- Árbol de Expresión --------------------
struct ExprNode {
    Token tok; ExprNode* left{nullptr}; ExprNode* right{nullptr};
    explicit ExprNode(const Token& t):tok(t){}
};

class ExprTree {
public:
    ExprNode* root{nullptr};
    ~ExprTree(){ clear(root); }
    void clear(ExprNode* n){ if(!n) return; clear(n->left); clear(n->right); delete n; }

    void buildFromPostfix(LinkedList<Token>& post){
        Stack<ExprNode*> st;
        for(auto it=post.begin(); it!=post.end(); ++it){ Token t=*it;
            if(t.type==TokenType::Number || (t.type==TokenType::Identifier && t.text!="sqrt")){
                st.push(new ExprNode(t));
            } else if(t.type==TokenType::Operator || (t.type==TokenType::Identifier && t.text=="sqrt")){
                ExprNode* node=new ExprNode(t);
                if(t.unary || t.text=="sqrt"){
                    if(st.empty()) throw runtime_error("Operador unario sin operando");
                    node->right = st.top(); st.pop();
                } else {
                    if(st.size()<2) throw runtime_error("Operador binario con operandos insuficientes");
                    ExprNode* r=st.top(); st.pop(); ExprNode* l=st.top(); st.pop();
                    node->left=l; node->right=r;
                }
                st.push(node);
            }
        }
        if(st.size()!=1) throw runtime_error("Expresión inválida");
        root=st.top(); st.pop();
    }

    // recorridos para prefix/posfix y "tree"
    void printPrefix(ExprNode* n){ if(!n) return; cout << tokenToStr(n->tok) << ' '; printPrefix(n->left); printPrefix(n->right); }
    void printPostfix(ExprNode* n){ if(!n) return; printPostfix(n->left); printPostfix(n->right); cout << tokenToStr(n->tok) << ' '; }
    void printTree(ExprNode* n, int depth=0){ if(!n) return; printTree(n->right, depth+1); cout << string(2*depth, ' ') << tokenToStr(n->tok) << "\n"; printTree(n->left, depth+1); }

    static string tokenToStr(const Token& t){
        switch(t.type){
            case TokenType::Number: { ostringstream os; os<<t.value; return os.str(); }
            case TokenType::Identifier: return t.text;
            case TokenType::Operator: return t.text;
            case TokenType::LParen: return "(";
            case TokenType::RParen: return ")";
            default: return "?";
        }
    }
};

// -------------------- Evaluador --------------------
// -------------------- Evaluador con entorno de variables --------------------
struct VarEnv {
    unordered_map<string,double> vars;
    bool has(const string& k) const { return vars.find(k)!=vars.end(); }
    double get(const string& k) const { auto it=vars.find(k); if(it==vars.end()) throw runtime_error("Variable no definida: "+k); return it->second; }
    void set(const string& k, double v){ vars[k]=v; }
};

class Evaluator {
public:
    explicit Evaluator(VarEnv* env):env(env){}
    double eval(ExprNode* n){ if(!n) throw runtime_error("Árbol vacío"); return evalNode(n);
    }
private:
    VarEnv* env;
    double evalNode(ExprNode* n){
        if(n->tok.type==TokenType::Number) return n->tok.value;
        if(n->tok.type==TokenType::Identifier && n->tok.text!="sqrt"){
            // Soporte mínimo: variable ans (y futuras variables del entorno)
            if(env){
                if(env->has(n->tok.text)) return env->get(n->tok.text);
                throw runtime_error("Variable no definida: "+n->tok.text);
            }
            throw runtime_error("Variable no soportada: "+n->tok.text);
        }
        // Operadores
        if(n->tok.unary || n->tok.text=="sqrt"){
            double a = evalNode(n->right);
            if(n->tok.text=="-") return -a;
            if(n->tok.text=="+") return +a;
            if(n->tok.text=="sqrt"){ if(a<0) throw runtime_error("sqrt de negativo"); return sqrt(a);}            
        } else {
            double a = evalNode(n->left);
            double b = evalNode(n->right);
            if(n->tok.text=="+") return a+b;
            if(n->tok.text=="-") return a-b;
            if(n->tok.text=="*") return a*b;
            if(n->tok.text=="/") { if(b==0) throw runtime_error("division por cero"); return a/b; }
            if(n->tok.text=="^") return pow(a,b);
        }
        throw runtime_error("Operador desconocido: "+n->tok.text);
    }
};

// -------------------- REPL mínimo --------------------
int main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);
    bool interactive = isatty(fileno(stdin));
if (interactive) cout << "EdaCal v1 - escribe una expresion o 'exit'\n";

    string line; Tokenizer tk; ShuntingYard sy; 
    VarEnv env; env.set("ans", 0.0);
    Evaluator ev(&env);

    while(true){
        if (interactive) cout << ">> ";
        if(!getline(cin, line)) break;
        if(line=="exit") break;

        // comando simple: show <var>
        if(line.size()>5 && line.substr(0,5)=="show "){
            string var = line.substr(5);
            try{
                double v = env.get(var);
cout << var << " -> " << fixed << setprecision(10) << v << "\n";
            }catch(const exception& ex){
if (interactive) cerr << "Error: " << ex.what() << "\n"; else cout << "Error: " << ex.what() << "\n";
            }
            continue;
        }

        try{
            auto infix = tk.tokenize(line);
            auto postfix = sy.toPostfix(infix);
            ExprTree tree; tree.buildFromPostfix(postfix);
            double res = ev.eval(tree.root);
            env.set("ans", res);
cout << "ans -> " << fixed << setprecision(10) << res << "\n";
        } catch(const exception& ex){
if (interactive) cerr << "Error: " << ex.what() << "\n";
else cout << "Error: " << ex.what() << "\n";
        }
    }
    return 0;
}

// ================================
// Notas de diseño
// - Cumple con: lista enlazada propia, pila propia, shunting yard (cap. 5), árbol de expresión.
// - Pendiente para v2: variables (tabla hash), asignación x = expr, comandos show/prefix/posfix/tree separados del REPL.
// - Tests sugeridos:
//   1) 6+5 -> 11
//   2) 5+3*5+2 -> 22
//   3) -3 + 5 -> 2
//   4) sqrt(9)+2^3 -> 11
//   5) (2+3)*(4+5) -> 45
// ================================
