#include <iostream>

#include <CppLogger2/CppLogger2.h>

#include "CppLogger2/include/CppLogger.h"
#include "YAPL.h"
#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"
#include "Lexer/TokenUtils.hpp"

int main(int argc, char *argv[])
{
    CppLogger::CppLogger mainConsole(CppLogger::Level::Trace, "Main");

    CppLogger::Format mainFormat({
            CppLogger::FormatAttribute::Name,
            CppLogger::FormatAttribute::Message
    });

    mainConsole.setFormat(mainFormat);

    std::cout << "YAPL version " << VERSION << std::endl;

    Parser parser("../example.yapl", CppLogger::Level::Trace);

    parser.parse();

    return 0;
}
