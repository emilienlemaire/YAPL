#include <iostream>
#include <CppLogger2/CppLogger2.h>
#include <memory>
#include <filesystem>
#include <stdexcept>
#include <string>

#include "YAPL.h"
#include "Lexer/Lexer.hpp"
#include "Lexer/TokenUtils.hpp"
#include "IRGenerator/IRGenerator.hpp"

int main(int argc, char *argv[])
{
    CppLogger::CppLogger mainConsole(CppLogger::Level::Trace, "Main");

    CppLogger::Format mainFormat({
            CppLogger::FormatAttribute::Name,
            CppLogger::FormatAttribute::Message
    });

    mainConsole.printTrace("YAPL v.{}", VERSION);

    if (argc > 1) {
        std::string filepath = argv[1];
        IRGenerator generator(filepath);
        generator.generate();
    } else {
       IRGenerator generator("");
       generator.generate();
    }

    return 0;
}
