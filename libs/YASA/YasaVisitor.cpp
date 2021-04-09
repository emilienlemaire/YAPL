#include "YASA/YasaVisitor.hpp"
#include "AST/ASTExprNode.hpp"
#include <memory>
#include <type_traits>


namespace yapl {

    std::shared_ptr<Type> YasaVisitor::getExprType(ASTExprNode *expr) {
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
            std::vector<std::shared_ptr<Type>> argTypes;

            for (auto &arg : *argList)
            {
                auto argType = getExprType(arg.get());
                argTypes.push_back(argType);
            }

            auto mangledTypeName = Type::MangleArgumentListType(argTypes);

            if (auto typeValue = argList->getScope()->lookup(mangledTypeName)) {
                auto type = typeValue->getType();
                return type;
            }

            auto type = Type::CreateArgumentListType(argTypes);
            auto typeValue = Value::CreateTypeValue(mangledTypeName, type);

            argList->getScope()->insert(typeValue);

            return type;
        }

        if (auto boolLit = dynamic_cast<ASTBoolLiteralExpr*>(expr)) {
            auto type = boolLit->getScope()->lookup("bool")->getType();

            return type;
        }

        if (auto binExpr = dynamic_cast<ASTBinaryExpr*>(expr)) {
            auto lhsType = getExprType(binExpr->getLHS());
            auto rhsType = getExprType(binExpr->getRHS());

            if (lhsType != rhsType) {
                if (rhsType->isNumeric() && lhsType->isNumeric()) {
                    auto castExpr = std::make_unique<ASTCastExpr>(binExpr->getScope());
                    castExpr->setTargetType(rhsType->getIdentifier());
                    castExpr->setExpr(std::move(binExpr->getLHSPtr()));

                    binExpr->setLHS(std::move(castExpr));
                } else {
                    m_Logger.printError("Binary expression between two incompatible types:");
                    m_ASTPrinter.dispatchBinaryExpr(binExpr);
                    return binExpr->getScope()->lookup("void")->getType();
                }
            }

            if (binExpr->getOperator() > Operator::MINUS) {
                return binExpr->getScope()->lookup("bool")->getType();
            }

            return rhsType;
        }

        if (auto rangeExpr = dynamic_cast<ASTRangeExpr*>(expr)) {
            auto startExpr = rangeExpr->getStart();
            auto startType = getExprType(startExpr);

            if (startType->isArray()) {
                return startType->getElementsType();
            }

            if (auto endExpr = rangeExpr->getEnd()) {
                auto endType = getExprType(endExpr);

                if(startType != endType) {
                    if (startType->isNumeric() && endType->isNumeric()) {
                        auto castExpr = std::make_unique<ASTCastExpr>(endExpr->getScope());
                        castExpr->setTargetType(startType->getIdentifier());
                        castExpr->setExpr(std::move(rangeExpr->getEndPtr()));

                        rangeExpr->setEnd(std::move(castExpr));

                        return startType;
                    }

                    m_Logger.printError("Incompatible types in range expression:");
                    m_ASTPrinter.dispatchRangeExpr(rangeExpr);

                    return rangeExpr->getScope()->lookup("void")->getType();
                }

                if (startType->isNumeric()) {
                    return startType;
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

        // FIXME: Modify SymbolTable / Value / Whatever so we can have easy access to
        //  struct field types.
        if (auto attributeAcces = dynamic_cast<ASTAttributeAccessExpr*>(expr)) {
            m_Logger.printError("Not Yet Implemented; {}", __PRETTY_FUNCTION__);
            return attributeAcces->getScope()->lookup("void")->getType();
        }

        if (auto arrayAccessExpr = dynamic_cast<ASTArrayAccessExpr*>(expr)) {
            auto array = arrayAccessExpr->getArray();

            auto arrayType = getExprType(array);

            return arrayType->getElementsType();
        }

        if (auto functionCall = dynamic_cast<ASTFunctionCallExpr*>(expr)) {
            auto funcType = getExprType(functionCall->getFunction());

            return funcType->getReturnType();
        }

        return nullptr;
    }

    void YasaVisitor::dispatchProgram(ASTProgramNode* programNode) {
        for (auto &node : *programNode) {
            node->accept(*this);
        }
    }

    void YasaVisitor::dispatchNegExpr(ASTNegExpr* negExpr) {

    }

    void YasaVisitor::dispatchNotExpr(ASTNotExpr* notExpr) {}

    void YasaVisitor::dispatchParExpr(ASTParExpr* parExpr) {}

    void YasaVisitor::dispatchArgumentList(ASTArgumentList* argumentList) {}

    void YasaVisitor::dispatchBoolLiteralExpr(ASTBoolLiteralExpr* boolLiteralExpr) {}

    void YasaVisitor::dispatchBinaryExpr(ASTBinaryExpr* binaryExpr) {}

    void YasaVisitor::dispatchRangeExpr(ASTRangeExpr* rangeExpr) {}

    void YasaVisitor::dispatchFloatNumberExpr(ASTFloatNumberExpr* floatNumberExpr) {}

    void YasaVisitor::dispatchDoubleNumberExpr(ASTDoubleNumberExpr* doubleNumberExpr) {}

    void YasaVisitor::dispatchIntegerNumberExpr(ASTIntegerNumberExpr* integerNumberExpr) {}

    void YasaVisitor::dispatchIdentifierExpr(ASTIdentifierExpr* identifierExpr) {}

    void YasaVisitor::dispatchAttributeAccessExpr(ASTAttributeAccessExpr* attributeAccessExpr) {}

    void YasaVisitor::dispatchArrayAccessExpr(ASTArrayAccessExpr* arrayAccessExpr) {}

    void YasaVisitor::dispatchFunctionCallExpr(ASTFunctionCallExpr* functionCallExpr) {}

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
