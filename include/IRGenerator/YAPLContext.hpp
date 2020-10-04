#pragma once

#include "IRGenerator/Scope.hpp"

class YAPLContext {
private:
    std::shared_ptr<Scope> m_CurrentScope;

public:
    YAPLContext() {
        m_CurrentScope = Scope::Create(nullptr);
    }
    const std::shared_ptr<Scope> &getCurrentScope() const;
    void pushScope();
    void popScope();

    bool isAtTopLevelScope();
};
