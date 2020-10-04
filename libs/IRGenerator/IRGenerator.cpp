#include <iostream>

#include "AST/ASTExprNode.hpp"
#include "IRGenerator/IRGenerator.hpp"

unsigned IRGenerator::m_AnonCount = 0;

void IRGenerator::generate() {
    m_Parser->parse();

    m_Program = m_Parser->getProgram();

    m_Module = std::make_unique<llvm::Module>("main", m_LLVMContext);

    for (const auto& node : *m_Program) {
        if (node)
            generate(node.get());
    }


    auto anonFuncType = llvm::FunctionType::get(llvm::Type::getInt32Ty(m_LLVMContext), false);
    auto anonFunc = llvm::Function::Create(
            anonFuncType,
            llvm::Function::InternalLinkage,
            "__anon_expr",
            m_Module.get()
            );
    auto BB = llvm::BasicBlock::Create(m_LLVMContext, "entry", anonFunc);

    //llvm::GlobalVariable gVar(*m_Module.get(),
            //llvm::Type::getInt32Ty(m_LLVMContext),
            //false,
            //llvm::GlobalVariable::PrivateLinkage,
            //nullptr,
            //"myGlobVar");

    m_Module->dump();

    m_Module->dropAllReferences();
}

llvm::Value *IRGenerator::generate(ASTNode* node) {
    if (auto expr = dynamic_cast<ASTExprNode*>(node)) {
        auto genExpr = generateExpr(expr);
        auto anonFuncType = llvm::FunctionType::get(genExpr->getType(), false);
        auto anonFunc = llvm::Function::Create(
                anonFuncType,
                llvm::Function::InternalLinkage,
                "__anon_expr",
                m_Module.get()
                );
        auto BB = llvm::BasicBlock::Create(m_LLVMContext, "entry", anonFunc);
        m_Builder.SetInsertPoint(BB);
        m_Builder.CreateRet(genExpr);

        //m_Logger.printInfo("Generated anon expr:");
        //anonFunc->print(llvm::errs());
        //fprintf(stderr, "\n");

        return genExpr;
    } else {
        if (auto declaration = dynamic_cast<ASTDeclarationNode*>(node)){
            return generateDeclaration(declaration);
        }

        if (auto assignment = dynamic_cast<ASTAssignmentNode*>(node)) {
            //return generateAssignement(assignment);
        }
    }

    m_Logger.printError("The code you wrote cannot be compiled yet :(");
    return nullptr;
}

llvm::Value *IRGenerator::generateExpr(ASTExprNode *expr) {
    if (auto bin = dynamic_cast<ASTBinaryNode*>(expr)) {
        return generateBinary(bin);
    }

    if (auto literalInt = dynamic_cast<ASTLiteralNode<int>*>(expr)) {
        return generateLiteralInt(literalInt);
    }

    if (auto literalDouble = dynamic_cast<ASTLiteralNode<double>*>(expr)) {
        return generateLiteralDouble(literalDouble);
    }

    if (auto literalBool = dynamic_cast<ASTLiteralNode<bool>*>(expr)) {
        return generateLiteralBool(literalBool);
    }

    m_Logger.printError("The code you wrote cannot be compiled yet :(");
    return nullptr;
}

llvm::Value *IRGenerator::generateLiteralInt(ASTLiteralNode<int> *literalInt) {
    auto val = llvm::ConstantInt::get(m_LLVMContext, llvm::APInt(sizeof(uint32_t), literalInt->getValue()));
    return val;
}

llvm::Value *IRGenerator::generateLiteralDouble(ASTLiteralNode<double> *literalDouble) {
    auto val = llvm::ConstantFP::get(m_LLVMContext, llvm::APFloat(literalDouble->getValue()));
    return val;
}

llvm::Value *IRGenerator::generateLiteralBool(ASTLiteralNode<bool> *literalBool) {
    auto val = llvm::ConstantInt::get(m_LLVMContext, llvm::APInt(sizeof(bool), literalBool->getValue()));
    return val;
}

