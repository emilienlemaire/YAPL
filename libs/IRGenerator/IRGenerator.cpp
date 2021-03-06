#include <array>
#include <iostream>

#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/Verifier.h>
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


    //auto anonFuncType = llvm::FunctionType::get(llvm::Type::getInt32Ty(m_LLVMContext), false);
    //auto anonFunc = llvm::Function::Create(
            //anonFuncType,
            //llvm::Function::ExternalLinkage,
            //"__anon_expr",
            //m_Module.get()
            //);
    //llvm::BasicBlock::Create(m_LLVMContext, "entry", anonFunc);

    m_Module->print(llvm::errs(), nullptr);

    m_Module->dropAllReferences();

    if (m_DeferredErrors) {
        std::string str;
        llvm::raw_string_ostream os(str);
        os << m_DeferredErrors;
        m_Logger.printError("Unhandled errors:\n  {}", os.str());
    }

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

        if (auto ifNode = dynamic_cast<ASTIfNode*>(node)) {
            return generateIf(ifNode);
        }

        if (auto forNode = dynamic_cast<ASTForNode*>(node)) {
            return generateFor(forNode);
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

    if (auto methodCall = dynamic_cast<ASTMethodCallNode*>(expr)) {
        return generateMethodCall(methodCall);
    }

    if (auto attrAccess = dynamic_cast<ASTAttributeAccessNode*>(expr)) {
        return generateAttributeAccess(attrAccess);
    }

    if (auto arrAccess = dynamic_cast<ASTArrayAccessNode*>(expr)) {
        return generateArrayAccess(arrAccess);
    }

    if (auto funcCall = dynamic_cast<ASTFunctionCallNode*>(expr)) {
        return generateFunctionCall(funcCall);
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
        m_DeferredErrors = llvm::joinErrors(std::move(m_DeferredErrors), std::move(Err));
        return nullptr;
    }

    auto value = m_Builder.CreateLoad(*valueOrErr, identifier->getName());

    return value;
}

llvm::Value *IRGenerator::generateBinary(ASTBinaryNode *bin) {
    auto lhs = bin->getLeftOperrand();
    auto rhs = bin->getRightOperrand();

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
            m_DeferredErrors = llvm::joinErrors(std::move(m_DeferredErrors),
                    llvm::make_error<RedefinitionError>(declaration->getName()));
            return nullptr;
        } else {
            llvm::consumeError(var.takeError());
        }

        auto llvmType = ASTTypeToLLVM(declaration->getType(), declaration->getStructName());
        m_Module->getOrInsertGlobal(declaration->getName(), llvmType);
        llvm::GlobalVariable *globalVar = m_Module->getNamedGlobal(declaration->getName());
        globalVar->setLinkage(llvm::GlobalValue::PrivateLinkage);
        globalVar->setAlignment(llvm::MaybeAlign(4));
        llvm::cantFail(m_YAPLContext->getCurrentScope()->pushValue(declaration->getName(), globalVar));

        return globalVar;
    }

    if (auto val = m_YAPLContext->getCurrentScope()->lookupScope(declaration->getName())) {
        m_Logger.printError("Redefintion of {}", declaration->getName());
        m_DeferredErrors = llvm::joinErrors(std::move(m_DeferredErrors),
                llvm::make_error<RedefinitionError>(declaration->getName()));
        return nullptr;
    } else {
        llvm::consumeError(val.takeError());
    }

    auto llvmType = ASTTypeToLLVM(declaration->getType(), declaration->getStructName());

    auto currentFunction = m_YAPLContext->getCurrentScope()->getCurrentFunction();

    llvm::IRBuilder<> tmpBuilder(&currentFunction->getEntryBlock(),
            currentFunction->getEntryBlock().begin());

    auto variable = tmpBuilder.CreateAlloca(llvmType, nullptr, declaration->getName());

    llvm::cantFail(m_YAPLContext->getCurrentScope()->pushValue(declaration->getName(), variable));

    return variable;
}

