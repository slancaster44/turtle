#include "codegen.hpp"
#include <cstddef>
#include <llvm/ADT/APInt.h>
#include <llvm/IR/Constants.h>

#include <iostream>
#include <llvm/Support/raw_ostream.h>
using namespace std;


Generator::Generator(vector<Node*> ast) {
    Generator::ast = ast;
    Generator::llvmContext = make_unique<llvm::LLVMContext>();
    Generator::llvmBuilder = make_unique<llvm::IRBuilder<>>(*llvmContext);
    Generator::llvmModule = make_unique<llvm::Module>("turtle_main", *llvmContext);
    Generator::jumpTable[INT_NODE] = &Generator::genIntegerCode;
};

void Generator::raiseError(string et, string m, Token t) {
    cout << "(" << t.Line << ":" << t.Column << ") -- ";
    cout << et << "Error: " << m;
    cout << " '" << t.Contents << "'" << "<" << t.Type << ">" << endl;
    exit(1);
}

void Generator::GenCode() {
    for (Node* n: Generator::ast) {
        genExprCode(n);
    }
}

void Generator::PrintCode() {
    Generator::llvmModule->print(llvm::outs(), nullptr);
}

/* Generates code for a node type, returns any return value, and
 * deletes the node.
 */
llvm::Value* Generator::genExprCode(Node* n) {
    if (Generator::jumpTable.find(n->node_id) == Generator::jumpTable.end()) {
        Generator::raiseError("Generation", "Unknown expression", n->tok);
    }

    codeGenFn fn = jumpTable[n->node_id];
    llvm::Value* ret_val = (this->*fn)(n);

    delete n;

    return ret_val;
}

llvm::Value* Generator::genIntegerCode(Node* n) {
    Int* intNode = dynamic_cast<Int*>(n);

    unsigned bitWidth = llvm::APInt::APINT_BITS_PER_WORD; //TODO: Is this a full 64-bits?
    llvm::APInt llvmVal(bitWidth, intNode->contents);

    return llvm::ConstantInt::get(*Generator::llvmContext, llvmVal);
}