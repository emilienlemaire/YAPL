/**
 * libs/AST/ASTStatementNode.cpp
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
#include <algorithm>
#include <cstddef>
#include <deque>
#include <memory>
#include <string>
#include <vector>

#include "AST/ASTNode.hpp"
#include "AST/ASTStatementNode.hpp"

namespace yapl {
    ASTStatementNode::ASTStatementNode(SharedScope scope)
        : ASTNode(scope)
    {}

    ASTBlockNode::ASTBlockNode(SharedScope scope)
        : ASTNode(scope)
    {}

    void ASTBlockNode::addStatement(std::unique_ptr<ASTStatementNode> statement) {
        m_Statements.push_back(statement);
    }

    ASTExprStatementNode::ASTExprStatementNode(SharedScope scope)
        : ASTStatementNode(scope)
    {}

    void ASTExprStatementNode::setExpr(std::unique_ptr<ASTExprNode> expr) {
        m_Expr = std::move(expr);
    }

    const ASTExprNode *ASTExprStatementNode::getExpr() const {
        return m_Expr.get();
    }

    ASTDeclarationNode::ASTDeclarationNode(SharedScope scope)
        : ASTStatementNode(scope)
    {}

    void ASTDeclarationNode::setType(const std::string &type) {
        m_Type = std::move(type);
    }

    void ASTDeclarationNode::setIdentifier(const std::string &identifier) {
        m_Identifier = identifier;
    }

    std::string ASTDeclarationNode::getType() const {
        return m_Type;
    }

    std::string ASTDeclarationNode::getIdentifier() const {
        return m_Identifier;
    }

    ASTArrayDeclarationNode::ASTArrayDeclarationNode(SharedScope scope)
        :ASTDeclarationNode(scope)
    {}

    void ASTArrayDeclarationNode::setSize(int size) {
        m_Size = size;
    }

    int ASTArrayDeclarationNode::getSize() const {
        return m_Size;
    }

    ASTInitializationNode::ASTInitializationNode(SharedScope scope)
        : ASTDeclarationNode(scope)
    {}

    void ASTInitializationNode::setValue(std::unique_ptr<ASTExprNode> value) {
        m_Value = std::move(value);
    }

    const ASTExprNode *ASTInitializationNode::getValue() const {
        return m_Value.get();
    }

    ASTArrayInitializationNode::ASTArrayInitializationNode(SharedScope scope)
        :ASTArrayDeclarationNode(scope)
    {}

    void ASTArrayInitializationNode::setValues(std::unique_ptr<ASTArgumentList> values) {
        m_Values = std::move(values);
    }

    const ASTArgumentList *ASTArrayInitializationNode::getValues() const {
        return m_Values.get();
    }

    ASTStructInitializationNode::ASTStructInitializationNode(SharedScope scope)
        : ASTDeclarationNode(scope)
    {}

    void ASTStructInitializationNode::setAttributeValues(std::unique_ptr<ASTArgumentList> values) {
        m_AttributeValues = std::move(values);
    }

    const ASTArgumentList *ASTStructInitializationNode::getAttributeValues() const {
        return m_AttributeValues.get();
    }

    ASTFunctionDefinitionNode::ASTFunctionDefinitionNode(SharedScope scope)
        : ASTStatementNode(scope)
    {}

    void ASTFunctionDefinitionNode::setFunctionName(const std::string &functionName) {
        m_FunctionName = std::move(functionName);
    }

    void ASTFunctionDefinitionNode::setReturnType(const std::string &returnType) {
        m_ReturnType = std::move(m_ReturnType);
    }

    void ASTFunctionDefinitionNode::addParameter(std::unique_ptr<ASTDeclarationNode> parameter) {
        m_Parameters.push_back(std::move(parameter));
    }

    void ASTFunctionDefinitionNode::setBody(std::unique_ptr<ASTBlockNode> body) {
        m_Body = std::move(body);
    }

    std::string ASTFunctionDefinitionNode::getFunctionName() const {
        return m_FunctionName;
    }

    std::string ASTFunctionDefinitionNode::getReturnType() const {
        return m_ReturnType;
    }

    auto ASTFunctionDefinitionNode::getParameters() const -> decltype(m_Parameters) {
        return m_Parameters;
    }

    const ASTBlockNode *ASTFunctionDefinitionNode::getBody() const {
        return m_Body.get();
    }

    ASTStructDefinitionNode::ASTStructDefinitionNode(SharedScope scope)
        : ASTStatementNode(scope)
    {}

    void ASTStructDefinitionNode::setStructName(const std::string &structName) {
        m_StructName = std::move(structName);
    }

    void ASTStructDefinitionNode::addAttribute(std::unique_ptr<ASTDeclarationNode> attribute) {
        m_Attributes.push_back(std::move(attribute));
    }

    std::string ASTStructDefinitionNode::getStructName() const {
        return m_StructName;
    }

    auto ASTStructDefinitionNode::getAttributes() const -> decltype(m_Attributes) {
        return m_Attributes;
    }

    auto ASTStructDefinitionNode::getMethods() const -> decltype(m_Methods) {
        return m_Methods;
    }

    ASTImportNode::ASTImportNode(SharedScope scope)
        : ASTStatementNode(scope)
    {}

    void ASTImportNode::addNamespace(const std::string &t_Namespace) {
        m_Namespaces.push_back(std::move(t_Namespace));
    }

    void ASTImportNode::addImportedValue(const std::string &value) {
        m_ImportedValues.push_back(std::move(value));
    }

    const std::vector<std::string> &ASTImportNode::getNamespaces() const {
        return m_Namespaces;
    }

    const std::vector<std::string> &ASTImportNode::getImportedValues() const {
        return m_ImportedValues;
    }

    ASTExportNode::ASTExportNode(SharedScope scope)
        : ASTStatementNode(scope)
    {}

    void ASTExportNode::addExportedValue(const std::string &value) {
        m_ExportedValues.push_back(std::move(value));
    }

    const std::vector<std::string> &ASTExportNode::getExportedValues() const {
        return m_ExportedValues;
    }

    ASTReturnNode::ASTReturnNode(SharedScope scope)
        : ASTStatementNode(scope)
    {}

    void ASTReturnNode::setExpr(std::unique_ptr<ASTExprNode> expr) {
        m_Expr = std::move(expr);
    }

    const ASTExprNode *ASTReturnNode::getExpr() const {
        return m_Expr.get();
    }

    ASTIfNode::ASTIfNode(SharedScope scope)
        : ASTStatementNode(scope)
    {}

    void ASTIfNode::setCondition(std::unique_ptr<ASTExprNode> condition) {
        m_Condition = std::move(condition);
    }

    void ASTIfNode::setThenBlock(std::unique_ptr<ASTBlockNode> thenBlock) {
        m_ThenBlock = std::move(thenBlock);
    }

    void ASTIfNode::setElseBlock(std::unique_ptr<ASTBlockNode> elseBlock) {
        m_ElseBlock = std::move(elseBlock);
    }

    const ASTExprNode *ASTIfNode::getCondition() const {
        return m_Condition.get();
    }

    const ASTBlockNode *ASTIfNode::getThenBlock() const {
        return m_ThenBlock.get();
    }

    const ASTBlockNode *ASTIfNode::getElseBlock() const {
        return m_ElseBlock.get();
    }

    ASTForNode::ASTForNode(SharedScope scope)
        : ASTStatementNode(scope)
    {}

    void ASTForNode::setIteratorVariable(const std::string &iteratorVariable) {
        m_IteratorVariable = std::move(iteratorVariable);
    }

    void ASTForNode::setRangeExpr(std::unique_ptr<ASTRangeExpr> range) {
        m_RangeExpr = std::move(range);
    }

    void ASTForNode::setBlock(std::unique_ptr<ASTBlockNode> block) {
        m_Block = std::move(block);
    }

    std::string ASTForNode::getIteratorVariable() const {
        return m_IteratorVariable;
    }

    const ASTRangeExpr *ASTForNode::getRangeExpr() const {
        return m_RangeExpr.get();
    }

    const ASTBlockNode *ASTForNode::getBlock() const {
        return m_Block.get();
    }

    ASTAssignmentNode::ASTAssignmentNode(SharedScope scope)
        : ASTStatementNode(scope)
    {}

    void ASTAssignmentNode::setVariable(std::unique_ptr<ASTAssignableExpr> variable) {
        m_Variable = std::move(variable);
    }

    void ASTAssignmentNode::setValue(std::unique_ptr<ASTExprNode> value) {
        m_Value = std::move(value);
    }

    const ASTAssignableExpr *ASTAssignmentNode::getVariable() const {
        return m_Variable.get();
    }

    const ASTExprNode *ASTAssignmentNode::getValue() const {
        return m_Value.get();
    }
} // namespace yapl
