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
#pragma once

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

    class ASTBlockNode : public ASTNode {
    private:
        std::vector<std::unique_ptr<ASTStatementNode>> m_Statements;
    public:
        explicit ASTBlockNode(SharedScope);

        void addStatement(std::unique_ptr<ASTStatementNode>);

        [[nodiscard]] inline auto getStatements() const -> const decltype(m_Statements)& {
            return m_Statements;
        }

        [[nodiscard]] inline auto begin() const -> decltype(m_Statements.begin()) {
            return m_Statements.begin();
        }

        [[nodiscard]] inline auto end() const -> decltype(m_Statements.end()) {
            return m_Statements.end();
        }

        [[nodiscard]] inline auto cbegin() const -> decltype(m_Statements.cbegin()) {
            return m_Statements.cbegin();
        }

        [[nodiscard]] inline auto cend() const -> decltype(m_Statements.cend()) {
            return m_Statements.cend();
        }

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTExprStatementNode : public ASTStatementNode {
    private:
        std::unique_ptr<ASTExprNode> m_Expr;
    public:
        explicit ASTExprStatementNode(SharedScope);

        void setExpr(std::unique_ptr<ASTExprNode>);

        [[nodiscard]] ASTExprNode *getExpr() const;

        virtual void accept(ASTVisitor &visitor) override;
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

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTArrayDeclarationNode : public ASTDeclarationNode {
    private:
        int m_Size = 0;
    public:
        explicit ASTArrayDeclarationNode(SharedScope);

        void setSize(int);

        [[nodiscard]] int getSize() const;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTInitializationNode : public ASTDeclarationNode {
    private:
        std::unique_ptr<ASTExprNode> m_Value;
    public:
        explicit ASTInitializationNode(SharedScope);

        void setValue(std::unique_ptr<ASTExprNode>);

        [[nodiscard]] ASTExprNode *getValue() const;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTArrayInitializationNode : public ASTArrayDeclarationNode {
    private:
        std::unique_ptr<ASTExprNode> m_Values;
    public:
        explicit ASTArrayInitializationNode(SharedScope);

        void setValues(std::unique_ptr<ASTExprNode>);

        [[nodiscard]] ASTExprNode *getValues() const;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTStructInitializationNode : public ASTDeclarationNode {
    private:
        std::unique_ptr<ASTExprNode> m_AttributeValues;
    public:
        explicit ASTStructInitializationNode(SharedScope);

        void setAttributeValues(std::unique_ptr<ASTExprNode>);

        [[nodiscard]] ASTExprNode *getAttributeValues() const;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTFunctionDefinitionNode : public ASTStatementNode {
    private:
        std::string m_FunctionName;
        std::string m_ReturnType;
        std::vector<std::unique_ptr<ASTDeclarationNode>> m_Parameters;
        std::unique_ptr<ASTBlockNode> m_Body;
        void addParameterFirst(std::unique_ptr<ASTDeclarationNode>);
        void overrideBody(ASTBlockNode *body);

        ASTBlockNode *releaseBody();

        friend class ASTStructDefinitionNode;
        friend class ASTMethodExtractor;

    public:
        explicit ASTFunctionDefinitionNode(SharedScope);

        void setFunctionName(const std::string&);
        void setReturnType(const std::string&);
        void addParameter(std::unique_ptr<ASTDeclarationNode>);
        void setBody(std::unique_ptr<ASTBlockNode>);

        [[nodiscard]] std::string getFunctionName() const;
        [[nodiscard]] std::string getReturnType() const;
        [[nodiscard]] std::vector<std::unique_ptr<ASTDeclarationNode>> const &getParameters() const;
        [[nodiscard]] ASTBlockNode *getBody() const;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTStructDefinitionNode : public ASTStatementNode {
    private:
        std::string m_StructName;
        std::vector<std::unique_ptr<ASTDeclarationNode>> m_Attributes;
        std::vector<std::unique_ptr<ASTFunctionDefinitionNode>> m_Methods;

        void removeMethod(const std::string &name);

        friend class ASTMethodExtractor;
    public:
        explicit ASTStructDefinitionNode(SharedScope);

        void setStructName(const std::string&);
        void addAttribute(std::unique_ptr<ASTDeclarationNode>);
        void addMethod(std::unique_ptr<ASTFunctionDefinitionNode>);

        [[nodiscard]] std::string getStructName() const;
        [[nodiscard]] auto getAttributes() const -> const decltype(m_Attributes)&;
        [[nodiscard]] auto getMethods() const -> const decltype(m_Methods)&;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTImportNode : public ASTStatementNode {
    private:
        std::vector<std::string> m_Namespaces;
        std::vector<std::string> m_ImportedValues;
    public:
        explicit ASTImportNode(SharedScope);
        void addNamespace(const std::string&);
        void addImportedValue(const std::string&);

        [[nodiscard]] const std::vector<std::string> &getNamespaces() const;
        [[nodiscard]] const std::vector<std::string> &getImportedValues() const;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTExportNode : public ASTStatementNode {
    private:
        std::vector<std::string> m_ExportedValues;
    public:
        explicit ASTExportNode(SharedScope);

        void addExportedValue(const std::string&);

        [[nodiscard]] const std::vector<std::string> &getExportedValues() const;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTReturnNode : public ASTStatementNode {
    private:
        std::unique_ptr<ASTExprNode> m_Expr;
    public:
        explicit ASTReturnNode(SharedScope);

        void setExpr(std::unique_ptr<ASTExprNode>);

        [[nodiscard]] ASTExprNode *getExpr() const;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTIfNode : public ASTStatementNode {
    private:
        std::unique_ptr<ASTExprNode> m_Condition;
        std::unique_ptr<ASTBlockNode> m_ThenBlock;
        std::unique_ptr<ASTBlockNode> m_ElseBlock;
    public:
        explicit ASTIfNode(SharedScope);

        void setCondition(std::unique_ptr<ASTExprNode>);
        void setThenBlock(std::unique_ptr<ASTBlockNode>);
        void setElseBlock(std::unique_ptr<ASTBlockNode>);

        [[nodiscard]] ASTExprNode *getCondition() const;
        [[nodiscard]] ASTBlockNode *getThenBlock() const;
        [[nodiscard]] ASTBlockNode *getElseBlock() const;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTForNode : public ASTStatementNode {
    private:
        std::string m_IteratorVariable;
        std::unique_ptr<ASTRangeExpr> m_RangeExpr;
        std::unique_ptr<ASTBlockNode> m_Block;
    public:
        explicit ASTForNode(SharedScope);

        void setIteratorVariable(const std::string&);
        void setRangeExpr(std::unique_ptr<ASTRangeExpr>);
        void setBlock(std::unique_ptr<ASTBlockNode>);

        [[nodiscard]] std::string getIteratorVariable() const;
        [[nodiscard]] ASTRangeExpr *getRangeExpr() const;
        [[nodiscard]] ASTBlockNode *getBlock() const;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTAssignmentNode : public ASTStatementNode {
    private:
        std::unique_ptr<ASTAssignableExpr> m_Variable;
        std::unique_ptr<ASTExprNode> m_Value;
    public:
        explicit ASTAssignmentNode(SharedScope);

        void setVariable(std::unique_ptr<ASTAssignableExpr>);
        void setValue(std::unique_ptr<ASTExprNode>);

        [[nodiscard]] ASTAssignableExpr *getVariable() const;
        [[nodiscard]] ASTExprNode *getValue() const;

        virtual void accept(ASTVisitor &visitor) override;
    };
} // namespace yapl
