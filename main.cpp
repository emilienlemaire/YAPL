#include <iostream>

#include <CppLogger2/CppLogger2.h>

#include "YAPL.h"
#include "Lexer/Lexer.hpp"

int main(int argc, char *argv[])
{
    CppLogger::CppLogger mainConsole(CppLogger::Level::Trace, "Main");

    CppLogger::Format mainFormat({
            CppLogger::FormatAttribute::Name,
            CppLogger::FormatAttribute::Message
    });

    mainConsole.setFormat(mainFormat);

    std::string input = "";
    std::cout << "YAPL version" << VERSION << std::endl;

    Lexer lexer = Lexer();

    while (input != "exit()") {
        std::cout << "YAPL >>>";
        Token tok = lexer.getNextToken();
        mainConsole.printInfo("Vous avez ecrit: {}", tok);
    }

    mainConsole.printInfo("Fin du programme");

    return 0;
}
