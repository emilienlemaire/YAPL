#include "YASA/YasaVisitor.hpp"
#include "AST/ASTExprNode.hpp"
#include "AST/ASTNode.hpp"
#include "Symbol/PrimitiveType.hpp"
#include "Symbol/Type.hpp"
#include "Symbol/ArrayType.hpp"
#include "Symbol/FunctionType.hpp"
#include "Symbol/StructType.hpp"
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// TODO: Make an error handling/diagnostic library
// TODO: Change the way we handle methods (maybe from the parser to the IR Code)
namespace yapl {

    std::vector<ASTExprNode*> YasaVisitor::s_ReturnExprs = std::vector<ASTExprNode*>();

    YasaVisitor::YasaVisitor(std::unique_ptr<ASTProgramNode> program)
        : m_Program(std::move(program)), m_Logger(CppLogger::Level::Trace, "YASA")
    {
        CppLogger::Format yasaFormat({
                CppLogger::FormatAttribute::Name,
                CppLogger::FormatAttribute::Message
                });

        m_Logger.setFormat(yasaFormat);

        if (!m_Program) {
            m_Logger.printFatalError("The program is a nullptr!!!");
            exit(EXIT_FAILURE);
        }

        m_SymbolTable = m_Program->getScope();
    }

    std::map<ASTExprNode*, Type*> YasaVisitor::getExprTypeMap() const {
        return m_ExprTypeMap;
    }

    std::map<ASTExprNode*, Type*> YasaVisitor::releaseExprTypeMap() {
        return std::move(m_ExprTypeMap);
    }

    std::unique_ptr<ASTProgramNode> YasaVisitor::releaseProgram() {
        return std::move(m_Program);
    }

