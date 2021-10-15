/**
 * include/Symbol/StructType.hpp
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

#include "Symbol/Type.hpp"
#include "Symbol/ArrayType.hpp"
#include "parallel_hashmap/phmap.h"
#include <string>
#include <vector>

namespace yapl {
    class StructType : public Type {
    private:
        std::string m_Identifier;
        std::vector<Type*> v_ElementsType;
        phmap::flat_hash_map<std::string, uint64_t> m_FieldTypeMap;

        StructType(std::string idenitifier, std::vector<std::string> fieldNames, std::vector<Type*> elementsType);

        [[nodiscard]] virtual bool isEqual(const Type &o) const override;

        friend class Type;
    public:
        [[nodiscard]] virtual size_t hash() const override;
        [[nodiscard]] virtual const std::string dump() const override;

        auto begin() -> decltype(m_FieldTypeMap.begin());
        auto end() -> decltype(m_FieldTypeMap.end());
        auto cbegin() -> decltype(m_FieldTypeMap.cbegin());
        auto cend() -> decltype(m_FieldTypeMap.cend());

        [[nodiscard]] const std::string &getIdentifier() const { return m_Identifier; }
        [[nodiscard]] const std::vector<Type*> &getElementsType() const { return v_ElementsType; }
        [[nodiscard]] uint64_t getFieldIndex(const std::string &field) const { return m_FieldTypeMap.at(field); }
        [[nodiscard]] Type *getElementType(size_t idx) const { return v_ElementsType[idx]; }
        [[nodiscard]] Type *getFieldType(const std::string &name) const;
        [[nodiscard]] bool isField(const std::string &fieldName) const;
        [[nodiscard]] ArrayType *toArrayType();
    };
} // namespace yapl
