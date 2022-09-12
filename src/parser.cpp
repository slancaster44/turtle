#include "parser.hpp"

#include <iostream>
using namespace std;

#define NULL_TOKEN Token(0, 0, "", NULL_TT)



Parser::Parser(vector<Token> t) {
    Parser::Tokens = t;

    Parser::curIter = -1;
    Parser::nextIter = 0;

    PrefixFns[INT_LIT] = &Parser::parseInt; 
    PrefixFns[STR_LIT] = &Parser::parseStr;
    PrefixFns[CHR_LIT] = &Parser::parseChr;
    PrefixFns[FLT_LIT] = &Parser::parseFlt;

    InfixFns[PLUS]  = &Parser::parseBinop;
    InfixFns[MINUS] = &Parser::parseBinop;
    InfixFns[MUL]   = &Parser::parseBinop;
    InfixFns[DIV]   = &Parser::parseBinop;

    precMap[PLUS]   = 1;
    precMap[MINUS]  = 1;
    precMap[MUL]    = 2;
    precMap[DIV]    = 2;

    opInputOutputMap[make_pair(INT, PLUS)] = INT;
    opInputOutputMap[make_pair(INT, MINUS)] = INT;
    opInputOutputMap[make_pair(INT, MUL)] = INT;
    opInputOutputMap[make_pair(INT, DIV)] = INT;

    validOperatorInputs[PLUS] = {INT};
    validOperatorInputs[MINUS] = {INT};
    validOperatorInputs[MUL] = {INT};
    validOperatorInputs[DIV] = {INT};

    Parser::loadTokens();
}

void Parser::raiseError(string et, string m) {
    Token t = Parser::curTok;
    cout << "(" << t.Line << ":" << t.Column << ") -- ";
    cout << et << "Error: " << m << endl;
    cout << t.Stringify() << endl;
    exit(1);
}

vector<Node*> Parser::Parse() {
    while (!areToksEqual(NULL_TOKEN, Parser::curTok)) {
        Parser::Ast.push_back(Parser::parseExpr(0));
    }

    return Parser::Ast;
}

void Parser::skipWhitespace() {
    while (Parser::curTok.Type == EOS) {
        Parser::loadTokens();
    }
}

void Parser::loadTokens() {
    int numOfToks = Parser::Tokens.size();

    Parser::curIter++;
    Parser::nextIter++;

    if (Parser::curIter >= numOfToks) {
        Parser::curTok = NULL_TOKEN;
        Parser::nextTok = NULL_TOKEN;
    } else if (Parser::nextIter == numOfToks) {
        Parser::nextTok = NULL_TOKEN;
        Parser::curTok = Parser::Tokens.at(Parser::curIter);
    } else {
        Parser::nextTok = Parser::Tokens.at(Parser::nextIter);
        Parser::curTok = Parser::Tokens.at(Parser::curIter);
    }
}

Node* Parser::parseExpr(int precedence) {
    Parser::skipWhitespace();

    if (Parser::PrefixFns.find(Parser::curTok.Type) == Parser::PrefixFns.end()) {
        Parser::raiseError("Syntax", "Unexpected prefix");
    }

    auto prefixFunc = Parser::PrefixFns[Parser::curTok.Type];
    Node* parsedValue = (this->*prefixFunc)();

    while (Parser::notDoneParsing(precedence)) {
        if (Parser::InfixFns.find(Parser::curTok.Type) == Parser::InfixFns.end()) {
            return parsedValue;
        }   

        auto infixFunc = Parser::InfixFns[Parser::curTok.Type];
        parsedValue = (this->*infixFunc)(parsedValue);
    }

    Parser::skipWhitespace();
    return parsedValue;
}

bool Parser::notDoneParsing(int precedence) {
    return !areToksEqual(NULL_TOKEN, Parser::curTok) && precedence < Parser::precMap[Parser::curTok.Type];
}

Node* Parser::parseFlt() {
    Token tok = Parser::curTok;

    Flt* ret_val = new Flt(tok, stod(tok.Contents));
    Parser::loadTokens();

    return ret_val;
}

Node* Parser::parseInt() {
    Token tok = Parser::curTok;

    Int* ret_val = new Int(tok, stoi(tok.Contents));
    Parser::loadTokens();

    return ret_val;
}

Node* Parser::parseStr() {
    Token tok = Parser::curTok;

    String* ret_val = new String(tok, tok.Contents);
    Parser::loadTokens();
    
    return ret_val;
}

Node* Parser::parseChr() {
    Token tok = Parser::curTok;

    Char* ret_val = new Char(tok, tok.Contents[0]);
    Parser::loadTokens();

    return ret_val;
}

Node* Parser::parseBinop(Node* left) {
    string op = Parser::curTok.Contents;

    Token tok = Parser::curTok;
    Parser::loadTokens();

    Node* right = Parser::parseExpr(Parser::precMap[tok.Type]);

    if (right->ret_type != left->ret_type) {
        Parser::raiseError("Type", "Mismatched types on either side of binary operator");
    }

    enum ReturnType inputType = right->ret_type;

    vector<ReturnType> validInputsForThisOperator = Parser::validOperatorInputs[tok.Type];
    bool isValidInput = false;
    
    for (ReturnType validInputType: validInputsForThisOperator) {
        if (inputType == validInputType) {
            isValidInput = true;
        }
    }

    if (!isValidInput) {
        Parser::raiseError("Type", "Operator '" + op + "' can not take provided input type");
    }

    enum ReturnType exprRetType = Parser::opInputOutputMap[make_pair(inputType, tok.Type)];

    return new BinaryOperation(tok, left, right, op, exprRetType);
}

