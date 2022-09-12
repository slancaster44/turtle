#include "parser.hpp"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <memory>


class Generator {
public:
    Generator(vector<Node*> ast);

    void GenCode(); //TODO: Make return valid binary;
    void PrintCode();
private:
    vector<Node*> ast;

    typedef llvm::Value* (Generator::*codeGenFn)(Node*);
    map<NodeID, codeGenFn> jumpTable;

    unique_ptr<llvm::LLVMContext> llvmContext;
    unique_ptr<llvm::IRBuilder<>> llvmBuilder;
    unique_ptr<llvm::Module> llvmModule;
    std::map<std::string, llvm::Value*> symTab;

    void raiseError(string, string, Token);

    llvm::Value* genExprCode(Node*);
    llvm::Value* genIntegerCode(Node*);
};