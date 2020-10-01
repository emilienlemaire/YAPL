#pragma once


#include <string>
#include <system_error>
#include <vector>
#include <parallel_hashmap/phmap.h>

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringMap.h"

#include "llvm/Support/Error.h"
#include "llvm/Object/Error.h"

#include "IRGenerator/YAPLValue.hpp"
#include "IRGenerator/YAPLFunction.hpp"

class Scope {
private:
    std::shared_ptr<Scope> m_ParentScope;
    
    std::vector<std::shared_ptr<Scope>> m_ImportedScopes;

    phmap::parallel_node_hash_map<std::string, std::shared_ptr<YAPLValue>> m_Values;
    phmap::parallel_node_hash_map<std::string, std::shared_ptr<YAPLValue>> m_ImportedValues;
    phmap::parallel_node_hash_map<std::string, std::shared_ptr<YAPLFunction>> m_Functions;
public:
    Scope(std::shared_ptr<Scope> parentScope = nullptr)
        : m_ParentScope(parentScope)
    {}
    static std::shared_ptr<Scope> Create(std::shared_ptr<Scope> parentScope) {
        return std::make_shared<Scope>(parentScope);
    }

    std::shared_ptr<YAPLValue> lookup(llvm::StringRef);
    std::shared_ptr<YAPLFunction> lookupFunction(llvm::StringRef);
    llvm::Error pushValue(llvm::StringRef, std::shared_ptr<YAPLValue>);
    llvm::Error pushFunction(llvm::StringRef, std::shared_ptr<YAPLFunction>);
};
