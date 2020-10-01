#include <iostream>

#include "IRGenerator/IRGenerator.hpp"

void IRGenerator::generate() {
    m_Parser->parse();

    m_Program = m_Parser->getProgram();

    for (const auto& node : *m_Program) {
        if (auto literalInt = dynamic_cast<ASTLiteralNode<int>*>(node.get())) {
            generateLiteralInt(literalInt);
        }
    }

}

void IRGenerator::generateLiteralInt(ASTLiteralNode<int> *literalInt) {
    m_Logger.printInfo("Generating literal int");
}
