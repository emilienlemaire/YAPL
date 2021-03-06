/**
 * libs/AST/ASTExprNode.cpp
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
#include <memory>
#include <string>
#include <vector>

#include "AST/ASTExprNode.hpp"

namespace yapl {
    ASTExprNode::ASTExprNode(SharedScope scope)
        : ASTNode(scope)
    {}

    ASTNumberExpr::ASTNumberExpr(SharedScope scope)
        : ASTExprNode(scope)
    {}

    ASTFloatNumberExpr::ASTFloatNumberExpr(SharedScope scope)
        :ASTNumberExpr(scope)
    {}

    void ASTFloatNumberExpr::setValue(float value) {
        m_Value = value;
    }

    float ASTFloatNumberExpr::getValue() const {
        return m_Value;
    }

    ASTDoubleNumberExpr::ASTDoubleNumberExpr(SharedScope scope)
        :ASTNumberExpr(scope)
    {}

    void ASTDoubleNumberExpr::setValue(double value) {
        m_Value = value;
    }

    double ASTDoubleNumberExpr::getValue() const {
        return m_Value;
    }

    ASTIntegerNumberExpr::ASTIntegerNumberExpr(SharedScope scope)
        : ASTNumberExpr(scope)
    {}

    void ASTIntegerNumberExpr::setValue(int value) {
        m_Value = value;
    }

    int ASTIntegerNumberExpr::getValue() const {
        return m_Value;
    }
}
