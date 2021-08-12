/**
 * include/AST/ASTVisitor.hpp
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

#include "ASTNode.hpp"
#include "ASTExprNode.hpp"
#include "ASTStatementNode.hpp"

namespace yapl {

    class ASTVisitor {
    public:
        virtual void dispatchProgram(ASTProgramNode*) = 0;

        virtual void dispatchCastExpr(ASTCastExpr*) = 0;
        virtual void dispatchNegExpr(ASTNegExpr*) = 0;
        virtual void dispatchNotExpr(ASTNotExpr*) = 0;
        virtual void dispatchParExpr(ASTParExpr*) = 0;
        virtual void dispatchArgumentList(ASTArgumentList*) = 0;
        virtual void dispatchArrayLiteralExpr(ASTArrayLiteralExpr*) = 0;
        virtual void dispatchBoolLiteralExpr(ASTBoolLiteralExpr*) = 0;
        virtual void dispatchBinaryExpr(ASTBinaryExpr*) = 0;
        virtual void dispatchRangeExpr(ASTRangeExpr*) = 0;
        virtual void dispatchFloatNumberExpr(ASTFloatNumberExpr*) = 0;
        virtual void dispatchDoubleNumberExpr(ASTDoubleNumberExpr*) = 0;
        virtual void dispatchIntegerNumberExpr(ASTIntegerNumberExpr*) = 0;
        virtual void dispatchIdentifierExpr(ASTIdentifierExpr*) = 0;
        virtual void dispatchAttributeAccessExpr(ASTAttributeAccessExpr*) = 0;
        virtual void dispatchArrayAccessExpr(ASTArrayAccessExpr*) = 0;
        virtual void dispatchFunctionCallExpr(ASTFunctionCallExpr*) = 0;

        virtual void dispatchBlock(ASTBlockNode*) = 0;
        virtual void dispatchExprStatement(ASTExprStatementNode*) = 0;
        virtual void dispatchDeclaration(ASTDeclarationNode*) = 0;
        virtual void dispatchArrayDeclaration(ASTArrayDeclarationNode*) = 0;
        virtual void dispatchInitialization(ASTInitializationNode*) = 0;
        virtual void dispatchArrayInitialization(ASTArrayInitializationNode*) = 0;
        virtual void dispatchStructInitialization(ASTStructInitializationNode*) = 0;
        virtual void dispatchFunctionDefinition(ASTFunctionDefinitionNode*) = 0;
        virtual void dispatchStructDefinition(ASTStructDefinitionNode*) = 0;
        virtual void dispatchImport(ASTImportNode*) = 0;
        virtual void dispatchExport(ASTExportNode*) = 0;
        virtual void dispatchReturn(ASTReturnNode*) = 0;
        virtual void dispatchIf(ASTIfNode*) = 0;
        virtual void dispatchFor(ASTForNode*) = 0;
        virtual void dispatchAssignment(ASTAssignmentNode*) = 0;
    };
} // namespace yapl
