/**
 * libs/IRGenerator/IRGenerator.cpp
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
#include "IRGenerator/IRGenerator.hpp"
#include "AST/ASTExprNode.hpp"
#include "AST/ASTNode.hpp"
#include "AST/ASTStatementNode.hpp"
#include "Symbol/PrimitiveType.hpp"
#include "Symbol/ArrayType.hpp"
#include "Symbol/FunctionType.hpp"
#include "Symbol/StructType.hpp"
#include <array>
#include <filesystem>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "llvm/IR/Constants.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/Casting.h"

namespace yapl {
    IRGenerator::IRGenerator(
            std::map<ASTExprNode*, Type*> exprTypeMap,
            std::unique_ptr<ASTProgramNode> program,
            llvm::StringRef filepath)
    :
        m_ExprTypeMap(std::move(exprTypeMap)),
        m_Program(std::move(program)),
        m_Logger(CppLogger::Level::Trace, "IR Generator"),
        m_Builder(m_LLVMContext)
    {
        CppLogger::Format format({
                CppLogger::FormatAttribute::Name,
                CppLogger::FormatAttribute::Level,
                CppLogger::FormatAttribute::Message
            });
        m_Logger.setFormat(format);

        std::filesystem::path path(filepath.str());

        std::string name = path.filename();

        m_Module = std::make_unique<llvm::Module>(name, m_LLVMContext);
    }

    llvm::Type *IRGenerator::getOrCreateLLVMType(Type *YAPLType) {
        auto LLVMTypeOrEnd = m_YAPLLLVMTypeMap.find(YAPLType);

        if (LLVMTypeOrEnd != m_YAPLLLVMTypeMap.end()) {
            return LLVMTypeOrEnd->second;
        }

        if (auto primType = dynamic_cast<PrimitiveType*>(YAPLType)) {
            if (primType->getTypeID() == SymbolTable::GetIntID()) {
                auto LLVMType = llvm::Type::getInt32Ty(m_LLVMContext);
                m_YAPLLLVMTypeMap[YAPLType] = LLVMType;

                return LLVMType;
            }

            if (primType->getTypeID() == SymbolTable::GetFloatID()) {
                auto LLVMType = llvm::Type::getFloatTy(m_LLVMContext);
                m_YAPLLLVMTypeMap[YAPLType] = LLVMType;

                return LLVMType;
            }
            if (primType->getTypeID() == SymbolTable::GetDoubleID()) {
                auto LLVMType = llvm::Type::getDoubleTy(m_LLVMContext);
                m_YAPLLLVMTypeMap[YAPLType] = LLVMType;

                return LLVMType;
            }
            if (primType->getTypeID() == SymbolTable::GetBoolID()) {
                auto LLVMType = llvm::Type::getInt1Ty(m_LLVMContext);
                m_YAPLLLVMTypeMap[YAPLType] = LLVMType;

                return LLVMType;
            }
            if (primType->getTypeID() == SymbolTable::GetCharID()) {
                auto LLVMType = llvm::Type::getInt8Ty(m_LLVMContext);
                m_YAPLLLVMTypeMap[YAPLType] = LLVMType;

                return LLVMType;
            }
            if (primType->getTypeID() == SymbolTable::GetVoidID()) {
                auto LLVMType = llvm::Type::getVoidTy(m_LLVMContext);
                m_YAPLLLVMTypeMap[YAPLType] = LLVMType;

                return LLVMType;
            }
        }

        if (auto arrType = dynamic_cast<ArrayType*>(YAPLType)) {
            auto LLVMMemeberType = getOrCreateLLVMType(arrType->getElementsType());
            auto LLVMType = llvm::ArrayType::get(LLVMMemeberType, arrType->getNumElements());

            m_YAPLLLVMTypeMap[YAPLType] = LLVMType;

            return LLVMType;
        }

        if (auto funcType = dynamic_cast<FunctionType*>(YAPLType)) {
            auto returnType = getOrCreateLLVMType(funcType->getReturnType());

            std::vector<llvm::Type *> LLVMParamsType;

            for (auto YAPLParamType : funcType->getParamsType()) {
                LLVMParamsType.push_back(getOrCreateLLVMType(YAPLParamType));
            }

            auto LLVMType = llvm::FunctionType::get(returnType, LLVMParamsType, false);

            m_YAPLLLVMTypeMap[YAPLType] = LLVMType;

            return LLVMType;
        }

        if (auto structType = dynamic_cast<StructType*>(YAPLType)) {

            std::vector<llvm::Type*> LLVMFieldsType;

            for (auto [name, idx] : *structType) {
                auto LLVMFieldType = getOrCreateLLVMType(structType->getFieldType(name));
                if (!LLVMFieldType->isFunctionTy()) {
                    LLVMFieldsType.push_back(LLVMFieldType);
                }
            }

            auto LLVMType = llvm::StructType::create(
                    m_LLVMContext,
                    LLVMFieldsType,
                    structType->getIdentifier()
                );

            m_YAPLLLVMTypeMap[YAPLType] = LLVMType;

            return LLVMType;
        }

        return nullptr;
    }

    // TODO: Check for p_LastValue before use
    void IRGenerator::generate() {
        m_Program->accept(*this);
        // llvm::verifyModule(*m_Module, &llvm::outs());
    }

    void IRGenerator::dispatchProgram(ASTProgramNode *programNode) {
        for (auto &node : *programNode) {
            node->accept(*this);
        }
        m_Module->print(llvm::errs(), nullptr);
        std::cout << std::endl;
    }

    void IRGenerator::dispatchNegExpr(ASTNegExpr* negExpr) {
        negExpr->getValue()->accept(*this);

        p_LastValue = p_LastValue->getType()->isIntegerTy() ?
                m_Builder.CreateNeg(p_LastValue) :
                m_Builder.CreateFNeg(p_LastValue);
    }

    void IRGenerator::dispatchNotExpr(ASTNotExpr* notExpr) {
        notExpr->getValue()->accept(*this);

        p_LastValue = m_Builder.CreateNot(p_LastValue);
    }

    void IRGenerator::dispatchParExpr(ASTParExpr* parExpr) {
        parExpr->getExpr()->accept(*this);
    }

    // FIXME: Find a way to pass the type of the arglist from YASA to here.
    void IRGenerator::dispatchArgumentList(ASTArgumentList* argumentList) {
        std::vector<llvm::Value*> argVals;
        std::vector<llvm::Type*> argTypes;

        auto type = m_ExprTypeMap[argumentList];

        if (auto structType = dynamic_cast<StructType*>(m_ExprTypeMap[argumentList])) {
            for(auto &arg : *argumentList) {
                arg->accept(*this);
                argVals.push_back(p_LastValue);
                argTypes.push_back(p_LastValue->getType());
            }

            auto llvmStructType = llvm::StructType::create(argTypes);

            std::vector<llvm::Constant*> constVals;

            for (auto arg : argVals) {
                if (auto constVal = llvm::dyn_cast<llvm::Constant>(arg)) {
                    constVals.push_back(constVal);
                } else {
                    m_Logger.printError("The value in an argument list must be constant");
                    p_LastValue = nullptr;
                    return;
                }
            }

            p_LastValue = llvm::ConstantStruct::get(llvmStructType, constVals);
        } else if (auto arrayType = dynamic_cast<ArrayType*>(m_ExprTypeMap[argumentList])) {
            for(auto &arg : *argumentList) {
                arg->accept(*this);
                argVals.push_back(p_LastValue);
            }

            auto llvmArrayType = llvm::ArrayType::get(argVals[0]->getType(), argVals.size());

            std::vector<llvm::Constant*> constVals;

            for (auto arg : argVals) {
                if (auto constVal = llvm::dyn_cast<llvm::Constant>(arg)) {
                    constVals.push_back(constVal);
                } else {
                    m_Logger.printError("The value in an argument list must be constant");
                    p_LastValue = nullptr;
                    return;
                }
            }

            p_LastValue = llvm::ConstantArray::get(llvmArrayType, constVals);
        } else {
            m_Logger.printError(
                    "The type of the argList is erronous: {}",
                    m_ExprTypeMap[argumentList]->dump()
                );
        }
    }

    void IRGenerator::dispatchArrayLiteralExpr(ASTArrayLiteralExpr* arrayLiteral) {
        std::vector<llvm::Value *> arrVals;
        arrVals.reserve(arrayLiteral->getValues().size());

        for (auto &val : *arrayLiteral) {
            val->accept(*this);
        }

        bool isTopLevel = arrayLiteral->getScope()->isTopLevel();

        auto type = getOrCreateLLVMType(m_ExprTypeMap[arrayLiteral]);

        if (auto *arrType = llvm::dyn_cast<llvm::ArrayType>(type)) {
            std::vector<llvm::Constant*> arrConsts;

            for (auto val : arrVals) {
                if (auto constant = llvm::dyn_cast<llvm::Constant>(val)) {
                    arrConsts.push_back(constant);
                } else {
                    m_Logger.printError("An array literal must be filled with constant values");
                    p_LastValue = nullptr;
                    return;
                }
            }
            p_LastValue = llvm::ConstantArray::get(arrType, arrConsts);
        } else {
            p_LastValue = nullptr;
        }
    }

    void IRGenerator::dispatchBoolLiteralExpr(ASTBoolLiteralExpr* boolLiteralExpr) {
        p_LastValue = llvm::ConstantInt::getBool(m_LLVMContext, boolLiteralExpr->getValue());
    }

    void IRGenerator::dispatchBinaryExpr(ASTBinaryExpr* binaryExpr) {
        binaryExpr->getLHS()->accept(*this);
        auto lhsVal = p_LastValue;
        binaryExpr->getRHS()->accept(*this);
        auto rhsVal = p_LastValue;

        bool isFloatingPoint = lhsVal->getType()->isFloatingPointTy();

        switch (binaryExpr->getOperator()) {
            case Operator::TIMES:
                p_LastValue = isFloatingPoint ?
                    m_Builder.CreateFMul(lhsVal, rhsVal) :
                    m_Builder.CreateMul(lhsVal, rhsVal);
                break;
            case Operator::BY:
                p_LastValue = isFloatingPoint ?
                    m_Builder.CreateFDiv(lhsVal, rhsVal) :
                    m_Builder.CreateSDiv(lhsVal, rhsVal);
                break;
            case Operator::MOD:
                p_LastValue = m_Builder.CreateSRem(lhsVal, rhsVal);
                break;
            case Operator::PLUS:
                p_LastValue = isFloatingPoint ?
                    m_Builder.CreateFAdd(lhsVal, rhsVal) :
                    m_Builder.CreateAdd(lhsVal, rhsVal);
                break;
            case Operator::MINUS:
                p_LastValue = isFloatingPoint ?
                    m_Builder.CreateFSub(lhsVal, rhsVal) :
                    m_Builder.CreateSub(lhsVal, rhsVal);
                break;
            case Operator::LTH:
                p_LastValue = isFloatingPoint ?
                    m_Builder.CreateFCmpOLT(lhsVal, rhsVal) :
                    m_Builder.CreateICmpSLT(lhsVal, rhsVal);
                break;
            case Operator::MTH:
                p_LastValue = isFloatingPoint ?
                    m_Builder.CreateFCmpOGT(lhsVal, rhsVal) :
                    m_Builder.CreateICmpSGT(lhsVal, rhsVal);
                break;
            case Operator::LEQ:
                p_LastValue = isFloatingPoint ?
                    m_Builder.CreateFCmpOLE(lhsVal, rhsVal) :
                    m_Builder.CreateICmpSLE(lhsVal, rhsVal);
                break;
            case Operator::MEQ:
                p_LastValue = isFloatingPoint ?
                    m_Builder.CreateFCmpOGE(lhsVal, rhsVal) :
                    m_Builder.CreateICmpSGE(lhsVal, rhsVal);
                break;
            case Operator::EQ:
                p_LastValue = isFloatingPoint ?
                    m_Builder.CreateFCmpOEQ(lhsVal, rhsVal) :
                    m_Builder.CreateICmpEQ(lhsVal, rhsVal);
                break;
            case Operator::NEQ:
                p_LastValue = isFloatingPoint ?
                    m_Builder.CreateFCmpONE(lhsVal, rhsVal) :
                    m_Builder.CreateICmpNE(lhsVal, rhsVal);
                break;
            case Operator::AND:
                p_LastValue = m_Builder.CreateAnd(lhsVal, rhsVal);
                break;
            case Operator::OR:
                p_LastValue = m_Builder.CreateOr(lhsVal, rhsVal);
                break;
            default:
                p_LastValue = nullptr;
                llvm_unreachable("There shouldn't be any 'NONE' operator");
        }
    }

    // TODO: Figure it out in the for statement handler
    void IRGenerator::dispatchRangeExpr(ASTRangeExpr* rangeExpr) {}

    void IRGenerator::dispatchFloatNumberExpr(ASTFloatNumberExpr* floatNumberExpr) {
        p_LastValue =
            llvm::ConstantFP::get(llvm::Type::getFloatTy(m_LLVMContext), floatNumberExpr->getValue());
    }

    void IRGenerator::dispatchDoubleNumberExpr(ASTDoubleNumberExpr* doubleNumberExpr) {
        p_LastValue =
            llvm::ConstantFP::get(llvm::Type::getDoubleTy(m_LLVMContext), doubleNumberExpr->getValue());
    }

    void IRGenerator::dispatchIntegerNumberExpr(ASTIntegerNumberExpr* integerNumberExpr) {
        p_LastValue = llvm::ConstantInt::getSigned(
                llvm::Type::getInt32Ty(m_LLVMContext),
                integerNumberExpr->getValue()
            );
    }

    void IRGenerator::dispatchIdentifierExpr(ASTIdentifierExpr* identifierExpr) {
        auto value = m_NameValueMap[identifierExpr->getIdentifier()];

        if (!value) {
            p_LastValue = m_Module->getFunction(identifierExpr->getIdentifier());
            return;
        }

        auto llvmType = value->getType();

        if (isa<llvm::GlobalValue>(value)) {
            p_LastValue = value;
            return;
        }

        if (llvmType->isPointerTy()) {
            if (llvmType->getPointerElementType()->isArrayTy()) {
                p_LastValue = m_Builder.CreateLoad(llvmType, value);
            } else {
                p_LastValue = m_Builder.CreateLoad(llvmType->getPointerElementType(), value);
            }

            return;
        }

        p_LastValue = m_Builder.CreateLoad(llvmType, value);
    }

    void IRGenerator::dispatchAttributeAccessExpr(ASTAttributeAccessExpr* attributeAccessExpr) {
        attributeAccessExpr->getStruct()->accept(*this);

        auto yaplType = m_ExprTypeMap[attributeAccessExpr->getStruct()];

        // TODO: Add info about the struct type because we need to use "this"

        if (auto identExpr = dynamic_cast<ASTIdentifierExpr*>(attributeAccessExpr->getStruct())) {
            if (identExpr->getIdentifier() == "this") {
                yaplType = p_CurrentYaplStruct;
            }
        }

        if (auto yaplStructType = dynamic_cast<StructType*>(yaplType)) {
            auto attrExpr = attributeAccessExpr->getAttribute();
            auto attrName = attrExpr->getIdentifier();

            uint64_t idx = yaplStructType->getFieldIndex(attrName);

            auto llvmType = getOrCreateLLVMType(yaplStructType);

            m_Logger.printInfo("Trying to GEP index {}", idx);
            if (auto llvmStructType = llvm::dyn_cast<llvm::StructType>(llvmType)) {
                auto structValue = p_LastValue;
                auto attributeType = llvmStructType->getElementType(idx);
                p_LastValue = m_Builder.CreateAlloca(llvmStructType);
                m_Builder.CreateStore(structValue, p_LastValue);
                llvm::Value* attrGEP = m_Builder.CreateStructGEP(llvmType, p_LastValue, idx, "gepAttr" + std::to_string(idx));
                p_LastValue = m_Builder.CreateLoad(attributeType, attrGEP);
                return;
            }
        }
        p_LastValue = nullptr;
    }

    void IRGenerator::dispatchArrayAccessExpr(ASTArrayAccessExpr* arrayAccessExpr) {
        arrayAccessExpr->getArray()->accept(*this);
        auto llvmType = p_LastValue->getType();

        if (llvmType->isPointerTy()) {
            llvmType = llvmType->getPointerElementType();
        }

        if (auto llvmArrType = llvm::dyn_cast<llvm::ArrayType>(llvmType)) {
            llvm::Value *arrValue = p_LastValue;
            arrayAccessExpr->getIndex()->accept(*this);
            std::vector<llvm::Value*> idxArr;
            idxArr = {
                llvm::ConstantInt::get(llvm::Type::getInt32Ty(m_LLVMContext), 0),
                p_LastValue
            };

            p_LastValue = m_Builder.CreateInBoundsGEP(llvmArrType, arrValue, idxArr);
            p_LastValue = m_Builder.CreateLoad(llvmArrType->getArrayElementType(), p_LastValue);

            return;
        }

        m_Logger.printError("Trying to access a non array type");
        p_LastValue = nullptr;
    }

    void IRGenerator::dispatchFunctionCallExpr(ASTFunctionCallExpr* functionCallExpr) {
        functionCallExpr->getFunction()->accept(*this);
        auto funcValue = p_LastValue;
        auto llvmType = funcValue->getType();

        if (llvmType->isPointerTy()) {
            llvmType = llvmType->getPointerElementType();
        }

        std::vector<llvm::Value*> argVals;

        for (auto &arg : *functionCallExpr->getArguments()) {
            arg->accept(*this);
            argVals.push_back(p_LastValue);
        }

        if (auto llvmFuncType = llvm::dyn_cast<llvm::FunctionType>(llvmType)) {
            p_LastValue = m_Builder.CreateCall(
                    llvmFuncType,
                    funcValue,
                    argVals
                );
            return;
        }

        m_Logger.printError("Trying to call a non function type:");
        funcValue->getType()->print(llvm::errs(), true);
        p_LastValue = nullptr;
    }

    void IRGenerator::dispatchCastExpr(ASTCastExpr* castExpr) {
        castExpr->getExpr()->accept(*this);
        auto yaplTargetType = SymbolTable::GetTypeByID(castExpr->getTargetType());
        auto llvmTargetType = getOrCreateLLVMType(yaplTargetType.get());
        auto castOp = llvm::CastInst::getCastOpcode(p_LastValue, true, llvmTargetType, true);
        p_LastValue = m_Builder.CreateCast(castOp, p_LastValue, llvmTargetType);
    }

    void IRGenerator::dispatchBlock(ASTBlockNode* blockNode) {
        for (auto &statement : *blockNode) {
            statement->accept(*this);
        }
    }

    void IRGenerator::dispatchExprStatement(ASTExprStatementNode* exprStatementNode) {
        if (auto functionCallExpr =
                dynamic_cast<ASTFunctionCallExpr*>(exprStatementNode->getExpr())) {
            functionCallExpr->accept(*this);
        }
    }
    void IRGenerator::dispatchDeclaration(ASTDeclarationNode* declarationNode) {
        bool isAtTopLevel = declarationNode->getScope()->isTopLevel();
        auto typeName = declarationNode->getType();
        auto yaplType = declarationNode->getScope()->lookup(typeName)->getType();
        auto llvmType = getOrCreateLLVMType(yaplType);

        if (isAtTopLevel) {
            m_Module->getOrInsertGlobal(declarationNode->getIdentifier(), llvmType);
            auto globVar = m_Module->getGlobalVariable(declarationNode->getIdentifier());
            globVar->setDSOLocal(true);
            m_NameValueMap[declarationNode->getIdentifier()] = globVar;
        } else {
            // FIXME: Method definition seems to break this
            p_LastValue = m_Builder.CreateAlloca(
                    llvmType,
                    nullptr,
                    declarationNode->getIdentifier()
                );
            m_NameValueMap[declarationNode->getIdentifier()] = p_LastValue;
        }
    }

    void IRGenerator::dispatchArrayDeclaration(ASTArrayDeclarationNode* arrayDeclarationNode) {
        bool isAtTopLevel = arrayDeclarationNode->getScope()->isTopLevel();
        auto typeName = arrayDeclarationNode->getType();
        auto yaplType = arrayDeclarationNode->getScope()->lookup(typeName)->getType();
        auto llvmType = getOrCreateLLVMType(yaplType);

        if (isAtTopLevel) {
            m_Module->getOrInsertGlobal(arrayDeclarationNode->getIdentifier(), llvmType);
            auto globVar = m_Module->getGlobalVariable(arrayDeclarationNode->getIdentifier());
            globVar->setDSOLocal(true);
            m_NameValueMap[arrayDeclarationNode->getIdentifier()] = globVar;
        } else {
            p_LastValue = m_Builder.CreateAlloca(llvmType, nullptr, arrayDeclarationNode->getIdentifier());
            m_NameValueMap[arrayDeclarationNode->getIdentifier()] = p_LastValue;
        }
    }

    void IRGenerator::dispatchInitialization(ASTInitializationNode* initializationNode) {
        bool isAtTopLevel = initializationNode->getScope()->isTopLevel();
        auto typeName = initializationNode->getType();
        auto yaplType = initializationNode->getScope()->lookup(typeName)->getType();
        auto llvmType = getOrCreateLLVMType(yaplType);

        initializationNode->getValue()->accept(*this);

        if (isAtTopLevel) {
            m_Module->getOrInsertGlobal(initializationNode->getIdentifier(), llvmType);
            auto globVar = m_Module->getGlobalVariable(initializationNode->getIdentifier());
            globVar->setDSOLocal(true);
            globVar->setInitializer(llvm::dyn_cast<llvm::Constant>(p_LastValue));
            m_NameValueMap[initializationNode->getIdentifier()] = globVar;
        } else {
            auto initValue = p_LastValue;
            p_LastValue = m_Builder.CreateAlloca(
                    llvmType,
                    nullptr,
                    initializationNode->getIdentifier()
                );
            m_Builder.CreateStore(initValue, p_LastValue);
            m_NameValueMap[initializationNode->getIdentifier()] = p_LastValue;
        }
    }

    void IRGenerator::dispatchArrayInitialization(
            ASTArrayInitializationNode* arrayInitializationNode
    ) {
        bool isAtTopLevel = arrayInitializationNode->getScope()->isTopLevel();
        auto typeName = arrayInitializationNode->getType() +
            "[" + std::to_string(arrayInitializationNode->getSize()) + "]";
        auto yaplType = arrayInitializationNode->getScope()->lookup(typeName)->getType();
        auto llvmType = getOrCreateLLVMType(yaplType);

        if (isAtTopLevel) {
            if (auto argList =
                    dynamic_cast<ASTArgumentList*>(arrayInitializationNode->getValues())) {
                auto structType = dynamic_cast<StructType*>(m_ExprTypeMap[argList]);
                m_ExprTypeMap[argList] = structType->toArrayType();
            }
            arrayInitializationNode->getValues()->accept(*this);
            m_Module->getOrInsertGlobal(arrayInitializationNode->getIdentifier(), llvmType);
            auto globVar = m_Module->getGlobalVariable(arrayInitializationNode->getIdentifier());
            globVar->setDSOLocal(true);
            globVar->setInitializer(llvm::dyn_cast<llvm::Constant>(p_LastValue));
            m_NameValueMap[arrayInitializationNode->getIdentifier()] = globVar;
        } else {
            llvm::Value* arrAlloc = m_Builder.CreateAlloca(
                    llvmType, nullptr, arrayInitializationNode->getIdentifier()
                );
            if (auto argList =
                    dynamic_cast<ASTArgumentList*>(arrayInitializationNode->getValues())) {
                unsigned idx = 0;
                for (auto &valExpr : *argList) {
                    valExpr->accept(*this);
                    llvm::Value *gep = m_Builder.CreateConstGEP2_32(llvmType, arrAlloc, 0, idx);
                    idx++;
                    m_Builder.CreateStore(p_LastValue, gep);
                }
            } else if (auto arrLit =
                    dynamic_cast<ASTArrayLiteralExpr*>(arrayInitializationNode->getValues())) {
                unsigned idx = 0;
                for (auto &valExpr : *arrLit) {
                    valExpr->accept(*this);
                    llvm::Value *gep = m_Builder.CreateConstGEP2_32(llvmType, arrAlloc, 0, idx);
                    idx++;
                    m_Builder.CreateStore(p_LastValue, gep);
                }
            } else {
                arrayInitializationNode->getValues()->accept(*this);
                m_Builder.CreateStore(p_LastValue, arrAlloc);
            }
            m_NameValueMap[arrayInitializationNode->getIdentifier()] = arrAlloc;
        }
    }

    void IRGenerator::dispatchStructInitialization(
        ASTStructInitializationNode* structInitializationNode
    ) {
        auto structName = structInitializationNode->getType();
        auto yaplType = structInitializationNode->getScope()->lookup(structName)->getType();
        auto llvmType = getOrCreateLLVMType(yaplType);
        auto llvmStructType = llvm::dyn_cast<llvm::StructType>(llvmType);

        if (structInitializationNode->getScope()->isTopLevel()) {
            structInitializationNode->getAttributeValues()->accept(*this);
            m_Module->getOrInsertGlobal(structInitializationNode->getIdentifier(), llvmType);
            auto globVar = m_Module->getGlobalVariable(structInitializationNode->getIdentifier());
            // TODO: Add a check over p_LastValue
            auto size = llvmType->getStructNumElements();
            std::vector<llvm::Constant*> elements;
            elements.reserve(size);
            for (int i = 0; i < size; i++) {
                elements.push_back(
                        llvm::dyn_cast<llvm::ConstantStruct>(p_LastValue)->getAggregateElement(i)
                    );
            }
            auto constant = llvm::ConstantStruct::get(llvmStructType, elements);
            globVar->setDSOLocal(true);
            globVar->setInitializer(constant);
            m_NameValueMap[structInitializationNode->getIdentifier()] = globVar;
        } else {
            llvm::Value* structAlloc =
                m_Builder.CreateAlloca(llvmType, nullptr, structInitializationNode->getIdentifier());
            if (auto argList =
                    dynamic_cast<ASTArgumentList*>(structInitializationNode->getAttributeValues())) {
                unsigned idx = 0;
                for (auto &valExpr : *argList) {
                    valExpr->accept(*this);
                    llvm::Value *gep = m_Builder.CreateStructGEP(llvmType, structAlloc, idx);
                    idx++;
                    m_Builder.CreateStore(p_LastValue, gep);
                }
            } else {
                structInitializationNode->getAttributeValues()->accept(*this);
                m_Builder.CreateStore(p_LastValue, structAlloc);
            }
            m_NameValueMap[structInitializationNode->getIdentifier()] = structAlloc;
        }
    }

    void IRGenerator::dispatchFunctionDefinition(
        ASTFunctionDefinitionNode* functionDefinitionNode
    ) {
        auto functionName = functionDefinitionNode->getFunctionName();
        auto yaplType = functionDefinitionNode->getScope()->lookup(functionName)->getType();
        auto llvmType = getOrCreateLLVMType(yaplType);

        if (auto llvmFuncType = llvm::dyn_cast<llvm::FunctionType>(llvmType)) {
            // llvmFuncType->dump();
            if (p_CurrentStruct) {
                auto params = std::vector<llvm::Type*>();
                params.push_back(p_CurrentStruct);
                for (unsigned i = 0; i < llvmFuncType->getNumParams(); i++) {
                    params.push_back(llvmFuncType->getParamType(i));
                }
                llvmFuncType = llvm::FunctionType::get(
                        llvmFuncType->getReturnType(),
                        params,
                        false
                    );
            }
            // llvmFuncType->dump();
            auto func = llvm::Function::Create(
                    llvmFuncType,
                    llvm::Function::ExternalLinkage,
                    functionName,
                    m_Module.get()
                );

            auto entryBlock = llvm::BasicBlock::Create(m_LLVMContext, "entry", func);
            m_Builder.SetInsertPoint(entryBlock);

            unsigned idx = 0;
            for (auto &param : functionDefinitionNode->getParameters()) {
                // llvmFuncType->dump();
                // param->accept(*this);
                m_Logger.printInfo("Param {} {} {}", param->getType(), param->getIdentifier(), idx);
                func->getArg(idx)->getType()->dump();
                p_LastValue = m_Builder.CreateAlloca(func->getArg(idx)->getType(), nullptr, param->getIdentifier());
                m_NameValueMap[param->getIdentifier()] = p_LastValue;
                m_Builder.CreateStore(func->getArg(idx), p_LastValue);
                idx++;
            }

            functionDefinitionNode->getBody()->accept(*this);

            if (llvmFuncType->getReturnType()->isVoidTy()) {
                m_Builder.CreateRetVoid();
            }


            if (llvm::verifyFunction(*func, &llvm::errs())) {
                m_Logger.printError("There is a probleme with the function {}", functionName);
            }

            func->dump();

            return;
        }

        m_Logger.printError(
            "Trying to define a function that does not have a function type {}",
            functionName
        );
    }

    void IRGenerator::dispatchStructDefinition(ASTStructDefinitionNode* structDefinitionNode) {
        std::string structName = structDefinitionNode->getStructName();
        auto yaplType = structDefinitionNode->getScope()->lookup(structName)->getType();

        if (auto yaplStructType = dynamic_cast<StructType*>(yaplType)) {
            auto llvmStructType = getOrCreateLLVMType(yaplStructType);
            p_CurrentStruct = llvmStructType;
            p_CurrentYaplStruct = yaplStructType;
            p_CurrentStruct->dump();
            for (auto &method : structDefinitionNode->getMethods()) {
                /* auto param = std::make_unique<ASTDeclarationNode>(method->getScope());
§u                param->setIdentifier("this");
                param->setType(structDefinitionNode->getStructName());
                method->addParameter(std::move(param)); */
                m_Logger.printInfo("Making method {}", method->getFunctionName());
                method->accept(*this);
            }
            p_CurrentStruct = nullptr;
            p_CurrentYaplStruct = nullptr;
        }

    }

    void IRGenerator::dispatchImport(ASTImportNode* importNode) {}
    void IRGenerator::dispatchExport(ASTExportNode* exportNode) {}
    void IRGenerator::dispatchReturn(ASTReturnNode* returnNode) {
        returnNode->getExpr()->accept(*this);
        m_Builder.CreateRet(p_LastValue);
    }
    void IRGenerator::dispatchIf(ASTIfNode* ifNode) {}
    void IRGenerator::dispatchFor(ASTForNode* forNode) {}
    void IRGenerator::dispatchAssignment(ASTAssignmentNode* assignmentNode) {}
} // namespace yapl
