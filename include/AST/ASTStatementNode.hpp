#pragma once

#include <array>
#include <cstddef>
#include <string>
#include <vector>
#include <memory>

#include "AST/ASTNode.hpp"

namespace yapl
{
    using SharedScope = std::shared_ptr<SymbolTable>;

    class ASTStatementNode : public ASTNode {
    private:
    public:
        ASTStatementNode(SharedScope);
        virtual ~ASTStatementNode() = default;
    };

    class ASTImportNode : public ASTStatementNode {
    private:
        std::vector<std::string> m_Namespaces;
        std::string m_ImportedValue;
    public:
        ASTImportNode(SharedScope);
        void addNamespace(const std::string&);
        void setImportedValue(const std::string&);

        std::vector<std::string> getNamesapces() const;
        std::string getImportedValue() const;
    };

    class ASTExportNode : public ASTStatementNode {
    private:
        std::vector<std::string> m_ExportedValues;
    public:
        ASTExportNode(SharedScope);
        void addExportedValue(const std::string&);

        std::vector<std::string> getExportedValues() const;
    };

    class ASTFunctionDefinitionNode : public ASTStatementNode {
    private:
        std::string m_FunctionName;
        std::string m_ReturnType;
        std::vector<std::string> m_ParamNames;
    public:
        ASTFunctionDefinitionNode(SharedScope);
        void setFunctionName(const std::string&);
        void setReturnType(const std::string&);
        void addParam(const std::string &);

        std::string getFunctionName() const;
        std::string getReturnType() const;
        std::vector<std::string> getParamNames() const;
    };

    class ASTStructDefinitionNode : public ASTStatementNode {
    private:
        std::string m_StructName;
        std::vector<std::string> m_AttributeNames;
        std::vector<std::string> m_MethodNames;
    public:
        ASTStructDefinitionNode(SharedScope);
        
        void setStructName(const std::string&);
        void addAttribute(const std::string&);
        void addMethod(const std::string&);

        std::string getStructName() const;
        std::vector<std::string> getAttributeNames() const;
        std::vector<std::string> getMethodNames() const;
    };
}
