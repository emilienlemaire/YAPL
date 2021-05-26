/**
 * include/Symbol/ArrayType.cpp
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

#include "Type.hpp"

namespace yapl {
    class ArrayType : public Type {
    private:
        uint64_t m_NumElements;
        Type *p_ElementsType;

        ArrayType(Type* elementsType, uint64_t numElements);

        virtual bool isEqual(const Type &o) const override;
        friend class Type;
    public:

        [[nodiscard]] virtual size_t hash() const override;
        [[nodiscard]] uint64_t getNumElements() const { return m_NumElements; }
        [[nodiscard]] Type *getElementsType() const { return p_ElementsType; }
    };
} // namespace yapl
