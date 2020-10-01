#include "IRGenerator/Scope.hpp"

std::shared_ptr<YAPLValue> Scope::lookup(llvm::StringRef searchValue) {
    if (m_Values.find(searchValue) == m_Values.end()) {
        if (m_ParentScope) {
            return m_ParentScope->lookup(searchValue);
        }

        return nullptr;
    }
    
    return m_Functions[searchValue];
}

std::shared_ptr<YAPLFunction> Scope::lookupFunction(llvm::StringRef searchFunction) {
    if (m_Functions.find(searchFunction) == m_Functions.end()) {
        if (m_ParentScope) {
            return m_ParentScope->lookupFunction(searchFunction);
        }

        return nullptr;
    }
    
    return m_Functions[searchFunction];
}

llvm::Error Scope::pushValue(llvm::StringRef name, std::shared_ptr<YAPLValue> value) {
    if (m_Values.find(name) != m_Values.end()) {
        return llvm::make_error<llvm::StringError>(name, std::make_error_code(std::errc::executable_format_error));
    }

    m_Values[name] = value;

    return llvm::Error::success();
}

llvm::Error Scope::pushFunction(llvm::StringRef name, std::shared_ptr<YAPLFunction> value) {
    if (m_Functions.find(name) != m_Functions.end()) {
        return llvm::make_error<llvm::StringError>(name, std::make_error_code(std::errc::executable_format_error));
    }

    m_Functions[name] = value;

    return llvm::Error::success();
}