    void YasaVisitor::analyze() {
        if (m_Program.get()) {
            dispatchProgram(m_Program.get());
        } else {
            m_Logger.printError("The program pointer cannot be found");
            exit(EXIT_FAILURE);
        }
        m_Logger.printInfo("YASA Done");
    }

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
            return getExprType(expr);
        }

        if (auto argList = dynamic_cast<ASTArgumentList*>(expr)) {
            std::vector<Type *> argTypes;
            std::vector<std::string> argNames;

            std::string argListName = "ArgList";
            uint64_t argNum = 0;
            for (auto &arg : *argList)
            {
                auto argType = getExprType(arg.get());
                argTypes.push_back(argType);
                argNames.push_back(std::to_string(argNum));
                argListName += argNames.back();
                argNum++;
            }

            auto type = Type::CreateStructType(argListName, argNames, argTypes);
            auto inserted = Type::GetOrInsertType(type);
            auto typeValue = Value::CreateTypeValue(argListName, inserted.get());

            argList->getScope()->insert(typeValue);

            return inserted.get();
        }

        if (auto arrLit = dynamic_cast<ASTArrayLiteralExpr*>(expr)) {
            auto elemType = getExprType(arrLit->getValues()[0].get());

            auto size = arrLit->getValues().size();

            auto arrType = Type::CreateArrayType(elemType, size);
            auto inserted = Type::GetOrInsertType(arrType);

            return inserted.get();
        }

        if (auto boolLit = dynamic_cast<ASTBoolLiteralExpr*>(expr)) {
            auto type = boolLit->getScope()->lookup("bool")->getType();

            return type;
        }

        // A binary expr type is defined by it's left component. In the visitor dispatch, we check
        // those types are compatible.
        if (auto binExpr = dynamic_cast<ASTBinaryExpr*>(expr)) {
            auto lhsType = getExprType(binExpr->getLHS());

            if (binExpr->getOperator() > Operator::MINUS) {
                return binExpr->getScope()->lookup("bool")->getType();
            }

            return lhsType;
        }

        if (auto rangeExpr = dynamic_cast<ASTRangeExpr*>(expr)) {
            auto startExpr = rangeExpr->getStart();
            auto startType = getExprType(startExpr);

            if (auto arrType = dynamic_cast<ArrayType*>(startType)) {
                return arrType->getElementsType();
            }

            return getExprType(startExpr);
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

            if (identifierExpr->getIdentifier() == "this") {
                if (p_CurrentYaplStruct) {
                    return p_CurrentYaplStruct;
                }

                m_Logger.printError("Trying to acces 'this' outside a strcut scope");
                return m_SymbolTable->lookup("void")->getType();
            }

            if (!identVal) {
                m_Logger.printError("Cannot find symbol: {}", identifierExpr->getIdentifier());
                return m_SymbolTable->lookup("void")->getType();
            }

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
            auto arrType = getExprType(array);

            if (auto arrayType = dynamic_cast<ArrayType*>(arrType)) {
                return arrayType->getElementsType();
            }

            return arrType;
        }

        if (auto functionCall = dynamic_cast<ASTFunctionCallExpr*>(expr)) {
            if (auto funcType = dynamic_cast<FunctionType*>(getExprType(functionCall->getFunction()))) {
                return funcType->getReturnType();
            }
        }

        return nullptr;
    }

    bool YasaVisitor::isLiteralExpr(ASTExprNode* expr) {
        if (auto boolLit = dynamic_cast<ASTBoolLiteralExpr*>(expr)) {
            return true;
        }

        if (auto numberLit = dynamic_cast<ASTNumberExpr*>(expr)) {
            return true;
        }

        if (auto argList = dynamic_cast<ASTArgumentList*>(expr)) {
            bool isLit = true;
            for (auto &val : argList->getArguments()) {
                isLit &= isLiteralExpr(val.get());
            }
            return isLit;
        }

        if (auto arrLit = dynamic_cast<ASTArrayLiteralExpr*>(expr)) {
            bool isLit = true;
            for (auto &val : arrLit->getValues()) {
                isLit &= isLiteralExpr(val.get());
            }
            return isLit;
        }

        return false;
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
                m_ExprTypeMap[negExpr] = type;
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
            m_Logger.printError(
                    "This expression cannot be negated, it has a wrong type: {}",
                    type->dump()
                );
            m_ASTPrinter.dispatchNotExpr(notExpr);
            return;
        }

        m_ExprTypeMap[notExpr] = type;
    }

    void YasaVisitor::dispatchParExpr(ASTParExpr* parExpr) {
        parExpr->getExpr()->accept(*this);

        auto type = getExprType(parExpr->getExpr());

        /* int f1(int a, int b) {
         *      return a + b;
         * }
         *
         * float f2(float a, float b) {
         *      return a + b;
         * }
         *
         * */
        m_ExprTypeMap[parExpr] = type;
    }

    void YasaVisitor::dispatchArgumentList(ASTArgumentList* argumentList) {
        for (auto &arg: *argumentList) {
            arg->accept(*this);
        }

        auto type = getExprType(argumentList);
        m_ExprTypeMap[argumentList] = type;
    }

    void YasaVisitor::dispatchArrayLiteralExpr(ASTArrayLiteralExpr* arrayLiteral) {
        for (auto &value: *arrayLiteral) {
            value->accept(*this);
        }

        if (arrayLiteral->getScope()->isTopLevel()) {
            if (!isLiteralExpr(arrayLiteral)) {
                m_Logger.printError("A top level array must be fully literal");
                return;
            }
        }

        auto firstElementType = getExprType(arrayLiteral->getValues()[0].get());

        for (auto &value: *arrayLiteral) {
            auto elemType = getExprType(value.get());

            if (*elemType != *firstElementType) {
                m_Logger.printError("All elements of an array literal must have the same type");
                m_ASTPrinter.dispatchArrayLiteralExpr(arrayLiteral);
                return;
            }
        }

        auto type = Type::CreateArrayType(firstElementType, arrayLiteral->getValues().size());
        auto inserted = Type::GetOrInsertType(type);
        m_ExprTypeMap[arrayLiteral] = inserted.get();
    }

    void YasaVisitor::dispatchBoolLiteralExpr(ASTBoolLiteralExpr* boolLiteralExpr) {
        auto type = SymbolTable::GetBoolType();
        m_ExprTypeMap[boolLiteralExpr] = type.get();
    }

    void YasaVisitor::dispatchBinaryExpr(ASTBinaryExpr* binaryExpr) {
        // We make all necessary checks and castings to the lhs and rhs expressions.
        binaryExpr->getLHS()->accept(*this);
        binaryExpr->getRHS()->accept(*this);

        auto lhsType = getExprType(binaryExpr->getLHS());
        auto rhsType = getExprType(binaryExpr->getRHS());

        // FIXME: The modulo operator should be available only for int to int.
        if (*lhsType != *rhsType) {

            if (auto [lhsPrimitiveType, rhsPrimitiveType] =
                    std::make_pair(
                        dynamic_cast<PrimitiveType*>(lhsType),
                        dynamic_cast<PrimitiveType*>(rhsType)
                        );
                    lhsPrimitiveType && rhsPrimitiveType) {

                if (rhsPrimitiveType->isNumeric() && lhsPrimitiveType->isNumeric()) {
                    auto castExpr = std::make_unique<ASTCastExpr>(binaryExpr->getScope());
                    castExpr->setTargetType(rhsPrimitiveType->getTypeID());
                    castExpr->setExpr(std::move(binaryExpr->getLHSPtr()));

                    binaryExpr->setLHS(std::move(castExpr));
                } else {
                    m_Logger.printError("Binary expression between two incompatible types:");
                    m_ASTPrinter.dispatchBinaryExpr(binaryExpr);
                    return;
                }
            } else {
                m_Logger.printError("Binary exprssion between none primitive types:");
                m_ASTPrinter.dispatchBinaryExpr(binaryExpr);
                return;
            }
        }

        m_ExprTypeMap[binaryExpr] = lhsType;
    }

    void YasaVisitor::dispatchRangeExpr(ASTRangeExpr* rangeExpr) {
        rangeExpr->getStart()->accept(*this);

        auto startExpr = rangeExpr->getStart();
        auto startType = getExprType(startExpr);

        if (auto arrType = dynamic_cast<ArrayType*>(startType)) {
            return;
        }

        if (auto endExpr = rangeExpr->getEnd()) {
            auto endType = getExprType(endExpr);

            auto [startPrimitiveType, endPrimitiveType] =
                std::make_pair(
                        dynamic_cast<PrimitiveType*>(startType),
                        dynamic_cast<PrimitiveType*>(endType)
                    );
            if (startPrimitiveType && endPrimitiveType) {
                if(startPrimitiveType != endPrimitiveType) {
                    if (startPrimitiveType->isNumeric() && endPrimitiveType->isNumeric()) {
                        auto castExpr = std::make_unique<ASTCastExpr>(endExpr->getScope());
                        castExpr->setTargetType(startPrimitiveType->getTypeID());
                        castExpr->setExpr(std::move(rangeExpr->getEndPtr()));

                        rangeExpr->setEnd(std::move(castExpr));

                        m_ExprTypeMap[rangeExpr] = startType;

                        return;
                    }

                    m_Logger.printError("Incompatible types in range expression:");
                    m_ASTPrinter.dispatchRangeExpr(rangeExpr);

                    return;
                }
            }

            if (startPrimitiveType->isNumeric()) {
                m_ExprTypeMap[rangeExpr] = startType;
                return;
            }
        }

        m_Logger.printError("Invalid type in range expression:");
        m_ASTPrinter.dispatchRangeExpr(rangeExpr);
    }

    void YasaVisitor::dispatchFloatNumberExpr(ASTFloatNumberExpr* floatNumberExpr) {
        auto type = SymbolTable::GetFloatType().get();
        m_ExprTypeMap[floatNumberExpr] = type;
    }

    void YasaVisitor::dispatchDoubleNumberExpr(ASTDoubleNumberExpr* doubleNumberExpr) {
        auto type = SymbolTable::GetDoubleType().get();
        m_ExprTypeMap[doubleNumberExpr] = type;
    }

    void YasaVisitor::dispatchIntegerNumberExpr(ASTIntegerNumberExpr* integerNumberExpr) {
        auto type = SymbolTable::GetIntType().get();
        m_ExprTypeMap[integerNumberExpr] = type;
    }

    void YasaVisitor::dispatchIdentifierExpr(ASTIdentifierExpr* identifierExpr) {
        auto type = getExprType(identifierExpr);
        m_ExprTypeMap[identifierExpr] = type;
    }

    void YasaVisitor::dispatchAttributeAccessExpr(ASTAttributeAccessExpr* attributeAccessExpr) {
        attributeAccessExpr->getStruct()->accept(*this);
        // attributeAccessExpr->getAttribute()->accept(*this);
        auto structExpr = attributeAccessExpr->getStruct();
        auto structT = getExprType(structExpr);

        if (auto structType = dynamic_cast<StructType*>(structT)) {
            auto attrIdentifier = attributeAccessExpr->getAttribute()->getIdentifier();
            if (!structType->isField(attrIdentifier)) {
                if(auto methValue = attributeAccessExpr->getScope()->lookup(attrIdentifier)) {
                    m_Logger.printInfo("We have a method {}", methValue->getName());
                    if (auto methType = dynamic_cast<FunctionType*>(methValue->getType())) {
                        m_Logger.printInfo("We have a type {}", methValue->getName());
                        return;
                    }
                }
                m_Logger.printError(
                        "Trying to access an inexistant field from struct: {}",
                        attributeAccessExpr->getAttribute()->getIdentifier()
                    );
                m_Logger.printError("Available fields:");
                for (auto [field, type] : *structType)
                {
                    std::cout << field << ", ";
                }
                std::cout << std::endl;
                m_ASTPrinter.dispatchAttributeAccessExpr(attributeAccessExpr);
                return;
            }

            if (!structType->getFieldType(attrIdentifier)) {
                m_Logger.printError("The type of the field {} is undefined", attrIdentifier);
                m_ASTPrinter.dispatchAttributeAccessExpr(attributeAccessExpr);
            }

            auto type = structType->getFieldType(attrIdentifier);
            m_ExprTypeMap[attributeAccessExpr] = type;
        }
    }

    void YasaVisitor::dispatchArrayAccessExpr(ASTArrayAccessExpr* arrayAccessExpr) {
        auto indexExpr = arrayAccessExpr->getIndex();
        auto indexType = getExprType(indexExpr);

        if (auto indexPrimType = dynamic_cast<PrimitiveType*>(indexType)) {
            if (indexPrimType->getTypeID() == SymbolTable::GetIntID()) {
                auto type = getExprType(arrayAccessExpr);
                m_ExprTypeMap[arrayAccessExpr] = type;
                return;
            }
        }

        m_Logger.printError(
                "The type of the index expression of an array must be 'int', got '{}'",
                indexType->dump()
            );
        m_ASTPrinter.dispatchArrayAccessExpr(arrayAccessExpr);
    }

    void YasaVisitor::dispatchFunctionCallExpr(ASTFunctionCallExpr* functionCallExpr) {
        functionCallExpr->getFunction()->accept(*this);
        functionCallExpr->getArguments()->accept(*this);

        auto calleeType = getExprType(functionCallExpr->getFunction());

        if (auto funcType = dynamic_cast<FunctionType*>(calleeType)) {
            if (funcType->getNumParams() == functionCallExpr->getArguments()->getArguments().size()) {
                for (int i = 0; i < funcType->getNumParams(); i++) {
                    auto paramType = funcType->getParamType(i);
                    auto argType = getExprType(
                            functionCallExpr->getArguments()->getArguments()[i].get()
                        );

                    if (paramType != argType) {
                        m_Logger.printError("Invalid Argument type on argument number: {}", i);
                        m_ASTPrinter.dispatchFunctionCallExpr(functionCallExpr);
                        return;
                    }
                }

                m_ExprTypeMap[functionCallExpr] = funcType->getReturnType();
                return;
            }
            m_Logger.printError(
                    "Invalid argument number, expecting {} insted of {}",
                    funcType->getNumParams(),
                    functionCallExpr->getArguments()->getArguments().size()
                );
            m_ASTPrinter.dispatchFunctionCallExpr(functionCallExpr);
            return;
        }
        m_Logger.printError("The type of the callee is not a FunctionType.");
        m_ASTPrinter.dispatchFunctionCallExpr(functionCallExpr);
    }

    void YasaVisitor::dispatchCastExpr(ASTCastExpr* castExpr) {
        // Should do some type compatibility, maybe. But since only yasa can cast as of now this is
        // not yet necessary.
        castExpr->getExpr()->accept(*this);

        auto type = SymbolTable::GetTypeByID(castExpr->getTargetType());

        m_ExprTypeMap[castExpr] = type.get();
    }

    void YasaVisitor::dispatchBlock(ASTBlockNode* blockNode) {
        for (auto &statement : *blockNode) {
            statement->accept(*this);
        }
    }

    void YasaVisitor::dispatchExprStatement(ASTExprStatementNode* exprStatementNode) {
        exprStatementNode->getExpr()->accept(*this);
    }

    void YasaVisitor::dispatchDeclaration(ASTDeclarationNode* declarationNode) {}

    void YasaVisitor::dispatchArrayDeclaration(ASTArrayDeclarationNode* arrayDeclarationNode) {}

    void YasaVisitor::dispatchInitialization(ASTInitializationNode* initializationNode) {
        initializationNode->getValue()->accept(*this);

        auto varType = initializationNode->getScope()->lookup(initializationNode->getType())->getType();
        auto exprType = getExprType(initializationNode->getValue());

        if (varType != exprType) {
            if (auto [varStructType, argListType] = std::make_pair<StructType*, StructType*>(
                    dynamic_cast<StructType*>(varType),
                    dynamic_cast<StructType*>(exprType)
                ); varStructType && argListType) {

                for(int i = 0; i < argListType->getElementsType().size(); i++) {
                    if (varStructType->getElementType(i) != argListType->getElementType(i)) {
                        m_Logger.printError(
                                "Field {} of the assigned structure must be of type {}, got type {}",
                                i,
                                varStructType->getElementType(i)->dump(),
                                argListType->getElementType(i)->dump()
                            );
                        m_ASTPrinter.dispatchInitialization(initializationNode);
                    }
                }

                return;
            }
            m_Logger.printError(
                    "The type of the expression ({}) doesn't match the type of the variable ({})",
                    exprType->dump(),
                    varType->dump()
                );
            m_ASTPrinter.dispatchInitialization(initializationNode);
        }
    }

    void YasaVisitor::dispatchArrayInitialization(ASTArrayInitializationNode* arrayInitializationNode) {
        arrayInitializationNode->getValues()->accept(*this);

        auto arrMembersTypeName = arrayInitializationNode->getType();
        auto arrMembersType = arrayInitializationNode->getScope()->lookup(arrMembersTypeName)->getType();
        auto arrSize = arrayInitializationNode->getSize();
        auto rvalueType = getExprType(arrayInitializationNode->getValues());

        // FIXME: Multidimension arrays won't work this way, maybe a helper function to recurse on
        // the nested arrays.
        if (auto structType = dynamic_cast<StructType*>(rvalueType)) {
            if (auto structToArr = structType->toArrayType()) {
                rvalueType = structToArr;
            } else {
                m_Logger.printError("Cannot transform this initialization list to an array:");
                m_ASTPrinter.dispatchArrayInitialization(arrayInitializationNode);
                return;
            }
        }

        auto arrType = arrayInitializationNode->getScope()->lookup(
                arrMembersTypeName + "[" + std::to_string(arrSize) +"]"
            )->getType();

        if (*arrType != *rvalueType) {
            m_Logger.printError(
                    "rvalue of type {} does not match the array of type {}",
                    rvalueType->dump(),
                    arrType->dump()
                );
            m_Logger.printError("ArrType name: {}", arrMembersTypeName);
            m_ASTPrinter.dispatchArrayInitialization(arrayInitializationNode);
        }
    }

    void YasaVisitor::dispatchStructInitialization(
            ASTStructInitializationNode* structInitializationNode
        ) {
        auto structT = structInitializationNode->getScope()
            ->lookup(structInitializationNode->getType())
            ->getType();

        if (auto structType = dynamic_cast<StructType*>(structT)){
            if (auto attrList =
                    dynamic_cast<ASTArgumentList*>(structInitializationNode->getAttributeValues())
               ) {
                uint64_t i = 0;
                for (auto &attr : *attrList) {
                    attr->accept(*this);
                    auto attrType = getExprType(attr.get());
                    if (attrType != structType->getElementType(i)) {
                        m_Logger.printError("Attribute {} does not have the good type");
                        m_ASTPrinter.dispatchStructInitialization(structInitializationNode);
                    }
                }
                m_ExprTypeMap[attrList] = structT;
                return;
            }
            m_Logger.printError("Expecting an argument list.");
            m_ASTPrinter.dispatchStructInitialization(structInitializationNode);

            return;
        }
        m_Logger.printError("Expecting a struct type.");
        m_ASTPrinter.dispatchStructInitialization(structInitializationNode);
    }

    void YasaVisitor::dispatchFunctionDefinition(ASTFunctionDefinitionNode* functionDefinitionNode) {

        s_ReturnExprs = std::vector<ASTExprNode*>();

        functionDefinitionNode->getBody()->accept(*this);

        auto returnType = functionDefinitionNode
            ->getScope()
            ->lookup(functionDefinitionNode->getReturnType())
            ->getType();

        bool hasReturn = false;
        for (auto retExpr : s_ReturnExprs) {
            auto returnStatementType = getExprType(retExpr);

            // TODO: Check for casting possibility.
            if (returnStatementType != returnType) {
                m_Logger.printError("The returned expression does not correspond to the return "
                        "type of the function");
                m_ASTPrinter.dispatchFunctionDefinition(functionDefinitionNode);
                return;
            }
            hasReturn = true;
        }

        // TODO: Check for "return;" statement.
        if (!hasReturn && (returnType != SymbolTable::GetVoidType().get())) {
            m_Logger.printError("Expecting a return statement in non-void function");
            m_ASTPrinter.dispatchFunctionDefinition(functionDefinitionNode);
        }
    }

    void YasaVisitor::dispatchStructDefinition(ASTStructDefinitionNode* structDefinitionNode) {
        p_CurrentYaplStruct = dynamic_cast<StructType*>(
                structDefinitionNode
                ->getScope()
                ->lookup(structDefinitionNode->getStructName())
                ->getType()
            );
        for (auto &attr : structDefinitionNode->getAttributes()) {
            attr->accept(*this);
        }

        for (auto &method : structDefinitionNode->getMethods()) {
            method->accept(*this);
        }
         p_CurrentYaplStruct = nullptr;
    }

    void YasaVisitor::dispatchImport(ASTImportNode* importNode) {}

    void YasaVisitor::dispatchExport(ASTExportNode* exportNode) {}

    void YasaVisitor::dispatchReturn(ASTReturnNode* returnNode) {
        returnNode->getExpr()->accept(*this);

        s_ReturnExprs.push_back(returnNode->getExpr());
    }

    // TODO: Check if there is a return in then and else;
    void YasaVisitor::dispatchIf(ASTIfNode* ifNode) {
        ifNode->getCondition()->accept(*this);
        ifNode->getThenBlock()->accept(*this);
        if (ifNode->getElseBlock()) {
            ifNode->getCondition()->accept(*this);
        }

        auto condType = getExprType(ifNode->getCondition());

        if (auto primType = dynamic_cast<PrimitiveType*>(condType)) {
            return;
        }

        m_Logger.printError("The type of the condition expression must be a primitive type");
        m_ASTPrinter.dispatchIf(ifNode);
    }

    void YasaVisitor::dispatchFor(ASTForNode* forNode) {
        forNode->getRangeExpr()->accept(*this);
        forNode->getBlock()->accept(*this);
    }

    void YasaVisitor::dispatchAssignment(ASTAssignmentNode* assignmentNode) {
        // TODO: Work on casting
        assignmentNode->getVariable()->accept(*this);
        assignmentNode->getValue()->accept(*this);
        auto assigneeType = getExprType(assignmentNode->getVariable());
        auto valueType = getExprType(assignmentNode->getVariable());

        if (assigneeType != valueType) {
            if (auto [structTypeVariable, structTypeValue] =
                    std::make_pair(
                        dynamic_cast<StructType*>(assigneeType),
                        dynamic_cast<StructType*>(valueType)
                    );
                structTypeVariable && structTypeValue
            ) {
                uint64_t i = 0;
                for (auto type : structTypeValue->getElementsType()) {
                    if (structTypeVariable->getElementType(i) != type) {
                        m_Logger.printError(
                                "The assigned value type does not correspond to the assignee type"
                            );
                        m_ASTPrinter.dispatchAssignment(assignmentNode);
                        return;
                    }
                }
            }
            m_Logger.printError("The assigned value type does not correspond to the assignee type");
            m_ASTPrinter.dispatchAssignment(assignmentNode);
        }
    }

} // namespace yapl