llvm::Value *IRGenerator::generateInitialization(ASTInitializationNode *initialization) {
    if (m_YAPLContext->isAtTopLevelScope()) {
        if (auto var = m_YAPLContext->getCurrentScope()->lookup(initialization->getName())) {
            m_Logger.printError("Redefintion of {}.", initialization->getName());
            m_DeferredErrors = llvm::joinErrors(std::move(m_DeferredErrors),
                    llvm::make_error<RedefinitionError>(initialization->getName()));
            return nullptr;
        } else {
            llvm::consumeError(var.takeError());
        }

        auto llvmType = ASTTypeToLLVM(initialization->getType());
        auto valuePtr = initialization->getValue();
        auto value = generateExpr(valuePtr);
        m_Module->getOrInsertGlobal(initialization->getName(), llvmType);
        llvm::GlobalVariable *globalVar = m_Module->getNamedGlobal(initialization->getName());
        globalVar->setLinkage(llvm::GlobalValue::PrivateLinkage);
        globalVar->setAlignment(llvm::MaybeAlign(4));

        globalVar->setInitializer(static_cast<llvm::Constant *>(value));
        llvm::cantFail(m_YAPLContext->getCurrentScope()->pushValue(initialization->getName(), globalVar));

        return globalVar;
    }

    if (auto var = m_YAPLContext->getCurrentScope()->lookupScope(initialization->getName())) {
        m_Logger.printError("Redefintion of {}", initialization->getName());
        m_DeferredErrors = llvm::joinErrors(std::move(m_DeferredErrors),
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
        m_DeferredErrors = llvm::joinErrors(std::move(m_DeferredErrors), std::move(err));
        return nullptr;
    }
}

llvm::Value *IRGenerator::generateFunctionDefinition(ASTFunctionDefinitionNode *funcDef) {
    if (auto var = m_YAPLContext->getCurrentScope()->lookupFunction(funcDef->getName())) {
        m_Logger.printError("Redefintion of {}.", funcDef->getName());
        m_DeferredErrors = llvm::joinErrors(std::move(m_DeferredErrors),
                llvm::make_error<RedefinitionError>(funcDef->getName()));
        return nullptr;
    } else {
        llvm::consumeError(std::move(var.takeError()));
    }

    auto llvmReturnType = ASTTypeToLLVM(funcDef->getType(), funcDef->getReturnStructName());
    auto argsVector = std::move(funcDef->getArgs());
    llvm::SmallVector<llvm::Type *, 10> argsType;

    for ( const auto& arg: argsVector ) {
        argsType.push_back(ASTTypeToLLVM(arg->getType(), arg->getStructName()));
    }

    auto funcType = llvm::FunctionType::get(llvmReturnType, argsType, false);
    auto func = llvm::Function::Create(funcType,
            llvm::Function::ExternalLinkage,
            funcDef->getName(),
            m_Module.get());

    auto parentBlock = m_Builder.GetInsertBlock();
    llvm::BasicBlock* returnBlock = llvm::BasicBlock::Create(m_LLVMContext, "return");
    m_Builder.SetInsertPoint(returnBlock);
    llvm::PHINode* returnNode = m_Builder.CreatePHI(llvmReturnType, 1, "returnNode");
    if (funcDef->getType() != ASTNode::VOID) {
        m_YAPLContext->setReturnHelper(returnBlock, returnNode);
        m_Builder.CreateRet(returnNode);
    }
    m_Builder.SetInsertPoint(parentBlock);

    m_YAPLContext->pushScope();
    m_YAPLContext->getCurrentScope()->setCurrentFunction(func);

    auto entryBlock = llvm::BasicBlock::Create(m_LLVMContext, "entry", func);

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
        llvm::cantFail(m_YAPLContext->getCurrentScope()->pushFunction(funcDef->getName(), func));
        if (funcDef->getType() != ASTNode::VOID)
            func->getBasicBlockList().push_back(m_YAPLContext->getReturnBlock());
        else
            m_Builder.CreateRetVoid();
        m_YAPLContext->resetReturnHelper();
        if (llvm::verifyFunction(*func, &llvm::outs())) {
            m_Logger.printError("Bad function: {}", func->getName().str());
            //func->print(llvm::errs());
        }
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

    llvm::BasicBlock* incomingBlock = m_Builder.GetInsertBlock();
    
    m_YAPLContext->addPhiNodeIncomming(incomingBlock, genExpr);

    return m_Builder.CreateBr(m_YAPLContext->getReturnBlock());
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
        m_DeferredErrors = llvm::joinErrors(std::move(m_DeferredErrors),
                llvm::make_error<RedefinitionError>(structInit->getName()));
        return nullptr;
    } else {
        llvm::consumeError(std::move(var.takeError()));
    }

    llvm::SmallVector<llvm::Constant *, 5> structVals;
    auto variableAlloc = m_Builder.CreateAlloca(structType, nullptr, structInit->getName());

    int i = 0;
    for ( const auto &elt: structInit->getAttributesValues() ) {
        auto val = generateExpr(elt.get());
        auto gep = m_Builder.CreateConstGEP2_32(structType, variableAlloc, 0, i, "gep" + std::to_string(i));
        auto store = m_Builder.CreateStore(val, gep);
        i++;
    }

    llvm::cantFail(m_YAPLContext->getCurrentScope()->pushValue(structInit->getName(), variableAlloc));

    return variableAlloc;
}

