/**
 * libs/Symbol/StructType.cpp
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

#include "Symbol/ArrayType.hpp"
#include "Symbol/Type.hpp"
#include "Symbol/StructType.hpp"
#include "parallel_hashmap/phmap_utils.h"
#include <iostream>
#include <vector>

namespace yapl {
    StructType::StructType(
        std::string identifier,
        std::vector<std::string> fieldsName,
        std::vector<Type*> elementsType
    ):
        Type(),
        m_Identifier(identifier),
        v_ElementsType(elementsType),
        m_FieldTypeMap(phmap::flat_hash_map<std::string, uint64_t>())
    {
        if (v_ElementsType.size() == fieldsName.size()) {
            for (size_t i = 0; i < v_ElementsType.size(); i++)
            {
                m_FieldTypeMap[fieldsName[i]] = i;
            }
        }
    }

    auto StructType::begin() -> decltype(m_FieldTypeMap.begin()) {
        return m_FieldTypeMap.begin();
    }

    auto StructType::end() -> decltype(m_FieldTypeMap.end()) {
        return m_FieldTypeMap.end();
    }

    auto StructType::cbegin() -> decltype(m_FieldTypeMap.cbegin()) {
        return m_FieldTypeMap.cbegin();
    }

    auto StructType::cend() -> decltype(m_FieldTypeMap.cend()) {
        return m_FieldTypeMap.begin();
    }

    bool StructType::isEqual(const Type &o) const {
        if (auto t = dynamic_cast<const StructType*>(&o)) {
            if (v_ElementsType.size() == t->getElementsType().size()) {
                for (size_t i = 0; i < v_ElementsType.size(); i++)
                {
                    if (*(v_ElementsType[i]) != *(t->getElementType(i))) {
                        return false;
                    }
                }

                if (m_Identifier != t->getIdentifier()) {
                    return false;
                }

                return true;
            }
        }

        return false;
    }

    Type *StructType::getFieldType(const std::string &name) const {
        if (m_FieldTypeMap.contains(name)) {
            auto idx = m_FieldTypeMap.at(name);

            return v_ElementsType[idx];
        }
        return nullptr;
    }

    size_t StructType::hash() const {
        auto hash = phmap::HashState().combine(0);
        for (const auto &elemType : v_ElementsType) {
            hash = phmap::HashState().combine(0, hash, *elemType);
        }
        return hash;
    }

    bool StructType::isField(const std::string &fieldName) const {
        return m_FieldTypeMap.contains(fieldName);
    }

    ArrayType *StructType::toArrayType() {
        auto firstElementType = v_ElementsType[0];

        for (auto elemType : v_ElementsType) {
            if (*elemType != *firstElementType) {
                std::cerr << "The types don't match" << std::endl;
                return nullptr;
            }
        }

        if (auto structType = dynamic_cast<StructType*>(firstElementType)) {
            std::cerr << "Trying to convert the inner init list" << std::endl;
            firstElementType = structType->toArrayType();
            if (!firstElementType) {
                std::cerr << "The inner types don't match" << std::endl;
                return nullptr;
            }
        }

        const auto size = v_ElementsType.size();

        auto arrType = Type::CreateArrayType(firstElementType, size);
        Type::GetOrInsertType(arrType);

        return arrType.get();
    }
} // namespace yapl
