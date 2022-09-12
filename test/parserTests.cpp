#include <iostream>
using namespace std;

#include "../src/parser.hpp"

#include "parserTests.hpp"
#include "testutil.hpp"


int parsePrimatives() {
    Lexer l("./trtl/parsePrimatives.trtl");
    vector<Token> toks = l.Tokenize();
    Parser p(toks);

    vector<Node*> ast = p.Parse();
    int ret_val = 0;

    if (ast.at(0)->ret_type != INT) {
        cout << ast.at(0)->ret_type << endl;
        ret_val = 1;
    } else if (ast.at(1)->ret_type != STR) {
        cout << ast.at(1)->ret_type << endl;
        ret_val = 2;
    } else if (ast.at(2)->ret_type != CHR) {
        cout << ast.at(2)->ret_type << endl;
        ret_val = 3;
    } else if (ast.at(3)->ret_type != FLT) {
        cout << ast.at(3)->ret_type << endl;
        ret_val = 4;
    } else if (ast.size() != 4) {
        cout << ast.size() << endl;
        ret_val = 5;
    }

    return ret_val;
}

int parseInfix() {
    Lexer l("./trtl/parseInfix.trtl");
    Parser p(l.Tokenize());

    vector<Node*> ast = p.Parse();

    BinaryOperation* firstExpr = dynamic_cast<BinaryOperation*>(ast.at(0));
    Int* firstExprLeft = dynamic_cast<Int*>(firstExpr->left);
    Int* firstExprRight = dynamic_cast<Int*>(firstExpr->right);

    if (firstExpr->op != "+") {
        return 1;
    } else if (firstExprLeft->contents != 2) {
        return 2;
    } else if (firstExprRight->contents != 3) {
        return 3;
    } else if(firstExpr->ret_type != INT) {
        return 4;
    }

    BinaryOperation* secondExpr = dynamic_cast<BinaryOperation*>(ast.at(1));
    if (secondExpr->op != "+") {
        return 4;
    } else if (secondExpr->left->ret_type != INT) {
        return 5;
    }

    Int* secondExprLeft = dynamic_cast<Int*>(secondExpr->left);
    if (secondExprLeft->contents != 2) {
        return 6;
    } else if (secondExpr->right->node_id != BINOP_NODE) {
        return 7;
    }

    BinaryOperation* secondExprRight = dynamic_cast<BinaryOperation*>(secondExpr->right);
    if (secondExprRight->op != "*") {
        return 8;
    } else if (secondExprRight->left->ret_type != INT) {
        return 9;
    } else if (secondExprRight->right->ret_type != INT) {
        return 10;
    }

    Int* shouldBeFour = dynamic_cast<Int*>(secondExprRight->left);
    Int* shouldBeThree = dynamic_cast<Int*>(secondExprRight->right);

    if (shouldBeFour->contents != 4) {
        return 11;
    } else if (shouldBeThree->contents != 3) {
        return 12;
    }

    return 0;
}

void runParserTests() {
    cout << endl << "-- Parser Tests --" << endl;
    failIfNotSuccess(parsePrimatives(), "Parse Primatives Test");
    failIfNotSuccess(parseInfix(), "Parse Infix Test");
    cout << "PASS: All Parser Tests" << endl;
}
