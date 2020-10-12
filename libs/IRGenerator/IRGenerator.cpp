#include <iostream>

#include <llvm/ADT/SmallVector.h>
#include <llvm/Support/Error.h>

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

    m_Module->dump();

    m_Module->dropAllReferences();

    llvm::logAllUnhandledErrors(std::move(deferredErrors), llvm::errs(), "Unexpected errors: ");
}

llvm::Value *IRGenerator::generate(ASTNode* node) {
    if (auto expr = dynamic_cast<ASTExprNode*>(node)) {
        auto genExpr = generateExpr(expr);
        return genExpr;
    } else {
        if (auto declaration = dynamic_cast<ASTDeclarationNode*>(node)){
            return generateDeclaration(declaration);
        }

        if (auto assignment = dynamic_cast<ASTAssignmentNode*>(node)) {
            return generateAssignment(assignment);
        }

        if (auto returnStatement = dynamic_cast<ASTReturnNode*>(node)) {
            return generateReturn(returnStatement);
        }
        if (auto funcDef = dynamic_cast<ASTFunctionDefinitionNode*>(node)) {
            return generateFunctionDefinition(funcDef);
        }
        if (auto structDef = dynamic_cast<ASTStructDefinitionNode*>(node)) {
            return generateStructDefinition(structDef);
        }
        if (auto structInit = dynamic_cast<ASTStructInitializationNode*>(node)) {
            return generateStructInitialization(structInit);
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

    if (auto identifier = dynamic_cast<ASTIdentifierNode*>(expr)) {
        return generateIdentifier(identifier);
    }

    m_Logger.printError("The code you wrote cannot be compiled yet :(");
    return nullptr;
}

llvm::Value *IRGenerator::generateLiteralInt(ASTLiteralNode<int> *literalInt) {
    auto val = llvm::ConstantInt::get(m_LLVMContext, llvm::APInt(llvm::Type::getInt32Ty(m_LLVMContext)->getScalarSizeInBits(), literalInt->getValue()));
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

llvm::Value *IRGenerator::generateIdentifier(ASTIdentifierNode *identifier) {
    auto valueOrErr = (m_YAPLContext->getCurrentScope()->lookup(identifier->getName()));

    if (auto Err = valueOrErr.takeError()) {
        m_Logger.printError("Symbol not found: '{}'", identifier->getName());
        deferredErrors = llvm::joinErrors(std::move(deferredErrors), std::move(Err));
        return nullptr;
    }

    return *valueOrErr;
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
        if (auto var = m_YAPLContext->getCurrentScope()->lookup(declaration->getName())) {
            m_Logger.printError("Redefintion of {}.", declaration->getName());
            deferredErrors = llvm::joinErrors(std::move(deferredErrors),
                    llvm::make_error<RedefinitionError>(declaration->getName()));
            return nullptr;
        } else {
            llvm::consumeError(var.takeError());
        }

        auto llvmType = ASTTypeToLLVM(declaration->getType());
        m_Module->getOrInsertGlobal(declaration->getName(), llvmType);
        llvm::GlobalVariable *globalVar = m_Module->getNamedGlobal(declaration->getName());
        globalVar->setLinkage(llvm::GlobalValue::PrivateLinkage);
        globalVar->setAlignment(llvm::MaybeAlign(4));
        llvm::cantFail(m_YAPLContext->getCurrentScope()->pushValue(declaration->getName(), globalVar));

        return globalVar;
    }

    if (auto val = m_YAPLContext->getCurrentScope()->lookupScope(declaration->getName())) {
        m_Logger.printError("Redefintion of {}", declaration->getName());
        deferredErrors = llvm::joinErrors(std::move(deferredErrors),
                llvm::make_error<RedefinitionError>(declaration->getName()));
        return nullptr;
    } else {
        llvm::consumeError(val.takeError());
    }

    auto llvmType = ASTTypeToLLVM(declaration->getType());

    auto currentFunction = m_YAPLContext->getCurrentScope()->getCurrentFunction();

    llvm::IRBuilder<> tmpBuilder(&currentFunction->getEntryBlock(),
            currentFunction->getEntryBlock().begin());

    auto variable = tmpBuilder.CreateAlloca(llvmType, 0, declaration->getName());

    llvm::cantFail(m_YAPLContext->getCurrentScope()->pushValue(declaration->getName(), variable));

    return variable;
}

llvm::Value *IRGenerator::generateInitialization(ASTInitializationNode *initialization) {
    if (m_YAPLContext->isAtTopLevelScope()) {
        if (auto var = m_YAPLContext->getCurrentScope()->lookup(initialization->getName())) {
            m_Logger.printError("Redefintion of {}.", initialization->getName());
            deferredErrors = llvm::joinErrors(std::move(deferredErrors),
                    llvm::make_error<RedefinitionError>(initialization->getName()));
            return nullptr;
        } else {
            llvm::consumeError(std::move(var.takeError()));
        }

        auto llvmType = ASTTypeToLLVM(initialization->getType());
        auto valuePtr = initialization->getValue();
        auto value = generateExpr(valuePtr);
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

    if (auto var = m_YAPLContext->getCurrentScope()->lookupScope(initialization->getName())) {
        m_Logger.printError("Redefintion of {}", initialization->getName());
        deferredErrors = llvm::joinErrors(std::move(deferredErrors),
                llvm::make_error<RedefinitionError>(initialization->getName()));
        return nullptr;
    } else {
        llvm::consumeError(std::move(var.takeError()));
    }

    auto llvmType = ASTTypeToLLVM(initialization->getType());
    auto valuePtr = initialization->getValue();
    auto value = generateExpr(valuePtr);

    auto variableAlloc = m_Builder.CreateAlloca(llvmType, nullptr, initialization->getName());
    auto variableStore = m_Builder.CreateStore(value, variableAlloc);

    llvm::cantFail(m_YAPLContext->getCurrentScope()->pushValue(initialization->getName(), variableStore));

    return variableStore;
}

llvm::Value *IRGenerator::generateAssignment(ASTAssignmentNode *assignment) {
    if (m_YAPLContext->isAtTopLevelScope()) {
        m_Logger.printError("Assignment are forbidden outside of a function body");
        return nullptr;
    }

    auto variable = m_YAPLContext->getCurrentScope()->lookup(assignment->getName());
    if (variable) {
        llvm::Value *value = generateExpr(assignment->getValue());
        return m_Builder.CreateStore(value, *variable);
    } else {
        auto err = variable.takeError();
        deferredErrors = llvm::joinErrors(std::move(deferredErrors), std::move(err));
        return nullptr;
    }
}

llvm::Value *IRGenerator::generateFunctionDefinition(ASTFunctionDefinitionNode *funcDef) {
    if (auto var = m_YAPLContext->getCurrentScope()->lookupFunction(funcDef->getName())) {
        m_Logger.printError("Redefintion of {}.", funcDef->getName());
        deferredErrors = llvm::joinErrors(std::move(deferredErrors),
                llvm::make_error<RedefinitionError>(funcDef->getName()));
        return nullptr;
    } else {
        llvm::consumeError(std::move(var.takeError()));
    }

    auto llvmReturnType = ASTTypeToLLVM(funcDef->getType());
    auto argsVector = std::move(funcDef->getArgs());
    llvm::SmallVector<llvm::Type *, 10> argsType;

    for ( const auto& arg: argsVector ) {
        argsType.push_back(ASTTypeToLLVM(arg->getType()));
    }

    auto funcType = llvm::FunctionType::get(llvmReturnType, argsType, false);
    auto func = llvm::Function::Create(funcType,
            llvm::Function::PrivateLinkage,
            funcDef->getName(),
            m_Module.get());

    llvm::cantFail(m_YAPLContext->getCurrentScope()->pushFunction(funcDef->getName(), func));

    m_YAPLContext->pushScope();
    m_YAPLContext->getCurrentScope()->setCurrentFunction(func);

    auto entryBlock = llvm::BasicBlock::Create(m_LLVMContext, "entry", func);

    auto parentBlock = m_Builder.GetInsertBlock();
    m_Builder.SetInsertPoint(entryBlock);

    uint32_t i = 0;
    for ( const auto& arg: argsVector ) {
        func->getArg(i)->setName(arg->getName());
        auto argDecl = generateDeclaration(arg.get());
        m_Builder.CreateStore(func->getArg(i), argDecl);
        ++i;
    }

    if(generateBlock(funcDef->getBody())) {
        return func;
    }
    
    m_Builder.SetInsertPoint(parentBlock);

    func->eraseFromParent();

    return nullptr;
}

bool IRGenerator::generateBlock(ASTBlockNode *block) {

    for( const auto& node : *block ) {
        auto generatedNode = generate(node.get());

        if (!generatedNode) {
            return false;
        }
    }

    return true;
}

llvm::Value *IRGenerator::generateReturn(ASTReturnNode* returnNode) {
    auto retExpr = returnNode->getExpr();
    auto genExpr = generateExpr(retExpr);

    return m_Builder.CreateRet(genExpr);
}

// TODO:
//  - Make a YAPLStruct which save methods and args and add method at each new struct init
llvm::Value *IRGenerator::generateStructDefinition(ASTStructDefinitionNode* structDef) {
    std::string name = structDef->getName();
    auto attributes = structDef->getAttributes();
    auto methods = structDef->getMethods();

    llvm::SmallVector<llvm::Type *, 10> llvmTypes;

    for ( const auto &attribute: attributes ) {
        llvmTypes.push_back(ASTTypeToLLVM(attribute->getType()));
    }
    
    for ( const auto &method: methods ) {
        auto methodRet = ASTTypeToLLVM(method->getType());
        auto argsVector = std::move(method->getArgs());
        llvm::SmallVector<llvm::Type *, 10> argsType;

        for ( const auto& arg: argsVector ) {
            argsType.push_back(ASTTypeToLLVM(arg->getType()));
        }

        auto funcType = llvm::FunctionType::get(methodRet, argsType, false);
        llvmTypes.push_back(funcType);
    }

    llvm::StructType *structType = llvm::StructType::create(m_LLVMContext, llvmTypes, "struct." + name);

    return nullptr;
}

llvm::Value *IRGenerator::generateStructInitialization(ASTStructInitializationNode* structInit) {
    auto structType = m_Module->getTypeByName("struct." + structInit->getStruct()->getName());
    
    if (!structType) {
        m_Logger.printError("Unknown struct identifier: {}", structInit->getStruct()->getName());
    }

    m_Module->getOrInsertGlobal(structInit->getName(), structType);
    llvm::GlobalVariable *globalVar = m_Module->getNamedGlobal(structInit->getName());
    globalVar->setLinkage(llvm::GlobalValue::PrivateLinkage);
    globalVar->setAlignment(llvm::MaybeAlign(4));
    llvm::cantFail(m_YAPLContext->getCurrentScope()->pushValue(structInit->getName(), globalVar));

    llvm::SmallVector<llvm::Constant *, 5> structVals;

    for ( const auto& val:structInit->getAttributesValues() ) {
        auto expr = generateExpr(val.get());
        structVals.push_back((llvm::Constant *)expr);
    }

    llvm::ConstantStruct::get(structType, structVals);


    return globalVar;
}
