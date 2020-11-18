#pragma once

#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/Function.h>

class YAPLStruct {
private:
    std::string m_Name;
    llvm::SmallVector<llvm::Function *, 5> m_Methods;

public:
    YAPLStruct() = default;

    YAPLStruct(llvm::StringRef name) : m_Name(name) {}

    YAPLStruct(const YAPLStruct &other)
        : m_Name(other.getName()), m_Methods(other.getMethods())
    {}

    const std::string &getName() const { return m_Name; }
    void addMethod(llvm::Function* method) { m_Methods.push_back(method); }
    llvm::Function *getMethod(size_t i) { return m_Methods[i]; }
    llvm::SmallVector<llvm::Function *, 5> getMethods() const { return m_Methods; }
};

