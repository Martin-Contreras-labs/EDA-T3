// ================================
// EdaCal — Skeleton v2.1 (C++11)
// + Variables (ans y nombres arbitrarios), asignacion: x = expr
// + Comandos: show <var>, prefix, posfix, tree, help
// + CLI: --version
// + Funciones: sqrt, sin, cos, tan, log (base10), ln
// + Constantes: pi, e
// Estructuras: LinkedList, Stack, Arbol de Expresion
// Flujo: Tokenizar (lista) -> Infija→Posfija (pila) -> Arbol -> Evaluar
// ================================

#include <bits/stdc++.h>
#ifdef _WIN32
  #include <io.h>
  #define isatty _isatty
  #define fileno _fileno
#endif
using namespace std;

// -------------------- Utilidad --------------------
static inline bool isSpace(char c){ return c==' '||c=='\t'||c=='\n'||c=='\r'; }
static inline bool isDigitC(char c){ return c>='0' && c<='9'; }
static inline bool isAlphaC(char c){ return (c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='_'; }
static inline string ltrim(string s){ size_t i=0; while(i<s.size() && isSpace(s[i])) ++i; return s.substr(i); }
static inline string rtrim(string s){ int i=(int)s.size()-1; while(i>=0 && isSpace(s[i])) --i; return s.substr(0,i+1); }
static inline string trim(string s){ return rtrim(ltrim(s)); }

// -------------------- Token --------------------
enum class TokenType { Number, Identifier, Operator, LParen, RParen, End };

struct Token {
    TokenType type{TokenType::End};
    string text;   // para operadores e identificadores
    double value{0.0};  // para numeros
    int precedence{ -1 }; // solo si es operador
    bool rightAssoc{ false }; // ^ es asociativo a derecha
    bool unary{ false }; // para +/− unario y funciones tipo sqrt/sin/etc.
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
    iterator end()   { return iterator(nullptr); }
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
    static bool isFunc(const string& name){
        static const unordered_set<string> F = {"sqrt","sin","cos","tan","log","ln"};
        return F.count(name)>0;
    }
public:
    LinkedList<Token> tokenize(const string& s){
        LinkedList<Token> out;
        size_t i=0; size_t n=s.size();
        while(i<n){
            if(isSpace(s[i])){ ++i; continue; }
            if(isDigitC(s[i]) || (s[i]=='.')){
                size_t j=i; bool dot=(s[i]=='.'); ++i;
                while(i<n && (isDigitC(s[i]) || (!dot && s[i]=='.'))){ dot = dot || (s[i]=='.'); ++i; }
                double val = stod(s.substr(j, i-j));
                Token t; t.type=TokenType::Number; t.text=""; t.value=val; t.precedence=-1; t.rightAssoc=false; t.unary=false;
                out.push_back(t); continue;
            }
            if(isAlphaC(s[i])){
                size_t j=i; ++i; while(i<n && (isAlphaC(s[i])||isDigitC(s[i]))) ++i; // identificador o funcion
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
            // operadores y parentesis
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
static inline int precedence(const Token& t){ return t.precedence; }
static inline bool rightAssoc(const Token& t){ return t.rightAssoc; }

// -------------------- Shunting Yard: infija -> posfija (lista) --------------------
class ShuntingYard {
public:
    LinkedList<Token> toPostfix(LinkedList<Token>& infix){
        LinkedList<Token> out; Stack<Token> ops;
        Token prev; bool hasPrev=false;
        for(auto it=infix.begin(); it!=infix.end(); ++it){ Token t=*it;
            if(t.type==TokenType::Number || t.type==TokenType::Identifier){
                out.push_back(t); hasPrev=true; prev=t; continue;
            }
            if(t.type==TokenType::LParen){ ops.push(t); hasPrev=false; continue; }
            if(t.type==TokenType::RParen){
                while(!ops.empty() && ops.top().type!=TokenType::LParen){ out.push_back(ops.top()); ops.pop(); }
                if(ops.empty()) throw runtime_error("Parentesis desbalanceados");
                ops.pop(); // saca '('
                hasPrev=true; continue;
            }
            if(t.type==TokenType::Operator){
                // detectar unario para +/− al inicio o despues de '(' u otro operador
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
            if(ops.top().type==TokenType::LParen||ops.top().type==TokenType::RParen) throw runtime_error("Parentesis desbalanceados");
            out.push_back(ops.top()); ops.pop();
        }
        return out;
    }
};

// -------------------- Arbol de Expresion --------------------
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
            if(t.type==TokenType::Number || t.type==TokenType::Identifier){
                st.push(new ExprNode(t));
            } else if(t.type==TokenType::Operator){
                ExprNode* node=new ExprNode(t);
                if(t.unary){
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
        if(st.size()!=1) throw runtime_error("Expresion invalida");
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
    double eval(ExprNode* n){ if(!n) throw runtime_error("Arbol vacio"); return evalNode(n); }
private:
    VarEnv* env;
    double evalNode(ExprNode* n){
        if(n->tok.type==TokenType::Number) return n->tok.value;
        if(n->tok.type==TokenType::Identifier){
            if(env){
                if(env->has(n->tok.text)) return env->get(n->tok.text);
                throw runtime_error("Variable no definida: "+n->tok.text);
            }
            throw runtime_error("Variable no soportada: "+n->tok.text);
        }
        // Operadores
        if(n->tok.unary){
            double a = evalNode(n->right);
            if(n->tok.text=="-") return -a;
            if(n->tok.text=="+") return +a;
            if(n->tok.text=="sqrt"){ if(a<0) throw runtime_error("sqrt de negativo"); return sqrt(a);}            
            if(n->tok.text=="sin") return sin(a);
            if(n->tok.text=="cos") return cos(a);
            if(n->tok.text=="tan") return tan(a);
            if(n->tok.text=="log") { if(a<=0) throw runtime_error("log de no-positivo"); return log10(a); }
            if(n->tok.text=="ln")  { if(a<=0) throw runtime_error("ln de no-positivo");  return log(a); }
        } else {
            double a = evalNode(n->left);
            double b = evalNode(n->right);
            if(n->tok.text=="+") return a+b;
            if(n->tok.text=="-") return a-b;
            if(n->tok.text=="*") return a*b;
            if(n->tok.text=="/") { if(b==0) throw runtime_error("division por cero"); return a/b; }
            if(n->tok.text=="^") return pow(a,b);
        }
        throw runtime_error(string("Operador desconocido: ")+n->tok.text);
    }
};

// -------------------- REPL --------------------
int main(int argc, char** argv){
    ios::sync_with_stdio(false); cin.tie(nullptr);
    bool interactive = isatty(fileno(stdin));
    if (argc>1 && string(argv[1])=="--version") { cout << "EdaCal v2.1" << "\n"; return 0; }
    if (interactive) cout << "EdaCal v2.1 - escribe una expresion o 'exit'\n";

    string line; Tokenizer tk; ShuntingYard sy; 
    VarEnv env; env.set("ans", 0.0); env.set("pi", 3.14159265358979323846); env.set("e", 2.71828182845904523536);
    Evaluator ev(&env);

    // almacenamos la ultima expresion en posfija para prefix/posfix/tree
    vector<Token> lastPostfix;
    auto savePostfix = [&](LinkedList<Token>& post){
        lastPostfix.clear();
        for(auto it=post.begin(); it!=post.end(); ++it) lastPostfix.push_back(*it);
    };

    auto rebuildTreeFromLast = [&](){
        if(lastPostfix.empty()) throw runtime_error("No hay expresion previa");
        LinkedList<Token> post;
        for(const auto& t: lastPostfix) post.push_back(t);
        ExprTree* tree = new ExprTree();
        tree->buildFromPostfix(post);
        return tree; // caller borra
    };

    auto printPosfixFromLast = [&](){
        if(lastPostfix.empty()) throw runtime_error("No hay expresion previa");
        for(const auto& t: lastPostfix) cout << ExprTree::tokenToStr(t) << ' ';
        cout << "\n";
    };

    auto findTopLevelEq = [&](const string& s)->int{
        int bal=0; for(int i=0;i<(int)s.size();++i){ char c=s[i];
            if(c=='(') ++bal; else if(c==')') --bal; else if(c=='=' && bal==0) return i;
        } return -1;
    };

    while(true){
        if (interactive) cout << ">> ";
        if(!getline(cin, line)) break;
        line = trim(line);
        if(line=="exit") break;
        if(line.empty()) continue;

        // ayuda
        if(line=="help"){
            cout << "Comandos disponibles:\n"
                 << "  help               -> esta ayuda\n"
                 << "  --version          -> imprime la version y termina\n"
                 << "  show <var>         -> muestra valor de variable\n"
                 << "  let x = expr       -> alias de asignacion\n"
                 << "  vars               -> lista variables definidas\n"
                 << "  del <var>          -> elimina variable (excepto pi, e, ans)\n"
                 << "  posfix [expr]      -> imprime notacion posfija (de expr o ultima)\n"
                 << "  prefix [expr]      -> imprime notacion prefija (de expr o ultima)\n"
                 << "  tree   [expr]      -> imprime arbol (de expr o ultima)\n"
                 << "Asignacion: x = expresion\n"
                 << "Funciones: sqrt, sin, cos, tan, log(base10), ln\n"
                 << "Constantes: pi, e (radianes)\n";
            continue;
        }

        // comandos que usan la ultima expresion (ahora con argumento opcional)
        auto hasArg = [&](const string& cmd)->bool {
            return line.size() > cmd.size() && line.substr(0, cmd.size()) == cmd && !trim(line.substr(cmd.size())).empty();
        };
        auto argOf = [&](size_t k)->string { return trim(line.substr(k)); };

        // posfix
        if(line=="posfix" || line.rfind("posfix",0)==0){
            try{
                if(hasArg("posfix")){
                    string expr = argOf(6); // despues de "posfix"
                    auto infix = tk.tokenize(expr);
                    auto postfix = sy.toPostfix(infix);
                    for(auto it=postfix.begin(); it!=postfix.end(); ++it) cout << ExprTree::tokenToStr(*it) << ' ';
                    cout << "\n";
                }else{
                    printPosfixFromLast();
                }
            } catch(const exception& ex){
                if(interactive) cerr << "Error: " << ex.what() << "\n";
                else cout << "Error: " << ex.what() << "\n";
            }
            continue;
        }

        // prefix
        if(line=="prefix" || line.rfind("prefix",0)==0){
            try{
                unique_ptr<ExprTree> t;
                if(hasArg("prefix")){
                    string expr = argOf(6); // despues de "prefix"
                    auto infix = tk.tokenize(expr);
                    auto postfix = sy.toPostfix(infix);
                    t.reset(new ExprTree());
                    t->buildFromPostfix(postfix);
                }else{
                    t.reset(rebuildTreeFromLast());
                }
                t->printPrefix(t->root); cout << "\n";
            } catch(const exception& ex){
                if(interactive) cerr << "Error: " << ex.what() << "\n";
                else cout << "Error: " << ex.what() << "\n";
            }
            continue;
        }

        // tree
        if(line=="tree" || line.rfind("tree",0)==0){
            try{
                unique_ptr<ExprTree> t;
                if(hasArg("tree")){
                    string expr = argOf(4); // despues de "tree"
                    auto infix = tk.tokenize(expr);
                    auto postfix = sy.toPostfix(infix);
                    t.reset(new ExprTree());
                    t->buildFromPostfix(postfix);
                }else{
                    t.reset(rebuildTreeFromLast());
                }
                t->printTree(t->root);
            } catch(const exception& ex){
                if(interactive) cerr << "Error: " << ex.what() << "\n";
                else cout << "Error: " << ex.what() << "\n";
            }
            continue;
        }

        // vars: lista todas las variables ordenadas alfabeticamente 
        if (line == "vars") {
            std::vector<std::pair<std::string,double>> kv(env.vars.begin(), env.vars.end());
            std::sort(kv.begin(), kv.end(),
                [](const std::pair<std::string,double>& a, const std::pair<std::string,double>& b){
                    return a.first < b.first;
                }
            );
            for (const auto& p : kv) {
                cout << p.first << " -> " << std::fixed << std::setprecision(10) << p.second << "\n";
            }
            continue;
        }


        // del <var>: elimina una variable (protegemos pi, e, ans)
        if (line.rfind("del ", 0) == 0) {
            string var = trim(line.substr(4));
            if (var.empty()) { cout << "Error: nombre vacio\n"; continue; }
            if (var == "pi" || var == "e" || var == "ans") {
                cout << "Error: variable protegida\n"; 
                continue;
            }
            auto it = env.vars.find(var);
            if (it == env.vars.end()) {
                cout << "Error: variable no existe\n";
            } else {
                env.vars.erase(it);
                cout << "ok\n";
            }
            continue;
        }

        // show <var>
        if(line.size()>5 && line.substr(0,5)=="show "){
            string var = trim(line.substr(5));
            try{ double v = env.get(var); cout << var << " -> " << fixed << setprecision(10) << v << "\n"; }
            catch(const exception& ex){ if(interactive) cerr<<"Error: "<<ex.what() << "\n"; else cout<<"Error: "<<ex.what() << "\n"; }
            continue;
        }

        // alias: let x = expr  (reescribimos la linea y dejamos que siga el flujo normal)
        if (line.rfind("let ", 0) == 0) {
            line = trim(line.substr(4));
        }
        int posEq = findTopLevelEq(line);
        if(posEq>0){
            string lhs = trim(line.substr(0,posEq));
            string rhs = trim(line.substr(posEq+1));
            if(lhs.empty() || !isAlphaC(lhs[0])){ if(interactive) cerr<<"Error: LHS invalido" << "\n"; else cout<<"Error: LHS invalido\n"; continue; }
            try{
                auto infix = tk.tokenize(rhs);
                auto postfix = sy.toPostfix(infix);
                savePostfix(postfix);
                ExprTree tree; tree.buildFromPostfix(postfix);
                double res = ev.eval(tree.root);
                env.set(lhs, res);
                env.set("ans", res);
                cout << lhs << " -> " << fixed << setprecision(10) << res << "\n";
            } catch(const exception& ex){ if(interactive) cerr<<"Error: "<<ex.what() << "\n"; else cout<<"Error: "<<ex.what() << "\n"; }
            continue;
        }

        // evaluacion normal
        try{
            auto infix = tk.tokenize(line);
            auto postfix = sy.toPostfix(infix);
            savePostfix(postfix);
            ExprTree tree; tree.buildFromPostfix(postfix);
            double res = ev.eval(tree.root);
            env.set("ans", res);
            cout << "ans -> " << fixed << setprecision(10) << res << "\n";
        } catch(const exception& ex){ if (interactive) cerr << "Error: " << ex.what() << "\n"; else cout << "Error: " << ex.what() << "\n"; }
    }
    return 0;
}

// ================================
// Notas de diseno
// - Cumple con: lista enlazada propia, pila propia, shunting yard (cap. 5), arbol de expresion.
// - Tests sugeridos:
//   1) 6+5 -> 11
//   2) 5+3*5+2 -> 22
//   3) -3 + 5 -> 2
//   4) sqrt(9)+2^3 -> 11
//   5) (2+3)*(4+5) -> 45
// ================================
