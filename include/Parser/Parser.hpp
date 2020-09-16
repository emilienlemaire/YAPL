#include "AST/ASTNode.hpp"
#include "CppLogger2/include/CppLogger.h"
#include "Lexer/Lexer.hpp"
#include <CppLogger2/CppLogger2.h>
#include <memory>

class Parser {
private:
    CppLogger::CppLogger m_Logger;
    Lexer m_Lexer;

    template<typename... T>
    std::unique_ptr<ASTNode> parseError(std::string msg, T... var) {
        m_Logger.printError(msg, var...);
        return nullptr;
    }

public:
    Parser(std::string file="", CppLogger::Level level=CppLogger::Level::Warn);
    std::unique_ptr<ASTNode> parseNext();
};
