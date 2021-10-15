/**
 * main.cpp
 * Copyright (c) 2021 Emilien Lemaire <emilien.lem@icloud.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <CppLogger2/CppLogger2.h>

#include "IRGenerator/IRGenerator.hpp"
#include "YAPL.h"
#include "Parser/Parser.hpp"
#include "Printer/ASTPrinter.hpp"
#include "YASA/YasaVisitor.hpp"

int main(int argc, char *argv[]) {
    CppLogger::CppLogger mainConsole(CppLogger::Level::Trace, "Main");

    CppLogger::Format mainFormat({
            CppLogger::FormatAttribute::Name,
            CppLogger::FormatAttribute::Message
    });

    mainConsole.setFormat(mainFormat);

    mainConsole.printTrace("YAPL v.0.0.1");

    if (argc > 1) {
        std::string filepath = argv[1]; //NOLINT: We know this is a good index, we check it before.
        yapl::Parser parser(filepath, CppLogger::Level::Trace);
        parser.parse();
        /* yapl::ASTPrinter printer(std::move(parser.getProgram()));
        printer.dump();
        auto prog = printer.releaseProgram(); */
        yapl::YasaVisitor yasaVisitor(std::move(parser.getProgram()));
        yasaVisitor.analyze();
        yapl::IRGenerator irGenerator = yapl::IRGenerator(
                yasaVisitor.getExprTypeMap(),
                yasaVisitor.releaseProgram(),
                filepath
            );
        irGenerator.generate();
    } else {
        mainConsole.printFatalError("The REPL is not yet implemented");
    }

}

