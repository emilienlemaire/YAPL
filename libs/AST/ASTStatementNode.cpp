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
#include <memory>
#include <string>
#include <vector>

#include "AST/ASTStatementNode.hpp"

namespace yapl {
    /*
     * ASTStatementNode
     *
     * */
    ASTStatementNode::ASTStatementNode(SharedScope scope)
        : ASTNode(scope)
    {}

    /*
     * ASTImportNode
     *
     * */
    ASTImportNode::ASTImportNode(SharedScope scope)
        : ASTStatementNode(scope)
    {}

    void ASTImportNode::addNamespace(const std::string &ns) {
        m_Namespaces.push_back(std::move(ns));
    }

    void ASTImportNode::setImportedValue(const std::string &name) {
        m_ImportedValue = std::move(name);
    }

    std::vector<std::string> ASTImportNode::getNamesapces() const {
        return m_Namespaces;
    }

    std::string ASTImportNode::getImportedValue() const {
        return m_ImportedValue;
    }

    /*
     * ASTExportNode
     *
     * */
    ASTExportNode::ASTExportNode(SharedScope scope)
        :ASTStatementNode(scope)
    {}

    void ASTExportNode::addExportedValue(const std::string &name) {
        m_ExportedValues.push_back(std::move(name));
    }

    std::vector<std::string> ASTExportNode::getExportedValues() const {
        return m_ExportedValues;
    }

    /*
     * ASTFunctionDefinitionNode
     *
     * */
    ASTFunctionDefinitionNode::ASTFunctionDefinitionNode(SharedScope scope)
        : ASTStatementNode(scope)
    {}

    void ASTFunctionDefinitionNode::setFunctionName(const std::string &name) {
        m_FunctionName = std::move(name);
    }

    void ASTFunctionDefinitionNode::setReturnType(const std::string &type) {
        m_ReturnType = std::move(type);
    }

    void ASTFunctionDefinitionNode::addParam(const std::string &name) {
        m_ParamNames.push_back(std::move(name));
    }

    std::string ASTFunctionDefinitionNode::getFunctionName() const {
        return m_FunctionName;
    }

    std::string ASTFunctionDefinitionNode::getReturnType() const {
        return m_ReturnType;
    }

    std::vector<std::string> ASTFunctionDefinitionNode::getParamNames() const {
        return m_ParamNames;
    }

    /*
     * ASTStructDefinitionNode
     *
     * */
    ASTStructDefinitionNode::ASTStructDefinitionNode(SharedScope scope)
        : ASTStatementNode(scope)
    {}

    void ASTStructDefinitionNode::setStructName(const std::string &name) {
        m_StructName = std::move(name);
    }

    void ASTStructDefinitionNode::addAttribute(const std::string &name) {
        m_AttributeNames.push_back(std::move(name));
    }

    void ASTStructDefinitionNode::addMethod(const std::string &name) {
        m_MethodNames.push_back(std::move(name));
    }

    std::string ASTStructDefinitionNode::getStructName() const {
        return m_StructName;
    }

    std::vector<std::string> ASTStructDefinitionNode::getAttributeNames() const {
        return m_AttributeNames;
    }

    std::vector<std::string> ASTStructDefinitionNode::getMethodNames() const {
        return m_MethodNames;
    }

    ASTDeclarationNode::ASTDeclarationNode(SharedScope scope)
        : ASTStatementNode(scope)
    {}

    void ASTDeclarationNode::setType(const std::string &type) {
        m_Type = std::move(type);
    }

    void ASTDeclarationNode::setIdentifier(const std::string &identifier) {
        m_Identifier = std::move(identifier);
    }

    std::string ASTDeclarationNode::getType() const {
        return m_Type;
    }

    std::string ASTDeclarationNode::getIdentifier() const {
        return m_Identifier;
    }
}
