#include "codegen.hpp"

#include <iostream>
using namespace std;

void printToks() {
    Lexer l("./testfile.trtl");
    for (auto t: l.Tokenize()) {
        cout << t.Stringify() << endl;
    }
    cout << "----" << endl << endl;
}

void printNodes() {
    Lexer l("./testfile.trtl");
    Parser p(l.Tokenize());

    for (auto n : p.Parse()) {
        cout << n->node_id << " " << n->ret_type << " " << n->tok.Stringify() << endl;
    }
}

int main() {
    //printToks();
    //printNodes();

    Lexer l("./testfile.trtl");
    Parser p(l.Tokenize());
    Generator g(p.Parse());
    g.GenCode();
    g.PrintCode();

}