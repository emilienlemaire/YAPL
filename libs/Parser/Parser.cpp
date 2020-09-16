#include "Parser/Parser.hpp"
#include "CppLogger2/include/CppLogger.h"
#include "CppLogger2/include/Format.h"

Parser::Parser(std::string filepath, CppLogger::Level level)
    : m_Logger(level, "Parser")
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
