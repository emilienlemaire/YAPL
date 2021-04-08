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
#include "AST/ASTVisitor.hpp"

namespace yapl {

    // Virtual classes
    ASTExprNode::ASTExprNode(SharedScope scope)
        : ASTNode(std::move(scope))
    {}

    ASTUnaryExpr::ASTUnaryExpr(SharedScope scope)
        : ASTExprNode(std::move(scope))
    {}

    ASTCallableExpr::ASTCallableExpr(SharedScope scope)
        : ASTExprNode(std::move(scope))
    {}

    ASTAssignableExpr::ASTAssignableExpr(SharedScope scope)
        : ASTAccessibleExpr(std::move(scope))
    {}

    ASTAccessibleExpr::ASTAccessibleExpr(SharedScope scope)
        : ASTCallableExpr(std::move(scope))
    {}

    ASTNumberExpr::ASTNumberExpr(SharedScope scope)
        : ASTExprNode(std::move(scope))
    {}

    // Non virtual classes
    ASTNegExpr::ASTNegExpr(SharedScope scope)
        : ASTUnaryExpr(std::move(scope))
    {}

    void ASTNegExpr::setValue(std::unique_ptr<ASTExprNode> value) {
        m_Value = std::move(value);
    }

    ASTExprNode *ASTNegExpr::getValue() const {
        return m_Value.get();
    }

    void ASTNegExpr::accept(ASTVisitor &visitor) {
        visitor.dispatchNegExpr(this);
    }

    ASTNotExpr::ASTNotExpr(SharedScope scope)
        : ASTUnaryExpr(std::move(scope))
    {}

    void ASTNotExpr::setValue(std::unique_ptr<ASTExprNode> value) {
        m_Value = std::move(value);
    }

    ASTExprNode *ASTNotExpr::getValue() const {
        return m_Value.get();
    }

    void ASTNotExpr::accept(ASTVisitor &visitor) {
        visitor.dispatchNotExpr(this);
    }

    ASTParExpr::ASTParExpr(SharedScope scope)
        : ASTExprNode(std::move(scope))
    {}

    void ASTParExpr::setExpr(std::unique_ptr<ASTExprNode> value) {
        m_Expr = std::move(value);
    }

    ASTExprNode *ASTParExpr::getExpr() const {
        return m_Expr.get();
    }

    void ASTParExpr::accept(ASTVisitor &visitor) {
        visitor.dispatchParExpr(this);
    }

    ASTArgumentList::ASTArgumentList(SharedScope scope)
        : ASTExprNode(std::move(scope))
    {}

    void ASTArgumentList::addArgument(std::unique_ptr<ASTExprNode> argument) {
        m_Arguments.push_back(std::move(argument));
    }

    const ASTArgumentList::vectorType &ASTArgumentList::getArguments() const {
        return m_Arguments;
    }

    void ASTArgumentList::accept(ASTVisitor &visitor) {
        visitor.dispatchArgumentList(this);
    }

    ASTBoolLiteralExpr::ASTBoolLiteralExpr(SharedScope scope)
        : ASTExprNode(std::move(scope))
    {}

    void ASTBoolLiteralExpr::setValue(bool value) {
        m_Value = value;
    }

    bool ASTBoolLiteralExpr::getValue() const {
        return m_Value;
    }

    void ASTBoolLiteralExpr::accept(ASTVisitor &visitor) {
        visitor.dispatchBoolLiteralExpr(this);
    }

    ASTBinaryExpr::ASTBinaryExpr(SharedScope scope)
        : ASTExprNode(std::move(scope))
    {}

    void ASTBinaryExpr::setLHS(std::unique_ptr<ASTExprNode> lhs) {
        m_LHS = std::move(lhs);
    }

    void ASTBinaryExpr::setRHS(std::unique_ptr<ASTExprNode> lhs) {
        m_RHS = std::move(lhs);
    }

    void ASTBinaryExpr::setOperator(Operator op) {
        m_Operator = op;
    }

    ASTExprNode* ASTBinaryExpr::getLHS() const {
        return m_LHS.get();
    }


    ASTExprNode* ASTBinaryExpr::getRHS() const {
        return m_RHS.get();
    }

    Operator ASTBinaryExpr::getOperator() const {
        return m_Operator;
    }

    void ASTBinaryExpr::accept(ASTVisitor &visitor) {
        visitor.dispatchBinaryExpr(this);
    }

    ASTRangeExpr::ASTRangeExpr(SharedScope scope)
        : ASTExprNode(std::move(scope))
    {}


    void ASTRangeExpr::setStart(std::unique_ptr<ASTExprNode> start) {
        m_Start = std::move(start);
    }

    void ASTRangeExpr::setEnd(std::unique_ptr<ASTExprNode> end) {
        m_End = std::move(end);
    }

