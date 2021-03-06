/**
 * libs/AST/ASTExprNode.cpp
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
#include <memory>
#include <string>
#include <vector>

#include "AST/ASTExprNode.hpp"

namespace yapl {

    // Virtual classes
    ASTExprNode::ASTExprNode(SharedScope scope)
        : ASTNode(scope)
    {}

    ASTAssignableExpr::ASTAssignableExpr(SharedScope scope)
        : ASTExprNode(scope)
    {}

    ASTCallableExpr::ASTCallableExpr(SharedScope scope)
        : ASTAssignableExpr(scope)
    {}

    ASTAccessibleExpr::ASTAccessibleExpr(SharedScope scope)
        : ASTAssignableExpr(scope)
    {}

    ASTNumberExpr::ASTNumberExpr(SharedScope scope)
        : ASTExprNode(scope)
    {}

    // Non virtual classes
    ASTArgumentList::ASTArgumentList(SharedScope scope)
        : ASTExprNode(scope)
    {}

    void ASTArgumentList::setArguments(const ASTArgumentList::vectorType &arguments) {
        m_Arguments = std::move(arguments);
    }

    void ASTArgumentList::addArgument(std::unique_ptr<ASTExprNode> argument) {
        m_Arguments.emplace_back(std::move(argument));
    }

    const ASTArgumentList::vectorType &ASTArgumentList::getArguments() const {
        return m_Arguments;
    }

    ASTBoolLiteralExpr::ASTBoolLiteralExpr(SharedScope scope)
        : ASTExprNode(scope)
    {}

    void ASTBoolLiteralExpr::setValue(bool value) {
        m_Value = value;
    }

    bool ASTBoolLiteralExpr::getValue() const {
        return m_Value;
    }

    ASTBinaryExpr::ASTBinaryExpr(SharedScope scope)
        : ASTExprNode(scope)
    {}

    void ASTRangeExpr::setStart(std::unique_ptr<ASTExprNode> start) {
        m_Start = std::move(start);
    }

    void ASTRangeExpr::setEnd(std::unique_ptr<ASTExprNode> end) {
        m_End = std::move(end);
    }

    const ASTExprNode *ASTRangeExpr::getStart() const {
        return m_Start.get();
    }

    const ASTExprNode *ASTRangeExpr::getEnd() const {
        return m_End.get();
    }

    ASTFloatNumberExpr::ASTFloatNumberExpr(SharedScope scope)
        :ASTNumberExpr(scope)
    {}

    void ASTFloatNumberExpr::setValue(float value) {
        m_Value = value;
    }

    float ASTFloatNumberExpr::getValue() const {
        return m_Value;
    }

    ASTDoubleNumberExpr::ASTDoubleNumberExpr(SharedScope scope)
        :ASTNumberExpr(scope)
    {}

    void ASTDoubleNumberExpr::setValue(double value) {
        m_Value = value;
    }

    double ASTDoubleNumberExpr::getValue() const {
        return m_Value;
    }

    ASTIntegerNumberExpr::ASTIntegerNumberExpr(SharedScope scope)
        : ASTNumberExpr(scope)
    {}

    void ASTIntegerNumberExpr::setValue(int value) {
        m_Value = value;
    }

    int ASTIntegerNumberExpr::getValue() const {
        return m_Value;
    }

    ASTIdentifierExpr::ASTIdentifierExpr(SharedScope scope)
        : ASTCallableExpr(scope)
    {}

    void ASTIdentifierExpr::setIdentifier(const std::string &identifier) {
        m_Identifier = std::move(identifier);
    }

    std::string ASTIdentifierExpr::getIdentifier() const {
        return m_Identifier;
    }

    ASTAttributeAccessExpr::ASTAttributeAccessExpr(SharedScope scope)
        : ASTCallableExpr(scope)
    {}

    void ASTAttributeAccessExpr::setStruct(std::unique_ptr<ASTAccessibleExpr> t_Struct) {
        m_Struct = std::move(t_Struct);
    }

    void ASTAttributeAccessExpr::setAttribute(std::unique_ptr<ASTIdentifierExpr> attribute) {
        m_Attribute = std::move(attribute);
    }

    const ASTAccessibleExpr *ASTAttributeAccessExpr::getStruct() const {
        return m_Struct.get();
    }

    const ASTIdentifierExpr *ASTAttributeAccessExpr::getAttribute() const {
        return m_Attribute.get();
    }

    ASTArrayAccessExpr::ASTArrayAccessExpr(SharedScope scope)
        :ASTAssignableExpr(scope)
    {}

    void ASTArrayAccessExpr::setArray(std::unique_ptr<ASTAccessibleExpr> array) {
        m_Array = std::move(array);
    }

    void ASTArrayAccessExpr::setIndex(std::unique_ptr<ASTExprNode> index) {
        m_Index = std::move(index);
    }

    const ASTAccessibleExpr *ASTArrayAccessExpr::getArray() const {
        return m_Array.get();
    }

    const ASTExprNode *ASTArrayAccessExpr::getIndex() const {
        return m_Index.get();
    }

    ASTFunctionCallExpr::ASTFunctionCallExpr(SharedScope scope)
        :ASTAccessibleExpr(scope)
    {}

    void ASTFunctionCallExpr::setFunction(std::unique_ptr<ASTCallableExpr> function) {
        m_Function = std::move(function);
    }

    void ASTFunctionCallExpr::setArguments(std::unique_ptr<ASTArgumentList> arguments) {
        m_Arguments  = std::move(arguments);
    }

    const ASTCallableExpr *ASTFunctionCallExpr::getFunction() const {
        return m_Function.get();
    }

    const ASTArgumentList *ASTFunctionCallExpr::getArguments() const {
        return m_Arguments.get();
    }
}
