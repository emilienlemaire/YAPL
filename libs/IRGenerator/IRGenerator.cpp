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

    llvm::logAllUnhandledErrors(std::move(deferredErrors), llvm::errs(), "Unexpected errors: \n");
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
        if (auto structAssignment = dynamic_cast<ASTStructAssignmentNode*>(node)) {
            return generateStructAssignment(structAssignment);
        }
        if (auto attrAssignment = dynamic_cast<ASTAttributeAssignmentNode*>(node)) {
            return generateAttributeAssignment(attrAssignment);
        }
        if (auto arrAssignment = dynamic_cast<ASTArrayAssignmentNode*>(node)) {
            return generateArrayAssignment(arrAssignment);
        }
        if (auto arrMemAssignment = dynamic_cast<ASTArrayMemeberAssignmentNode*>(node)) {
            return generateArrayMemberAssignment(arrMemAssignment);
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

    if (auto attrAccess = dynamic_cast<ASTAttributeAccessNode*>(expr)) {
        return generateAttributeAccess(attrAccess);
    }

    if (auto arrAccess = dynamic_cast<ASTArrayAccessNode*>(expr)) {
        return generateArrayAccess(arrAccess);
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

    auto value = m_Builder.CreateLoad(*valueOrErr, identifier->getName());

    return value;
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

llvm::Value *IRGenerator::generateDeclaration(ASTDeclarationNode *declaration) {
    if (auto initialization = dynamic_cast<ASTInitializationNode*>(declaration)) {
        return generateInitialization(initialization);
    }

    if (auto arrDef = dynamic_cast<ASTArrayDefinitionNode*>(declaration)) {
        return generateArrayDefinition(arrDef);
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

    if (!value)
        return nullptr;

    auto variableAlloc = m_Builder.CreateAlloca(llvmType, nullptr, initialization->getName());
    auto variableStore = m_Builder.CreateStore(value, variableAlloc);

    llvm::cantFail(m_YAPLContext->getCurrentScope()->pushValue(initialization->getName(), variableAlloc));

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
        m_YAPLContext->popScope();
        return func;
    }

    m_Builder.SetInsertPoint(parentBlock);

    m_YAPLContext->popScope();
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
//      - Remove YAPLStruct, doesn't work.
//      - Figure out a way to make methods work
llvm::Value *IRGenerator::generateStructDefinition(ASTStructDefinitionNode* structDef) {
    std::string name = structDef->getName();
    auto attributes = structDef->getAttributes();
    auto methods = structDef->getMethods();

    llvm::SmallVector<llvm::Type *, 10> llvmTypes;
    llvm::SmallVector<std::string, 10> argsName;

    for ( const auto &attribute: attributes ) {
        llvmTypes.push_back(ASTTypeToLLVM(attribute->getType()));
        argsName.push_back(attribute->getName());
    }

    llvm::StructType *structType = llvm::StructType::create(m_LLVMContext, llvmTypes, "struct." + name);

    uint32_t i = 0;
    for ( const auto &attr : attributes ) {
        m_YAPLContext->addAttributeOffset("struct." + name + "." + attr->getName(), i);
        i++;
    }

    for ( const auto &method: methods ) {
        generateMethod(structType, argsName, method.get());
    }

    return nullptr;
}

llvm::Value *IRGenerator::generateStructInitialization(ASTStructInitializationNode* structInit) {
    auto structType = m_Module->getTypeByName("struct." + structInit->getStruct()->getName());

    if (!structType) {
        m_Logger.printError("Unknown struct identifier: {}", structInit->getStruct()->getName());
    }
    if (m_YAPLContext->isAtTopLevelScope()) {
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

        globalVar->setInitializer(llvm::ConstantStruct::get(structType, structVals));

        return globalVar;
    }

    if (auto var = m_YAPLContext->getCurrentScope()->lookupScope(structInit->getName())) {
        m_Logger.printError("Redefintion of {}", structInit->getName());
        deferredErrors = llvm::joinErrors(std::move(deferredErrors),
                llvm::make_error<RedefinitionError>(structInit->getName()));
        return nullptr;
    } else {
        llvm::consumeError(std::move(var.takeError()));
    }

    llvm::SmallVector<llvm::Constant *, 5> structVals;

    for ( const auto &elt: structInit->getAttributesValues() ) {
        auto val = generateExpr(elt.get());
        structVals.push_back((llvm::Constant *)val);
    }

    auto variableAlloc = m_Builder.CreateAlloca(structType, nullptr, structInit->getName());
    auto variableStore = m_Builder.CreateStore(llvm::ConstantStruct::get(structType, structVals), variableAlloc);

    llvm::cantFail(m_YAPLContext->getCurrentScope()->pushValue(structInit->getName(), variableAlloc));

    return variableStore;
}

llvm::Value *IRGenerator::generateMethod(llvm::StructType* structType,
        llvm::SmallVector<std::string, 10> attrName,
        ASTFunctionDefinitionNode *method) {
    auto retType = ASTTypeToLLVM(method->getType());
    auto args = method->getArgs();

    llvm::SmallVector<llvm::Type *, 5> argsType;
    argsType.push_back(structType);

    for ( const auto &arg: args ) {
        argsType.push_back(ASTTypeToLLVM(arg->getType()));
    }

    auto methodType = llvm::FunctionType::get(retType, argsType, false);
    auto methodDef = llvm::Function::Create(
            methodType,
            llvm::Function::LinkOnceODRLinkage,
            structType->getName() + "." + method->getName(),
            m_Module.get());

    llvm::cantFail(m_YAPLContext->getCurrentScope()->pushFunction(method->getName(), methodDef));

    m_YAPLContext->pushScope();
    m_YAPLContext->getCurrentScope()->setCurrentFunction(methodDef);

    auto entryBlock = llvm::BasicBlock::Create(m_LLVMContext, "entry", methodDef);

    auto parentBlock = m_Builder.GetInsertBlock();
    m_Builder.SetInsertPoint(entryBlock);

    methodDef->getArg(0)->setName("this");
    auto structDecl = m_Builder.CreateAlloca(structType, nullptr, "this");
    m_Builder.CreateStore(methodDef->getArg(0), structDecl);
    auto currentFunction = m_YAPLContext->getCurrentScope()->getCurrentFunction();

    uint32_t i = 1;
    for ( const auto& arg: args ) {
        methodDef->getArg(i)->setName(arg->getName());
        auto argDecl = generateDeclaration(arg.get());
        m_Builder.CreateStore(methodDef->getArg(i), argDecl);
        i++;
    }

    i = 0;
    for ( const auto& attr: structType->elements() ) {
        llvm::IRBuilder<> tmpBuilder(&currentFunction->getEntryBlock(),
                currentFunction->getEntryBlock().begin());
        auto attrDecl = tmpBuilder.CreateAlloca(attr, 0, attrName[i]);
        llvm::cantFail(m_YAPLContext->getCurrentScope()->pushValue(attrName[i], attrDecl));
        auto GEP = m_Builder.CreateStructGEP(structDecl, i, "gep." + attrName[i]);
        auto load = m_Builder.CreateLoad(GEP, attrName[i]);
        auto store = m_Builder.CreateStore(load, attrDecl);
        i++;
    }


    if(generateBlock(method->getBody())) {
        m_YAPLContext->popScope();
        return methodDef;
    }

    m_Builder.SetInsertPoint(parentBlock);

    m_YAPLContext->popScope();
    methodDef->eraseFromParent();

    return nullptr;
}

llvm::Value *IRGenerator::generateStructAssignment(ASTStructAssignmentNode *structAssignment) {
    if (m_YAPLContext->isAtTopLevelScope()) {
        m_Logger.printError("Assignment outside of a function.");
        return nullptr;
    } 

    auto structValue = m_YAPLContext->getCurrentScope()->lookup(structAssignment->getName());

    if (auto err = structValue.takeError()) {
        deferredErrors = llvm::joinErrors(std::move(deferredErrors), std::move(err));
        return nullptr;
    }

    auto newValues = structAssignment->getAttributesValues();

    auto structDebug = *structValue;

    uint32_t numElts = (*structValue)->getType()->getPointerElementType()->getStructNumElements();
    if (numElts != newValues.size()) {
        m_Logger.printError("Struct assignment to '{}' expected {} values instead of {}",
                structAssignment->getName(),
                numElts,
                newValues.size()
                );
        return nullptr;
    }

    uint32_t i = 0;
    for (const auto &value: newValues) {
        auto eltPtr = m_Builder.CreateStructGEP(*structValue, i, "elt" + llvm::itostr(i) + "ptr");
        auto eltValue = generateExpr(newValues[i].get());
        m_Builder.CreateStore(eltValue, eltPtr);
        i++;
    }

    return *structValue;
}

llvm::Value *IRGenerator::generateAttributeAssignment(ASTAttributeAssignmentNode *attrAssignment) {
    auto structPtrOrErr = m_YAPLContext->getCurrentScope()->lookup(attrAssignment->getStructName());

    if (auto err = structPtrOrErr.takeError()) {
        m_Logger.printError("Undefined symbol {}", attrAssignment->getStructName());
        deferredErrors = llvm::joinErrors(std::move(deferredErrors), std::move(err));
    }

    auto structPtr = *structPtrOrErr;
    std::string typeName = structPtr->getType()->getPointerElementType()->getStructName().str();

    uint32_t offset = m_YAPLContext->getAttributeOffset(typeName + "." + attrAssignment->getAttributeName());

    auto eltPtr = m_Builder.CreateStructGEP(structPtr, offset, attrAssignment->getAttributeName() + "gep");
    auto expr = generateExpr(attrAssignment->getValue());
    auto store = m_Builder.CreateStore(expr, eltPtr);

    return structPtr;
}

llvm::Value *IRGenerator::generateAttributeAccess(ASTAttributeAccessNode *attrAccess) {
    auto structPtrOrErr = m_YAPLContext->getCurrentScope()->lookup(attrAccess->getName());

    if (auto err = structPtrOrErr.takeError()) {
        m_Logger.printError("Undefined symbol: {}", attrAccess->getName());
        deferredErrors = llvm::joinErrors(std::move(deferredErrors), std::move(err));
        return nullptr;
    }

    auto structPtr = *structPtrOrErr;
    std::string typeName = structPtr->getType()->getPointerElementType()->getStructName().str();
    
    uint32_t offset = m_YAPLContext->getAttributeOffset(typeName + "." + attrAccess->getAttribute());
    
    auto eltPtr = m_Builder.CreateStructGEP(structPtr, offset, attrAccess->getAttribute() + "gep");
    auto load = m_Builder.CreateLoad(eltPtr, attrAccess->getName() + "." + attrAccess->getAttribute());

    return load;
}

llvm::Value *IRGenerator::generateArrayDefinition(ASTArrayDefinitionNode *arrDef) {
    if (auto var = m_YAPLContext->getCurrentScope()->lookup(arrDef->getName())) {
        m_Logger.printError("Redefintion of {}.", arrDef->getName());
        deferredErrors = llvm::joinErrors(std::move(deferredErrors),
                llvm::make_error<RedefinitionError>(arrDef->getName()));
        return nullptr;
    } else {
        llvm::consumeError(var.takeError());
    }

    if (auto arrInit = dynamic_cast<ASTArrayInitializationNode*>(arrDef)) {
        return generateArrayInitialization(arrInit);
    }

    if (m_YAPLContext->isAtTopLevelScope()) {
        auto globalVar = (llvm::GlobalVariable *)m_Module->getOrInsertGlobal(arrDef->getName(),
                llvm::ArrayType::get(ASTTypeToLLVM(arrDef->getType()), arrDef->getSize())
                );
        globalVar->setLinkage(llvm::GlobalVariable::PrivateLinkage);
        globalVar->setAlignment(llvm::MaybeAlign(4));
        llvm::cantFail(m_YAPLContext->getCurrentScope()->pushValue(arrDef->getName(), globalVar));
        
        return globalVar;
    }

    auto llvmType = llvm::ArrayType::get(ASTTypeToLLVM(arrDef->getType()), arrDef->getSize());

    auto currentFunction = m_YAPLContext->getCurrentScope()->getCurrentFunction();

    llvm::IRBuilder<> tmpBuilder(&currentFunction->getEntryBlock(),
            currentFunction->getEntryBlock().begin());

    auto arr = tmpBuilder.CreateAlloca(llvmType, 0, arrDef->getName());

    llvm::cantFail(m_YAPLContext->getCurrentScope()->pushValue(arrDef->getName(), arr));

    return arr;
}

llvm::Value *IRGenerator::generateArrayInitialization(ASTArrayInitializationNode *arrInit) {
    if (m_YAPLContext->isAtTopLevelScope()) {
        auto globalVar = (llvm::GlobalVariable *)m_Module->getOrInsertGlobal(arrInit->getName(),
                llvm::ArrayType::get(ASTTypeToLLVM(arrInit->getType()), arrInit->getSize())
                );
        globalVar->setLinkage(llvm::GlobalVariable::PrivateLinkage);
        globalVar->setAlignment(llvm::MaybeAlign(4));

        std::vector<llvm::Constant *> arrVals;
        arrVals.reserve(arrInit->getSize());

        uint32_t i = 0;
        for ( const auto &val: *arrInit ) {
            auto value = generateExpr(val.get());
            arrVals.push_back((llvm::Constant *)value);
            i++;
        }

        if (i != arrInit->getSize()) {
            m_Logger.printError("Array initialization of wrong size.");
            return nullptr;
        }

        globalVar->setInitializer(llvm::ConstantArray::get(
                    llvm::ArrayType::get(ASTTypeToLLVM(arrInit->getType()), arrInit->getSize()),
                    arrVals));
        llvm::cantFail(m_YAPLContext->getCurrentScope()->pushValue(arrInit->getName(), globalVar));

        return globalVar;
    }

    auto llvmType = llvm::ArrayType::get(ASTTypeToLLVM(arrInit->getType()), arrInit->getSize());

    auto currentFunc = m_YAPLContext->getCurrentScope()->getCurrentFunction();

    llvm::IRBuilder<> tmpBuilder(&currentFunc->getEntryBlock(),
            currentFunc->getEntryBlock().begin());

    auto arr = tmpBuilder.CreateAlloca(llvmType, 0, arrInit->getName());
    std::vector<llvm::Value *> arrVals;

    uint32_t i = 0;
    for ( const auto &val: *arrInit ) {
        auto value = generateExpr(val.get());
        arrVals.push_back(value);
        i++;
    }

    if (i != arrInit->getSize()) {
        m_Logger.printError("Array initialization of wrong size.");
        return nullptr;
    }

    i = 0;
    for ( const auto &val: arrVals ) {
        auto eltPtr = m_Builder.CreateConstGEP2_32(
                llvmType,
                arr,
                0, i,
                "elt" + llvm::itostr(i) + ".gep"
                );
        auto store = m_Builder.CreateStore(val, eltPtr);
        i++;
    }

    llvm::cantFail(m_YAPLContext->getCurrentScope()->pushValue(arrInit->getName(), arr));

    return arr;
}

llvm::Value *IRGenerator::generateArrayAssignment(ASTArrayAssignmentNode *arrAssignment) {
    if (m_YAPLContext->isAtTopLevelScope()) {
        m_Logger.printError("Assignment at top level scope is forbidden");
        return nullptr;
    }
    auto arrOrErr = m_YAPLContext->getCurrentScope()->lookup(arrAssignment->getName());

    if (auto err = arrOrErr.takeError()) {
        m_Logger.printError("Undefined symbol: '{}'", arrAssignment->getName());
        deferredErrors = llvm::joinErrors(std::move(deferredErrors), std::move(err));
        return nullptr;
    }

    auto arr = *arrOrErr;

    if (arr->getType()->getPointerElementType()->getArrayNumElements() != arrAssignment->getSize()) {
        m_Logger.printError("Unexpected array assignment: Expecting {} elements instead of {}",
                arr->getType()->getPointerElementType()->getArrayNumElements(),
                arrAssignment->getSize());
        return nullptr;
    }

    uint32_t i = 0;
    for (const auto &val : *arrAssignment) {
        auto value = generateExpr(val.get());
        auto eltPtr = m_Builder.CreateConstGEP2_32(
                arr->getType()->getPointerElementType(),
                arr,
                0, i,
                "elt" + llvm::itostr(i) + ".gep"
                );
        m_Builder.CreateStore(value, eltPtr);
        i++;
    }

    return arr;
}

llvm::Value *IRGenerator::generateArrayMemberAssignment(ASTArrayMemeberAssignmentNode *arrMemAssignment) {
    auto arrOrErr = m_YAPLContext->getCurrentScope()->lookup(arrMemAssignment->getName());

    if (auto err = arrOrErr.takeError()) {
        m_Logger.printError("Undefined symbol: '{}'", arrMemAssignment->getName());
        deferredErrors = llvm::joinErrors(std::move(deferredErrors), std::move(err));
        return nullptr;
    }

    auto arr = *arrOrErr;

    size_t arrSize = arr->getType()->getPointerElementType()->getArrayNumElements();
    size_t index = arrMemAssignment->getIndex();

    if (index >= arrSize) {
        m_Logger.printError("Index out of bound {}: array '{}' has a size of {}",
                index,
                arrMemAssignment->getName(),
                arr->getType()->getPointerElementType()->getArrayNumElements()
                );
        return nullptr;
    }
    auto eltPtr = m_Builder.CreateConstGEP2_32(
            arr->getType()->getPointerElementType(),
            arr,
            0, arrMemAssignment->getIndex());

    auto val = generateExpr(arrMemAssignment->getValue());

    m_Builder.CreateStore(val, eltPtr);
    
    return arr;
}

llvm::Value *IRGenerator::generateArrayAccess(ASTArrayAccessNode *arrAccess) {
    auto arrOrErr = m_YAPLContext->getCurrentScope()->lookup(arrAccess->getName());

    if (auto err = arrOrErr.takeError()) {
        m_Logger.printError("Undefined symbol: '{}'", arrAccess->getName());
        deferredErrors = llvm::joinErrors(std::move(deferredErrors), std::move(err));
        return nullptr;
    }

    auto arr = *arrOrErr;

    size_t arrSize = arr->getType()->getPointerElementType()->getArrayNumElements();
    size_t index = arrAccess->getIndex();

    if (index >= arrSize) {
        m_Logger.printError("Index out of bound {}: array '{}' has a size of {}",
                index,
                arrAccess->getName(),
                arr->getType()->getPointerElementType()->getArrayNumElements()
                );
        return nullptr;
    }

    auto GEP = m_Builder.CreateConstGEP2_32(
            arr->getType()->getPointerElementType(),
            arr,
            0, arrAccess->getIndex());

    return m_Builder.CreateLoad(GEP);
}
