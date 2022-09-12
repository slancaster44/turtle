#include <iostream>
using namespace std;

#include "lexer.hpp"

#define NULL_CHAR 0
#define NULL_TOKEN Token(0, 0, "", NULL_TT)


Lexer::Lexer(string filename) {
    Lexer::hasTokenized = false;

    Lexer::line = 0;
    Lexer::column = 0;

    Lexer::inputFile.open(filename);
    if (!inputFile) {
        Lexer::raiseError("File", "Could not open file '" + filename + "'");
    }

    Lexer::skippedChars.push_back(' ');
    Lexer::skippedChars.push_back('\t');

    Lexer::singleCharMap[':'] = COLON;
    Lexer::singleCharMap[','] = COMMA;
    Lexer::singleCharMap[';'] = EOS;
    Lexer::singleCharMap['\n'] = EOS;
    Lexer::singleCharMap['='] = ASSIGN;
    Lexer::singleCharMap['+'] = PLUS;
    Lexer::singleCharMap['-'] = MINUS;
    Lexer::singleCharMap['*'] = MUL;
    Lexer::singleCharMap['/'] = DIV;

    
    Lexer::doubleCharMap["||"] = OR;
    Lexer::doubleCharMap["&&"] = AND;
    Lexer::doubleCharMap["=="] = EQ;
    Lexer::doubleCharMap["!="] = NE;

    Lexer::keywordMap["if"] = IF;
    Lexer::keywordMap["else"] = ELSE;
    Lexer::keywordMap["func"] = FUNC;
    Lexer::keywordMap["let"] = LET;
}

vector<Token> Lexer::Tokenize() {
    if (Lexer::hasTokenized) {
        return Lexer::tokens;
    }

    inputFile.get(Lexer::curChar);
    inputFile.get(Lexer::nextChar);

    while (Lexer::nextChar != NULL_CHAR) {
        if (Lexer::isSkippedChar(Lexer::curChar)) {
            Lexer::loadChars();
            continue;
        }

        Token t = mkToken();

        if (t.Type == NULL_TT) {
            Lexer::raiseError("Character", "Invalid Character");
        }

        Lexer::tokens.push_back(t);
    }

    

    return Lexer::tokens;
}

void Lexer::loadChars() {
    Lexer::curChar = Lexer::nextChar;

    if (!inputFile.eof()) {
        Lexer::inputFile.get(nextChar);
    } else {
        Lexer::nextChar = NULL_CHAR;
    }
}

Token Lexer::mkToken() {
    //Look up double tok
    string item(1, Lexer::curChar);
    item.push_back(Lexer::nextChar);

    if (Lexer::doubleCharMap.find(item) != Lexer::doubleCharMap.end()) {
        Lexer::loadChars();
        Lexer::loadChars();

        TokenType tt = Lexer::doubleCharMap[item];
        return Token(Lexer::line, Lexer::column, item, tt);

    //look up single tok
    } else if (Lexer::singleCharMap.find(Lexer::curChar) != Lexer::singleCharMap.end()) {
        TokenType tt = Lexer::singleCharMap[Lexer::curChar];
        string contents(1, Lexer::curChar);
        Lexer::loadChars();

        return Token(Lexer::line, Lexer::column, contents, tt);

    } else if (Lexer::isValidAlphaChar(Lexer::curChar)) {
        string ident = Lexer::buildIdent();

        TokenType tt = IDENT;

        if (Lexer::keywordMap.find(ident) != Lexer::keywordMap.end()) {
            tt = Lexer::keywordMap[ident];
        }

        return Token(Lexer::line, Lexer::column, ident, tt);

    } else if (Lexer::curChar == '\'') {
        string chr = Lexer::buildChar();

        return Token(Lexer::line, Lexer::column, chr, CHR_LIT);
    } else if (Lexer::curChar == '"') {
        string str = Lexer::buildString();

        return Token(Lexer::line, Lexer::column, str, STR_LIT);
    } else if (Lexer::isValidNumChar(Lexer::curChar)) {
        tuple<string, bool> ret_val = buildIntegerOrFloat();

        string value = get<0>(ret_val);
        bool isFloat = get<1>(ret_val);
        TokenType tt = isFloat ? FLT_LIT : INT_LIT;

        return Token(Lexer::line, Lexer::column, value, tt);
    }


    //mk identifier/string/int/flt

    return NULL_TOKEN;
}

string Lexer::buildIdent() {
    string out;

    while (isValidIdentChar(nextChar)) {
        out.push_back(Lexer::curChar);
        Lexer::loadChars();
    }

    out.push_back(Lexer::curChar);
    Lexer::loadChars();

    return out;
}

string Lexer::buildChar() {
    string out;
    Lexer::loadChars();

    out.push_back(Lexer::curChar);
    Lexer::loadChars();

    if (Lexer::curChar != '\'') {
        raiseError("Syntax", "Expected closing quotes on character");
    }
    Lexer::loadChars(); //Skip over closing quotes

    return out;
}

string Lexer::buildString() {
    string out;
    Lexer::loadChars();

    while (Lexer::curChar != '"') {
        out.push_back(Lexer::curChar);
        Lexer::loadChars();
    }
    Lexer::loadChars(); //Skip over closing quotes

    return out;
}

tuple <string, bool> Lexer::buildIntegerOrFloat() {
    
    string out;
    bool isFloat = false;

    while ((isValidNumChar(Lexer::curChar) || Lexer::curChar == '.') && Lexer::nextChar != NULL_CHAR) {
        if (Lexer::curChar == '.') {
            isFloat = true;
        }

        out.push_back(Lexer::curChar);
        Lexer::loadChars();
    }

    return make_tuple(out, isFloat);
}

//////////////////////////////////////////////////////////////
void Lexer::raiseError(string err_type, string msg) {
    cout << "(" << Lexer::line << ":" << Lexer::column << ") -- ";
    cout << err_type << "Error: " << msg << std::endl;
    exit(1);
}

bool Lexer::isSkippedChar(char c) {
    for (char skippedChar : Lexer::skippedChars) {
        if (skippedChar == c) {
            return true;
        }
    }

    return false;
}

bool Lexer::isValidAlphaChar(char c) {
    return c >= 'a' && c <='z' || c >= 'A' && c <= 'Z' || c == '_';
}

bool Lexer::isValidNumChar(char c) {
    return c >= '0' && c <= '9';
}

bool Lexer::isValidIdentChar(char c) {
    return isValidNumChar(c) || isValidAlphaChar(c);
}