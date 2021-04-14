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

#include <memory>
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

    // FIXME: Review with new types
    class Value {
    private:
        ValueKind m_Kind;
        std::string m_Name;
        Type* m_Type = nullptr; // For semantic analyzer
        FunctionType *m_FunctionType = nullptr;
        std::vector<std::shared_ptr<Value>> m_Params;
        std::shared_ptr<SymbolTable> m_Scope = nullptr;
        std::shared_ptr<SymbolTable> m_InnerScope = nullptr;

        static std::string MangleFunctionName(const Value&);

        Value() = default;
    public:
        static std::shared_ptr<Value> CreateTypeValue(const std::string&, Type*);
        static std::shared_ptr<Value> CreateVariableValue(const std::string&, Type*);
        static std::shared_ptr<Value> CreateFunctionValue(
                const std::string&,
                FunctionType*
            );

        void setInnerScope(std::shared_ptr<SymbolTable>);
        [[nodiscard]] std::string getName() const { return m_Name; }
        [[nodiscard]] ValueKind getKind() const { return m_Kind; }
        [[nodiscard]] Type *getType() const { return m_Type; }
        [[nodiscard]] FunctionType *getFunctionType() const { return m_FunctionType; }
        [[nodiscard]] std::shared_ptr<SymbolTable> getInnerScope() const { return m_InnerScope; }
    };
} // namespace yapl
