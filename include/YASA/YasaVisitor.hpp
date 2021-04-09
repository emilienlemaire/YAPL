/**
 * include/YASA/YasaVisitor.hpp
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
#include "AST/ASTVisitor.hpp"
#include "Printer/ASTPrinter.hpp"
#include "Symbol/SymbolTable.hpp"
#include "CppLogger2/CppLogger2.h"

#include <memory>

namespace yapl {
    class YasaVisitor : public ASTVisitor {
    private:
        std::shared_ptr<SymbolTable> m_SymboleTbale;

        std::shared_ptr<Type> m_CurrentType;
        std::shared_ptr<Type> getExprType(ASTExprNode*);

        std::unique_ptr<ASTProgramNode> m_Program;

        CppLogger::CppLogger m_Logger;

        ASTPrinter m_ASTPrinter = ASTPrinter(nullptr);
    public:
        virtual void dispatchProgram(ASTProgramNode* programNode) override;

        virtual void dispatchNegExpr(ASTNegExpr* negExpr) override;
        virtual void dispatchNotExpr(ASTNotExpr* notExpr) override;
        virtual void dispatchParExpr(ASTParExpr* parExpr) override;
        virtual void dispatchArgumentList(ASTArgumentList* argumentList) override;
        virtual void dispatchBoolLiteralExpr(ASTBoolLiteralExpr* boolLiteralExpr) override;
        virtual void dispatchBinaryExpr(ASTBinaryExpr* binaryExpr) override;
        virtual void dispatchRangeExpr(ASTRangeExpr* rangeExpr) override;
        virtual void dispatchFloatNumberExpr(ASTFloatNumberExpr* floatNumberExpr) override;
        virtual void dispatchDoubleNumberExpr(ASTDoubleNumberExpr* doubleNumberExpr) override;
        virtual void dispatchIntegerNumberExpr(ASTIntegerNumberExpr* integerNumberExpr) override;
        virtual void dispatchIdentifierExpr(ASTIdentifierExpr* identifierExpr) override;
        virtual void dispatchAttributeAccessExpr(ASTAttributeAccessExpr* attributeAccessExpr) override;
        virtual void dispatchArrayAccessExpr(ASTArrayAccessExpr* arrayAccessExpr) override;
        virtual void dispatchFunctionCallExpr(ASTFunctionCallExpr* functionCallExpr) override;

        virtual void dispatchBlock(ASTBlockNode* blockNode) override;
        virtual void dispatchExprStatement(ASTExprStatementNode* exprStatementNode) override;
        virtual void dispatchDeclaration(ASTDeclarationNode* declarationNode) override;
        virtual void dispatchArrayDeclaration(ASTArrayDeclarationNode* arrayDeclarationNode) override;
        virtual void dispatchInitialization(ASTInitializationNode* initializationNode) override;
        virtual void dispatchArrayInitialization(ASTArrayInitializationNode* arrayInitializationNode) override;
        virtual void dispatchStructInitialization(ASTStructInitializationNode* structInitializationNode) override;
        virtual void dispatchFunctionDefinition(ASTFunctionDefinitionNode* functionDefinitionNode) override;
        virtual void dispatchStructDefinition(ASTStructDefinitionNode* structDefinitionNode) override;
        virtual void dispatchImport(ASTImportNode* importNode) override;
        virtual void dispatchExport(ASTExportNode* exportNode) override;
        virtual void dispatchReturn(ASTReturnNode* returnNode) override;
        virtual void dispatchIf(ASTIfNode* ifNode) override;
        virtual void dispatchFor(ASTForNode* forNode) override;
        virtual void dispatchAssignment(ASTAssignmentNode* assignmentNode) override;
    };
} // namespace yapl