llvm::Value *IRGenerator::generateBinary(ASTBinaryNode *bin) {
    auto lhs = std::move(bin->getLeftOperrand());
    auto rhs = std::move(bin->getRightOperrand());

    auto genLhs = generateExpr(lhs.get());
    auto genRhs = generateExpr(rhs.get());

    auto genOp = [this](llvm::Value* L, Operator op, llvm::Value *R) {
        switch (op) {
        case Operator::plus:
            return m_Builder.CreateAdd(L, R, "addtmp");
        case Operator::minus:
            return m_Builder.CreateSub(L, R, "subtemp");
        case Operator::times:
            if (L->getType()->isDoubleTy()) {
                return m_Builder.CreateFMul(L, R, "multmp");
            }
            return m_Builder.CreateMul(L, R, "multmp");
        case Operator::divide:
            if (L->getType()->isDoubleTy()) {
                return m_Builder.CreateFDiv(L, R, "divtmp");
            }
            return m_Builder.CreateSDiv(L, R, "divtmp");
        case Operator::mod:
            if (L->getType()->isDoubleTy()) {
                return m_Builder.CreateFRem(L, R, "modtmp");
            }
            return m_Builder.CreateSRem(L, R, "modtmp");
        case Operator::lth:
            if (L->getType()->isDoubleTy()) {
                return m_Builder.CreateFCmpOLT(L, R, "lthtmp");
            }
            return m_Builder.CreateICmpSLT(L, R, "lthtmp");
        case Operator::mth:
            if (L->getType()->isDoubleTy()) {
               return m_Builder.CreateFCmpOGT(L, R, "gthtmp"); 
            }
            return m_Builder.CreateICmpSGE(L, R, "gthtmp");
        case Operator::orsym:
            return m_Builder.CreateOr(L, R, "ortmp");
        case Operator::andsym:
            return m_Builder.CreateAnd(L, R, "ortmp");
        case Operator::eqcomp:
            if (L->getType()->isDoubleTy()) {
                return m_Builder.CreateFCmpOEQ(L, R, "eqtmp");
            }
            return m_Builder.CreateICmpEQ(L, R, "eqtmp");
        case Operator::leq:
            if (L->getType()->isDoubleTy()) {
                return m_Builder.CreateFCmpOLE(L, R, "leqtmp");
            }
            return m_Builder.CreateICmpSLE(L, R, "leqtmp");
        case Operator::meq:
            if (L->getType()->isDoubleTy()) {
                return m_Builder.CreateFCmpOGE(L, R, "geqtmp");
            }
            return m_Builder.CreateICmpSGE(L, R, "geqtmp");
        case Operator::neq:
            if (L->getType()->isDoubleTy()) {
                return m_Builder.CreateFCmpONE(L, R, "neqtmp");
            }
            return m_Builder.CreateICmpNE(L, R, "neqtmp");
        } 
    };

    if (!genLhs || !genRhs){
        m_Logger.printError("Failed to generate code for a binary expr");
        return nullptr;
    }

    if (genLhs->getType() != genRhs->getType()) {
        if (genLhs->getType()->isIntegerTy() && genRhs->getType()->isDoubleTy()) {
            auto newRhs = m_Builder.CreateCast(llvm::Instruction::CastOps::FPToSI, genRhs, genLhs->getType());
            return genOp(genLhs, bin->getOperator(), newRhs);
        }

        if (genLhs->getType()->isDoubleTy() && genRhs->getType()->isIntegerTy()) {
            auto newRhs = m_Builder.CreateCast(llvm::Instruction::CastOps::SIToFP, genRhs, genLhs->getType());
            return genOp(genLhs, bin->getOperator(), newRhs);
        }

        m_Logger.printError("Binary op impossible between two types.");
        return nullptr;
    }

    return genOp(genLhs, bin->getOperator(), genRhs);
}

// TODO: Add to context, and make sure to delete the pointers in
// the context at destruction
llvm::Value *IRGenerator::generateDeclaration(ASTDeclarationNode *declaration) {
    if (auto initialization = dynamic_cast<ASTInitializationNode*>(declaration)) {
        return generateInitialization(initialization);
    }

    // We are on top level scope so we want a global variable
    if (m_YAPLContext->isAtTopLevelScope()) {
        auto llvmType = ASTTypeToLLVM(declaration->getType());
        m_Module->getOrInsertGlobal(declaration->getName(), llvmType);
        llvm::GlobalVariable *globalVar = m_Module->getNamedGlobal(declaration->getName());
        globalVar->setLinkage(llvm::GlobalValue::PrivateLinkage);
        globalVar->setAlignment(llvm::MaybeAlign(4));
        llvm::cantFail(m_YAPLContext->getCurrentScope()->pushValue(declaration->getName(), globalVar));

        return globalVar;
    }

    // TODO: Handle scoped declaration for inside a function;
    return nullptr;
}

llvm::Value *IRGenerator::generateInitialization(ASTInitializationNode *initialization) {
    if (m_YAPLContext->isAtTopLevelScope()) {
        auto llvmType = ASTTypeToLLVM(initialization->getType());
        auto valuePtr = initialization->getValue();
        auto value = generateExpr(initialization->getValue());
        m_Module->getOrInsertGlobal(initialization->getName(), llvmType);
        llvm::GlobalVariable *globalVar = m_Module->getNamedGlobal(initialization->getName());
        globalVar->setLinkage(llvm::GlobalValue::PrivateLinkage);
        globalVar->setAlignment(llvm::MaybeAlign(4));
        if (static_cast<llvm::Constant *>(value)) {
            m_Logger.printInfo("It is a constant");
        } else {
            m_Logger.printError("It is not a constant");
        }
        globalVar->setInitializer(static_cast<llvm::Constant *>(value));

        llvm::cantFail(m_YAPLContext->getCurrentScope()->pushValue(initialization->getName(), globalVar));

        return globalVar;
    }
    // TODO: Handle scoped initialization
    return nullptr;
}

llvm::Value *IRGenerator::generateAssignment(ASTAssignmentNode *assignment) {
    if (m_YAPLContext->isAtTopLevelScope()) {
        m_Logger.printError("Assignment are forbidden outside of a function body");
        return nullptr;
    }

    llvm::Value *variable = m_YAPLContext->getCurrentScope()->lookup(assignment->getName());
    llvm::Value *value = generateExpr(assignment->getValue());
    return m_Builder.CreateStore(value, variable);
}
