#include "../src/lexer.hpp"
#include "testutil.hpp"
#include <iostream>
using namespace std;

bool doToksMatch(Token t1, Token t2) {
    return t1.Type == t2.Type && t1.Contents == t2.Contents;
}

int createLexerTest() {
    Lexer l("./trtl/lexerCreation.trtl");

    return 0;
}


#define ASSIGN_TOK Token(0, 0, "=", ASSIGN)
#define EOS_TOK Token(0, 0, ";", EOS)
#define COMMA_TOK Token(0, 0, ",", COMMA)
#define COLON_TOK Token(0, 0, ":", COLON)

int tokenizeSingleCharTest() {
    Lexer l("./trtl/singleCharTest.trtl");
    vector<Token> toks = l.Tokenize();

    int ret_val = 0;
    int expected_len = 9;

    if (!doToksMatch(ASSIGN_TOK, toks.at(0))) {
        cout << toks.at(0).Stringify() << endl;
        ret_val = 1;
    } else if (!doToksMatch(EOS_TOK, toks.at(2))) {
        cout << toks.at(2).Stringify() << endl;
        ret_val = 2;
    } else if (!doToksMatch(COMMA_TOK, toks.at(4))) {
        cout << toks.at(5).Stringify() << endl;
        ret_val = 3;
    } else if (!doToksMatch(COLON_TOK, toks.at(8))) {
        cout << toks.at(8).Stringify() << endl;
        ret_val = 4;
    } else if (toks.size() != expected_len) {
        ret_val = 4;
    }

    if (ret_val != 0) {
        for (auto tok : toks) {
            cout << tok.Stringify() << endl;
        }
        return ret_val;
    }

    return ret_val;
}

#define OR_TOK Token(0, 0, "||", OR)
#define AND_TOK Token(0, 0, "&&", AND)
#define NE_TOK Token(0, 0, "!=", NE)
#define EQ_TOK Token(0, 0, "==", EQ)
int tokenizeDoubleCharTest() {
    Lexer l("./trtl/doubleCharTest.trtl");
    vector<Token> toks = l.Tokenize();

    int ret_val = 0;
    int expected_len = 8;

    if (!doToksMatch(OR_TOK, toks.at(0))) {
        ret_val = 1;
    } else if (!doToksMatch(AND_TOK, toks.at(2))) {
        ret_val = 2;
    } else if (!doToksMatch(NE_TOK, toks.at(4))) {
        ret_val = 3;
    } else if (!doToksMatch(EQ_TOK, toks.at(7))) {
        ret_val = 4;
    } else if (doToksMatch(ASSIGN_TOK, toks.back())) {
        ret_val = 5;
    } else if (toks.size() != expected_len) {
        ret_val = 6;
    }

    if (ret_val != 0) {
        for (auto tok : toks) {
            cout << tok.Stringify() << endl;
        }
        return ret_val;
    }

    return ret_val;
}

#define BANANA Token(0, 0, "banana", IDENT)
#define IF_TOK Token(0, 0, "if", IF)
#define ELSE_TOK Token(0, 0, "else", ELSE)
#define LET_TOK Token(0, 0, "let", LET)
#define IDENT_TOK Token(0, 0, "future", IDENT)
int tokenizeKeywordTest() {
    Lexer l("./trtl/keywordsAndIdents.trtl");
    vector<Token> toks = l.Tokenize();

    int ret_val = 0;
    int expected_len = 10;

    if (!doToksMatch(BANANA, toks.at(0))) {
        ret_val = 1;
    } else if (!doToksMatch(IF_TOK, toks.at(2))) {
        ret_val = 2;
    } else if (!doToksMatch(ELSE_TOK, toks.at(4))) {
        ret_val = 3;
    } else if (!doToksMatch(LET_TOK, toks.at(6))) {
        ret_val = 4;
    } else if (!doToksMatch(IDENT_TOK, toks.at(8))) {
        ret_val = 5;
    } else if (toks.size() != expected_len) {
        ret_val = 6;
    }


    if (ret_val != 0) {
        for (auto tok : toks) {
            cout << tok.Stringify() << endl;
        }
        return ret_val;
    }

    return 0;
}

#define CHR_TOK Token(0, 0, "c", CHR_LIT)
#define STR_TOK Token(0, 0, "string", STR_LIT)
#define INT_TOK Token(0, 0, "13", INT_LIT)
#define FLT_TOK Token(0, 0, "14.5", FLT_LIT)
int tokenizePrimativesTest() {
    Lexer l("./trtl/lexerPrimatives.trtl");
    vector<Token> toks = l.Tokenize();

    int ret_val = 0;
    int expected_len = 7;

    if (!doToksMatch(CHR_TOK, toks.at(0))) {
        ret_val = 1;
    } else if (!doToksMatch(STR_TOK, toks.at(2))) {
        ret_val = 2;
    } else if (!doToksMatch(INT_TOK, toks.at(4))) {
        ret_val = 3;
    } else if (!doToksMatch(FLT_TOK, toks.at(6))) {
        ret_val = 4;
    } else if (toks.size() != expected_len) {
        ret_val = 5;
    }

    if (ret_val != 0) {
        for (auto tok : toks) {
            cout << tok.Stringify() << endl;
        }
        return ret_val;
    }

    return ret_val;
}

int runLexerTests() {
    cout << "-- Lexer Tests --" << endl;
    failIfNotSuccess(createLexerTest(), "Lexer Creation Test");
    failIfNotSuccess(tokenizeSingleCharTest(), "Single Char Tokenization Test");
    failIfNotSuccess(tokenizeDoubleCharTest(), "Double Char Tokenization Test");
    failIfNotSuccess(tokenizeKeywordTest(), "Keyword Tokenization Test");
    failIfNotSuccess(tokenizePrimativesTest(), "Primative Tokenizeation Test");
    cout << "PASS: All Lexer Tests" << endl;

    return 0;
}