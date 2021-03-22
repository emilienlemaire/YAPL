#include "Symbol/Value.hpp"
#include <memory>

namespace yapl {
    std::shared_ptr<Value> Value::CreateTypeValue(
            const std::string &name,
            std::shared_ptr<Type> type
        ) {
        Value v;
        v.m_Kind = ValueKind::Type;
        v.m_Name = std::move(name);
        v.m_Type = type;

        return std::make_shared<Value>(v);
    }

    std::shared_ptr<Value>  Value::CreateVariableValue(const std::string &name, std::shared_ptr<Value> type) {
        Value v;
        v.m_Kind = ValueKind::Variable;
        v.m_Name = std::move(name);
        v.m_TypeValue = type;
        if (type) {
            v.m_Type = type->getType();
        }
        return std::make_shared<Value>(v);
    }

    std::shared_ptr<Value> Value::CreateFunctionValue(
            const std::string &name,
            std::shared_ptr<Value> type,
            std::vector<std::shared_ptr<Value>> parametersType
    ) {
        Value v;
        v.m_Kind = ValueKind::Function;
        v.m_Name = std::move(name);
        v.m_TypeValue = type;
        v.m_Params = parametersType;

        v.m_Name = MangleFunctionName(v);

        return std::make_shared<Value>(v);
    }

    std::string Value::MangleFunctionName(std::shared_ptr<Value> function) {
        auto baseName = function->getName();

        if (function->getKind() != ValueKind::Function) {
            return "";
        }

        auto params = function->m_Params;

        for (const auto &param : params) {
            auto typeName = param->getType()->getIdentifier();
            baseName += typeName.c_str()[0];
        }

        return baseName;
    }

    std::string Value::MangleFunctionName(Value function) {
        auto baseName = function.getName();

        if (function.getKind() != ValueKind::Function) {
            return "";
        }

        auto params = function.m_Params;

        for (const auto &param : params) {
            auto typeName = param->getTypeValue()->getName();
            baseName += typeName.c_str()[0];
        }

        return baseName;
    }
}
