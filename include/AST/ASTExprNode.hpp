/**
 * include/AST/ASTExprNode.hpp
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
#pragma once

#include "AST/ASTNode.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace yapl {
    using SharedScope = std::shared_ptr<SymbolTable>;

    class ASTExprNode : public ASTNode {
    private:
    public:
        ASTExprNode(SharedScope);
    };

    class ASTNumberExpr : public ASTExprNode {
    public:
        ASTNumberExpr(SharedScope);
    };

    class ASTFloatNumberExpr : public ASTNumberExpr {
    private:
        float m_Value;
    public:
        ASTFloatNumberExpr(SharedScope);

        void setValue(float);

        [[nodiscard]] float getValue() const;
    };

    class ASTDoubleNumberExpr : public ASTNumberExpr {
    private:
        double m_Value;
    public:
        ASTDoubleNumberExpr(SharedScope);

        void setValue(double);

        [[nodiscard]] double getValue() const;
    };

    class ASTIntegerNumberExpr : public ASTNumberExpr {
    private:
        int m_Value;
    public:
        ASTIntegerNumberExpr(SharedScope);

        void setValue(int);

        [[nodiscard]] int getValue() const;
    };
}
