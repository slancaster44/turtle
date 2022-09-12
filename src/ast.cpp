#include "ast.hpp"

#include <string>
#include <iostream>
using namespace std;


string Node::Stringify(string tab) {
    return tab + "Ast Node\n";
}

void Node::PrintAll(string tab) {
    cout << Node::Stringify(tab) << endl;
}


/* Primative Types *******************
 * Strings, Characters, Ints & Flts.
 */

String::String(Token t, string s) {
    String::contents = s;
    String::tok = t;
    String::ret_type = STR;
    String::node_id = STR_NODE;
}

string String::Stringify(string tab) {
    return tab + "String Node: " + String::contents + ";"; 
}

Char::Char(Token t, char c) {
    Char::contents = c;
    Char::tok = t;
    Char::ret_type = CHR;
    Char::node_id = CHR_NODE;
}

string Char::Stringify(string tab) {
    return tab + "Character Node: " + string(1, Char::contents) + ";";
}

Int::Int(Token t, int i) {
    Int::contents = i;
    Int::tok = t;
    Int::ret_type = INT;
    Int::node_id = INT_NODE;
}

string Int::Stringify(string tab) {
    return tab + "Integer Node: " + to_string(Int::contents) + ";";
}

Flt::Flt(Token t, double d) {
    Flt::contents = d;
    Flt::tok = t;
    Flt::ret_type = FLT;
    Flt::node_id = FLT_NODE;
}

string Flt::Stringify(string tab) {
    return tab + "Floating Point Node: " + to_string(Flt::contents) + ";";
}

BinaryOperation::BinaryOperation(Token t,
    Node* left,
    Node* right,
    string op,
    enum ReturnType ret_type) {

    BinaryOperation::right = right;
    BinaryOperation::left = left;
    BinaryOperation::op = op;
    BinaryOperation::ret_type = ret_type;
    BinaryOperation::node_id = BINOP_NODE;
}

string BinaryOperation::Stringify(string tab) {
    string ret_val = tab + "Binary Operator '" + op + "'\n";

    ret_val += BinaryOperation::right->Stringify(tab + "\t") + "\n";
    ret_val += BinaryOperation::left->Stringify(tab + "\t");

    return ret_val;
}