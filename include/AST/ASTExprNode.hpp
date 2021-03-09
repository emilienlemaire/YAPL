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

    // Virtual classes for the AST.
    class ASTExprNode : public ASTNode {
    private:
    public:
        explicit ASTExprNode(SharedScope);
    };

    class ASTUnaryExpr : public ASTExprNode {
    private:
    public:
        explicit ASTUnaryExpr(SharedScope);
    };

    class ASTAssignableExpr : public ASTExprNode {
    public:
        explicit ASTAssignableExpr(SharedScope);
    };

    class ASTCallableExpr : public ASTAssignableExpr {
    public:
        explicit ASTCallableExpr(SharedScope);
    };

    class ASTAccessibleExpr : public ASTAssignableExpr {
        public:
            explicit ASTAccessibleExpr(SharedScope);
    };

    class ASTNumberExpr : public ASTExprNode {
    public:
        explicit ASTNumberExpr(SharedScope);
    };

    // Non virtual classes

    class ASTNegExpr : public ASTUnaryExpr {
    private:
        std::unique_ptr<ASTExprNode> m_Value;
    public:
        explicit ASTNegExpr(SharedScope);

        void setValue(std::unique_ptr<ASTExprNode>);

        [[nodiscard]] const ASTExprNode *getValue() const;
    };

    class ASTNotExpr : public ASTUnaryExpr {
    private:
        std::unique_ptr<ASTExprNode> m_Value;
    public:
        explicit ASTNotExpr(SharedScope);

        void setValue(std::unique_ptr<ASTExprNode>);

        [[nodiscard]] const ASTExprNode *getValue() const;
    };

    class ASTParExpr : public ASTExprNode {
    private:
        std::unique_ptr<ASTExprNode> m_Expr;
    public:
        explicit ASTParExpr(SharedScope);

        void setExpr(std::unique_ptr<ASTExprNode>);

        [[nodiscard]] const ASTExprNode *getExpr() const;
    };

    class ASTArgumentList : public ASTExprNode {
    private:
        std::vector<std::unique_ptr<ASTExprNode>> m_Arguments;

    public:
        explicit ASTArgumentList(SharedScope);

        typedef typename std::vector<std::unique_ptr<ASTExprNode>> vectorType;
        typedef typename vectorType::iterator iterator;
        typedef typename vectorType::const_iterator const_iterator;

        void addArgument(std::unique_ptr<ASTExprNode>);

        const vectorType &getArguments() const;

        inline auto begin() noexcept -> decltype(m_Arguments.begin())
            { return m_Arguments.begin(); }
        inline auto end() noexcept -> decltype(m_Arguments.end())
            { return m_Arguments.end(); }
        inline auto cbegin() noexcept -> decltype(m_Arguments.cbegin())
            { return m_Arguments.cbegin(); }
        inline auto cend() noexcept -> decltype(m_Arguments.cend())
            { return m_Arguments.cend(); }
    };

    class ASTBoolLiteralExpr : public ASTExprNode {
    private:
        bool m_Value;
    public:
        ASTBoolLiteralExpr(SharedScope);

        void setValue(bool);

        [[nodiscard]] bool getValue() const;
    };

    class ASTBinaryExpr : public ASTExprNode {
    private:
        std::unique_ptr<ASTExprNode> m_LHS;
        std::unique_ptr<ASTExprNode> m_RHS;

        Operator m_Operator;
    public:
        explicit ASTBinaryExpr(SharedScope);

        void setLHS(std::unique_ptr<ASTExprNode>);
        void setRHS(std::unique_ptr<ASTExprNode>);
        void setOperator(Operator);

        [[nodiscard]] const ASTExprNode *getLHS() const;
        [[nodiscard]] const ASTExprNode *getRHS() const;
        [[nodiscard]] Operator getOperator() const;

        [[nodiscard]] inline auto getLHSPtr() -> decltype(m_LHS) {
            return std::move(m_LHS);
        }

        [[nodiscard]] inline auto getRHSPtr() -> decltype(m_RHS) {
            return std::move(m_RHS);
        }
    };

    class ASTRangeExpr : public ASTExprNode {
    private:
        std::unique_ptr<ASTExprNode> m_Start;
        std::unique_ptr<ASTExprNode> m_End;

    public:
        explicit ASTRangeExpr(SharedScope);

        void setStart(std::unique_ptr<ASTExprNode>);
        void setEnd(std::unique_ptr<ASTExprNode>);

        [[nodiscard]] const ASTExprNode *getStart() const;
        [[nodiscard]] const ASTExprNode *getEnd() const;
    };

    class ASTFloatNumberExpr : public ASTNumberExpr {
    private:
        float m_Value;
    public:
        explicit ASTFloatNumberExpr(SharedScope);

        void setValue(float);

        [[nodiscard]] float getValue() const;
    };

    class ASTDoubleNumberExpr : public ASTNumberExpr {
    private:
        double m_Value;
    public:
        explicit ASTDoubleNumberExpr(SharedScope);

        void setValue(double);

        [[nodiscard]] double getValue() const;
    };

    class ASTIntegerNumberExpr : public ASTNumberExpr {
    private:
        int m_Value;
    public:
        explicit ASTIntegerNumberExpr(SharedScope);

        void setValue(int);

        [[nodiscard]] int getValue() const;
    };

    class ASTIdentifierExpr : public ASTCallableExpr {
    private:
        std::string m_Identifier;
    public:
        explicit ASTIdentifierExpr(SharedScope);

        void setIdentifier(const std::string &);

        [[nodiscard]] std::string getIdentifier() const;
    };

    class ASTAttributeAccessExpr : public ASTCallableExpr {
    private:
        std::unique_ptr<ASTAccessibleExpr> m_Struct;
        std::unique_ptr<ASTIdentifierExpr> m_Attribute;
    public:
        explicit ASTAttributeAccessExpr(SharedScope);

        void setStruct(std::unique_ptr<ASTAccessibleExpr>);
        void setAttribute(std::unique_ptr<ASTIdentifierExpr>);

        [[nodiscard]] const ASTAccessibleExpr *getStruct() const;
        [[nodiscard]] const ASTIdentifierExpr *getAttribute() const;
    };

    class ASTArrayAccessExpr : public ASTAssignableExpr {
    private:
        std::unique_ptr<ASTAccessibleExpr> m_Array;
        std::unique_ptr<ASTExprNode> m_Index;
    public:
        explicit ASTArrayAccessExpr(SharedScope);

        void setArray(std::unique_ptr<ASTAccessibleExpr>);
        void setIndex(std::unique_ptr<ASTExprNode>);

        [[nodiscard]] const ASTAccessibleExpr* getArray() const;
        [[nodiscard]] const ASTExprNode* getIndex() const;
    };

    class ASTFunctionCallExpr : public ASTAccessibleExpr {
    private:
        std::unique_ptr<ASTCallableExpr> m_Function;
        std::unique_ptr<ASTArgumentList> m_Arguments;
    public:
        explicit ASTFunctionCallExpr(SharedScope);

        void setFunction(std::unique_ptr<ASTCallableExpr>);
        void setArguments(std::unique_ptr<ASTArgumentList>);

        void setFunction(ASTCallableExpr *callable) {
            m_Function.reset(callable);
        }

        [[nodiscard]] const ASTCallableExpr *getFunction() const;
        [[nodiscard]] const ASTArgumentList *getArguments() const;
    };
}
