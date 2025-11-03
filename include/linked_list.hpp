#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include "common.hpp"

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

#endif // LINKED_LIST_HPP
