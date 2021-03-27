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
        bool m_IsStruct = false;
        bool m_IsArray = false;
        bool m_IsFunctionType = false;
        bool m_HasName = false;
        size_t m_Size = 0;
        std::vector<std::shared_ptr<Type>> m_FieldsTypes;
        std::vector<std::shared_ptr<Type>> m_ParamsTypes;
        std::shared_ptr<Type> m_ElementsType = nullptr;
        std::shared_ptr<Type> m_ReturnType = nullptr;

        friend class SymbolTable;

        Type() = default;

        static std::shared_ptr<Type> CreateBaseType(const std::string&);
    public:
        Type& operator=(const Type&) = delete;

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
        static std::string MangleTypeName(const std::shared_ptr<Type>&);
        static std::string MangleTypeName(Type&);

        static std::string MangleArrayType(
                const std::shared_ptr<Type>&,
                size_t
                );
        static std::string MangleFunctionType(
                    const std::shared_ptr<Type>&,
                    const std::vector<std::shared_ptr<Type>>&
                );

        [[nodiscard]] bool hasName() const { return m_HasName; }
        [[nodiscard]] std::string getIdentifier() const { return m_Identifier; }
    };
} // namespace yapl
