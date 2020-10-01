#include <iostream>
#include <CppLogger2/CppLogger2.h>
#include <memory>
#include <filesystem>
#include <stdexcept>

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

    return 0;
}
