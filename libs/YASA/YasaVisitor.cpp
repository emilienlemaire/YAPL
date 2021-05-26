#include "YASA/YasaVisitor.hpp"
#include "AST/ASTExprNode.hpp"
#include "AST/ASTNode.hpp"
#include "Symbol/PrimitiveType.hpp"
#include "Symbol/Type.hpp"
#include "Symbol/ArrayType.hpp"
#include "Symbol/FunctionType.hpp"
#include "Symbol/StructType.hpp"
#include <memory>
#include <string>
#include <type_traits>
#include <utility>


namespace yapl {

    YasaVisitor::YasaVisitor(std::unique_ptr<ASTProgramNode> program)
        : m_Program(std::move(program)), m_Logger(CppLogger::Level::Trace, "YASA")
    {
        m_SymbolTable = m_Program->getScope();
    }
    // TODO: Rewrite this so it doesn't do nay check, we want checks to be done in the dispose functions
    Type* YasaVisitor::getExprType(ASTExprNode *expr) {
        if (auto negExpr = dynamic_cast<ASTNegExpr*>(expr)) {
            auto value = negExpr->getValue();
            return getExprType(value);
        }

        if (auto notExpr = dynamic_cast<ASTNotExpr*>(expr)) {
            auto value = notExpr->getValue();
            return getExprType(value);
        }

        if (auto parExpr = dynamic_cast<ASTParExpr*>(expr)) {
            auto expr = parExpr->getExpr();
            return getExprType(parExpr);
        }

        if (auto argList = dynamic_cast<ASTArgumentList*>(expr)) {
            std::vector<Type *> argTypes;
            std::vector<std::string> argName;

            std::string argListName = "ArgList";
            uint64_t argNum = 0;
            for (auto &arg : *argList)
            {
                auto argType = getExprType(arg.get());
                argTypes.push_back(argType);
                argName.push_back(std::to_string(argNum));
                argListName += argName.back();
                argNum++;
            }

            auto type = Type::CreateStructType(argListName, argName, argTypes);
            auto inserted = Type::GetOrInsertType(type);
            auto typeValue = Value::CreateTypeValue(argListName, inserted.get());

            argList->getScope()->insert(typeValue);

            return type.get();
        }

        if (auto boolLit = dynamic_cast<ASTBoolLiteralExpr*>(expr)) {
            auto type = boolLit->getScope()->lookup("bool")->getType();

            return type;
        }

        // TODO: Think if I want it to send the lhs only and do the check in the dispacth part
        if (auto binExpr = dynamic_cast<ASTBinaryExpr*>(expr)) {
            auto lhsType = getExprType(binExpr->getLHS());
            auto rhsType = getExprType(binExpr->getRHS());

            // TODO: Check if we want to do the cast before, I forgot about this line...
            //          This should be ok, but need testing ot be sure.
            if (*lhsType != *rhsType) {

                if (auto [lhsPrimitiveType, rhsPrimitiveType] =
                        std::make_pair(dynamic_cast<PrimitiveType*>(lhsType), dynamic_cast<PrimitiveType*>(rhsType));
                        lhsPrimitiveType && rhsPrimitiveType) {
                    if (rhsPrimitiveType->isNumeric() && lhsPrimitiveType->isNumeric()) {
                        auto castExpr = std::make_unique<ASTCastExpr>(binExpr->getScope());
                        castExpr->setTargetType(rhsPrimitiveType->getTypeID());
                        castExpr->setExpr(std::move(binExpr->getLHSPtr()));

                        binExpr->setLHS(std::move(castExpr));
                    } else {
                        m_Logger.printError("Binary expression between two incompatible types:");
                        m_ASTPrinter.dispatchBinaryExpr(binExpr);
                        return binExpr->getScope()->lookup("void")->getType();
                    }
                } else {
                    m_Logger.printError("Binary exprssion between none primitive types:");
                    m_ASTPrinter.dispatchBinaryExpr(binExpr);
                    return binExpr->getScope()->lookup("void")->getType();
                }
            }

            if (binExpr->getOperator() > Operator::MINUS) {
                return binExpr->getScope()->lookup("bool")->getType();
            }

            return rhsType;
        }

        // TODO: Think if I want it to send the start only and do the check in the dispacth part
        if (auto rangeExpr = dynamic_cast<ASTRangeExpr*>(expr)) {
            auto startExpr = rangeExpr->getStart();
            auto startType = getExprType(startExpr);

            if (auto arrType = dynamic_cast<ArrayType*>(startType)) {
                return arrType->getElementsType();
            }

            if (auto endExpr = rangeExpr->getEnd()) {
                auto endType = getExprType(endExpr);

                auto [startPrimitiveType, endPrimitiveType] =
                        std::make_pair(dynamic_cast<PrimitiveType*>(startType), dynamic_cast<PrimitiveType*>(endType));
                if (startPrimitiveType && endPrimitiveType) {
                    if(startPrimitiveType != endPrimitiveType) {
                        if (startPrimitiveType->isNumeric() && endPrimitiveType->isNumeric()) {
                            auto castExpr = std::make_unique<ASTCastExpr>(endExpr->getScope());
                            castExpr->setTargetType(startPrimitiveType->getTypeID());
                            castExpr->setExpr(std::move(rangeExpr->getEndPtr()));

                            rangeExpr->setEnd(std::move(castExpr));

                            return startPrimitiveType;
                        }

                        m_Logger.printError("Incompatible types in range expression:");
                        m_ASTPrinter.dispatchRangeExpr(rangeExpr);

                        return rangeExpr->getScope()->lookup("void")->getType();
                    }

                }

                if (startPrimitiveType->isNumeric()) {
                    return startPrimitiveType;
                }
            }

            m_Logger.printError("Invalid type in range expression:");
            return rangeExpr->getScope()->lookup("void")->getType();
        }

        if (auto floatLit = dynamic_cast<ASTFloatNumberExpr*>(expr)) {
            return floatLit->getScope()->lookup("float")->getType();
        }

        if (auto doubleLit = dynamic_cast<ASTDoubleNumberExpr*>(expr)) {
            return doubleLit->getScope()->lookup("double")->getType();
        }

        if (auto intLit = dynamic_cast<ASTIntegerNumberExpr*>(expr)) {
            return intLit->getScope()->lookup("int")->getType();
        }

        if (auto identifierExpr = dynamic_cast<ASTIdentifierExpr*>(expr)) {
            auto identVal = identifierExpr->getScope()->lookup(identifierExpr->getIdentifier());

            return identVal->getType();
        }

        if (auto attributeAcces = dynamic_cast<ASTAttributeAccessExpr*>(expr)) {
            auto structExpr = attributeAcces->getStruct();
            auto type = getExprType(structExpr);
            if (auto structType = dynamic_cast<StructType*>(type)) {
                auto attribute = attributeAcces->getAttribute();
                auto attributeName = attribute->getIdentifier();

                return structType->getFieldType(attributeName);
            }
            return attributeAcces->getScope()->lookup("void")->getType();
        }

        if (auto arrayAccessExpr = dynamic_cast<ASTArrayAccessExpr*>(expr)) {
            auto array = arrayAccessExpr->getArray();

            if (auto arrayType = dynamic_cast<ArrayType*>(getExprType(array))) {
                return arrayType->getElementsType();
            }

        }

        if (auto functionCall = dynamic_cast<ASTFunctionCallExpr*>(expr)) {
            if (auto funcType = dynamic_cast<FunctionType*>(getExprType(functionCall->getFunction()))) {
                return funcType->getReturnType();
            }
        }

        return nullptr;
    }

