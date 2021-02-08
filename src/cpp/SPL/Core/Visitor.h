/*
 * Copyright 2021 IBM Corporation
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

#ifndef SPL_VISITOR_H
#define SPL_VISITOR_H

#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Literal.h>
#include <SPL/CodeGen/Statement.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Utility/Debug.h>
#include <TRC/DistilleryDebug.h>

#include <cassert>
#include <iostream>
#include <vector>

namespace SPL {

#define VISITORMODIFIER(N)                                                                         \
    template<class T>                                                                              \
    class N                                                                                        \
    {                                                                                              \
      public:                                                                                      \
        N()                                                                                        \
          : done_(false)                                                                           \
        {}                                                                                         \
                                                                                                   \
        bool visit(T& context, SPL::Statement CONST& stmt);                                        \
        bool visitStatement(T& context, SPL::Statement CONST& stmt);                               \
                                                                                                   \
        bool visit(T& context, SPL::BlockStatement CONST& stmt);                                   \
        bool visitBlockStatements(T& context, SPL::BlockStatement CONST& stmt);                    \
        bool visit(T& context, SPL::BreakStatement CONST& stmt);                                   \
        bool visit(T& context, SPL::ContinueStatement CONST& stmt);                                \
        bool visit(T& context, SPL::DeclStatement CONST& stmt);                                    \
        bool visitDeclarationInitializer(T& context, SPL::DeclStatement CONST& stmt);              \
        bool visit(T& context, SPL::ExpressionStatement CONST& stmt);                              \
        bool visitExpressionStatement(T& context, SPL::ExpressionStatement CONST& stmt);           \
        bool visit(T& context, SPL::ForStatement CONST& stmt);                                     \
        bool visitForStatement(T& context, SPL::ForStatement CONST& stmt);                         \
        bool visit(T& context, SPL::IfStatement CONST& stmt);                                      \
        bool visitIfStatement(T& context, SPL::IfStatement CONST& stmt);                           \
        bool visit(T& context, SPL::ReturnStatement CONST& stmt);                                  \
        bool visitReturnStatement(T& context, SPL::ReturnStatement CONST& stmt);                   \
        bool visit(T& context, SPL::WhileStatement CONST& stmt);                                   \
        bool visitWhileStatement(T& context, SPL::WhileStatement CONST& stmt);                     \
                                                                                                   \
        bool visit(T& context, SPL::Expression CONST& expr);                                       \
        bool visitExpression(T& context, SPL::Expression CONST& expr);                             \
                                                                                                   \
        bool visit(T& context, SPL::AttributeExpression CONST& expr);                              \
        bool visitAttributeExpression(T& context, SPL::AttributeExpression CONST& expr);           \
        bool visit(T& context, SPL::BinaryExpression CONST& expr);                                 \
        bool visitBinaryExpression(T& context, SPL::BinaryExpression CONST& expr);                 \
        bool visit(T& context, SPL::CallExpression CONST& expr);                                   \
        bool visitCallExpression(T& context, SPL::CallExpression CONST& expr);                     \
        bool visit(T& context, SPL::CastExpression CONST& expr);                                   \
        bool visitCastExpression(T& context, SPL::CastExpression CONST& expr);                     \
        bool visit(T& context, SPL::ConditionalExpression CONST& expr);                            \
        bool visitConditionalExpression(T& context, SPL::ConditionalExpression CONST& expr);       \
        bool visit(T& context, SPL::EnumExpression CONST& expr);                                   \
        bool visit(T& context, SPL::LiteralExpression CONST& expr);                                \
        bool visitLiteralExpression(T& context, SPL::LiteralExpression CONST& expr);               \
        bool visit(T& context, SPL::LiteralSymbolExpression CONST& expr);                          \
        bool visit(T& context, SPL::PrefixExpression CONST& expr);                                 \
        bool visitPrefixExpression(T& context, SPL::PrefixExpression CONST& expr);                 \
        bool visit(T& context, SPL::PostfixExpression CONST& expr);                                \
        bool visitPostfixExpression(T& context, SPL::PostfixExpression CONST& expr);               \
        bool visit(T& context, SPL::StreamHistorySymbolExpression CONST& expr);                    \
        bool visit(T& context, SPL::StreamSymbolExpression CONST& expr);                           \
        bool visit(T& context, SPL::SubscriptExpression CONST& expr);                              \
        bool visitSubscriptExpression(T& context, SPL::SubscriptExpression CONST& expr);           \
        bool visit(T& context, SPL::SymbolExpression CONST& expr);                                 \
        bool visit(T& context, SPL::CustomLiteralExpression CONST& expr);                          \
        bool visit(T& context, SPL::UnaryExpression CONST& expr);                                  \
        bool visitUnaryExpression(T& context, SPL::UnaryExpression CONST& expr);                   \
        bool visit(T& context, SPL::NaryExpression CONST& expr);                                   \
        bool visitNaryExpression(T& context, SPL::NaryExpression CONST& expr);                     \
        bool visit(T& context, SPL::Literal CONST& lit);                                           \
        bool visitLiteral(T& context, SPL::Literal CONST& lit);                                    \
        bool visit(T& context, SPL::UnwrapExpression CONST& expr);                                 \
        bool visitUnwrapExpression(T& context, SPL::UnwrapExpression CONST& expr);                 \
        bool visit(T& context, SPL::UnwrapOrElseExpression CONST& expr);                           \
        bool visitUnwrapOrElseExpression(T& context, SPL::UnwrapOrElseExpression CONST& expr);     \
        bool visit(T& context, SPL::IsPresentExpression CONST& expr);                              \
        bool visitIsPresentExpression(T& context, SPL::IsPresentExpression CONST& expr);           \
                                                                                                   \
      private:                                                                                     \
        bool done_;                                                                                \
    };                                                                                             \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::Statement CONST& stmt)                                       \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting stmt" << stmt.getKind() << "(" << (void*)&stmt   \
                                   << ")",                                                         \
               context.debugFlag());                                                               \
                                                                                                   \
        return N<T>::visitStatement(context, stmt);                                                \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitStatement(T& context, SPL::Statement CONST& stmt)                              \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting stmt" << stmt.getKind() << "(" << (void*)&stmt   \
                                   << ")",                                                         \
               context.debugFlag());                                                               \
                                                                                                   \
        switch (stmt.getKind()) {                                                                  \
            case Statement::Block:                                                                 \
                if (!done_) {                                                                      \
                    done_ = visit(context, stmt.as<SPL::BlockStatement>());                        \
                    break;                                                                         \
                }                                                                                  \
            case Statement::Break:                                                                 \
                if (!done_) {                                                                      \
                    done_ = visit(context, stmt.as<SPL::BreakStatement>());                        \
                    break;                                                                         \
                }                                                                                  \
            case Statement::Continue:                                                              \
                if (!done_) {                                                                      \
                    done_ = visit(context, stmt.as<SPL::ContinueStatement>());                     \
                    break;                                                                         \
                }                                                                                  \
            case Statement::Decl:                                                                  \
                if (!done_) {                                                                      \
                    done_ = visit(context, stmt.as<SPL::DeclStatement>());                         \
                    break;                                                                         \
                }                                                                                  \
            case Statement::Expr:                                                                  \
                if (!done_) {                                                                      \
                    done_ = visit(context, stmt.as<SPL::ExpressionStatement>());                   \
                    break;                                                                         \
                }                                                                                  \
            case Statement::For:                                                                   \
                if (!done_) {                                                                      \
                    done_ = visit(context, stmt.as<SPL::ForStatement>());                          \
                    break;                                                                         \
                }                                                                                  \
            case Statement::If:                                                                    \
                if (!done_) {                                                                      \
                    done_ = visit(context, stmt.as<SPL::IfStatement>());                           \
                    break;                                                                         \
                }                                                                                  \
            case Statement::Return:                                                                \
                if (!done_) {                                                                      \
                    done_ = visit(context, stmt.as<SPL::ReturnStatement>());                       \
                    break;                                                                         \
                }                                                                                  \
            case Statement::While:                                                                 \
                if (!done_) {                                                                      \
                    done_ = visit(context, stmt.as<SPL::WhileStatement>());                        \
                    break;                                                                         \
                }                                                                                  \
            default:                                                                               \
                assert(!"Missing case in switch in visitStatement");                               \
        }                                                                                          \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::BlockStatement CONST& stmt)                                  \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting stmt" << stmt.getKind() << "(" << (void*)&stmt   \
                                   << ")",                                                         \
               context.debugFlag());                                                               \
        return N<T>::visitBlockStatements(context, stmt);                                          \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitBlockStatements(T& context, SPL::BlockStatement CONST& stmt)                   \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting stmt" << stmt.getKind() << "(" << (void*)&stmt   \
                                   << ")",                                                         \
               context.debugFlag());                                                               \
        std::vector<Statement*> CONST& stmts = stmt.getStmts();                                    \
        for (uint32_t i = 0; i < stmts.size(); ++i) {                                              \
            if (!done_) {                                                                          \
                done_ = visit(context, *stmts[i]);                                                 \
            }                                                                                      \
        }                                                                                          \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::BreakStatement CONST& stmt)                                  \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting stmt" << stmt.getKind() << "(" << (void*)&stmt   \
                                   << ")",                                                         \
               context.debugFlag());                                                               \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::ContinueStatement CONST& stmt)                               \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting stmt" << stmt.getKind() << "(" << (void*)&stmt   \
                                   << ")",                                                         \
               context.debugFlag());                                                               \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::DeclStatement CONST& stmt)                                   \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting stmt" << stmt.getKind() << "(" << (void*)&stmt   \
                                   << ")",                                                         \
               context.debugFlag());                                                               \
        return N<T>::visitDeclarationInitializer(context, stmt);                                   \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitDeclarationInitializer(T& context, SPL::DeclStatement CONST& stmt)             \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting stmt" << stmt.getKind() << "(" << (void*)&stmt   \
                                   << ")",                                                         \
               context.debugFlag());                                                               \
        if (stmt.getExpression()) {                                                                \
            done_ = visit(context, *stmt.getExpression());                                         \
        }                                                                                          \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::ForStatement CONST& stmt)                                    \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting stmt" << stmt.getKind() << "(" << (void*)&stmt   \
                                   << ")",                                                         \
               context.debugFlag());                                                               \
        return N<T>::visitForStatement(context, stmt);                                             \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitForStatement(T& context, SPL::ForStatement CONST& stmt)                        \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting stmt" << stmt.getKind() << "(" << (void*)&stmt   \
                                   << ")",                                                         \
               context.debugFlag());                                                               \
        done_ = visit(context, stmt.getExpression());                                              \
        if (!done_) {                                                                              \
            done_ = visit(context, stmt.getStatement());                                           \
        }                                                                                          \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::ExpressionStatement CONST& stmt)                             \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting stmt" << stmt.getKind() << "(" << (void*)&stmt   \
                                   << ")",                                                         \
               context.debugFlag());                                                               \
        return N<T>::visitExpressionStatement(context, stmt);                                      \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitExpressionStatement(T& context, SPL::ExpressionStatement CONST& stmt)          \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting stmt" << stmt.getKind() << "(" << (void*)&stmt   \
                                   << ")",                                                         \
               context.debugFlag());                                                               \
        return visit(context, stmt.getExpression());                                               \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::IfStatement CONST& stmt)                                     \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting stmt" << stmt.getKind() << "(" << (void*)&stmt   \
                                   << ")",                                                         \
               context.debugFlag());                                                               \
        return N<T>::visitIfStatement(context, stmt);                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitIfStatement(T& context, SPL::IfStatement CONST& stmt)                          \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting stmt" << stmt.getKind() << "(" << (void*)&stmt   \
                                   << ")",                                                         \
               context.debugFlag());                                                               \
        done_ = visit(context, stmt.getCondition());                                               \
        if (!done_) {                                                                              \
            done_ = visit(context, stmt.getThenClause());                                          \
        }                                                                                          \
        if (!done_ && stmt.getElseClause()) {                                                      \
            done_ = visit(context, *stmt.getElseClause());                                         \
        }                                                                                          \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::ReturnStatement CONST& stmt)                                 \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting stmt" << stmt.getKind() << "(" << (void*)&stmt   \
                                   << ")",                                                         \
               context.debugFlag());                                                               \
        return N<T>::visitReturnStatement(context, stmt);                                          \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitReturnStatement(T& context, SPL::ReturnStatement CONST& stmt)                  \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting stmt" << stmt.getKind() << "(" << (void*)&stmt   \
                                   << ")",                                                         \
               context.debugFlag());                                                               \
        if (stmt.hasExpression()) {                                                                \
            done_ = visit(context, stmt.getExpression());                                          \
        }                                                                                          \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::WhileStatement CONST& stmt)                                  \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting stmt" << stmt.getKind() << "(" << (void*)&stmt   \
                                   << ")",                                                         \
               context.debugFlag());                                                               \
        return N<T>::visitWhileStatement(context, stmt);                                           \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitWhileStatement(T& context, SPL::WhileStatement CONST& stmt)                    \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting stmt" << stmt.getKind() << "(" << (void*)&stmt   \
                                   << ")",                                                         \
               context.debugFlag());                                                               \
        done_ = visit(context, stmt.getExpression());                                              \
        if (!done_) {                                                                              \
            done_ = visit(context, stmt.getStatement());                                           \
        }                                                                                          \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::Expression CONST& expr)                                      \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
                                                                                                   \
        return N<T>::visitExpression(context, expr);                                               \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitExpression(T& context, SPL::Expression CONST& expr)                            \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
                                                                                                   \
        switch (expr.getKind()) {                                                                  \
            case Expression::Attribute:                                                            \
                if (!done_) {                                                                      \
                    done_ = visit(context, expr.as<SPL::AttributeExpression>());                   \
                    break;                                                                         \
                }                                                                                  \
            case Expression::Binary:                                                               \
                if (!done_) {                                                                      \
                    done_ = visit(context, expr.as<SPL::BinaryExpression>());                      \
                    break;                                                                         \
                }                                                                                  \
            case Expression::Call:                                                                 \
                if (!done_) {                                                                      \
                    done_ = visit(context, expr.as<SPL::CallExpression>());                        \
                    break;                                                                         \
                }                                                                                  \
            case Expression::Cast:                                                                 \
                if (!done_) {                                                                      \
                    done_ = visit(context, expr.as<SPL::CastExpression>());                        \
                    break;                                                                         \
                }                                                                                  \
            case Expression::Conditional:                                                          \
                if (!done_) {                                                                      \
                    done_ = visit(context, expr.as<SPL::ConditionalExpression>());                 \
                    break;                                                                         \
                }                                                                                  \
            case Expression::Enum:                                                                 \
                if (!done_) {                                                                      \
                    done_ = visit(context, expr.as<SPL::EnumExpression>());                        \
                    break;                                                                         \
                }                                                                                  \
            case Expression::Lit:                                                                  \
                if (!done_) {                                                                      \
                    done_ = visit(context, expr.as<SPL::LiteralExpression>());                     \
                    break;                                                                         \
                }                                                                                  \
            case Expression::LiteralSymbol:                                                        \
                if (!done_) {                                                                      \
                    done_ = visit(context, expr.as<SPL::LiteralSymbolExpression>());               \
                    break;                                                                         \
                }                                                                                  \
            case Expression::Prefix:                                                               \
                if (!done_) {                                                                      \
                    done_ = visit(context, expr.as<SPL::PrefixExpression>());                      \
                    break;                                                                         \
                }                                                                                  \
            case Expression::Postfix:                                                              \
                if (!done_) {                                                                      \
                    done_ = visit(context, expr.as<SPL::PostfixExpression>());                     \
                    break;                                                                         \
                }                                                                                  \
            case Expression::StreamHistorySymbol:                                                  \
                if (!done_) {                                                                      \
                    done_ = visit(context, expr.as<SPL::StreamHistorySymbolExpression>());         \
                    break;                                                                         \
                }                                                                                  \
            case Expression::StreamSymbol:                                                         \
                if (!done_) {                                                                      \
                    done_ = visit(context, expr.as<SPL::StreamSymbolExpression>());                \
                    break;                                                                         \
                }                                                                                  \
            case Expression::Subscript:                                                            \
                if (!done_) {                                                                      \
                    done_ = visit(context, expr.as<SPL::SubscriptExpression>());                   \
                    break;                                                                         \
                }                                                                                  \
            case Expression::Symbol:                                                               \
                if (!done_) {                                                                      \
                    done_ = visit(context, expr.as<SPL::SymbolExpression>());                      \
                    break;                                                                         \
                }                                                                                  \
            case Expression::CustomLiteral:                                                        \
                if (!done_) {                                                                      \
                    done_ = visit(context, expr.as<SPL::CustomLiteralExpression>());               \
                    break;                                                                         \
                }                                                                                  \
            case Expression::Unary:                                                                \
                if (!done_) {                                                                      \
                    done_ = visit(context, expr.as<SPL::UnaryExpression>());                       \
                    break;                                                                         \
                }                                                                                  \
            case Expression::Nary:                                                                 \
                if (!done_) {                                                                      \
                    done_ = visit(context, expr.as<SPL::NaryExpression>());                        \
                    break;                                                                         \
                }                                                                                  \
            case Expression::Unwrap:                                                               \
                if (!done_) {                                                                      \
                    done_ = visit(context, expr.as<SPL::UnwrapExpression>());                      \
                    break;                                                                         \
                }                                                                                  \
            case Expression::UnwrapOrElse:                                                         \
                if (!done_) {                                                                      \
                    done_ = visit(context, expr.as<SPL::UnwrapOrElseExpression>());                \
                    break;                                                                         \
                }                                                                                  \
            case Expression::IsPresent:                                                            \
                if (!done_) {                                                                      \
                    done_ = visit(context, expr.as<SPL::IsPresentExpression>());                   \
                    break;                                                                         \
                }                                                                                  \
            default:                                                                               \
                assert(!"Missing case in switch in visitExpression");                              \
        }                                                                                          \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::AttributeExpression CONST& expr)                             \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return N<T>::visitAttributeExpression(context, expr);                                      \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitAttributeExpression(T& context, SPL::AttributeExpression CONST& expr)          \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        done_ = visit(context, expr.getLhs());                                                     \
        if (!done_) {                                                                              \
            done_ = visit(context, expr.getRhs());                                                 \
        }                                                                                          \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::BinaryExpression CONST& expr)                                \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return N<T>::visitBinaryExpression(context, expr);                                         \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitBinaryExpression(T& context, SPL::BinaryExpression CONST& expr)                \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        done_ = visit(context, expr.getLhs());                                                     \
        if (!done_) {                                                                              \
            done_ = visit(context, expr.getRhs());                                                 \
        }                                                                                          \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::CallExpression CONST& expr)                                  \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return N<T>::visitCallExpression(context, expr);                                           \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitCallExpression(T& context, SPL::CallExpression CONST& expr)                    \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        std::vector<Expression*> CONST& args = expr.getArgs();                                     \
        for (uint32_t i = 0; i < args.size(); ++i) {                                               \
            if (!done_) {                                                                          \
                done_ = visit(context, *args[i]);                                                  \
            }                                                                                      \
        }                                                                                          \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::CastExpression CONST& expr)                                  \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return N<T>::visitCastExpression(context, expr);                                           \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitCastExpression(T& context, SPL::CastExpression CONST& expr)                    \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return visit(context, expr.getExpn());                                                     \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::ConditionalExpression CONST& expr)                           \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return N<T>::visitConditionalExpression(context, expr);                                    \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitConditionalExpression(T& context, SPL::ConditionalExpression CONST& expr)      \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        done_ = visit(context, expr.getCondition());                                               \
        if (!done_) {                                                                              \
            done_ = visit(context, expr.getLhs());                                                 \
        }                                                                                          \
        if (!done_) {                                                                              \
            done_ = visit(context, expr.getRhs());                                                 \
        }                                                                                          \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::EnumExpression CONST& expr)                                  \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::LiteralExpression CONST& expr)                               \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return visitLiteralExpression(context, expr);                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitLiteralExpression(T& context, SPL::LiteralExpression CONST& expr)              \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return visit(context, expr.getValue());                                                    \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::Literal CONST& lit)                                          \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting literal " << lit.getKind() << "(" << (void*)&lit \
                                   << ")",                                                         \
               context.debugFlag());                                                               \
        return visitLiteral(context, lit);                                                         \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitLiteral(T& context, SPL::Literal CONST& lit)                                   \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting literal " << lit.getKind() << "(" << (void*)&lit \
                                   << ")",                                                         \
               context.debugFlag());                                                               \
        switch (lit.getKind()) {                                                                   \
            case SPL::Literal::Invalid:                                                            \
            case SPL::Literal::Nul:                                                                \
            case SPL::Literal::Primitive:                                                          \
            case SPL::Literal::XML:                                                                \
                break;                                                                             \
            case SPL::Literal::Expn:                                                               \
                visit(context, lit.expression());                                                  \
                break;                                                                             \
            case SPL::Literal::List:                                                               \
            case SPL::Literal::Set: {                                                              \
                for (uint32_t i = 0, n = lit.numElements(); i < n; i++) {                          \
                    visit(context, lit.listSetElement(i));                                         \
                    if (done_)                                                                     \
                        break;                                                                     \
                }                                                                                  \
            } break;                                                                               \
            case SPL::Literal::Map: {                                                              \
                for (uint32_t i = 0, n = lit.numElements(); i < n; i++) {                          \
                    SPL::Literal::MapLiteral const& m = lit.mapElement(i);                         \
                    visit(context, *m._key);                                                       \
                    if (done_)                                                                     \
                        break;                                                                     \
                    visit(context, *m._value);                                                     \
                    if (done_)                                                                     \
                        break;                                                                     \
                }                                                                                  \
            } break;                                                                               \
            case SPL::Literal::Tuple: {                                                            \
                for (uint32_t i = 0, n = lit.numElements(); i < n; i++) {                          \
                    SPL::Literal::TupleLiteral const& t = lit.tupleElement(i);                     \
                    visit(context, *t._value);                                                     \
                    if (done_)                                                                     \
                        break;                                                                     \
                }                                                                                  \
            } break;                                                                               \
        }                                                                                          \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::LiteralSymbolExpression CONST& expr)                         \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::PrefixExpression CONST& expr)                                \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return N<T>::visitPrefixExpression(context, expr);                                         \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitPrefixExpression(T& context, SPL::PrefixExpression CONST& expr)                \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return visit(context, expr.getExpn());                                                     \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::PostfixExpression CONST& expr)                               \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return N<T>::visitPostfixExpression(context, expr);                                        \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitPostfixExpression(T& context, SPL::PostfixExpression CONST& expr)              \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return visit(context, expr.getExpn());                                                     \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::StreamHistorySymbolExpression CONST& expr)                   \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::StreamSymbolExpression CONST& expr)                          \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::SubscriptExpression CONST& expr)                             \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return N<T>::visitSubscriptExpression(context, expr);                                      \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitSubscriptExpression(T& context, SPL::SubscriptExpression CONST& expr)          \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        done_ = visit(context, expr.getLhs());                                                     \
        if (!done_ && expr.lower()) {                                                              \
            done_ = visit(context, *expr.lower());                                                 \
        }                                                                                          \
        if (!done_ && expr.upper()) {                                                              \
            done_ = visit(context, *expr.upper());                                                 \
        }                                                                                          \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::SymbolExpression CONST& expr)                                \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::CustomLiteralExpression CONST& expr)                         \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::UnaryExpression CONST& expr)                                 \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return N<T>::visitUnaryExpression(context, expr);                                          \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitUnaryExpression(T& context, SPL::UnaryExpression CONST& expr)                  \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return visit(context, expr.getExpn());                                                     \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::NaryExpression CONST& expr)                                  \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return N<T>::visitNaryExpression(context, expr);                                           \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visitNaryExpression(T& context, SPL::NaryExpression CONST& expr)                    \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        std::vector<Expression*> CONST& opds = expr.getOpds();                                     \
        for (uint32_t i = 0; i < opds.size(); ++i) {                                               \
            if (!done_) {                                                                          \
                done_ = visit(context, *opds[i]);                                                  \
            }                                                                                      \
        }                                                                                          \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::UnwrapExpression CONST& expr)                                \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return N<T>::visitUnwrapExpression(context, expr);                                         \
    }                                                                                              \
    template<class T>                                                                              \
    bool N<T>::visitUnwrapExpression(T& context, SPL::UnwrapExpression CONST& expr)                \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return visit(context, expr.getLHS());                                                      \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::UnwrapOrElseExpression CONST& expr)                          \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return N<T>::visitUnwrapOrElseExpression(context, expr);                                   \
    }                                                                                              \
    template<class T>                                                                              \
    bool N<T>::visitUnwrapOrElseExpression(T& context, SPL::UnwrapOrElseExpression CONST& expr)    \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        if (!done_) {                                                                              \
            done_ = visit(context, expr.getLHS());                                                 \
        }                                                                                          \
        if (!done_) {                                                                              \
            done_ = visit(context, expr.getRHS());                                                 \
        }                                                                                          \
        return done_;                                                                              \
    }                                                                                              \
                                                                                                   \
    template<class T>                                                                              \
    bool N<T>::visit(T& context, SPL::IsPresentExpression CONST& expr)                             \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return N<T>::visitIsPresentExpression(context, expr);                                      \
    }                                                                                              \
    template<class T>                                                                              \
    bool N<T>::visitIsPresentExpression(T& context, SPL::IsPresentExpression CONST& expr)          \
    {                                                                                              \
        SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "("            \
                                   << (void*)&expr << ")",                                         \
               context.debugFlag());                                                               \
        return visit(context, expr.getLHS());                                                      \
    }

#define CONST const
VISITORMODIFIER(DepthFirstVisitor)
#undef CONST
#define CONST /* */
VISITORMODIFIER(DepthFirstModifier)
#undef CONST
#undef VISITORMODIFIER
} // namespace SPL

#endif // SPL_VISITOR_H
