#include "AST/Type.hpp"
#include <vector>

namespace yapl {
    Array::Array(size_t arraySize, StaticType *elementsType)
        : m_Size(arraySize), m_ElementsType(elementsType)
    {}

    size_t Array::getSize() const {
        return m_Size;
    }

    StaticType *Array::getElementsType() const {
        return m_ElementsType;
    }

    Struct::Struct(const std::string &structName, const std::vector<StaticType*> &filedsTypes)
        : m_StructName(std::move(structName)), m_FieldsTypes(filedsTypes)
    {}

    std::vector<StaticType*> Struct::getFieldsTypes() const {
        return m_FieldsTypes;
    }

    std::string Struct::getStructName() const {
        return m_StructName;
    }

    FunctionType::FunctionType(StaticType *returnType, const std::vector<StaticType*>& paramsTypes)
        : m_ReturnType(returnType), m_ParametersTypes(std::move(paramsTypes))
    {}

    StaticType *FunctionType::getReturnType() const {
        return m_ReturnType;
    }

    std::vector<StaticType*> FunctionType::getParametersTypes() const {
        return m_ParametersTypes;
    }
}
