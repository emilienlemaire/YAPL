/**
 * include/Symbol/Type.hpp
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

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace yapl {
    class Type {
    private:
        std::string m_Identifier;
        bool m_IsBaseType = false;
        bool m_IsNumeric = false;
        bool m_IsStruct = false;
        bool m_IsArray = false;
        bool m_IsFunctionType = false;
        bool m_HasName = false;
        bool m_IsArgList = false;
        size_t m_Size = 0;
        std::vector<std::shared_ptr<Type>> m_FieldsTypes;
        std::vector<std::shared_ptr<Type>> m_ParamsTypes;
        std::shared_ptr<Type> m_ElementsType = nullptr;
        std::shared_ptr<Type> m_ReturnType = nullptr;

        friend class SymbolTable;

        Type() = default;

        static std::shared_ptr<Type> CreateBaseType(const std::string&, bool isNumeric = false);
    public:
        Type& operator=(const Type&) = delete;

        bool operator==(const Type&) const;
        bool operator!=(const Type&) const;

        static std::shared_ptr<Type> CreateSimpleType(
                    const std::string&
                );
        static std::shared_ptr<Type> CreateStructType(
                    const std::string&,
                    const std::vector<std::shared_ptr<Type>>&
                );
        static std::shared_ptr<Type> CreateArrayType(
                    size_t,
                    std::shared_ptr<Type>
                );
        static std::shared_ptr<Type> CreateFunctionType(
                    std::shared_ptr<Type>,
                    const std::vector<std::shared_ptr<Type>>&
                );

        static std::shared_ptr<Type> CreateArgumentListType(
                    const std::vector<std::shared_ptr<Type>>&
                );

        static std::string MangleTypeName(const std::shared_ptr<Type>&);
        static std::string MangleTypeName(Type&);
        static std::string MangleTypeName(const Type&);

        static std::string MangleArrayType(
                const std::shared_ptr<Type>&,
                size_t
                );
        static std::string MangleFunctionType(
                    const std::shared_ptr<Type>&,
                    const std::vector<std::shared_ptr<Type>>&
                );
        static std::string MangleArgumentListType(
                    const std::vector<std::shared_ptr<Type>>&
                );

        [[nodiscard]] bool hasName() const { return m_HasName; }
        [[nodiscard]] bool isNumeric() const { return m_IsNumeric; }
        [[nodiscard]] bool isArray() const { return m_IsArray; }
        [[nodiscard]] std::string getIdentifier() const { return m_Identifier; }
        [[nodiscard]] std::vector<std::shared_ptr<Type>> getFieldsType() const { return m_FieldsTypes; }
        [[nodiscard]] std::shared_ptr<Type> getElementsType() const { return m_ElementsType; }
        [[nodiscard]] std::shared_ptr<Type> getReturnType() const { return m_ReturnType; }
    };
} // namespace yapl
