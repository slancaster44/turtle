#include "token.hpp"
#include <string>
using namespace std;

Token::Token(int ln, int cn, string c, enum TokenType t) {
    Token::Line = ln;
    Token::Column = cn;
    Token::Contents = c;
    Token::Type = t;
} 

string Token::Stringify() {
    return "{" + Token::Contents + " " + to_string(Token::Type) + "}";
}

bool areToksEqual(Token t1, Token t2)  {
    return t1.Contents == t2.Contents && t1.Type == t2.Type;
}