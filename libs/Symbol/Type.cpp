#include "Symbol/Type.hpp"
#include <exception>
#include <memory>
#include <string>

namespace yapl {

    std::shared_ptr<Type> Type::CreateBaseType(const std::string &name) {
        Type t;

        t.m_HasName = true;
        t.m_IsBaseType = true;
        t.m_Identifier = std::move(name);

        return std::make_shared<Type>(t);
    }

    std::shared_ptr<Type> Type::CreateSimpleType(const std::string &name) {
        Type t;
        t.m_Identifier = std::move(name);
        
        return std::make_shared<Type>(t);
    }

    std::shared_ptr<Type> Type::CreateStructType(
            const std::string &name,
            const std::vector<std::shared_ptr<Type>> &fields
    ) {
        Type t;

        t.m_Identifier = std::move(name);
        t.m_FieldsTypes = std::move(fields);
        t.m_IsStruct = true;
        t.m_HasName = true;

        return std::make_shared<Type>(t);
    }

    std::shared_ptr<Type> Type::CreateArrayType(size_t size, std::shared_ptr<Type> elementsType) {
        Type t;

        t.m_IsArray = true;
        t.m_ElementsType = elementsType;
        t.m_Size = size;

        return std::make_shared<Type>(t);
    }

    std::shared_ptr<Type> Type::CreateFunctionType(
            std::shared_ptr<Type> retType,
            const std::vector<std::shared_ptr<Type>> &paramsType) {
        Type t;

        t.m_IsFunctionType = true;
        t.m_ReturnType = retType;
        t.m_ParamsTypes = paramsType;

        return std::make_shared<Type>(t);
    }

    std::string Type::MangleTypeName(std::shared_ptr<Type> type) {
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

            for(auto paramType : type.m_ParamsTypes) {
                if (!paramType->hasName()) {
                    MangleTypeName(paramType);
                }
                mangle += paramType->getIdentifier();
            }

            type.m_HasName = true;
            type.m_Identifier = mangle;

            return mangle;
        }
    }

    std::string Type::MangleArrayType(std::shared_ptr<Type> elementsType, size_t size) {
            std::string mangle = "Arr";
            if (!elementsType->hasName()) {
                MangleTypeName(elementsType);
            }
            mangle += std::to_string(size);
            mangle += elementsType->getIdentifier();

            return mangle;
    }

    std::string Type::MangleFunctionType(
        std::shared_ptr<Type> retType,
        std::vector<std::shared_ptr<Type>> paramsTypes
    ) {
            std::string mangle = "Func";
            if (!retType->hasName()) {
                MangleTypeName(retType);
            }
            mangle += retType->getIdentifier();

            for(auto paramType : paramsTypes) {
                if (!paramType->hasName()) {
                    MangleTypeName(paramType);
                }
                mangle += paramType->getIdentifier();
            }

            return mangle;
    }
}
