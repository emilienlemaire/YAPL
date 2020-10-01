#pragma once

#include "IRGenerator/YAPLValue.hpp"
#include <vector>

class YAPLFunction : public YAPLValue {
private:
    std::vector<YAPLType> m_ArgType;
public:
    YAPLFunction(llvm::StringRef name, YAPLType returnType, std::vector<YAPLType> argType)
        : YAPLValue(name, returnType), m_ArgType(argType)
    {}
};