    void YasaVisitor::dispatchProgram(ASTProgramNode* programNode) {
        for (auto &node : *programNode) {
            node->accept(*this);
        }
    }

    void YasaVisitor::dispatchNegExpr(ASTNegExpr* negExpr) {
        negExpr->getValue()->accept(*this);
        auto type = getExprType(negExpr->getValue());
        if (auto primitiveType = dynamic_cast<PrimitiveType*>(type)) {
            if (primitiveType->isNumeric()) {
                return;
            }
        }
        m_Logger.printError("This expression cannot be negated, it has a wrong type:");
        m_ASTPrinter.dispatchNegExpr(negExpr);
    }

    void YasaVisitor::dispatchNotExpr(ASTNotExpr* notExpr) {
        notExpr->getValue()->accept(*this);

        auto type = getExprType(notExpr->getValue());
        if (*type != *SymbolTable::GetBoolType()) {
            m_Logger.printError("This expression cannot be negated, it has a wrong type:");
            m_ASTPrinter.dispatchNotExpr(notExpr);
        }
    }

    void YasaVisitor::dispatchParExpr(ASTParExpr* parExpr) {
        parExpr->getExpr()->accept(*this);
    }

    void YasaVisitor::dispatchArgumentList(ASTArgumentList* argumentList) {
        for (auto &arg: *argumentList) {
            arg->accept(*this);
        }
    }

