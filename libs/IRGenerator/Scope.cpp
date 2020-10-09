#include "IRGenerator/Scope.hpp"


llvm::Value *Scope::lookupScope(llvm::StringRef searchValue) {
    auto it = m_Values.find(searchValue);
    if (it == m_Values.end()) {
        return nullptr;
    }

    return it->second;
}

llvm::Function *Scope::lookupFunctionScope(llvm::StringRef searchFunction) {
    auto it = m_Functions.find(searchFunction);

    if (it == m_Functions.end()) {
        return nullptr;
    }

    return it->second;
}

llvm::Value *Scope::lookup(llvm::StringRef searchValue) {
    auto it = m_Values.find(searchValue);

    if (it == m_Values.end()) {
        if (m_ParentScope) {
            return m_ParentScope->lookup(searchValue);
        }

        return nullptr;
    }
    
    return it->second;
}

llvm::Function *Scope::lookupFunction(llvm::StringRef searchFunction) {
    auto it = m_Functions.find(searchFunction);
    if (it == m_Functions.end()) {
        if (m_ParentScope) {
            return m_ParentScope->lookupFunction(searchFunction);
        }

        return nullptr;
    }
    
    return it->second;
}

void Scope::setCurrentFunction(llvm::Function *func) {
    m_CurrentFunction = func;
}

llvm::Function *Scope::getCurrentFunction() {
    return m_CurrentFunction;
}

llvm::Error Scope::pushValue(llvm::StringRef name, llvm::Value *value) {
    if (m_Values.find(name) != m_Values.end()) {
        return llvm::make_error<llvm::StringError>(name, std::make_error_code(std::errc::executable_format_error));
    }

    m_Values[name] = value;

    return llvm::Error::success();
}

llvm::Error Scope::pushFunction(llvm::StringRef name, llvm::Function *value) {
    if (m_Functions.find(name) != m_Functions.end()) {
        return llvm::make_error<llvm::StringError>(name, std::make_error_code(std::errc::executable_format_error));
    }

    m_Functions[name] = value;

    return llvm::Error::success();
}
