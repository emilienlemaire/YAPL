#include <iostream>
#include <llvm/IR/Constants.h>

#include "IRGenerator/IRGenerator.hpp"

unsigned IRGenerator::m_AnonCount = 0;

void IRGenerator::generate() {
    m_Parser->parse();

    m_Program = m_Parser->getProgram();

    for (const auto& node : *m_Program) {
        if (auto literalInt = dynamic_cast<ASTLiteralNode<int>*>(node.get())) {
            generateLiteralInt(literalInt);
        }

        if (auto literalDouble = dynamic_cast<ASTLiteralNode<double>*>(node.get())) {
            generateLiteralDouble(literalDouble);
        }

        if (auto literalBool = dynamic_cast<ASTLiteralNode<bool>*>(node.get())) {
            generateLiteralBool(literalBool);
        }
    }

}

llvm::Value *IRGenerator::generateLiteralInt(ASTLiteralNode<int> *literalInt) {
    // TODO: Clean this and add to YAPLContext (maybe jsut for variable and funcs?)?
    auto val = llvm::ConstantInt::get(m_LLVMContext, llvm::APInt(sizeof(uint32_t), literalInt->getValue()));
    auto anonFuncType = llvm::FunctionType::get(llvm::Type::getInt32Ty(m_LLVMContext), false);
    auto anonFunc = llvm::Function::Create(
            anonFuncType,
            llvm::Function::PrivateLinkage,
            "__anon_expr",
            m_Module.get()
            );
    
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(m_LLVMContext, "entry", anonFunc);
    m_Builder.SetInsertPoint(BB);
    m_Builder.CreateRet(val);

    m_Logger.printInfo("Generated literal int: ");
    anonFunc->print(llvm::errs());
    fprintf(stderr, "\n");
    
    return val;
}

llvm::Value *IRGenerator::generateLiteralDouble(ASTLiteralNode<double> *literalDouble) {
    auto val = llvm::ConstantFP::get(m_LLVMContext, llvm::APFloat(literalDouble->getValue()));
    auto anonFuncType = llvm::FunctionType::get(llvm::Type::getDoubleTy(m_LLVMContext), false);
    auto anonFunc = llvm::Function::Create(
            anonFuncType,
            llvm::Function::PrivateLinkage,
            "__anon_expr",
            m_Module.get()
            );


    llvm::BasicBlock *BB = llvm::BasicBlock::Create(m_LLVMContext, "entry", anonFunc);
    m_Builder.SetInsertPoint(BB);
    m_Builder.CreateRet(val);

    m_Logger.printInfo("Generated literal double: {}", literalDouble->getValue());
    anonFunc->print(llvm::errs());
    fprintf(stderr, "\n");
    
    return val;
}

llvm::Value *IRGenerator::generateLiteralBool(ASTLiteralNode<bool> *literalBool) {
    auto val = llvm::ConstantInt::get(m_LLVMContext, llvm::APInt(sizeof(bool), literalBool->getValue()));
    auto anonFuncType = llvm::FunctionType::get(llvm::Type::getInt1Ty(m_LLVMContext), false);
    auto anonFunc = llvm::Function::Create(
            anonFuncType,
            llvm::Function::PrivateLinkage,
            "__anon_expr",
            m_Module.get()
            );

    llvm::BasicBlock *BB = llvm::BasicBlock::Create(m_LLVMContext, "entry", anonFunc);
    m_Builder.SetInsertPoint(BB);
    m_Builder.CreateRet(val);

    m_Logger.printInfo("Generated literal bool:");
    anonFunc->print(llvm::errs());
    fprintf(stderr, "\n");
    
    return val;
}