    void YasaVisitor::dispatchBoolLiteralExpr(ASTBoolLiteralExpr* boolLiteralExpr) {}

    void YasaVisitor::dispatchBinaryExpr(ASTBinaryExpr* binaryExpr) {
        binaryExpr->getLHS()->accept(*this);
        binaryExpr->getRHS()->accept(*this);
    }

    void YasaVisitor::dispatchRangeExpr(ASTRangeExpr* rangeExpr) {}

    void YasaVisitor::dispatchFloatNumberExpr(ASTFloatNumberExpr* floatNumberExpr) {}

    void YasaVisitor::dispatchDoubleNumberExpr(ASTDoubleNumberExpr* doubleNumberExpr) {}

    void YasaVisitor::dispatchIntegerNumberExpr(ASTIntegerNumberExpr* integerNumberExpr) {}

    void YasaVisitor::dispatchIdentifierExpr(ASTIdentifierExpr* identifierExpr) {}

    void YasaVisitor::dispatchAttributeAccessExpr(ASTAttributeAccessExpr* attributeAccessExpr) {}

    void YasaVisitor::dispatchArrayAccessExpr(ASTArrayAccessExpr* arrayAccessExpr) {}

    void YasaVisitor::dispatchFunctionCallExpr(ASTFunctionCallExpr* functionCallExpr) {}

    void YasaVisitor::dispatchCastExpr(ASTCastExpr* functionCallExpr) {}

    void YasaVisitor::dispatchBlock(ASTBlockNode* blockNode) {}

    void YasaVisitor::dispatchExprStatement(ASTExprStatementNode* exprStatementNode) {}

    void YasaVisitor::dispatchDeclaration(ASTDeclarationNode* declarationNode) {}

    void YasaVisitor::dispatchArrayDeclaration(ASTArrayDeclarationNode* arrayDeclarationNode) {}

    void YasaVisitor::dispatchInitialization(ASTInitializationNode* initializationNode) {}

    void YasaVisitor::dispatchArrayInitialization(ASTArrayInitializationNode* arrayInitializationNode) {}

    void YasaVisitor::dispatchStructInitialization(ASTStructInitializationNode* structInitializationNode) {}

    void YasaVisitor::dispatchFunctionDefinition(ASTFunctionDefinitionNode* functionDefinitionNode) {}

    void YasaVisitor::dispatchStructDefinition(ASTStructDefinitionNode* structDefinitionNode) {}

    void YasaVisitor::dispatchImport(ASTImportNode* importNode) {}

    void YasaVisitor::dispatchExport(ASTExportNode* exportNode) {}

    void YasaVisitor::dispatchReturn(ASTReturnNode* returnNode) {}

    void YasaVisitor::dispatchIf(ASTIfNode* ifNode) {}

    void YasaVisitor::dispatchFor(ASTForNode* forNode) {}

    void YasaVisitor::dispatchAssignment(ASTAssignmentNode* assignmentNode) {}

} // namespace yapl
