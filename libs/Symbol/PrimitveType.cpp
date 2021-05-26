/**
 * libs/Symbol/PrimitveType.cpp
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
#include "Symbol/PrimitiveType.hpp"
#include "parallel_hashmap/phmap_utils.h"

namespace yapl {
    uint64_t PrimitiveType::s_TypeID = 0;

    PrimitiveType::PrimitiveType(bool isNumeric)
        :Type(), m_IsNumeric(isNumeric), m_TypeID(s_TypeID++)
    {}

    bool PrimitiveType::isEqual(const Type &o) const {
        if (auto t = dynamic_cast<const PrimitiveType*>(&o)) {
            return m_TypeID == t->m_TypeID;
        }

        return false;
    }

    size_t PrimitiveType::hash() const {
        return phmap::HashState().combine(0, m_IsNumeric, m_TypeID);
    }
} // namespace yapl
