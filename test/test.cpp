#include "tokenTests.hpp"
#include "lexerTests.hpp"
#include "parserTests.hpp"

int main() {
    runParserTests();
    runLexerTests();
    runTokenTests();
}