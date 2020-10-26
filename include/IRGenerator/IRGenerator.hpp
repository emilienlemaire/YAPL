#pragma once

#include <CppLogger2/CppLogger2.h>

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

#include "IRGenerator/YAPLContext.hpp"
#include "Parser/Parser.hpp"
#include "AST/ASTNode.hpp"
#include "AST/ASTStatementNode.hpp"
#include "AST/ASTExprNode.hpp"

class IRGenerator {
private:
    llvm::LLVMContext m_LLVMContext;
    llvm::IRBuilder<> m_Builder;
    std::unique_ptr<llvm::Module> m_Module;
    
    CppLogger::CppLogger m_Logger;

    std::unique_ptr<YAPLContext> m_YAPLContext;
    std::unique_ptr<ASTProgramNode> m_Program;

    std::unique_ptr<Parser> m_Parser;

    llvm::Value *generate(ASTNode*);
    llvm::Value *generateExpr(ASTExprNode*);
    llvm::Value *generateBinary(ASTBinaryNode*);
    llvm::Value *generateLiteralInt(ASTLiteralNode<int>*);
    llvm::Value *generateLiteralDouble(ASTLiteralNode<double>*);
    llvm::Value *generateLiteralBool(ASTLiteralNode<bool>*);
    llvm::Value *generateIdentifier(ASTIdentifierNode*);
    llvm::Value *generateAttributeAccess(ASTAttributeAccessNode*);
    llvm::Value *generateArrayAccess(ASTArrayAccessNode*);
    llvm::Value *generateFunctionCall(ASTFunctionCallNode*);
    llvm::Value *generateMethodCall(ASTMethodCallNode*);

    llvm::Value *generateDeclaration(ASTDeclarationNode*);
    llvm::Value *generateInitialization(ASTInitializationNode*);
    llvm::Value *generateAssignment(ASTAssignmentNode*);
    llvm::Value *generateFunctionDefinition(ASTFunctionDefinitionNode*);
    llvm::Value *generateReturn(ASTReturnNode*);
    llvm::Value *generateStructDefinition(ASTStructDefinitionNode*);
    llvm::Value *generateStructInitialization(ASTStructInitializationNode*);
    llvm::Value *generateStructAssignment(ASTStructAssignmentNode*);
    llvm::Value *generateAttributeAssignment(ASTAttributeAssignmentNode*);
    llvm::Value *generateArrayDefinition(ASTArrayDefinitionNode*);
    llvm::Value *generateArrayInitialization(ASTArrayInitializationNode*);
    llvm::Value *generateArrayAssignment(ASTArrayAssignmentNode*);
    llvm::Value *generateArrayMemberAssignment(ASTArrayMemeberAssignmentNode*);
    llvm::Value *generateIf(ASTIfNode*);

    llvm::Value *generateMethod(llvm::StructType*, llvm::SmallVector<std::string, 10>, ASTFunctionDefinitionNode*);

    llvm::Error deferredErrors = llvm::Error::success();

    bool generateBlock(ASTBlockNode *);

    static unsigned m_AnonCount;

    llvm::Type *ASTTypeToLLVM(ASTNode::TYPE type) {
        switch (type) {
            case ASTNode::TYPE::INT:
                return llvm::Type::getInt32Ty(m_LLVMContext);
            case ASTNode::TYPE::DOUBLE:
                return llvm::Type::getDoubleTy(m_LLVMContext);
            case ASTNode::TYPE::BOOL:
                return llvm::Type::getInt1Ty(m_LLVMContext);
            case ASTNode::TYPE::VOID:
                return llvm::Type::getVoidTy(m_LLVMContext);
            case ASTNode::TYPE::STRING:
            case ASTNode::TYPE::NONE:
                llvm::llvm_unreachable_internal();
        }
    }

public:
    IRGenerator(llvm::StringRef filepath)
    : m_Builder(m_LLVMContext), m_Logger(CppLogger::Level::Trace, "IR Generator")
    {
        CppLogger::Format format({
                CppLogger::FormatAttribute::Name,
                CppLogger::FormatAttribute::Level,
                CppLogger::FormatAttribute::Message
               });

        m_Logger.setFormat(format);

        m_Parser = std::make_unique<Parser>(filepath.str(), CppLogger::Level::Trace);

        m_YAPLContext = std::make_unique<YAPLContext>();
    }

    void generate();
};

