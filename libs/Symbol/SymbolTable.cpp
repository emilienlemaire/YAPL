/**
 * libs/Symbol/SymbolTable.cpp
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
#include "Symbol/SymbolTable.hpp"
#include "Symbol/Type.hpp"
#include "Symbol/PrimitiveType.hpp"
#include <memory>
#include <utility>

namespace yapl {
    std::shared_ptr<SymbolTable> SymbolTable::InitTopSymTab() {
        auto int_t = Type::CreatePrimitiveType(true);
        auto float_t = Type::CreatePrimitiveType(true);
        auto double_t = Type::CreatePrimitiveType(true);
        auto bool_t = Type::CreatePrimitiveType(false);
        auto char_t = Type::CreatePrimitiveType(false);
        auto void_t = Type::CreatePrimitiveType(false);

        Type::GetOrInsertType(int_t);
        Type::GetOrInsertType(float_t);
        Type::GetOrInsertType(double_t);
        Type::GetOrInsertType(bool_t);
        Type::GetOrInsertType(char_t);
        Type::GetOrInsertType(void_t);

        auto int_v = Value::CreateTypeValue("int", int_t.get());
        auto float_v = Value::CreateTypeValue("float", float_t.get());
        auto double_v = Value::CreateTypeValue("double", double_t.get());
        auto bool_v = Value::CreateTypeValue("bool", bool_t.get());
        auto char_v = Value::CreateTypeValue("char", char_t.get());
        auto void_v = Value::CreateTypeValue("void", void_t.get());

        SymbolTable s;

        s.m_ParentScope = nullptr;
        s.m_ChildrenScopes = std::vector<std::shared_ptr<SymbolTable>>();

        s.insert(int_v);
        s.insert(float_v);
        s.insert(double_v);
        s.insert(bool_v);
        s.insert(char_v);
        s.insert(void_v);

        return std::make_shared<SymbolTable>(s);
    }

    bool SymbolTable::insert(std::shared_ptr<Value> val) {
        std::string name = val->getName();

        if (m_Symbols.find(name) != m_Symbols.end()) {
            return false;
        }

        m_Symbols[name] = std::move(val);

        return true;
    }

    std::shared_ptr<Value> SymbolTable::lookup(const std::string &name) {
        auto it = m_Symbols.find(name);

        if (it == m_Symbols.end()) {
            if (m_ParentScope != nullptr) {
                return m_ParentScope->lookup(name);
            }

            return nullptr;
        }

        return it->second;
    }

    std::shared_ptr<SymbolTable> SymbolTable::pushScope(
            std::shared_ptr<SymbolTable> parent
    ) {
        SymbolTable s;
        s.m_ParentScope = std::move(parent);

        auto s_ptr = std::make_shared<SymbolTable>(s);

        m_ChildrenScopes.push_back(s_ptr);

        return s_ptr;
    }

    std::shared_ptr<SymbolTable> SymbolTable::popScope() {
        return m_ParentScope;
    }
} // namespace yapl
