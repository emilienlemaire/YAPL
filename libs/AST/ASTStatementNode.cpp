#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "AST/ASTStatementNode.hpp"

namespace yapl {
    /*
     * ASTStatementNode
     *
     * */
    ASTStatementNode::ASTStatementNode(SharedScope scope)
        : ASTNode(scope)
    {}

    /* 
     * ASTImportNode
     *
     * */
    ASTImportNode::ASTImportNode(SharedScope scope)
        : ASTStatementNode(scope)
    {}

    void ASTImportNode::addNamespace(const std::string &ns) {
        m_Namespaces.push_back(std::move(ns));
    }

    void ASTImportNode::setImportedValue(const std::string &name) {
        m_ImportedValue = std::move(name);
    }

    std::vector<std::string> ASTImportNode::getNamesapces() const {
        return m_Namespaces;
    }

    std::string ASTImportNode::getImportedValue() const {
        return m_ImportedValue;
    }

    /*
     * ASTExportNode
     *
     * */
    ASTExportNode::ASTExportNode(SharedScope scope)
        :ASTStatementNode(scope)
    {}

    void ASTExportNode::setExportedValue(const std::string &name) {
        m_ExportedValue = std::move(name);
    }

    std::string ASTExportNode::getExportedValue() const {
        return m_ExportedValue;
    }

    /* 
     * ASTFunctionDefinitionNode
     *
     * */
    ASTFunctionDefinitionNode::ASTFunctionDefinitionNode(SharedScope scope)
        : ASTStatementNode(scope)
    {}

    void ASTFunctionDefinitionNode::setFunctionName(const std::string &name) {
        m_FunctionName = std::move(name);
    }

    void ASTFunctionDefinitionNode::setReturnType(const std::string &type) {
        m_ReturnType = std::move(type);
    }

    void ASTFunctionDefinitionNode::addParam(const std::string &name) {
        m_ParamNames.push_back(std::move(name));
    }

    std::string ASTFunctionDefinitionNode::getFunctionName() const {
        return m_FunctionName;
    }

    std::string ASTFunctionDefinitionNode::getReturnType() const {
        return m_ReturnType;
    }

    std::vector<std::string> ASTFunctionDefinitionNode::getParamNames() const {
        return m_ParamNames;
    }

    /* 
     * ASTStructDefinitionNode
     *
     * */
    ASTStructDefinitionNode::ASTStructDefinitionNode(SharedScope scope)
        : ASTStatementNode(scope)
    {}

    void ASTStructDefinitionNode::setStructName(const std::string &name) {
        m_StructName = std::move(name);
    }

    void ASTStructDefinitionNode::addAttribute(const std::string &name) {
        m_AttributeNames.push_back(std::move(name));
    }

    void ASTStructDefinitionNode::addMethod(const std::string &name) {
        m_MethodNames.push_back(std::move(name));
    }

    std::string ASTStructDefinitionNode::getStructName() const {
        return m_StructName;
    }

    std::vector<std::string> ASTStructDefinitionNode::getAttributeNames() const {
        return m_AttributeNames;
    }

    std::vector<std::string> ASTStructDefinitionNode::getMethodNames() const {
        return m_MethodNames;
    }
}
