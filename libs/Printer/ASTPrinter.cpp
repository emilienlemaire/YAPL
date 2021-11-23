/**
 * libs/Printer/ASTPrinter.cpp
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
#include "AST/ASTNode.hpp"
#include "Printer/ASTPrinter.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>

namespace yapl {
    ASTPrinter::ASTPrinter(std::unique_ptr<ASTProgramNode> program)
        : m_Program(std::move(program))
    {}

    void ASTPrinter::printTabs() {
        for (int i = 0; i < m_Tabs; i++) {
            std::cout << "  ";
        }
    }

    void ASTPrinter::dump() {
        if (m_Program.get()) {
            dispatchProgram(m_Program.get());
        } else {
            std::cerr << "The program node has been released, cannot iterate over AST." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    std::unique_ptr<ASTProgramNode> ASTPrinter::releaseProgram() {
        return std::move(m_Program);
    }

    void ASTPrinter::dispatchProgram(ASTProgramNode *program) {
        for (auto &node : *program) {
            node->accept(*this);
        }
    }

    void ASTPrinter::dispatchCastExpr(ASTCastExpr *castExpr) {
        printTabs();
        std::cout << "ASTCastExpr:" << std::endl;
        m_Tabs++;
        auto expr = castExpr->getExpr();
        expr->accept(*this);
        m_Tabs--;
    }

    void ASTPrinter::dispatchNegExpr(ASTNegExpr* negExpr) {
        printTabs();
        std::cout << "ASTNegExpr:" << std::endl;
        auto expr = negExpr->getValue();
        m_Tabs++;
        expr->accept(*this);
        m_Tabs--;
    }

    void ASTPrinter::dispatchNotExpr(ASTNotExpr* notExpr) {
        printTabs();
        std::cout << "ASTNotExpr:" << std::endl;
        auto expr = notExpr->getValue();
        m_Tabs++;
        expr->accept(*this);
        m_Tabs--;
    }

    void ASTPrinter::dispatchParExpr(ASTParExpr* parExpr) {
        printTabs();
        std::cout << "ASTParExpr:" << std::endl;
        auto expr = parExpr->getExpr();
        m_Tabs++;
        expr->accept(*this);
        m_Tabs--;
    }

    void ASTPrinter::dispatchArgumentList(ASTArgumentList* argumentList) {
        printTabs();
        std::cout << "ASTArgumentList:" << std::endl;
        m_Tabs++;
        for(auto &arg : argumentList->getArguments()) {
            arg->accept(*this);
        }
        m_Tabs--;
    }

    void ASTPrinter::dispatchArrayLiteralExpr(ASTArrayLiteralExpr* arrayLiteral) {
        printTabs();
        std::cout << "ASTArrayLiteralExpr:" << std::endl;
        m_Tabs++;
        for(auto &value : arrayLiteral->getValues()) {
            value->accept(*this);
        }
        m_Tabs--;
    }

    void ASTPrinter::dispatchBoolLiteralExpr(ASTBoolLiteralExpr* boolLiteralExpr) {
        printTabs();
        std::cout << "ASTBoolLiteralExpr: value = " <<
            std::to_string(boolLiteralExpr->getValue()) << std::endl;
    }
    void ASTPrinter::dispatchBinaryExpr(ASTBinaryExpr* binaryExpr) {
        printTabs();
        std::cout << "ASTBinaryExpr:" << std::endl;
        m_Tabs++;
        m_Tabs++;
        printTabs();
        std::cout << "lhs: " << std::endl;
        m_Tabs++;
        auto lhs = binaryExpr->getLHS();
        lhs->accept(*this);
        m_Tabs--;
        printTabs();
        std::cout << "operator: " << ASTNode::operatorToString(binaryExpr->getOperator()) << std::endl;
        printTabs();
        std::cout << "rhs: " << std::endl;
        m_Tabs++;
        auto rhs = binaryExpr->getRHS();
        rhs->accept(*this);
        m_Tabs--;
        m_Tabs--;
        m_Tabs--;
    }
    void ASTPrinter::dispatchRangeExpr(ASTRangeExpr* rangeExpr) {
        printTabs();
        std::cout << "ASTRangeExpr:" << std::endl;
        m_Tabs++;
        m_Tabs++;
        printTabs();
        std::cout << "start:" << std::endl;
        auto start = rangeExpr->getStart();
        start->accept(*this);
        m_Tabs--;
        printTabs();
        std::cout << "end:" << std::endl;
        m_Tabs++;
        if (auto end = rangeExpr->getEnd()) {
            end->accept(*this);
        }
        m_Tabs--;
        m_Tabs--;
    }

    void ASTPrinter::dispatchFloatNumberExpr(ASTFloatNumberExpr* floatNumberExpr) {
        printTabs();
        std::cout << "ASTFloatNumberExpr: value: " << floatNumberExpr->getValue() << std::endl;
    }

    void ASTPrinter::dispatchDoubleNumberExpr(ASTDoubleNumberExpr* doubleNumberExpr) {
        printTabs();
        std::cout << "ASTDoubleNumberExpr: value: " << doubleNumberExpr->getValue() << std::endl;
    }

    void ASTPrinter::dispatchIntegerNumberExpr(ASTIntegerNumberExpr* integerNumberExpr) {
        printTabs();
        std::cout << "ASTIntegerNumberExpr: value: " << integerNumberExpr->getValue() << std::endl;
    }

    void ASTPrinter::dispatchIdentifierExpr(ASTIdentifierExpr* identifierExpr) {
        printTabs();
        std::cout << "ASTIdentifierExpr: value: " << identifierExpr->getIdentifier() << std::endl;
    }

    void ASTPrinter::dispatchAttributeAccessExpr(ASTAttributeAccessExpr* attributeAccessExpr) {
        printTabs();
        std::cout << "ASTAttributeAccessExpr:" << std::endl;
        m_Tabs++;
        printTabs();
        std::cout << "struct: " << std::endl;
        m_Tabs++;
        auto structExpr = attributeAccessExpr->getStruct();
        structExpr->accept(*this);
        m_Tabs--;
        printTabs();
        std::cout << "attribute:" << std::endl;
        m_Tabs++;
        auto attribute = attributeAccessExpr->getAttribute();
        attribute->accept(*this);
        m_Tabs--;
        m_Tabs--;
    }

    void ASTPrinter::dispatchArrayAccessExpr(ASTArrayAccessExpr* arrayAccessExpr) {
        printTabs();
        std::cout << "ASTArrayAccessExpr:" << std::endl;
        m_Tabs++;
        printTabs();
        std::cout << "array: " << std::endl;
        m_Tabs++;
        auto array = arrayAccessExpr->getArray();
        array->accept(*this);
        m_Tabs--;
        printTabs();
        std::cout << "index:" << std::endl;
        m_Tabs++;
        auto index = arrayAccessExpr->getIndex();
        index->accept(*this);
        m_Tabs--;
        m_Tabs--;
    }

    void ASTPrinter::dispatchFunctionCallExpr(ASTFunctionCallExpr* functionCallExpr) {
        printTabs();
        std::cout << "ASTFunctionCallExpr:" << std::endl;
        m_Tabs++;
        printTabs();
        std::cout << "function: " << std::endl;
        m_Tabs++;
        auto function = functionCallExpr->getFunction();
        function->accept(*this);
        m_Tabs--;
        printTabs();
        std::cout << "arguments:" << std::endl;
        m_Tabs++;
        auto args = functionCallExpr->getArguments();
        args->accept(*this);
        m_Tabs--;
        m_Tabs--;
    }

    void ASTPrinter::dispatchBlock(ASTBlockNode* blockNode) {
        printTabs();
        std::cout << "ASTBlockNode:" << std::endl;
        m_Tabs++;
        for(auto &stmt: *blockNode) {
            stmt->accept(*this);
        }
        m_Tabs--;
    }

    void ASTPrinter::dispatchExprStatement(ASTExprStatementNode* exprStatementNode) {
        printTabs();
        std::cout << "ASTExprStatementNode:" << std::endl;
        m_Tabs++;
        auto expr = exprStatementNode->getExpr();
        expr->accept(*this);
        m_Tabs--;
    }

    void ASTPrinter::dispatchDeclaration(ASTDeclarationNode* declarationNode) {
        printTabs();
        std::cout << "ASTDeclarationNode:" << std::endl;
        m_Tabs++;
        printTabs();
        std::cout << "type: " << declarationNode->getType() << std::endl;
        printTabs();
        std::cout<< "name: " << declarationNode->getIdentifier() << std::endl;
        m_Tabs--;
    }

    void ASTPrinter::dispatchArrayDeclaration(ASTArrayDeclarationNode* arrayDeclarationNode) {
        printTabs();
        std::cout << "ASTArrayDeclarationNode:" << std::endl;
        m_Tabs++;
        printTabs();
        std::cout << "type: " << arrayDeclarationNode->getType() << std::endl;
        printTabs();
        std::cout<< "name: " << arrayDeclarationNode->getIdentifier() << std::endl;
        printTabs();
        std::cout<< "size: " << arrayDeclarationNode->getSize() << std::endl;
        m_Tabs--;
    }

    void ASTPrinter::dispatchInitialization(ASTInitializationNode* initializationNode) {
        printTabs();
        std::cout << "ASTInitializationNode:" << std::endl;
        m_Tabs++;
        printTabs();
        std::cout << "type: " << initializationNode->getType() << std::endl;
        printTabs();
        std::cout<< "name: " << initializationNode->getIdentifier() << std::endl;
        printTabs();
        std::cout << "value: " << std::endl;
        m_Tabs++;
        auto expr = initializationNode->getValue();
        expr->accept(*this);
        m_Tabs--;
        m_Tabs--;
    }

    void ASTPrinter::dispatchArrayInitialization(ASTArrayInitializationNode* arrayInitializationNode) {
        printTabs();
        std::cout << "ASTArrayInitializationNode:" << std::endl;
        m_Tabs++;
        printTabs();
        std::cout << "type: " << arrayInitializationNode->getType() << std::endl;
        printTabs();
        std::cout<< "name: " << arrayInitializationNode->getIdentifier() << std::endl;
        printTabs();
        std::cout<< "size: " << arrayInitializationNode->getSize() << std::endl;
        printTabs();
        std::cout << "values: " << std::endl;
        m_Tabs++;
        auto expr = arrayInitializationNode->getValues();
        expr->accept(*this);
        m_Tabs--;
        m_Tabs--;
    }

    void ASTPrinter::dispatchStructInitialization(ASTStructInitializationNode* structInitializationNode) {
        printTabs();
        std::cout << "ASTStructInitializationNode" << std::endl;
        m_Tabs++;
        printTabs();
        std::cout << "type: " << structInitializationNode->getType() << std::endl;
        printTabs();
        std::cout<< "name: " << structInitializationNode->getIdentifier() << std::endl;
        printTabs();
        std::cout << "Attributes:" << std::endl;
        m_Tabs++;
        auto expr = structInitializationNode->getAttributeValues();
        expr->accept(*this);
        m_Tabs--;
        m_Tabs--;
    }

    void ASTPrinter::dispatchFunctionDefinition(ASTFunctionDefinitionNode* functionDefinitionNode) {
        printTabs();
        std::cout << "ASTFunctionDefinitionNode:" << std::endl;
        m_Tabs++;
        printTabs();
        std::cout << "name: " << functionDefinitionNode->getFunctionName() << std::endl;
        printTabs();
        std::cout << "return type: " << functionDefinitionNode->getReturnType() << std::endl;
        printTabs();
        auto &params = functionDefinitionNode->getParameters();
        std::cout << "parameters: (" << params.size() << ")" << std::endl;
        m_Tabs++;
        for (auto &param : params) {
            param->accept(*this);
        }
        m_Tabs--;
        printTabs();
        std::cout << "body: " << std::endl;
        m_Tabs++;
        auto body = functionDefinitionNode->getBody();
        body->accept(*this);
        m_Tabs--;
        m_Tabs--;
    }

    void ASTPrinter::dispatchStructDefinition(ASTStructDefinitionNode* structDefinitionNode) {
        printTabs();
        std::cout << "ASTStructDefinitionNode:" << std::endl;
        m_Tabs++;
        printTabs();
        std::cout << "name: " << structDefinitionNode->getStructName() << std::endl;
        printTabs();
        std::cout << "attributes: " << std::endl;
        m_Tabs++;
        auto &attributes = structDefinitionNode->getAttributes();
        for (auto &attribute : attributes) {
            attribute->accept(*this);
        }
        m_Tabs--;
        printTabs();
        std::cout << "methods: " << std::endl;
        m_Tabs++;
        auto &methods = structDefinitionNode->getMethods();
        for (auto &method : methods) {
            method->accept(*this);
        }
        m_Tabs--;
        m_Tabs--;
    }

    void ASTPrinter::dispatchImport(ASTImportNode* importNode) {
        printTabs();
        std::cout << "ASTImportNode:" << std::endl;
        m_Tabs++;
        printTabs();
        std::cout << "namespaces:" << std::endl;
        m_Tabs++;
        auto namespaces = importNode->getNamespaces();
        for (const auto &ns : namespaces) {
            printTabs();
            std::cout << ns << std::endl;
        }
        m_Tabs--;
        printTabs();
        std::cout << "values:" << std::endl;
        m_Tabs++;
        auto values = importNode->getImportedValues();
        for (const auto &value: values) {
            printTabs();
            std::cout << value << std::endl;
        }
        m_Tabs--;
        m_Tabs--;
    }

    void ASTPrinter::dispatchExport(ASTExportNode* exportNode) {
        printTabs();
        std::cout << "ASTExportNode:" << std::endl;
        m_Tabs++;
        printTabs();
        std::cout << "values: " << std::endl;
        m_Tabs++;
        for (const auto &value : exportNode->getExportedValues()) {
            printTabs();
            std::cout << value << std::endl;
        }
        m_Tabs--;
        m_Tabs--;
    }

    void ASTPrinter::dispatchReturn(ASTReturnNode* returnNode) {
        printTabs();
        std::cout << "ASTReturnNode:" << std::endl;
        m_Tabs++;
        printTabs();
        std::cout << "value:" << std::endl;
        m_Tabs++;
        auto expr = returnNode->getExpr();
        expr->accept(*this);
        m_Tabs--;
        m_Tabs--;
    }

    void ASTPrinter::dispatchIf(ASTIfNode* ifNode) {
        printTabs();
        std::cout << "ASTIfNode:" << std::endl;
        m_Tabs++;
        printTabs();
        std::cout << "condition:" << std::endl;
        m_Tabs++;
        auto cond = ifNode->getCondition();
        cond->accept(*this);
        m_Tabs--;
        printTabs();
        std::cout << "then:" << std::endl;
        m_Tabs++;
        auto thenB = ifNode->getThenBlock();
        thenB->accept(*this);
        m_Tabs--;
        printTabs();
        std::cout << "else:" << std::endl;
        m_Tabs++;
        auto elseB = ifNode->getThenBlock();
        elseB->accept(*this);
        m_Tabs--;
        m_Tabs--;
    }

    void ASTPrinter::dispatchFor(ASTForNode* forNode) {
        printTabs();
        std::cout << "ASTForNode:" << std::endl;
        m_Tabs++;
        printTabs();
        std::cout << "iteration variable: " << std::endl;
        m_Tabs++;
        printTabs();
        std::cout << forNode->getIteratorVariable() << std::endl;
        m_Tabs--;
        printTabs();
        std::cout << "rang expr:" << std::endl;
        m_Tabs++;
        auto range = forNode->getRangeExpr();
        range->accept(*this);
        m_Tabs--;
        printTabs();
        std::cout << "body:" << std::endl;
        m_Tabs++;
        auto body = forNode->getBlock();
        body->accept(*this);
        m_Tabs--;
        m_Tabs--;
    }

    void ASTPrinter::dispatchAssignment(ASTAssignmentNode* assignmentNode) {
        printTabs();
        std::cout << "ASTAssignmentNode:" << std::endl;
        m_Tabs++;
        printTabs();
        std::cout << "variable:" << std::endl;
        m_Tabs++;
        auto var = assignmentNode->getVariable();
        var->accept(*this);
        m_Tabs--;
        printTabs();
        std::cout << "value:" << std::endl;
        m_Tabs++;
        auto value = assignmentNode->getValue();
        value->accept(*this);
        m_Tabs--;
        m_Tabs--;
    }

} // namespace yapl
