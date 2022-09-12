#include "../src/token.hpp"
#include "testutil.hpp"

#include <iostream>
using namespace std;

int createToken() {
    Token t = Token(0, 1, "\n", EOS);

    if (t.Line != 0) {
        return 1;
    } else if (t.Column != 1) {
        return 2;
    } else if (t.Contents != "\n") {
        return 3;
    } else if (t.Type != EOS) {
        return 4;
    }

    return 0; 
}

void runTokenTests() {    
    cout << "-- Token Tests --" << endl;
    failIfNotSuccess(createToken(), "Token Creation Test");
    cout << "PASS: All Token Tests" << endl;
}