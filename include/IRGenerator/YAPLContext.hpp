#pragma once

#include "IRGenerator/Scope.hpp"
#include "IRGenerator/YAPLStruct.hpp"
#include "parallel_hashmap/phmap.h"
#include <llvm/ADT/StringMap.h>

class YAPLContext {
private:
    std::shared_ptr<Scope> m_CurrentScope;
    typedef phmap::flat_hash_map<std::string, YAPLStruct> structMap;
    structMap m_Structs;
    llvm::StringMap<uint32_t> m_AttributeOffset;

public:
    YAPLContext() {
        m_CurrentScope = Scope::Create(nullptr);
    }
    const std::shared_ptr<Scope> &getCurrentScope() const;

    void addStruct(YAPLStruct);
    YAPLStruct getStruct(llvm::StringRef);
    void pushScope();
    void popScope();
    void addAttributeOffset(llvm::StringRef, uint32_t);
    uint32_t getAttributeOffset(llvm::StringRef);

    bool isAtTopLevelScope();
};
