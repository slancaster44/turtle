#include <string>
using namespace std;

enum TokenType {
    NULL_TT,
    COLON,
    COMMA,
    EOS,
    ASSIGN,

    PLUS,
    MINUS,
    DIV,
    MUL,

    OR,
    AND,
    NE,
    EQ,

    IDENT,
    IF,
    ELSE,
    FUNC,
    LET,

    STR_LIT,
    INT_LIT,
    CHR_LIT,
    FLT_LIT,
};

class Token {
public:
    Token(int=0, int=0, string="", enum TokenType=NULL_TT);
    string Stringify();
    int Line;
    int Column;
    string Contents;
    enum TokenType Type;
};

bool areToksEqual(Token, Token);