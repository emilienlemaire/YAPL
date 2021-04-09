/**
 * libs/Symbol/Type.cpp
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
#include <exception>
#include <memory>
#include <string>
#include <utility>

#include "Symbol/Type.hpp"

namespace yapl {

    bool Type::operator==(const Type &other) const {
        if (other.hasName() && hasName()) {
            return other.getIdentifier() == m_Identifier;
        }

        if (hasName()) {
            auto otherName = MangleTypeName(other);

            return otherName == m_Identifier;
        }

        if (other.hasName()) {
            auto thisName = MangleTypeName(*this);

            return other.getIdentifier() == thisName;
        }

        auto thisName = MangleTypeName(*this);
        auto otherName = MangleTypeName(other);

        return otherName == thisName;
    }

    bool Type::operator!=(const Type &other) const {
        return !(*this == other);
    }
    std::shared_ptr<Type> Type::CreateBaseType(const std::string &name, bool isNumeric) {
        Type t;

        t.m_HasName = true;
        t.m_IsBaseType = true;
        t.m_IsNumeric = isNumeric;
        t.m_Identifier = name;

        return std::make_shared<Type>(t);
    }

    std::shared_ptr<Type> Type::CreateSimpleType(const std::string &name) {
        Type t;
        t.m_HasName = true;
        t.m_Identifier = name;

        return std::make_shared<Type>(t);
    }

    std::shared_ptr<Type> Type::CreateStructType(
            const std::string &name,
            const std::vector<std::shared_ptr<Type>> &fields
    ) {
        Type t;

        t.m_HasName = true;
        t.m_Identifier = name;
        t.m_FieldsTypes = fields;
        t.m_IsStruct = true;
        t.m_HasName = true;

        return std::make_shared<Type>(t);
    }

    std::shared_ptr<Type> Type::CreateArrayType(size_t size, std::shared_ptr<Type> elementsType) {
        Type t;

        t.m_IsArray = true;
        t.m_ElementsType = std::move(elementsType);
        t.m_Size = size;

        t.m_HasName = true;
        t.m_Identifier = MangleArrayType(elementsType, size);

        return std::make_shared<Type>(t);
    }

    std::shared_ptr<Type> Type::CreateFunctionType(
            std::shared_ptr<Type> retType,
            const std::vector<std::shared_ptr<Type>> &paramsType) {
        Type t;

        t.m_IsFunctionType = true;
        t.m_ReturnType = std::move(retType);
        t.m_ParamsTypes = paramsType;

        t.m_HasName = true;
        t.m_Identifier = MangleFunctionType(retType, paramsType);

        return std::make_shared<Type>(t);
    }

    std::shared_ptr<Type> Type::CreateArgumentListType(const std::vector<std::shared_ptr<Type>> &types) {
        Type t;
        t.m_IsArgList = true;
        t.m_FieldsTypes = types;

        t.m_HasName = true;
        t.m_Identifier = MangleArgumentListType(types);

        return std::make_shared<Type>(t);
    }

    std::string Type::MangleTypeName(const std::shared_ptr<Type>& type) {
        return MangleTypeName(*type);
    }

    std::string Type::MangleTypeName(Type& type) {
        if (type.m_HasName) {
            return type.m_Identifier;
        }

        if (type.m_IsArray) {
            std::string mangle = "Arr";
            if (!type.m_ElementsType->hasName()) {
                MangleTypeName(type.m_ElementsType);
            }
            mangle += std::to_string(type.m_Size);
            mangle += type.m_ElementsType->getIdentifier();

            type.m_HasName = true;
            type.m_Identifier = mangle;

            return mangle;
        }

        if (type.m_IsFunctionType) {
            std::string mangle = "Func";
            if (!type.m_ReturnType->hasName()) {
                MangleTypeName(type.m_ReturnType);
            }
            mangle += type.m_ReturnType->getIdentifier();

            for(const auto& paramType : type.m_ParamsTypes) {
                if (!paramType->hasName()) {
                    MangleTypeName(paramType);
                }
                mangle += paramType->getIdentifier();
            }

            type.m_HasName = true;
            type.m_Identifier = mangle;

            return mangle;
        }

        if (type.m_IsArgList) {
            std::string mangle = "ArgList";

            for(const auto &argType : type.m_FieldsTypes)
            {
                if (!argType->hasName()) {
                    MangleTypeName(argType);
                }
                mangle += argType->getIdentifier();
            }

            type.m_HasName = true;
            type.m_Identifier = mangle;

            return mangle;
        }

        return "";
    }

    std::string Type::MangleTypeName(const Type& type) {
        if (type.m_HasName) {
            return type.m_Identifier;
        }

        if (type.m_IsArray) {
            std::string mangle = "Arr";
            if (!type.m_ElementsType->hasName()) {
                MangleTypeName(type.m_ElementsType);
            }
            mangle += std::to_string(type.m_Size);
            mangle += type.m_ElementsType->getIdentifier();

            return mangle;
        }

        if (type.m_IsFunctionType) {
            std::string mangle = "Func";
            if (!type.m_ReturnType->hasName()) {
                MangleTypeName(type.m_ReturnType);
            }
            mangle += type.m_ReturnType->getIdentifier();

            for(const auto& paramType : type.m_ParamsTypes) {
                if (!paramType->hasName()) {
                    MangleTypeName(paramType);
                }
                mangle += paramType->getIdentifier();
            }

            return mangle;
        }

        if (type.m_IsArgList) {
            std::string mangle = "Func";

            for(const auto& argType : type.m_FieldsTypes) {
                if (!argType->hasName()) {
                    MangleTypeName(argType);
                }
                mangle += argType->getIdentifier();
            }
        }

        return "";
    }

    std::string Type::MangleArrayType(const std::shared_ptr<Type> &elementsType, size_t size) {
            std::string mangle = "Arr";
            if (!elementsType->hasName()) {
                MangleTypeName(elementsType);
            }
            mangle += std::to_string(size);
            mangle += elementsType->getIdentifier();

            return mangle;
    }

    std::string Type::MangleFunctionType(
        const std::shared_ptr<Type> &retType,
        const std::vector<std::shared_ptr<Type>> &paramsTypes
    ) {
            std::string mangle = "Func";
            if (!retType->hasName()) {
                MangleTypeName(retType);
            }
            mangle += retType->getIdentifier();

            for(const auto& paramType : paramsTypes) {
                if (!paramType->hasName()) {
                    MangleTypeName(paramType);
                }
                mangle += paramType->getIdentifier();
            }

            return mangle;
    }

    std::string Type::MangleArgumentListType(
        const std::vector<std::shared_ptr<Type>> &paramsTypes
    ) {
            std::string mangle = "ArgLis";

            for(const auto& paramType : paramsTypes) {
                if (!paramType->hasName()) {
                    MangleTypeName(paramType);
                }
                mangle += paramType->getIdentifier();
            }

            return mangle;
    }
} // namespace yapl
