#include "IRGenerator/YAPLContext.hpp"
#include "IRGenerator/Scope.hpp"
#include "parallel_hashmap/phmap.h"
#include <memory>
#include <stdexcept>

const std::shared_ptr<Scope> &YAPLContext::getCurrentScope() const {
    return m_CurrentScope;
}

void YAPLContext::addStruct(YAPLStruct t_Struct) {
    if (m_Structs.find(t_Struct.getName()) != m_Structs.end()) {
        m_Structs[t_Struct.getName()] = YAPLStruct(t_Struct);
    }
}

YAPLStruct YAPLContext::getStruct(llvm::StringRef name) {
    auto it = m_Structs.find(name);

    if(it != m_Structs.end()) {
        return it->second;
    }

    return YAPLStruct(name);
}

void YAPLContext::pushScope() {
    auto newScope = Scope::Create(m_CurrentScope);
    m_CurrentScope = newScope;
}

void YAPLContext::popScope() {
    if (m_CurrentScope->getParentScope()) {
        m_CurrentScope = m_CurrentScope->getParentScope();
    } else {
        throw std::runtime_error("Trying to access a scope to high");
    }
}

bool YAPLContext::isAtTopLevelScope() {
    if (m_CurrentScope->getParentScope()) {
        return false;
    }

    return true;
}

void YAPLContext::addAttributeOffset(llvm::StringRef name, uint32_t offset) {
    m_AttributeOffset[name] = offset;
}

uint32_t YAPLContext::getAttributeOffset(llvm::StringRef name) {
    return m_AttributeOffset[name];
}
