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

        virtual bool isEqual(const Type &o) override;

        friend class Type;
    public:
        [[nodiscard]] virtual size_t hash() const override;

        auto begin() -> decltype(v_ElementsType.begin());
        auto end() -> decltype(v_ElementsType.end());
        auto cbegin() -> decltype(v_ElementsType.cbegin());
        auto cend() -> decltype(v_ElementsType.cend());

        [[nodiscard]] const std::string &getIdentifier() const { return m_Identifier; }
        [[nodiscard]] const std::vector<Type*> &getElementsType() const { return v_ElementsType; }
        [[nodiscard]] const Type *getElementType(size_t idx) const { return v_ElementsType[idx]; }
        [[nodiscard]] const Type *getFieldType(const std::string &name) const;
    };
} // namespace yapl
