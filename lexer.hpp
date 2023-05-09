#ifndef LEXER_H
#define LEXER_H

#include <map>
#include <string>
#include <unordered_set>
using namespace std;

enum Token_Type {
    INVALID,
    NEWLINE,
    COMMA,
    COLON,
    DOT,

    HEX_INT,
    BIN_INT,
    DEC_INT,

    STR_DQ,
    STR_SQ,
    IDENT,

    LD_TOK,
    NOP_TOK,
};

struct Token {
    string text;
    string filename;
    int lineNo;
    int columnNo;
    Token_Type tag;
};

class Lexer {
private:
    string text;
    string filename;
    int location;

    int lineNo;
    int columnNo;

    char getChar();
    void ungetChar(char c);

    void clearSkipChars();

    Token genString();
    Token genInt();
    Token genHex();
    Token genBin();
    Token genKeyword();
    Token genDoubleChar();
    Token genSingleChar();
protected:
    map<string, Token_Type> doubleCharMap;
    map<string, Token_Type> singleCharMap;
    map<string, Token_Type> keywordMap;
    unordered_set<char> skipChars;

    void panic(string msg);
    void setText(string txt, string file_name);
public:
    Token genToken();
    bool isDone();
};

class Z80ASM_Lexer : public Lexer {
public:
    Z80ASM_Lexer(string file_name, string text);
};

class Turtle_Lexer : public Lexer {};

#endif