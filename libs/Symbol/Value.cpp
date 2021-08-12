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
#include "Symbol/FunctionType.hpp"
#include <memory>

namespace yapl {

    std::shared_ptr<Value> Value::CreateTypeValue(const std::string &name, Type *type) {
        Value v;
        v.m_Kind = ValueKind::Type;
        v.m_Name = name;
        v.m_Type = type;

        return std::make_shared<Value>(v);
    }

    std::shared_ptr<Value>  Value::CreateVariableValue(const std::string &name, Type* type) {
        Value v;
        v.m_Kind = ValueKind::Variable;
        v.m_Name = name;
        v.m_Type = type;
        return std::make_shared<Value>(v);
    }

    std::shared_ptr<Value> Value::CreateFunctionValue(
            const std::string &name,
            FunctionType* type
    ) {
        Value v;
        v.m_Kind = ValueKind::Function;
        v.m_Name = name;
        v.m_FunctionType = type;
        v.m_Type = v.m_FunctionType;
        return std::make_shared<Value>(v);
    }

    void Value::setInnerScope(std::shared_ptr<SymbolTable> scope) {
        m_InnerScope = std::move(scope);
    }
} // namespace yapl
