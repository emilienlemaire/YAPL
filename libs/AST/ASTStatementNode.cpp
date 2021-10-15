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
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "AST/ASTNode.hpp"
#include "AST/ASTStatementNode.hpp"
#include "AST/ASTVisitor.hpp"

namespace yapl {
    ASTStatementNode::ASTStatementNode(SharedScope scope)
        : ASTNode(std::move(scope))
    {}

    ASTBlockNode::ASTBlockNode(SharedScope scope)
        : ASTNode(std::move(scope))
    {}

    void ASTBlockNode::addStatement(std::unique_ptr<ASTStatementNode> statement) {
        m_Statements.push_back(std::move(statement));
    }

    void ASTBlockNode::accept(ASTVisitor &visitor) {
        visitor.dispatchBlock(this);
    }

    ASTExprStatementNode::ASTExprStatementNode(SharedScope scope)
        : ASTStatementNode(std::move(scope))
    {}

    void ASTExprStatementNode::setExpr(std::unique_ptr<ASTExprNode> expr) {
        m_Expr = std::move(expr);
    }

    ASTExprNode *ASTExprStatementNode::getExpr() const {
        return m_Expr.get();
    }

    void ASTExprStatementNode::accept(ASTVisitor &visitor) {
        visitor.dispatchExprStatement(this);
    }

    ASTDeclarationNode::ASTDeclarationNode(SharedScope scope)
        : ASTStatementNode(std::move(scope))
    {}

