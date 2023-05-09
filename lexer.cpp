#include "lexer.hpp"

#include <iostream>
#include <sstream>
using namespace std;

#define END_OF_FILE 0

#define INV_TOK Token { "", "", 0, 0, INVALID }

bool Lexer::isDone() {
    return location >= text.size();
}

void Lexer::setText(string txt, string file_name) {
    lineNo = 1;
    columnNo = 1;
    location = 0;

    text = txt;    
    filename = file_name;
}

char Lexer::getChar() {
    if (isDone()) {
        return END_OF_FILE;
    }

    char c = text.at(location++);
    if (c == '\n') {
        lineNo++;
        columnNo = 1;
    } else {
        columnNo ++;
    }
        
    return c;
    
}

void Lexer::ungetChar(char c) {
    if (c == END_OF_FILE) { return; }

    location--;
    if (c == '\n') {
        lineNo--;
    }
    columnNo--;
}

void Lexer::clearSkipChars() {
    char c = getChar();
    while (skipChars.find(c) != skipChars.end()) {
        c = getChar();
    }
    ungetChar(c);
}

#define RET_IF_GOOD \
if (tok.tag != INVALID) { clearSkipChars(); return tok; }

Token Lexer::genToken() {
    Token tok;

    clearSkipChars();

    tok = genDoubleChar();
    RET_IF_GOOD
    
    tok = genSingleChar();
    RET_IF_GOOD

    tok = genHex();
    RET_IF_GOOD

    tok = genBin();
    RET_IF_GOOD

    tok = genInt();
    RET_IF_GOOD

    tok = genString();
    RET_IF_GOOD

    tok = genKeyword();
    RET_IF_GOOD

    stringstream ss;
    ss << "Lexer Error: Cannot lex character '" << getChar() << "'";
    panic(ss.str());
    return INV_TOK;
}

void Lexer::panic(string msg) {
    cerr << filename << ", ";
    cerr << "(" << lineNo << ":" << columnNo << ")";
    cerr << " -- " << msg;
    exit(1);
}

Token Lexer::genString() {
    char c = getChar();
    if (c != '"' && c != '\'') {
        ungetChar(c);
        return INV_TOK;
    }

    char terminator = c;
    c = getChar();
    string output;
    while (c != terminator) {
        if (location > text.size()) {
            panic("Lexer Error: Expected '\"' before EOF");
        }
        output.append(1, c);
        c = getChar();
    }
    getChar(); // skip over close quote

    if (terminator == '\'') {
        return Token { output, filename, lineNo, columnNo, STR_SQ };
    }

    return Token { output, filename, lineNo, columnNo, STR_DQ };
}

Token Lexer::genInt() {
    char c = getChar();
    if (!isdigit(c) || isDone()) { ungetChar(c); return INV_TOK; }

    string s;

    while (isdigit(c) && !(location > text.size())) {
        s.append(1, c);
        c = getChar();
    }
    ungetChar(c);

    return Token {
        s, filename,
        lineNo, columnNo,
        DEC_INT,
    };
}

Token Lexer::genHex() {
    auto isHexDigit = [](char c) {
        return isdigit(c) || 
        (c >= 'a' && c <= 'f') || 
        (c >= 'A' && c <= 'F');
    };

    auto helper = [&]() -> string {
        char c = getChar();

        string s;
        while (isHexDigit(c) && !(location > text.size())) {
            s.append(1, c);
            c = getChar();
        }
        ungetChar(c);

        return s;
    };

    char c1 = getChar();
    if (c1 == '$') {
        string val = helper();
        if (val == "") {
            panic("Lexer Error: Expected hex number after '$'");
        }

        return Token {val, filename, lineNo, columnNo, HEX_INT};
    }

    char c2 = getChar();
    if (c1 == '0' && c2 == 'x') {
        string val = helper();
        if (val == "") {
            panic("Lexer Error: Expected hex number after '0x'");
        }

        return Token {val, filename, lineNo, columnNo, HEX_INT};
    }


    ungetChar(c2);
    ungetChar(c1);
    return INV_TOK;
}

Token Lexer::genBin() {
    char c1 = getChar();
    if (c1 == '%') {
        Token tok = genInt();
        if (tok.tag == INVALID) {
            panic("Lexer Error: Expected number after '%'");
        }

        tok.tag = BIN_INT;
        return tok;
    }

    char c2 = getChar();
    if (c1 == '0' && c2 == 'b') {
        Token tok = genInt();
        if (tok.tag == INVALID) {
            panic("Lexer Error: Expected number after '0b'");
        }

        tok.tag = BIN_INT;
        return tok;
    }

    ungetChar(c2);
    ungetChar(c1);
    return INV_TOK;
}

Token Lexer::genKeyword() {
    char c = getChar();
    if (!isalpha(c)) { ungetChar(c); return INV_TOK; }

    string output;
    while (isalpha(c) || isdigit(c) || c == '_') {
        output.append(1, c);
        c = getChar();
    }
    ungetChar(c);

    if (keywordMap.find(output) != keywordMap.end()) {
        return Token {
            output, filename,
            lineNo, columnNo,
            keywordMap[output],
        };
    }

    return Token { output, filename, lineNo, columnNo, IDENT };
}

Token Lexer::genDoubleChar() {
    if (location == (text.size() - 1)) { return INV_TOK; };

    char c1 = getChar();
    char c2 = getChar();

    string val(1, c1);
    val.push_back(c2);

    if (doubleCharMap.find(val) != doubleCharMap.end()) {
        return Token {
            val, filename,
            lineNo, columnNo,
            doubleCharMap[val]
        };
    }

    ungetChar(c2);
    ungetChar(c1);
    return INV_TOK;
}

Token Lexer::genSingleChar() {
    char c = getChar();
    string val(1, c);

    if (singleCharMap.find(val) != singleCharMap.end()) {
        return Token {
            val, filename, 
            lineNo, columnNo, 
            singleCharMap[val]
        };
    }

    ungetChar(c);
    return INV_TOK;
}

Z80ASM_Lexer::Z80ASM_Lexer(string file_name, string text) {
    setText(text, file_name);

    singleCharMap["\n"] = NEWLINE;
    singleCharMap[":"] = COLON;
    singleCharMap[","] = COMMA;
    singleCharMap["."] = DOT;

    keywordMap["ld"] = LD_TOK;
    keywordMap["nop"] = NOP_TOK;

    skipChars.insert(' ');
    skipChars.insert('\t');
    skipChars.insert('\r');
}
