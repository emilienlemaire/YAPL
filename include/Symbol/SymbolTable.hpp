/**
 * include/Symbol/SymbolTable.hpp
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
#include <vector>

#include "parallel_hashmap/phmap.h"
#include "Value.hpp"

// FIXME: FIX (MOSTLY) EVERYTHING
namespace yapl {
    class SymbolTable {
    private:
        std::shared_ptr<SymbolTable> m_ParentScope;
        std::vector<std::shared_ptr<SymbolTable>> m_ChildrenScopes;
        phmap::node_hash_map<std::string, std::shared_ptr<Value>> m_Symbols;
        static phmap::node_hash_map<uint64_t, std::shared_ptr<Type>> s_PrimitiveType;
        constexpr static uint64_t intID = 0;
        constexpr static uint64_t floatID = 1;
        constexpr static uint64_t doubleID = 2;
        constexpr static uint64_t boolID = 3;
        constexpr static uint64_t charID = 4;
        constexpr static uint64_t voidID = 5;

    public:
        bool insert(std::shared_ptr<Value> val);
        std::shared_ptr<Value> lookup(const std::string&);
        std::shared_ptr<SymbolTable> pushScope(std::shared_ptr<SymbolTable>);
        std::shared_ptr<SymbolTable> popScope();

        static std::shared_ptr<SymbolTable> InitTopSymTab();

        static std::shared_ptr<Type> GetIntType() {
            return s_PrimitiveType[intID];
        }

        static std::shared_ptr<Type> GetFloatType() {
            return s_PrimitiveType[floatID];
        }

        static std::shared_ptr<Type> GetDoubleType() {
            return s_PrimitiveType[doubleID];
        }

        static std::shared_ptr<Type> GetBoolType() {
            return s_PrimitiveType[boolID];
        }

        static std::shared_ptr<Type> GetCharType() {
            return s_PrimitiveType[charID];
        }

        static std::shared_ptr<Type> GetVoidType() {
            return s_PrimitiveType[voidID];
        }

        constexpr static uint64_t GetIntID() {
            return intID;
        }

        constexpr static uint64_t GetFloatID() {
            return floatID;
        }

        constexpr static uint64_t GetDoubleID() {
            return doubleID;
        }

        constexpr static uint64_t GetBoolID() {
            return boolID;
        }

        constexpr static uint64_t GetCharID() {
            return charID;
        }

        constexpr static uint64_t GetVoidID() {
            return voidID;
        }
    };
} // namespace yapl
