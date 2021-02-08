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

#include <SPL/FrontEnd/SideEffectDetector.h>

#include <SPL/FrontEnd/Aliasing.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeFactory.h>

namespace SPL {

/// Is 'parent' an AST node for an expression that modifies its child at 'indexInParent'?
static bool isPathMutation(AstNode* parent, int indexInParent)
{
    if (NULL == parent) {
        return false;
    } else if (AST::callExpr::is(*parent)) {
        FunctionHeadSym* funSym = astSymbol(*parent).as<CallExpressionSym>().resolvedTgt();
        if (NULL != funSym) {
            FunctionHeadSym::Formals const& formals = funSym->formals();
            int iFormal = indexInParent - 1;
            assert(0 <= iFormal && iFormal < (int)formals.size());
            FunctionFormalSym* formalSym = formals.at(iFormal);
            return NULL != formalSym && formalSym->isMutable();
        }
    } else if (AST::infixExpr::is(*parent)) {
        return &astChild(*parent, indexInParent) == &AST::infixExpr::lhs(*parent) &&
               (AST::ampEqT::is(*parent) || AST::barEqT::is(*parent) || AST::eqT::is(*parent) ||
                AST::hatEqT::is(*parent) || AST::lShiftEqT::is(*parent) ||
                AST::minusEqT::is(*parent) || AST::modEqT::is(*parent) ||
                AST::plusEqT::is(*parent) || AST::rShiftEqT::is(*parent) ||
                AST::slashEqT::is(*parent) || AST::starEqT::is(*parent));
    } else if (AST::postfixExpr::is(*parent)) {
        return true;
    } else if (AST::prefixExpr::is(*parent)) {
        AstNode& op = AST::prefixExpr::prefixOp(*parent);
        return AST::plusPlusT::is(op) || AST::minusMinusT::is(op);
    }
    return false;
}

/// If an expression contains a write, and the written data is accessed anywhere
/// else in the same expression, then print an error message.
void SideEffectDetector::findSideEffects(ParserContext const& pCtx, AstNode& ast)
{
    if (AST::callExpr::is(ast)) {
        // Special case pseudo-functions
        SymbolTableEntry* sym = astSymbolPtr(AST::callExpr::target(ast));
        if (sym && sym->is<FunctionHeadSym>()) {
            FunctionHeadSym& func = sym->as<FunctionHeadSym>();
            if (func.isPseudoFunction()) {
                // Check the arguments independently
                for (int i = 0, n = AST::callExpr::actualCount(ast); i < n; i++) {
                    findSideEffects(pCtx, AST::callExpr::actual(ast, i));
                }
                return;
            }
        }
    }
    std::vector<AstNode*> variables;
    findAllVariables(ast, variables);
    InverseAst iast(ast);
    for (int i1 = 0, n = variables.size(); i1 < n; i1++) {
        for (int i2 = i1 + 1; i2 < n; i2++) {
            AstNode *p1 = NULL, *p2 = NULL;
            Aliasing::findLongestAliasedSuffix(*variables[i1], *variables[i2], iast, iast, &p1,
                                               &p2);
            if (NULL != p1 && NULL != p2) {
                AstNode *pp1 = iast.parent(*p1), *pp2 = iast.parent(*p2);
                if (!Aliasing::isPathSuffix(*p1, pp1) || !Aliasing::isPathSuffix(*p2, pp2)) {
                    // there can only be aliasing if one of the paths stop
                    while (
                      Aliasing::isPathSuffix(*p1, pp1)) { // continue moving up 'p1' if it continues
                        p1 = pp1;
                        pp1 = iast.parent(*p1);
                    }
                    while (
                      Aliasing::isPathSuffix(*p2, pp2)) { // continue moving up 'p2' if it continues
                        p2 = pp2;
                        pp2 = iast.parent(*p2);
                    }
                    if (isPathMutation(
                          pp1, iast.indexInParent(*p1))) { //'p1' modifies data accessed by 'p2'
                        useModError(
                          astText(*variables.at(i1)),
                          SourceLocationFactory::instance().create(pCtx, *variables.at(i1)),
                          astText(*variables.at(i2)),
                          SourceLocationFactory::instance().create(pCtx, *variables.at(i2)));
                    } else if (isPathMutation(pp2, iast.indexInParent(
                                                     *p2))) { //'p2' modifies data accessed by 'p1'
                        useModError(
                          astText(*variables.at(i2)),
                          SourceLocationFactory::instance().create(pCtx, *variables.at(i2)),
                          astText(*variables.at(i1)),
                          SourceLocationFactory::instance().create(pCtx, *variables.at(i1)));
                    }
                }
            }
        }
    }

    std::vector<AstNode*> calls;
    findAllCalls(ast, calls);
    if (2 <= calls.size()) {
        for (int i1 = 0, n = calls.size(); i1 < n; i1++) {
            FunctionHeadSym* tgt1 = astSymbol(*calls.at(i1)).as<CallExpressionSym>().resolvedTgt();
            if (NULL != tgt1 && tgt1->isStateful()) {
                for (int i2 = i1 + 1; i2 < n; i2++) {
                    FunctionHeadSym* tgt2 =
                      astSymbol(*calls.at(i2)).as<CallExpressionSym>().resolvedTgt();
                    if (NULL != tgt2 && tgt2->isStateful()) {
                        multipleStatefulCall(
                          tgt1->name(), tgt2->name(),
                          SourceLocationFactory::instance().create(pCtx, *calls.at(i1)));
                    }
                }
            }
        }
    }
}
}
