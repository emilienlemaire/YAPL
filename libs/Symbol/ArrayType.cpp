/**
 * libs/Symbol/ArrayType.cpp
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
#include "Symbol/ArrayType.hpp"
#include "parallel_hashmap/phmap_utils.h"
#include <string>

namespace yapl {

    ArrayType::ArrayType(Type *elementsType, uint64_t numElements)
        : Type(), p_ElementsType(elementsType), m_NumElements(numElements)
    {}

    bool ArrayType::isEqual(const Type &o) const {
        if (auto t = dynamic_cast<const ArrayType*>(&o)) {
            return (t->getElementsType()->isEqual(*p_ElementsType)) && t->getNumElements() == m_NumElements;
        }

        return false;
    }

    size_t ArrayType::hash() const {
        return phmap::HashState().combine(0, *p_ElementsType, m_NumElements);
    }

    const std::string ArrayType::dump() const {
        return "Array of (" + std::to_string(m_NumElements) + " " + p_ElementsType->dump() + ")";
    }
} // namespace yapl
