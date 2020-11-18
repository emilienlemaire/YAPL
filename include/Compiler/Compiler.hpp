#pragma once

#include <llvm/IR/Module.h>

class Compiler {
private:

public:
    void compile(llvm::Module*, llvm::StringRef);
};
