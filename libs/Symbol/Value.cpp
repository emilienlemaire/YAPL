/**
 * libs/Symbol/Value.cpp
 * Copyright (c) 2021 Emilien Lemaire <emilien.lem@icloud.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "Symbol/Value.hpp"
#include <memory>
#include <utility>

namespace yapl {
    std::shared_ptr<Value> Value::CreateTypeValue(
            const std::string &name,
            std::shared_ptr<Type> type
        ) {
        Value v;
        v.m_Kind = ValueKind::Type;
        v.m_Name = name;
        v.m_Type = std::move(type);

        return std::make_shared<Value>(v);
    }

    std::shared_ptr<Value>  Value::CreateVariableValue(const std::string &name, std::shared_ptr<Value> type) {
        Value v;
        v.m_Kind = ValueKind::Variable;
        v.m_Name = name;
        if (type) {
            v.m_Type = type->getType();
        }
        v.m_TypeValue = std::move(type);
        return std::make_shared<Value>(v);
    }

    std::shared_ptr<Value> Value::CreateFunctionValue(
            const std::string &name,
            std::shared_ptr<Value> type,
            std::vector<std::shared_ptr<Value>> parametersType
    ) {
        Value v;
        v.m_Kind = ValueKind::Function;
        v.m_Name = name;
        v.m_TypeValue = std::move(type);
        v.m_Params = std::move(parametersType);

        v.m_Name = MangleFunctionName(v);

        return std::make_shared<Value>(v);
    }

    std::string Value::MangleFunctionName(const std::shared_ptr<Value> &function) {
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

    std::string Value::MangleFunctionName(const Value &function) {
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
} // namesapce yapl
