#pragma once

#include <string>
#include <system_error>
#include <vector>
#include <parallel_hashmap/phmap.h>

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"
#include "llvm/Object/Error.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"

#include "IRGenerator/YAPLValue.hpp"
#include "IRGenerator/YAPLFunction.hpp"

class Scope {
private:
    std::shared_ptr<Scope> m_ParentScope;
    
    std::vector<std::shared_ptr<Scope>> m_ImportedScopes;

    llvm::Function *m_CurrentFunction;

    phmap::parallel_node_hash_map<std::string, llvm::Value *> m_Values;
    phmap::parallel_node_hash_map<std::string, llvm::Value *> m_ImportedValues;
    phmap::parallel_node_hash_map<std::string, llvm::Function *> m_Functions;
public:
    Scope(std::shared_ptr<Scope> parentScope = nullptr)
        : m_ParentScope(parentScope)
    {}
    static std::shared_ptr<Scope> Create(std::shared_ptr<Scope> parentScope) {
        return std::make_shared<Scope>(parentScope);
    }

    std::shared_ptr<Scope> getParentScope() { return m_ParentScope; }

    void setCurrentFunction(llvm::Function *);
    llvm::Function *getCurrentFunction();

    llvm::Value *lookupScope(llvm::StringRef);
    llvm::Function *lookupFunctionScope(llvm::StringRef);

    llvm::Value *lookup(llvm::StringRef);
    llvm::Function *lookupFunction(llvm::StringRef);
    
    llvm::Error pushValue(llvm::StringRef, llvm::Value *);
    llvm::Error pushFunction(llvm::StringRef, llvm::Function *);
};
