#pragma once

namespace yapl {
    class ASTProgramNode;

    class ASTNegExpr;
    class ASTNotExpr;
    class ASTParExpr;
    class ASTArgumentList;
    class ASTBoolLiteralExpr;
    class ASTBinaryExpr;
    class ASTRangeExpr;
    class ASTFloatNumberExpr;
    class ASTDoubleNumberExpr;
    class ASTIntegerNumberExpr;
    class ASTIdentifierExpr;
    class ASTAttributeAccessExpr;
    class ASTArrayAccessExpr;
    class ASTFunctionCallExpr;

    class ASTBlockNode;
    class ASTExprStatementNode;
    class ASTDeclarationNode;
    class ASTArrayDeclarationNode;
    class ASTInitializationNode;
    class ASTArrayInitializationNode;
    class ASTStructInitializationNode;
    class ASTFunctionDefinitionNode;
    class ASTStructDefinitionNode;
    class ASTImportNode;
    class ASTExportNode;
    class ASTReturnNode;
    class ASTIfNode;
    class ASTForNode;
    class ASTAssignmentNode;

    class ASTVisitor {
    public:
        virtual void dispatchProgram(ASTProgramNode*) = 0;

        virtual void dispatchNegExpr(ASTNegExpr*) = 0;
        virtual void dispatchNotExpr(ASTNotExpr*) = 0;
        virtual void dispatchParExpr(ASTParExpr*) = 0;
        virtual void dispatchArgumentList(ASTArgumentList*) = 0;
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
