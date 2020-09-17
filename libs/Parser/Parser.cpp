#include "Parser/Parser.hpp"
#include "AST/ASTNode.hpp"
#include "CppLogger2/include/CppLogger.h"
#include "CppLogger2/include/Format.h"
#include "Lexer/TokenUtils.hpp"
#include <memory>

Parser::Parser(std::string filepath, CppLogger::Level level)
    : m_Logger(level, "Parser"), m_Lexer(filepath)
{
    CppLogger::Format format({
            CppLogger::FormatAttribute::Name,
            CppLogger::FormatAttribute::Level,
            CppLogger::FormatAttribute::Message
        });

    m_Logger.setFormat(format);

    parseError("This is a test");
    parseError("This is a test with {}", 1);
}

void Parser::parse() {
    while (m_Lexer.peekToken().token != token::eof) {
        m_Program->addNode(std::move(parseNext()));
    }
}

std::unique_ptr<ASTNode> Parser::parseNext() {
    m_CurrentToken = m_Lexer.getNextToken();
    if (m_CurrentToken.token == token::importlabel) {
        return parseImport();
    }
    return nullptr;
}
