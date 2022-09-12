#include "lexer.hpp"
#include <memory>

enum ReturnType {
    NIL,
    STR,
    CHR,
    INT,
    FLT,
};

enum NodeID {
    INT_NODE,
    FLT_NODE,
    BOOL_NODE,
    STR_NODE,
    BINOP_NODE,
    CHR_NODE
};

class Node {
public:
    virtual string Stringify(string tab);
    virtual void PrintAll(string tab);
    enum ReturnType ret_type;
    enum NodeID node_id;
    Token tok;
};

class String :public Node {
public:
    String(Token, string);
    string Stringify(string tab) override;
    string contents;
};

class Char : public Node {
public:
    Char(Token, char);
    string Stringify(string tab) override;
    char contents;
};

class Int : public Node {
public:
    Int(Token, int);
    string Stringify(string tab) override;
    unsigned long contents;
};

class Flt : public Node {
public:
    Flt(Token, double);
    string Stringify(string tab) override;
    double contents;
};

class BinaryOperation : public Node {
public:
    BinaryOperation(Token, Node*, Node*, string, enum ReturnType);
    string Stringify(string tab) override;
    Node* right;
    Node* left;
    string op;
};