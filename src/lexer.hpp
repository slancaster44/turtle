#include <string>
#include <map>
#include <fstream>
#include <vector>
using namespace std;

#include "token.hpp"



class Lexer {
public:
    Lexer(string filename);
    vector<Token> Tokenize();
private:
    vector<Token> tokens;
    bool hasTokenized;
    
    ifstream inputFile;

    char curChar;
    char nextChar;

    void loadChars();

    int line;
    int column;

    map<char, enum TokenType> singleCharMap;
    map<string, enum TokenType> doubleCharMap;
    map<string, enum TokenType> keywordMap;

    vector<char> skippedChars;
    bool isSkippedChar(char);

    void raiseError(string err_type, string msg);
    Token mkToken();

    bool isValidIdentChar(char c);
    bool isValidNumChar(char c);
    bool isValidAlphaChar(char c);

    string buildIdent();
    //Boolean indicates if this returns an int or a flt
    tuple<string, bool> buildIntegerOrFloat();
    string buildString();
    string buildChar();
};