    void ASTDeclarationNode::setType(const std::string &type) {
        m_Type = type;
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

    void ASTDeclarationNode::accept(ASTVisitor &visitor) {
        visitor.dispatchDeclaration(this);
    }

    ASTArrayDeclarationNode::ASTArrayDeclarationNode(SharedScope scope)
        :ASTDeclarationNode(std::move(scope))
    {}

    void ASTArrayDeclarationNode::setSize(int size) {
        m_Size = size;
    }

    int ASTArrayDeclarationNode::getSize() const {
        return m_Size;
    }

    void ASTArrayDeclarationNode::accept(ASTVisitor &visitor) {
        visitor.dispatchArrayDeclaration(this);
    }

    ASTInitializationNode::ASTInitializationNode(SharedScope scope)
        : ASTDeclarationNode(std::move(scope))
    {}

    void ASTInitializationNode::setValue(std::unique_ptr<ASTExprNode> value) {
        m_Value = std::move(value);
    }

    ASTExprNode *ASTInitializationNode::getValue() const {
        return m_Value.get();
    }

    void ASTInitializationNode::accept(ASTVisitor &visitor) {
        visitor.dispatchInitialization(this);
    }

    ASTArrayInitializationNode::ASTArrayInitializationNode(SharedScope scope)
        :ASTArrayDeclarationNode(std::move(scope))
    {}

    void ASTArrayInitializationNode::setValues(std::unique_ptr<ASTExprNode> values) {
        m_Values = std::move(values);
    }

    ASTExprNode *ASTArrayInitializationNode::getValues() const {
        return m_Values.get();
    }

    void ASTArrayInitializationNode::accept(ASTVisitor &visitor) {
        visitor.dispatchArrayInitialization(this);
    }

    ASTStructInitializationNode::ASTStructInitializationNode(SharedScope scope)
        : ASTDeclarationNode(std::move(scope))
    {}

    void ASTStructInitializationNode::setAttributeValues(std::unique_ptr<ASTExprNode> values) {
        m_AttributeValues = std::move(values);
    }

    ASTExprNode *ASTStructInitializationNode::getAttributeValues() const {
        return m_AttributeValues.get();
    }

    void ASTStructInitializationNode::accept(ASTVisitor &visitor) {
        visitor.dispatchStructInitialization(this);
    }

    ASTFunctionDefinitionNode::ASTFunctionDefinitionNode(SharedScope scope)
        : ASTStatementNode(std::move(scope))
    {}

    void ASTFunctionDefinitionNode::addParameterFirst(std::unique_ptr<ASTDeclarationNode> declaration) {
        m_Parameters.insert(m_Parameters.begin(), std::move(declaration));
    }

    void ASTFunctionDefinitionNode::setFunctionName(const std::string &functionName) {
        m_FunctionName = functionName;
    }

    void ASTFunctionDefinitionNode::setReturnType(const std::string &returnType) {
        m_ReturnType = returnType;
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

    std::vector<std::unique_ptr<ASTDeclarationNode>> const &ASTFunctionDefinitionNode::getParameters() const {
        return m_Parameters;
    }

    ASTBlockNode *ASTFunctionDefinitionNode::getBody() const {
        return m_Body.get();
    }

    void ASTFunctionDefinitionNode::accept(ASTVisitor &visitor) {
        visitor.dispatchFunctionDefinition(this);
    }

    ASTStructDefinitionNode::ASTStructDefinitionNode(SharedScope scope)
        : ASTStatementNode(std::move(scope))
    {}

    void ASTStructDefinitionNode::setStructName(const std::string &structName) {
        m_StructName = structName;
    }

    void ASTStructDefinitionNode::addAttribute(std::unique_ptr<ASTDeclarationNode> attribute) {
        m_Attributes.push_back(std::move(attribute));
    }

    void ASTStructDefinitionNode::addMethod(std::unique_ptr<ASTFunctionDefinitionNode> method) {
        auto thisParam = std::make_unique<ASTDeclarationNode>(this->getScope());
        thisParam->setType(m_StructName);
        thisParam->setIdentifier("this");
        method->addParameterFirst(std::move(thisParam));
        m_Methods.push_back(std::move(method));
    }

    std::string ASTStructDefinitionNode::getStructName() const {
        return m_StructName;
    }

    auto ASTStructDefinitionNode::getAttributes() const -> const decltype(m_Attributes)& {
        return m_Attributes;
    }

    auto ASTStructDefinitionNode::getMethods() const -> const decltype(m_Methods)& {
        return m_Methods;
    }

    void ASTStructDefinitionNode::accept(ASTVisitor &visitor) {
        visitor.dispatchStructDefinition(this);
    }

    ASTImportNode::ASTImportNode(SharedScope scope)
        : ASTStatementNode(std::move(scope))
    {}

    void ASTImportNode::addNamespace(const std::string &t_Namespace) {
        m_Namespaces.push_back(t_Namespace);
    }

    void ASTImportNode::addImportedValue(const std::string &value) {
        m_ImportedValues.push_back(value);
    }

    const std::vector<std::string> &ASTImportNode::getNamespaces() const {
        return m_Namespaces;
    }

    const std::vector<std::string> &ASTImportNode::getImportedValues() const {
        return m_ImportedValues;
    }

    void ASTImportNode::accept(ASTVisitor &visitor) {
        visitor.dispatchImport(this);
    }

    ASTExportNode::ASTExportNode(SharedScope scope)
        : ASTStatementNode(std::move(scope))
    {}

    void ASTExportNode::addExportedValue(const std::string &value) {
        m_ExportedValues.push_back(value);
    }

    const std::vector<std::string> &ASTExportNode::getExportedValues() const {
        return m_ExportedValues;
    }

    void ASTExportNode::accept(ASTVisitor &visitor) {
        visitor.dispatchExport(this);
    }

    ASTReturnNode::ASTReturnNode(SharedScope scope)
        : ASTStatementNode(std::move(scope))
    {}

    void ASTReturnNode::setExpr(std::unique_ptr<ASTExprNode> expr) {
        m_Expr = std::move(expr);
    }

    ASTExprNode *ASTReturnNode::getExpr() const {
        return m_Expr.get();
    }

    void ASTReturnNode::accept(ASTVisitor &visitor) {
        visitor.dispatchReturn(this);
    }

    ASTIfNode::ASTIfNode(SharedScope scope)
        : ASTStatementNode(std::move(scope))
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

    ASTExprNode *ASTIfNode::getCondition() const {
        return m_Condition.get();
    }

    ASTBlockNode *ASTIfNode::getThenBlock() const {
        return m_ThenBlock.get();
    }

    ASTBlockNode *ASTIfNode::getElseBlock() const {
        return m_ElseBlock.get();
    }

    void ASTIfNode::accept(ASTVisitor &visitor) {
        visitor.dispatchIf(this);
    }

    ASTForNode::ASTForNode(SharedScope scope)
        : ASTStatementNode(std::move(scope))
    {}

    void ASTForNode::setIteratorVariable(const std::string &iteratorVariable) {
        m_IteratorVariable = iteratorVariable;
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

    ASTRangeExpr *ASTForNode::getRangeExpr() const {
        return m_RangeExpr.get();
    }

    ASTBlockNode *ASTForNode::getBlock() const {
        return m_Block.get();
    }

    void ASTForNode::accept(ASTVisitor &visitor) {
        visitor.dispatchFor(this);
    }

    ASTAssignmentNode::ASTAssignmentNode(SharedScope scope)
        : ASTStatementNode(std::move(scope))
    {}

    void ASTAssignmentNode::setVariable(std::unique_ptr<ASTAssignableExpr> variable) {
        m_Variable = std::move(variable);
    }

    void ASTAssignmentNode::setValue(std::unique_ptr<ASTExprNode> value) {
        m_Value = std::move(value);
    }

    ASTAssignableExpr *ASTAssignmentNode::getVariable() const {
        return m_Variable.get();
    }

    ASTExprNode *ASTAssignmentNode::getValue() const {
        return m_Value.get();
    }

    void ASTAssignmentNode::accept(ASTVisitor &visitor) {
        visitor.dispatchAssignment(this);
    }
} // namespace yapl
