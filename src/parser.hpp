#include "ast.hpp"

#include <vector>
#include <memory>
using namespace std;

class Parser {
public:
    Parser(vector<Token>);
    vector<Node*> Parse();
private:
    vector<Node*> Ast;
    vector<Token> Tokens;

    typedef Node* (Parser::*prefixFn)();
    map<TokenType, prefixFn> PrefixFns;

    typedef Node* (Parser::*infixFn)(Node*);
    map<TokenType, infixFn> InfixFns;

    Node* parseExpr(int precedence);
    bool notDoneParsing(int precedence);

    map<TokenType, int> precMap;

    //Associates operator to valid inputs
    map<TokenType, vector<ReturnType>> validOperatorInputs;

    //Associates Input type with output type
    map<pair<ReturnType, TokenType>, ReturnType> opInputOutputMap;

    Node* parseInt();
    Node* parseFlt();
    Node* parseStr();
    Node* parseChr();
    Node* parseBool();

    Node* parseBinop(Node*);

    void skipWhitespace();
    void raiseError(string, string);

    Token curTok;
    Token nextTok;

    int curIter;
    int nextIter;

    void loadTokens();
};


