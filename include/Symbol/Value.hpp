/**
 * include/Symbol/Value.hpp
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
#pragma once

#include <string>
#include <vector>

#include "Type.hpp"

namespace yapl {
    class SymbolTable;

    enum class ValueKind {
        Constant,
        Variable,
        Function,
        Type,
        Method
    };

    class Value {
    private:
        ValueKind m_Kind;
        std::string m_Name;
        std::shared_ptr<Type> m_Type = nullptr; // For semantic analyzer
        std::shared_ptr<Value> m_TypeValue; // For parser
        std::vector<std::shared_ptr<Value>> m_Params;
        std::shared_ptr<SymbolTable> m_Scope = nullptr;

        static std::string MangleFunctionName(Value);

        Value() = default;
        Value(ValueKind, const std::string&, Type);
    public:
        static std::shared_ptr<Value> CreateTypeValue(const std::string&, std::shared_ptr<Type>);
        static std::shared_ptr<Value> CreateVariableValue(const std::string&, std::shared_ptr<Value>);
        static std::shared_ptr<Value> CreateFunctionValue(
                const std::string&,
                std::shared_ptr<Value>,
                std::vector<std::shared_ptr<Value>>
            );

        static std::string MangleFunctionName(std::shared_ptr<Value>);

        [[nodiscard]] std::string getName() const { return m_Name; }
        [[nodiscard]] ValueKind getKind() const { return m_Kind; }
        [[nodiscard]] std::shared_ptr<Type> getType() const { return m_Type; }
        [[nodiscard]] std::shared_ptr<Value> getTypeValue() const {return m_TypeValue; }
    };
}
