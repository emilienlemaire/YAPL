#pragma once

#include "IRGenerator/Scope.hpp"

class YAPLContext {
private:
    Scope *m_CurrentScope;

public:
    Scope *getCurrentScope();
    void pushScope();
};
