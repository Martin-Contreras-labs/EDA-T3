#ifndef STACK_HPP
#define STACK_HPP

#include "common.hpp"

template<typename T>
class Stack {
    struct Node { T data; Node* next; Node(const T& d, Node* nx):data(d),next(nx){} };
    Node* topNode{nullptr};
    size_t n{0};
public:
    ~Stack(){ while(!empty()) pop(); }
    void push(const T& x){ topNode=new Node(x, topNode); ++n; }
    void pop(){ if(!topNode) return; Node* t=topNode; topNode=topNode->next; delete t; --n; }
    T& top(){ if(!topNode) throw std::runtime_error("stack empty"); return topNode->data; }
    const T& top() const { if(!topNode) throw std::runtime_error("stack empty"); return topNode->data; }
    bool empty() const { return topNode==nullptr; }
    size_t size() const { return n; }
};

#endif // STACK_HPP
