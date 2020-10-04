#include "IRGenerator/YAPLContext.hpp"
#include "IRGenerator/Scope.hpp"
#include <memory>
#include <stdexcept>

const std::shared_ptr<Scope> &YAPLContext::getCurrentScope() const {
    return m_CurrentScope;
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