    ASTExprNode *ASTRangeExpr::getStart() const {
        return m_Start.get();
    }

    ASTExprNode *ASTRangeExpr::getEnd() const {
        return m_End.get();
    }

    void ASTRangeExpr::accept(ASTVisitor &visitor) {
        visitor.dispatchRangeExpr(this);
    }

    ASTFloatNumberExpr::ASTFloatNumberExpr(SharedScope scope)
        :ASTNumberExpr(std::move(scope))
    {}

    void ASTFloatNumberExpr::setValue(float value) {
        m_Value = value;
    }

    float ASTFloatNumberExpr::getValue() const {
        return m_Value;
    }

    void ASTFloatNumberExpr::accept(ASTVisitor &visitor) {
        visitor.dispatchFloatNumberExpr(this);
    }

    ASTDoubleNumberExpr::ASTDoubleNumberExpr(SharedScope scope)
        :ASTNumberExpr(std::move(scope))
    {}

    void ASTDoubleNumberExpr::setValue(double value) {
        m_Value = value;
    }

    double ASTDoubleNumberExpr::getValue() const {
        return m_Value;
    }

    void ASTDoubleNumberExpr::accept(ASTVisitor &visitor) {
        visitor.dispatchDoubleNumberExpr(this);
    }

    ASTIntegerNumberExpr::ASTIntegerNumberExpr(SharedScope scope)
        : ASTNumberExpr(std::move(scope))
    {}

    void ASTIntegerNumberExpr::setValue(int value) {
        m_Value = value;
    }

    int ASTIntegerNumberExpr::getValue() const {
        return m_Value;
    }

    void ASTIntegerNumberExpr::accept(ASTVisitor &visitor) {
        visitor.dispatchIntegerNumberExpr(this);
    }

    ASTIdentifierExpr::ASTIdentifierExpr(SharedScope scope)
        : ASTAssignableExpr(std::move(scope))
    {}

    void ASTIdentifierExpr::setIdentifier(const std::string &identifier) {
        m_Identifier = identifier;
    }

    std::string ASTIdentifierExpr::getIdentifier() const {
        return m_Identifier;
    }

    void ASTIdentifierExpr::accept(ASTVisitor &visitor) {
        visitor.dispatchIdentifierExpr(this);
    }

    ASTAttributeAccessExpr::ASTAttributeAccessExpr(SharedScope scope)
        : ASTAssignableExpr(std::move(scope))
    {}

    void ASTAttributeAccessExpr::setStruct(std::unique_ptr<ASTAccessibleExpr> t_Struct) {
        m_Struct = std::move(t_Struct);
    }

    void ASTAttributeAccessExpr::setAttribute(std::unique_ptr<ASTIdentifierExpr> attribute) {
        m_Attribute = std::move(attribute);
    }

    ASTAccessibleExpr *ASTAttributeAccessExpr::getStruct() const {
        return m_Struct.get();
    }

    ASTIdentifierExpr *ASTAttributeAccessExpr::getAttribute() const {
        return m_Attribute.get();
    }

    void ASTAttributeAccessExpr::accept(ASTVisitor &visitor) {
        visitor.dispatchAttributeAccessExpr(this);
    }

    ASTArrayAccessExpr::ASTArrayAccessExpr(SharedScope scope)
        :ASTAssignableExpr(std::move(scope))
    {}

    void ASTArrayAccessExpr::setArray(std::unique_ptr<ASTAccessibleExpr> array) {
        m_Array = std::move(array);
    }

    void ASTArrayAccessExpr::setIndex(std::unique_ptr<ASTExprNode> index) {
        m_Index = std::move(index);
    }

    ASTAccessibleExpr *ASTArrayAccessExpr::getArray() const {
        return m_Array.get();
    }

    ASTExprNode *ASTArrayAccessExpr::getIndex() const {
        return m_Index.get();
    }

    void ASTArrayAccessExpr::accept(ASTVisitor &visitor) {
        visitor.dispatchArrayAccessExpr(this);
    }

    ASTFunctionCallExpr::ASTFunctionCallExpr(SharedScope scope)
        :ASTAccessibleExpr(std::move(scope))
    {}

    void ASTFunctionCallExpr::setFunction(std::unique_ptr<ASTCallableExpr> function) {
        m_Function = std::move(function);
    }

    void ASTFunctionCallExpr::setArguments(std::unique_ptr<ASTArgumentList> arguments) {
        m_Arguments  = std::move(arguments);
    }

    ASTCallableExpr *ASTFunctionCallExpr::getFunction() const {
        return m_Function.get();
    }

    ASTArgumentList *ASTFunctionCallExpr::getArguments() const {
        return m_Arguments.get();
    }

    void ASTFunctionCallExpr::accept(ASTVisitor &visitor) {
        visitor.dispatchFunctionCallExpr(this);
    }
} // namespace yapl
