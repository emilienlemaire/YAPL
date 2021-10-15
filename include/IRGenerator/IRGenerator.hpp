#pragma once

#include "AST/ASTNode.hpp"
#include "AST/ASTVisitor.hpp"
#include "CppLogger2/CppLogger2.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

#include <map>
#include <memory>
#include <string>

// TODO: Make ManeValueMap scoped
namespace yapl {
    class IRGenerator : public ASTVisitor {
    private:
        llvm::LLVMContext m_LLVMContext;
        llvm::IRBuilder<> m_Builder;
        std::unique_ptr<llvm::Module> m_Module;

        CppLogger::CppLogger m_Logger;

        std::unique_ptr<ASTProgramNode> m_Program;

        llvm::Type *p_LastType = nullptr;
        llvm::Value *p_LastValue = nullptr;

        std::map<std::string, llvm::Value*> m_NameValueMap = std::map<std::string, llvm::Value*>();
        std::map<Type*, llvm::Type*> m_YAPLLLVMTypeMap = std::map<Type*, llvm::Type*>();
        std::map<ASTExprNode*, Type*> m_ExprTypeMap;

        llvm::Type *getOrCreateLLVMType(Type *YAPLType);
    public:
        IRGenerator(
                std::map<ASTExprNode*, Type*> exprTypeMap,
                std::unique_ptr<ASTProgramNode> program,
                llvm::StringRef filepath
            );

        void generate();

        virtual void dispatchProgram(ASTProgramNode* programNode) override;

        virtual void dispatchNegExpr(ASTNegExpr* negExpr) override;
        virtual void dispatchNotExpr(ASTNotExpr* notExpr) override;
        virtual void dispatchParExpr(ASTParExpr* parExpr) override;
        virtual void dispatchArgumentList(ASTArgumentList* argumentList) override;
        virtual void dispatchArrayLiteralExpr(ASTArrayLiteralExpr* arrayLiteral) override;
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
        virtual void dispatchCastExpr(ASTCastExpr* functionCallExpr) override;

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

        void dispatchMethod(ASTFunctionDefinitionNode* methodDefinition, StructType* structType);
    };
} // namespace yapl
