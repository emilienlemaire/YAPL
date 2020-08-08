#include <iostream>

#include <CppLogger2/CppLogger2.h>

#include "YAPL.h"

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

    while (input != "exit()") {
        std::cout << "YAPL >>>";
        std::cin >> input;
        mainConsole.printInfo("Vous avez ecrit: {}", input);
    }

    mainConsole.printInfo("Fin du programme");

    return 0;
}
