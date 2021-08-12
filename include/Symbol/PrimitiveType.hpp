/**
 * include/Symbol/PrimitiveType.hpp
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

#include "Symbol/SymbolTable.hpp"
#include "Type.hpp"

namespace yapl {
    class PrimitiveType : public Type {
    private:
        static uint64_t s_TypeID;
        uint64_t m_TypeID;

        bool m_IsNumeric;
        explicit PrimitiveType(bool isNumeric);

        [[nodiscard]] virtual bool isEqual(const Type &o) const override;

        friend class Type;
        friend class SymbolTable;

    public:
        [[nodiscard]] virtual size_t hash() const override;
        [[nodiscard]] virtual const std::string dump() const override;
        [[nodiscard]] bool isNumeric() const { return m_IsNumeric; }
        [[nodiscard]] uint64_t getTypeID() const { return m_TypeID; }
    };
} // namespace yapl
