#include <iostream>

#include <CppLogger2/CppLogger2.h>

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

    Lexer lexer = Lexer("../example.yapl");

    Token tok = lexer.getNextToken();

    mainConsole.printInfo("Vous avez ecrit: {}", tok);

    while (tok.token != token::eof) {
        tok = lexer.getNextToken();
        if (tok.identifier == "\n") {
            tok.identifier = "\\n";
        }
        mainConsole.printInfo("Vous avez ecrit: {}", tok);
    }

    Parser parser;

    mainConsole.printInfo("Fin du programme");

    return 0;
}