llvm::Value *IRGenerator::generateMethod(llvm::StructType* structType,
        llvm::SmallVector<std::string, 10> attrName,
        ASTFunctionDefinitionNode *method) {
    auto retType = ASTTypeToLLVM(method->getType());
    auto args = method->getArgs();
    auto methodMangledName = structType->getName() + "." + method->getName();

    if(auto var = m_YAPLContext->getCurrentScope()->lookupFunction(methodMangledName.str())) {
        m_Logger.printError("Redefinition of {}", methodMangledName.str());
        m_DeferredErrors = llvm::joinErrors(std::move(m_DeferredErrors),
                llvm::make_error<RedefinitionError>(methodMangledName.str()));
    }

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


    auto parentBlock = m_Builder.GetInsertBlock();
    auto returnBlock = llvm::BasicBlock::Create(m_LLVMContext, "return");
    m_Builder.SetInsertPoint(returnBlock);
    auto returnNode = m_Builder.CreatePHI(retType, 1, "returnNode");
    if (method->getType() != ASTNode::VOID) {
        m_YAPLContext->setReturnHelper(returnBlock, returnNode);
        m_Builder.CreateRet(returnNode);
    }

    m_Builder.SetInsertPoint(parentBlock);

    m_YAPLContext->pushScope();
    m_YAPLContext->getCurrentScope()->setCurrentFunction(methodDef);

    auto entryBlock = llvm::BasicBlock::Create(m_LLVMContext, "entry", methodDef);
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
        auto attrDecl = tmpBuilder.CreateAlloca(attr, nullptr, attrName[i]);
        llvm::cantFail(m_YAPLContext->getCurrentScope()->pushValue(attrName[i], attrDecl));
        auto GEP = m_Builder.CreateStructGEP(structDecl, i, "gep." + attrName[i]);
        auto load = m_Builder.CreateLoad(GEP, attrName[i]);
        m_Builder.CreateStore(load, attrDecl);
        i++;
    }


    if(generateBlock(method->getBody())) {
        m_YAPLContext->popScope();
        llvm::cantFail(m_YAPLContext->getCurrentScope()->pushFunction(methodDef->getName(), methodDef));
        if (method->getType() != ASTNode::VOID)
            methodDef->getBasicBlockList().push_back(m_YAPLContext->getReturnBlock());
        m_YAPLContext->resetReturnHelper();
        llvm::verifyFunction(*methodDef, &llvm::errs());
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
        m_DeferredErrors = llvm::joinErrors(std::move(m_DeferredErrors), std::move(err));
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
        m_DeferredErrors = llvm::joinErrors(std::move(m_DeferredErrors), std::move(err));
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
        m_DeferredErrors = llvm::joinErrors(std::move(m_DeferredErrors), std::move(err));
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
        m_DeferredErrors = llvm::joinErrors(std::move(m_DeferredErrors),
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
        globalVar->setLinkage(llvm::GlobalVariable::ExternalLinkage);
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
        m_DeferredErrors = llvm::joinErrors(std::move(m_DeferredErrors), std::move(err));
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
        m_DeferredErrors = llvm::joinErrors(std::move(m_DeferredErrors), std::move(err));
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
        m_DeferredErrors = llvm::joinErrors(std::move(m_DeferredErrors), std::move(err));
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

llvm::Value *IRGenerator::generateFunctionCall(ASTFunctionCallNode *call) {
    auto calleeIdentifier = call->getCallee();
    auto args = call->getArgs();

    std::string name = calleeIdentifier->getName();
    llvm::SmallVector<llvm::Value *, 5> argsValue;

    for(const auto &arg : args) {
        auto val = generateExpr(arg.get());
        argsValue.push_back(val);
    }

    auto funcOrErr = m_YAPLContext->getCurrentScope()->lookupFunction(name);

    if (auto err = funcOrErr.takeError()) {
        m_DeferredErrors = llvm::joinErrors(std::move(m_DeferredErrors), std::move(err));
        return nullptr;
    }

    llvm::Function *func = *funcOrErr;

    auto callInst = m_Builder.CreateCall(
            func->getFunctionType(),
            func,
            argsValue,
            "call" + name);

    return callInst;
}

llvm::Value *IRGenerator::generateMethodCall(ASTMethodCallNode *methodCall) {
    auto structOrErr = m_YAPLContext->getCurrentScope()->lookup(methodCall->getName());
    
    if (auto err = structOrErr.takeError()) {
        m_DeferredErrors = llvm::joinErrors(std::move(m_DeferredErrors), std::move(err));
        return nullptr;
    }

    auto structPtr = *structOrErr;

    std::string typeName = structPtr->getType()->getPointerElementType()->getStructName().str();

    auto structVar = m_Builder.CreateLoad(structPtr, "this");

    auto methodOrErr = m_YAPLContext->getCurrentScope()->lookupFunction(
            typeName + "." + methodCall->getAttribute()
            );

    if (auto err = methodOrErr.takeError()) {
        m_DeferredErrors = llvm::joinErrors(std::move(m_DeferredErrors), std::move(err));
        return nullptr;
    }

    auto method = *methodOrErr;

    auto args = methodCall->getArgs();
    llvm::SmallVector<llvm::Value *, 5> argVals;

    argVals.push_back(structVar);

    for (const auto &arg : args) {
        auto expr = generateExpr(arg.get());
        argVals.push_back(expr);
    }

    auto callInst = m_Builder.CreateCall(
            method,
            argVals,
            "call" + method->getName());

    return callInst;
}

llvm::Value *IRGenerator::generateIf(ASTIfNode *ifNode) {
    auto condExpr = ifNode->getCond();
    auto condVal = generateExpr(condExpr);

    if (m_YAPLContext->isAtTopLevelScope()) {
        m_Logger.printError("Cannot have an if statement in top level scope.");
        return nullptr;
    }
    

    if (condVal->getType()->getTypeID() != llvm::Type::getDoubleTy(m_LLVMContext)->getTypeID()) {
        condVal = m_Builder.CreateCast(
                llvm::Instruction::CastOps::SIToFP,
                condVal,
                llvm::Type::getDoubleTy(m_LLVMContext),
                "cast"
                );
    }

    uint8_t numRet = 0;

    condVal = m_Builder.CreateFCmpOGT(
            condVal,
            llvm::ConstantFP::get(m_LLVMContext, llvm::APFloat(0.0)),
            "ifcond"
            );

    llvm::Function *parentFunction = m_Builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *thenBlock = llvm::BasicBlock::Create(m_LLVMContext, "then", parentFunction);
    llvm::BasicBlock *elseBlock = llvm::BasicBlock::Create(m_LLVMContext, "else");
    llvm::BasicBlock *mergeBlock = llvm::BasicBlock::Create(m_LLVMContext, "merge");

    m_Builder.CreateCondBr(condVal, thenBlock, elseBlock);

    m_Builder.SetInsertPoint(thenBlock);

    if (!generateBlock(ifNode->getThen())) {
        m_Logger.printError("Failed to generate then block");
        return nullptr;
    } else {
        numRet++;
    }

    llvm::BranchInst *mergeBr;

    if (!m_Builder.GetInsertBlock()->getTerminator()) {
        mergeBr = m_Builder.CreateBr(mergeBlock);
    }
    
    parentFunction->getBasicBlockList().push_back(elseBlock);
    m_Builder.SetInsertPoint(elseBlock);

    if (!ifNode->getElse()) {
        mergeBr = m_Builder.CreateBr(mergeBlock);
        parentFunction->getBasicBlockList().push_back(mergeBlock);
        m_Builder.SetInsertPoint(mergeBlock);
        return mergeBr;
    }


    if(!generateBlock(ifNode->getElse())) {
        m_Logger.printError("Failed to generate else block");
        return nullptr;
    }

    if (!m_Builder.GetInsertBlock()->getTerminator()) {
        mergeBr = m_Builder.CreateBr(mergeBlock);
    } else {
        numRet++;
    }

    parentFunction->getBasicBlockList().push_back(mergeBlock);

    if (numRet == 2) {
        mergeBlock->eraseFromParent();
        return llvm::UndefValue::get(parentFunction->getReturnType());
    }

    m_Builder.SetInsertPoint(mergeBlock);

    return mergeBr;
}

llvm::Value *IRGenerator::generateFor(ASTForNode *forNode) {
    if (m_YAPLContext->isAtTopLevelScope()) {
        m_Logger.printError("For loop cannot be at top level scope");
        return nullptr;
    }

    auto currFunc = m_YAPLContext->getCurrentScope()->getCurrentFunction();

    m_YAPLContext->pushScope();

    m_YAPLContext->getCurrentScope()->setCurrentFunction(currFunc);

    auto it = generateDeclaration(forNode->getDecl());

    if (auto range = dynamic_cast<ASTRangeNode*>(forNode->getCond())) {
        auto startVal = generateExpr(range->getStart());
        auto stopVal = generateExpr(range->getStop());
        auto store = m_Builder.CreateStore(startVal, it);
        auto func = m_Builder.GetInsertBlock()->getParent();
        auto loopBB = llvm::BasicBlock::Create(m_LLVMContext, "loop", func);
        auto afterLoopBB = llvm::BasicBlock::Create(m_LLVMContext, "afterLoop");
        m_Builder.CreateBr(loopBB);
        m_Builder.SetInsertPoint(loopBB);
        if(!generateBlock(forNode->getBlock())) {
            m_Logger.printError("Failed to generate for loop body");
            return nullptr;
        }

        llvm::Value *brLoopCond;

        RangeOperator op = range->getOp();
        switch (op) {
            case RangeOperator::ft: { 
                auto one = llvm::ConstantInt::get(
                        m_LLVMContext,
                        llvm::APInt(llvm::Type::getInt32Ty(m_LLVMContext)->getScalarSizeInBits(), 1)
                        );
                auto itVal = m_Builder.CreateLoad(it);
                auto nextVal = m_Builder.CreateAdd(itVal, one, "nextval");
                store = m_Builder.CreateStore(nextVal, it);
                auto cond = m_Builder.CreateICmpSGT(nextVal, stopVal);
                brLoopCond = m_Builder.CreateCondBr(cond, afterLoopBB, loopBB);
                break;
            }
            case RangeOperator::fmt: {
                auto one = llvm::ConstantInt::get(
                        m_LLVMContext,
                        llvm::APInt(llvm::Type::getInt32Ty(m_LLVMContext)->getScalarSizeInBits(), 1)
                        );
                auto itVal = m_Builder.CreateLoad(it);
                auto nextVal = m_Builder.CreateAdd(itVal, one, "nextval");
                store = m_Builder.CreateStore(nextVal, it);
                auto cond = m_Builder.CreateICmpSGT(nextVal, stopVal);
                brLoopCond = m_Builder.CreateCondBr(cond, afterLoopBB, loopBB);
                break;
            }
            case RangeOperator::ftl: {
                auto one = llvm::ConstantInt::get(
                        m_LLVMContext,
                        llvm::APInt(llvm::Type::getInt32Ty(m_LLVMContext)->getScalarSizeInBits(), 1)
                        );
                auto itVal = m_Builder.CreateLoad(it);
                auto nextVal = m_Builder.CreateAdd(itVal, one, "nextval");
                store = m_Builder.CreateStore(nextVal, it);
                auto cond = m_Builder.CreateICmpSGE(startVal, stopVal);
                brLoopCond = m_Builder.CreateCondBr(cond, afterLoopBB, loopBB);
                break;
            }
            case RangeOperator::ftm: {
                auto one = llvm::ConstantInt::get(
                        m_LLVMContext,
                        llvm::APInt(llvm::Type::getInt32Ty(m_LLVMContext)->getScalarSizeInBits(), 1)
                        );
                auto itVal = m_Builder.CreateLoad(it);
                auto nextVal = m_Builder.CreateSub(itVal, one, "nextval");
                store = m_Builder.CreateStore(nextVal, it);
                stopVal = m_Builder.CreateNeg(stopVal, "neg_stop");
                auto cond = m_Builder.CreateICmpSGE(nextVal, stopVal);
                brLoopCond = m_Builder.CreateCondBr(cond, afterLoopBB, loopBB);
                break;
            }
        }

        func->getBasicBlockList().push_back(afterLoopBB);
        m_Builder.SetInsertPoint(afterLoopBB);

        m_YAPLContext->popScope();

        return brLoopCond;
    }

    m_Logger.printError("For condition is expected to be a range");
    return nullptr;
}
