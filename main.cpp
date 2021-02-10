/*******************************************************************************
 * Copyright 2020 Emilien Lemaire
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *******************************************************************************/

#include <iostream>
#include <CppLogger2/CppLogger2.h>

#include "YAPL.h"
#include "Parser/Parser.hpp"



int main(int argc, char *argv[]) {
    CppLogger::CppLogger mainConsole(CppLogger::Level::Trace, "Main");

    CppLogger::Format mainFormat({
            CppLogger::FormatAttribute::Name,
            CppLogger::FormatAttribute::Message
    });

    mainConsole.printTrace("YAPL v.{}", VERSION);

    if (argc > 1) {
        std::string filepath = argv[1];
        yapl::Parser parser(filepath, CppLogger::Level::Trace);
        parser.parse();
    } else {
        yapl::Parser parser("", CppLogger::Level::Trace);
        parser.parse();
    }

    return 0;
}
