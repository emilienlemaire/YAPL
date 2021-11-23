#include "AST/ASTMethodExtractor.hpp"
#include "AST/ASTNode.hpp"
#include "Symbol/FunctionType.hpp"
#include <memory>
#include <iostream>


// TODO: Review the AST so that manipulating it is easier
namespace yapl
{
    ASTMethodExtractor::ASTMethodExtractor(std::unique_ptr<ASTProgramNode> program)
        : m_Program(std::move(program))
    {}

    std::unique_ptr<ASTProgramNode> ASTMethodExtractor::releaseProgram() {
        return std::move(m_Program);
    }

    void ASTMethodExtractor::extractMethods() {
        if (!m_Program) {
            std::cerr << "Method extractor does not own the program AST" << std::endl;
            exit(EXIT_FAILURE);
        }

        m_Program->accept(*this);
    }

    void ASTMethodExtractor::dispatchProgram(ASTProgramNode* program) {
        m_Iterator = program->begin();

        for (unsigned i = 0; i < program->getNumNodes(); i++) {
            (*m_Iterator)->accept(*this);
            m_Iterator++;
        }

        for (auto &meth : m_Methods) {
            m_Program->addNode(std::move(meth));
        }
    }
    void ASTMethodExtractor::dispatchCastExpr(ASTCastExpr* castExpr) {}
    void ASTMethodExtractor::dispatchNegExpr(ASTNegExpr* negExpr) {}
    void ASTMethodExtractor::dispatchNotExpr(ASTNotExpr* notExpr) {}
    void ASTMethodExtractor::dispatchParExpr(ASTParExpr* parExpr) {}
    void ASTMethodExtractor::dispatchArgumentList(ASTArgumentList* argumentList) {}
    void ASTMethodExtractor::dispatchArrayLiteralExpr(ASTArrayLiteralExpr* arrayLiteral) {}
    void ASTMethodExtractor::dispatchBoolLiteralExpr(ASTBoolLiteralExpr* boolLiteralExpr) {}
    void ASTMethodExtractor::dispatchBinaryExpr(ASTBinaryExpr* binaryExpr) {}
    void ASTMethodExtractor::dispatchRangeExpr(ASTRangeExpr* rangeExpr) {}
    void ASTMethodExtractor::dispatchFloatNumberExpr(ASTFloatNumberExpr* floatNumberExpr) {}
    void ASTMethodExtractor::dispatchDoubleNumberExpr(ASTDoubleNumberExpr* doubleNumberExpr) {}
    void ASTMethodExtractor::dispatchIntegerNumberExpr(ASTIntegerNumberExpr* integerNumberExpr) {}
    void ASTMethodExtractor::dispatchIdentifierExpr(ASTIdentifierExpr* identifierExpr) {}
    void ASTMethodExtractor::dispatchAttributeAccessExpr(ASTAttributeAccessExpr* attributeAccessExpr) {}
    void ASTMethodExtractor::dispatchArrayAccessExpr(ASTArrayAccessExpr* arrayAccessExpr) {}
    void ASTMethodExtractor::dispatchFunctionCallExpr(ASTFunctionCallExpr* functionCallExpr) {}

    void ASTMethodExtractor::dispatchBlock(ASTBlockNode* blockNode) {}
    void ASTMethodExtractor::dispatchExprStatement(ASTExprStatementNode* exprStatementNode) {}
    void ASTMethodExtractor::dispatchDeclaration(ASTDeclarationNode* declarationNode) {}
    void ASTMethodExtractor::dispatchArrayDeclaration(ASTArrayDeclarationNode* arrayDeclarationNode) {}
    void ASTMethodExtractor::dispatchInitialization(ASTInitializationNode* initializationNode) {}
    void ASTMethodExtractor::dispatchArrayInitialization(ASTArrayInitializationNode* arrayInitializationNode) {}
    void ASTMethodExtractor::dispatchStructInitialization(ASTStructInitializationNode* structInitializationNode) {}
    void ASTMethodExtractor::dispatchFunctionDefinition(ASTFunctionDefinitionNode* functionDefinitionNode) {}

    void ASTMethodExtractor::dispatchStructDefinition(ASTStructDefinitionNode* structDefinitionNode) {
        for (auto &method: structDefinitionNode->getMethods()) {

            auto extractedMethod = std::make_unique<ASTFunctionDefinitionNode>(method->getScope());
            auto thisParam = std::make_unique<ASTDeclarationNode>(method->getScope());

            thisParam->setType(structDefinitionNode->getStructName());
            thisParam->setIdentifier("this");
            extractedMethod->addParameter(std::move(thisParam));

            extractedMethod->setReturnType(method->getReturnType());

            auto paramsType = std::vector<Type*>();
            for (auto &param: *(&method->getParameters())) {
                auto newParam = std::make_unique<ASTDeclarationNode>(param->getScope());
                *newParam = *param;
                auto type = method->getScope()->lookup(newParam->getType())->getType();
                paramsType.push_back(type);
                extractedMethod->addParameter(std::move(newParam));
            }

            extractedMethod->setFunctionName(structDefinitionNode->getStructName() + "_" + method->getFunctionName());


            auto returnType = extractedMethod->getScope()->lookup(extractedMethod->getReturnType())->getType();
            auto funcType = Type::CreateFunctionType(returnType, paramsType);
            auto insertedType = Type::GetOrInsertType(std::dynamic_pointer_cast<Type>(funcType));
            auto functionValue = Value::CreateFunctionValue(extractedMethod->getFunctionName(), funcType.get());

            structDefinitionNode->getScope()->insert(functionValue);

            extractedMethod->overrideBody(method->releaseBody());
            structDefinitionNode->removeMethod(method->getFunctionName());
            m_Methods.push_back(std::move(extractedMethod));

        }
    }

    void ASTMethodExtractor::dispatchImport(ASTImportNode* importNode) {}
    void ASTMethodExtractor::dispatchExport(ASTExportNode* exportNode) {}
    void ASTMethodExtractor::dispatchReturn(ASTReturnNode* returnNode) {}
    void ASTMethodExtractor::dispatchIf(ASTIfNode* ifNode) {}
    void ASTMethodExtractor::dispatchFor(ASTForNode* forNode) {}
    void ASTMethodExtractor::dispatchAssignment(ASTAssignmentNode* assignmentNode) {}
} // namespace yapl
