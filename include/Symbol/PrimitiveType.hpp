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

#include "Type.hpp"

namespace yapl {
    class PrimitiveType : public Type {
    private:
        static uint64_t s_TypeID;
        uint64_t m_TypeID;

        bool m_IsNumeric;
        explicit PrimitiveType(bool isNumeric);

        virtual bool isEqual(const Type &o) override;

        friend class Type;

    public:
        [[nodiscard]] virtual size_t hash() const override;
    };
} // namespace yapl
