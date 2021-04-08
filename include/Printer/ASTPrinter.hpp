#pragma once

#include "AST/ASTVisitor.hpp"
#include <memory>

namespace yapl {
    class ASTPrinter : public ASTVisitor {
    private:
        int m_Tabs = 0;

        void printTabs();

        std::unique_ptr<ASTProgramNode> m_Program;

    public:
        explicit ASTPrinter(std::unique_ptr<ASTProgramNode>);

        void dump();

        virtual void dispatchProgram(ASTProgramNode* program) override;

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
