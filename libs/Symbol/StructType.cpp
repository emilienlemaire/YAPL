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

#include "Symbol/Type.hpp"
#include "Symbol/StructType.hpp"
#include "parallel_hashmap/phmap_utils.h"
#include <vector>

namespace yapl {
    StructType::StructType(std::string identifier, std::vector<std::string> fieldsName, std::vector<Type*> elementsType)
        : Type(), m_Identifier(std::move(identifier)), v_ElementsType(std::move(elementsType)),
          m_FieldTypeMap(phmap::flat_hash_map<std::string, uint64_t>())
    {
        if (v_ElementsType.size() == fieldsName.size()) {
            for (size_t i = 0; i < v_ElementsType.size(); i++)
            {
                m_FieldTypeMap[fieldsName[i]] = i;
            }
        }
    }

    auto StructType::begin() -> decltype(v_ElementsType.begin()) {
        return v_ElementsType.begin();
    }

    auto StructType::end() -> decltype(v_ElementsType.end()) {
        return v_ElementsType.end();
    }

    auto StructType::cbegin() -> decltype(v_ElementsType.cbegin()) {
        return v_ElementsType.cbegin();
    }

    auto StructType::cend() -> decltype(v_ElementsType.cend()) {
        return v_ElementsType.begin();
    }

    bool StructType::isEqual(const Type &o) {
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

    const Type *StructType::getFieldType(const std::string &name) const {
        auto idx = m_FieldTypeMap.at(name);

        return v_ElementsType[idx];
    }

    size_t StructType::hash() const {
        auto hash = phmap::HashState().combine(0);
        for (const auto &elemType : v_ElementsType) {
            hash = phmap::HashState().combine(0, hash, *elemType);
        }
        return hash;
    }
} // namespace yapl
