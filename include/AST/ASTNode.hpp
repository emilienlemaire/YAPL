/**
 * include/AST/ASTNode.hpp
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

#include <cstddef>
#include <memory>
#include <vector>
#include <string>

#include "Lexer/TokenUtils.hpp"
#include "Symbol/SymbolTable.hpp"

namespace yapl {

    enum class Operator {
        NONE = 0,
        TIMES,
        BY,
        MOD,
        PLUS,
        MINUS,
        LTH,
        MTH,
        LEQ,
        MEQ,
        EQ,
        NEQ,
        AND,
        OR
    };

    class ASTNode {
    private:
        std::shared_ptr<SymbolTable> m_Scope;
    public:
        explicit ASTNode(std::shared_ptr<SymbolTable>);
        virtual ~ASTNode() = default;

        [[nodiscard]] std::shared_ptr<SymbolTable> getScope() const;

        inline static Operator TokenToOperator(token tok) {
            switch(tok) {
                case token::TIMES:
                    return Operator::TIMES;
                case token::BY:
                    return Operator::BY;
                case token::MOD:
                    return Operator::MOD;
                case token::PLUS:
                    return Operator::PLUS;
                case token::MINUS:
                    return Operator::MINUS;
                case token::LTH:
                    return Operator::LTH;
                case token::MTH:
                    return Operator::MTH;
                case token::LEQ:
                    return Operator::LEQ;
                case token::MEQ:
                    return Operator::MEQ;
                case token::EQ:
                    return Operator::EQ;
                case token::NEQ:
                    return Operator::NEQ;
                case token::AND:
                    return Operator::AND;
                case token::OR:
                    return Operator::OR;
                default:
                    return Operator::NONE;
            }
        }
    };

    class ASTEOFNode : public ASTNode {
    public:
        explicit ASTEOFNode(std::shared_ptr<SymbolTable> st)
            : ASTNode(std::move(st))
        {}
    };

    class ASTProgramNode : public ASTNode {
    private:
        std::vector<std::unique_ptr<ASTNode>> m_Nodes;
    public:
        ASTProgramNode(std::shared_ptr<SymbolTable> scope, std::vector<std::unique_ptr<ASTNode>> nodes)
            :ASTNode(std::move(scope)), m_Nodes(std::move(nodes))
        {}
        void addNode(std::unique_ptr<ASTNode> node);

        using vec_type = std::vector<std::unique_ptr<ASTNode>>;
        using iterator = vec_type::iterator;
        using const_iterator = vec_type::const_iterator;

        [[nodiscard]] inline iterator begin() noexcept { return m_Nodes.begin(); }
        [[nodiscard]] inline const_iterator cbegin() const noexcept { return m_Nodes.cbegin(); }
        [[nodiscard]] inline iterator end() noexcept { return m_Nodes.end(); }
        [[nodiscard]] inline const_iterator cend() const noexcept { return m_Nodes.cend(); }
    };
} // namespace yapl
