#pragma once
/**
 * include/AST/ASTStatementNode.hpp
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

#include <array>
#include <cstddef>
#include <string>
#include <vector>
#include <memory>

#include "AST/ASTNode.hpp"
#include "AST/ASTExprNode.hpp"

namespace yapl
{
    using SharedScope = std::shared_ptr<SymbolTable>;

    class ASTStatementNode : public ASTNode {
    private:
    public:
        explicit ASTStatementNode(SharedScope);
        virtual ~ASTStatementNode() override = default;
    };

    class ASTImportNode : public ASTStatementNode {
    private:
        std::vector<std::string> m_Namespaces;
        std::string m_ImportedValue;
    public:
        explicit ASTImportNode(SharedScope);
        void addNamespace(const std::string&);
        void setImportedValue(const std::string&);

        [[nodiscard]] std::vector<std::string> getNamesapces() const;
        [[nodiscard]] std::string getImportedValue() const;
    };

    class ASTExportNode : public ASTStatementNode {
    private:
        std::vector<std::string> m_ExportedValues;
    public:
        explicit ASTExportNode(SharedScope);
        void addExportedValue(const std::string&);

        [[nodiscard]] std::vector<std::string> getExportedValues() const;
    };

    class ASTFunctionDefinitionNode : public ASTStatementNode {
    private:
        std::string m_FunctionName;
        std::string m_ReturnType;
        std::vector<std::string> m_ParamNames;
    public:
        explicit ASTFunctionDefinitionNode(SharedScope);
        void setFunctionName(const std::string&);
        void setReturnType(const std::string&);
        void addParam(const std::string &);

        [[nodiscard]] std::string getFunctionName() const;
        [[nodiscard]] std::string getReturnType() const;
        [[nodiscard]] std::vector<std::string> getParamNames() const;
    };

    class ASTStructDefinitionNode : public ASTStatementNode {
    private:
        std::string m_StructName;
        std::vector<std::string> m_AttributeNames;
        std::vector<std::string> m_MethodNames;
    public:
        explicit ASTStructDefinitionNode(SharedScope);

        void setStructName(const std::string&);
        void addAttribute(const std::string&);
        void addMethod(const std::string&);

        [[nodiscard]] std::string getStructName() const;
        [[nodiscard]] std::vector<std::string> getAttributeNames() const;
        [[nodiscard]] std::vector<std::string> getMethodNames() const;
    };

    class ASTDeclarationNode : public ASTStatementNode {
    private:
        std::string m_Type;
        std::string m_Identifier;
    public:
        explicit ASTDeclarationNode(SharedScope);

        void setType(const std::string&);
        void setIdentifier(const std::string&);

        [[nodiscard]] std::string getType() const;
        [[nodiscard]] std::string getIdentifier() const;
    };

    class ASTInitializationNode : public ASTDeclarationNode {
    private:
        std::unique_ptr<ASTExprNode> m_Value;
    public:
        explicit ASTInitializationNode(SharedScope);

        void setExpression(std::unique_ptr<ASTExprNode>);

        [[nodiscard]] ASTExprNode *getExpr() const;
    };
} // namespace yapl
