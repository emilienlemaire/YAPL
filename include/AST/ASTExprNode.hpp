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
        virtual ~ASTExprNode() override = default;
    };

    class ASTUnaryExpr : public ASTExprNode {
    private:
    public:
        explicit ASTUnaryExpr(SharedScope);
        virtual ~ASTUnaryExpr() override = default;
    };

    class ASTCallableExpr : public ASTExprNode {
    public:
        explicit ASTCallableExpr(SharedScope);
        virtual ~ASTCallableExpr() override = default;
    };

    class ASTAccessibleExpr : public ASTCallableExpr {
    public:
        explicit ASTAccessibleExpr(SharedScope);
        virtual ~ASTAccessibleExpr() override = default;
    };

    class ASTAssignableExpr : public ASTAccessibleExpr {
    public:
        explicit ASTAssignableExpr(SharedScope);
        virtual ~ASTAssignableExpr() override = default;
    };

    class ASTNumberExpr : public ASTExprNode {
    public:
        explicit ASTNumberExpr(SharedScope);
        virtual ~ASTNumberExpr() override = default;
    };

    // Non virtual classes

    class ASTCastExpr : public ASTExprNode {
    private:
        std::string m_TargetType;
        std::unique_ptr<ASTExprNode> m_Expr;

    public:
        explicit ASTCastExpr(SharedScope);

        void setTargetType(const std::string&);
        void setExpr(std::unique_ptr<ASTExprNode>);

        [[nodiscard]] const std::string &getTargetType() const;
        [[nodiscard]] ASTExprNode *getExpr() const;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTNegExpr : public ASTUnaryExpr {
    private:
        std::unique_ptr<ASTExprNode> m_Value;
    public:
        explicit ASTNegExpr(SharedScope);

        void setValue(std::unique_ptr<ASTExprNode>);

        [[nodiscard]] ASTExprNode *getValue() const;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTNotExpr : public ASTUnaryExpr {
    private:
        std::unique_ptr<ASTExprNode> m_Value;
    public:
        explicit ASTNotExpr(SharedScope);

        void setValue(std::unique_ptr<ASTExprNode>);

        [[nodiscard]] ASTExprNode *getValue() const;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTParExpr : public ASTExprNode {
    private:
        std::unique_ptr<ASTExprNode> m_Expr;
    public:
        explicit ASTParExpr(SharedScope);

        void setExpr(std::unique_ptr<ASTExprNode>);

        [[nodiscard]] ASTExprNode *getExpr() const;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTArgumentList : public ASTExprNode {
    private:
        std::vector<std::unique_ptr<ASTExprNode>> m_Arguments;

    public:
        explicit ASTArgumentList(SharedScope);

        using vectorType = typename std::vector<std::unique_ptr<ASTExprNode>>;
        using iterator = typename vectorType::iterator;
        using const_iterator = typename vectorType::const_iterator;

        void addArgument(std::unique_ptr<ASTExprNode>);

        [[nodiscard]] const vectorType &getArguments() const;

        inline auto begin() noexcept -> decltype(m_Arguments.begin())
            { return m_Arguments.begin(); }
        inline auto end() noexcept -> decltype(m_Arguments.end())
            { return m_Arguments.end(); }
        inline auto cbegin() noexcept -> decltype(m_Arguments.cbegin())
            { return m_Arguments.cbegin(); }
        inline auto cend() noexcept -> decltype(m_Arguments.cend())
            { return m_Arguments.cend(); }

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTBoolLiteralExpr : public ASTExprNode {
    private:
        bool m_Value = false;
    public:
        explicit ASTBoolLiteralExpr(SharedScope);

        void setValue(bool);

        [[nodiscard]] bool getValue() const;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTBinaryExpr : public ASTExprNode {
    private:
        std::unique_ptr<ASTExprNode> m_LHS;
        std::unique_ptr<ASTExprNode> m_RHS;

        Operator m_Operator = Operator::NONE;
    public:
        explicit ASTBinaryExpr(SharedScope);

        void setLHS(std::unique_ptr<ASTExprNode>);
        void setRHS(std::unique_ptr<ASTExprNode>);
        void setOperator(Operator);

        [[nodiscard]] ASTExprNode *getLHS() const;
        [[nodiscard]] ASTExprNode *getRHS() const;
        [[nodiscard]] Operator getOperator() const;

        [[nodiscard]] inline auto getLHSPtr() -> decltype(m_LHS) {
            return std::move(m_LHS);
        }

        [[nodiscard]] inline auto getRHSPtr() -> decltype(m_RHS) {
            return std::move(m_RHS);
        }

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTRangeExpr : public ASTExprNode {
    private:
        std::unique_ptr<ASTExprNode> m_Start;
        std::unique_ptr<ASTExprNode> m_End;

    public:
        explicit ASTRangeExpr(SharedScope);

        void setStart(std::unique_ptr<ASTExprNode>);
        void setEnd(std::unique_ptr<ASTExprNode>);

        [[nodiscard]] ASTExprNode *getStart() const;
        [[nodiscard]] ASTExprNode *getEnd() const;

        [[nodiscard]] inline auto getStartPtr() -> decltype(m_Start) {
            return std::move(m_Start);
        }

        [[nodiscard]] inline auto getEndPtr() -> decltype(m_End) {
            return std::move(m_End);
        }


        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTFloatNumberExpr : public ASTNumberExpr {
    private:
        float m_Value = 0;
    public:
        explicit ASTFloatNumberExpr(SharedScope);

        void setValue(float);

        [[nodiscard]] float getValue() const;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTDoubleNumberExpr : public ASTNumberExpr {
    private:
        double m_Value = 0;
    public:
        explicit ASTDoubleNumberExpr(SharedScope);

        void setValue(double);

        [[nodiscard]] double getValue() const;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTIntegerNumberExpr : public ASTNumberExpr {
    private:
        int m_Value = 0;
    public:
        explicit ASTIntegerNumberExpr(SharedScope);

        void setValue(int);

        [[nodiscard]] int getValue() const;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTIdentifierExpr : public ASTAssignableExpr {
    private:
        std::string m_Identifier;
    public:
        explicit ASTIdentifierExpr(SharedScope);

        void setIdentifier(const std::string &);

        [[nodiscard]] std::string getIdentifier() const;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTAttributeAccessExpr : public ASTAssignableExpr {
    private:
        std::unique_ptr<ASTAccessibleExpr> m_Struct;
        std::unique_ptr<ASTIdentifierExpr> m_Attribute;
    public:
        explicit ASTAttributeAccessExpr(SharedScope);

        void setStruct(std::unique_ptr<ASTAccessibleExpr>);
        void setAttribute(std::unique_ptr<ASTIdentifierExpr>);

        [[nodiscard]] ASTAccessibleExpr *getStruct() const;
        [[nodiscard]] ASTIdentifierExpr *getAttribute() const;

        virtual void accept(ASTVisitor &visitor) override;
    };

    class ASTArrayAccessExpr : public ASTAssignableExpr {
    private:
        std::unique_ptr<ASTAccessibleExpr> m_Array;
        std::unique_ptr<ASTExprNode> m_Index;
    public:
        explicit ASTArrayAccessExpr(SharedScope);

        void setArray(std::unique_ptr<ASTAccessibleExpr>);
        void setIndex(std::unique_ptr<ASTExprNode>);

        [[nodiscard]] ASTAccessibleExpr* getArray() const;
        [[nodiscard]] ASTExprNode* getIndex() const;

        virtual void accept(ASTVisitor &visitor) override;
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

        [[nodiscard]] ASTCallableExpr *getFunction() const;
        [[nodiscard]] ASTArgumentList *getArguments() const;

        virtual void accept(ASTVisitor &visitor) override;
    };
} // namespace yapl
