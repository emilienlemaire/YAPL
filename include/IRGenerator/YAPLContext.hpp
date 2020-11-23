#pragma once

#include "IRGenerator/Scope.hpp"
#include "IRGenerator/YAPLStruct.hpp"
#include "parallel_hashmap/phmap.h"
#include <llvm/ADT/StringMap.h>
#include <llvm/IR/Instructions.h>

struct ReturnHelper {
    llvm::BasicBlock *returnBlock = nullptr;
    llvm::PHINode *returnNode = nullptr;
};

class YAPLContext {
private:
    std::shared_ptr<Scope> m_CurrentScope;
    typedef phmap::flat_hash_map<std::string, YAPLStruct> structMap;
    structMap m_Structs;
    llvm::StringMap<uint32_t> m_AttributeOffset;
    ReturnHelper m_ReturnHelper;

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
    void resetReturnHelper();
    void setReturnHelper(llvm::BasicBlock*, llvm::PHINode*);
    void addPhiNodeIncomming(llvm::BasicBlock*, llvm::Value*);
    llvm::BasicBlock *getReturnBlock() const { return m_ReturnHelper.returnBlock; }

    bool isAtTopLevelScope();
};
