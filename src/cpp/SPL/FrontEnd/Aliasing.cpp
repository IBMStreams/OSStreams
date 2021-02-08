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

#include <SPL/FrontEnd/Aliasing.h>

#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/SymbolTableEntry.h>
#include <SPL/FrontEnd/TreeHelper.h>

namespace SPL {
Aliasing::Aliasing(AstNode& ast1, AstNode& ast2)
{
    AstNode &origin1 = pathOrigin(ast1), &origin2 = pathOrigin(ast2);
    if (isPathSameSuffix(origin1, origin2)) {
        InverseAst iast1(ast1), iast2(ast2);
        AstNode *suffix1 = NULL, *suffix2 = NULL;
        findLongestAliasedSuffix(origin1, origin2, iast1, iast2, &suffix1, &suffix2);
        assert(NULL != suffix1 && NULL != suffix2);
        if (&ast1 == suffix1) {
            if (&ast2 == suffix2) {
                _value = ALIASED;
            } else {
                _value = PREFIX_OF;
            }
        } else if (&ast2 == suffix2) {
            _value = SUFFIX_OF;
        } else {
            _value = DISJOINT;
        }
    } else {
        _value = DISJOINT;
    }
}

void Aliasing::findLongestAliasedSuffix(AstNode& origin1,
                                        AstNode& origin2,
                                        InverseAst& iast1,
                                        InverseAst& iast2,
                                        AstNode** result1,
                                        AstNode** result2)
{
    *result1 = NULL;
    *result2 = NULL;
    AstNode *p1 = &origin1, *p2 = &origin2;
    AstNode *pp1 = iast1.parent(*p1), *pp2 = iast2.parent(*p2);
    if (NULL != pp1 &&
        isPathSameSuffix(
          *pp1, *p2)) { // move up 'p1' to parent if alias, e.g., history or attribute on stream
        p1 = pp1;
        pp1 = iast1.parent(*p1);
    }
    if (NULL != pp2 &&
        isPathSameSuffix(
          *pp2, *p1)) { // move up 'p2' to parent if alias, e.g., history or attribute on stream
        p2 = pp2;
        pp2 = iast2.parent(*p2);
    }
    if (isPathSameSuffix(*p1, *p2)) {
        while (isPathSuffix(*p1, pp1) && isPathSuffix(*p2, pp2) &&
               isPathSameSuffix(*pp1, *pp2)) { // move up 'p1' and 'p2' while paths same
            p1 = pp1;
            pp1 = iast1.parent(*p1);
            p2 = pp2;
            pp2 = iast2.parent(*p2);
        }
        *result1 = p1;
        *result2 = p2;
    }
}

bool Aliasing::isAccessTo(AstNode& accessedAst, AstNode& otherAst)
{
    Aliasing a(accessedAst, otherAst);
    return DISJOINT != a.value();
}

bool Aliasing::isEntireAccessTo(AstNode& accessedAst, AstNode& otherAst)
{
    Aliasing a(accessedAst, otherAst);
    return PREFIX_OF == a.value() || SUFFIX_OF == a.value();
}

bool Aliasing::isPathSameSuffix(AstNode& e1, AstNode& e2)
{
    if (AST::idT::is(e1)) {
        if (AST::idT::is(e2)) {
            SymbolTableEntry *sym1 = astSymbolPtr(e1), *sym2 = astSymbolPtr(e2);
            if (!sym1 || !sym2) { // Not defined
                return false;
            }
            if (sym1 == sym2) { // same ID is always same
                return true;
            }
            if (symIsStream(*sym1) &&
                symIsStream(*sym2)) { // different ID, but may be aliases for same port
                OpInvokeSym const& oi = SymbolTable::instance().currentOpInvoke();
                bool isInput1 = false, isInput2 = false;
                int portIndex1 = -1, portIndex2 = -1;
                bool found1 =
                  findStreamInOpInvoke(sym1->name(), oi.ast(), NULL, &isInput1, &portIndex1, NULL);
                bool found2 =
                  findStreamInOpInvoke(sym2->name(), oi.ast(), NULL, &isInput2, &portIndex2, NULL);
                return found1 && found2 && isInput1 == isInput2 && portIndex1 == portIndex2;
            }
            return false;
        } else if (AST::attributeExpr::is(
                     e2)) { // simple attribute ID may be in scope in param/window/output clause
            if (astText(e1) == astText(AST::attributeExpr::id(e2))) {
                SymbolTableEntry& sym1 = astSymbol(e1);
                if (sym1.is<IndirectSym>()) {
                    SymbolTableEntry* base1 = sym1.as<IndirectSym>().base();
                    if (NULL != base1 && base1->hasAst()) {
                        AstNode& base1Ast = base1->ast();
                        return AST::opOutput::is(base1Ast) &&
                               isPathSameSuffix(AST::opOutput::id(base1Ast),
                                                AST::attributeExpr::base(e2));
                    }
                }
            }
            return false;
        } else if (AST::subscriptExpr::is(
                     e2)) { // history access is alias for current tuple, ignore the subscript
            AstNode& base2 = AST::subscriptExpr::expr(e2);
            return symIsStream(astSymbol(base2)) && isPathSameSuffix(e1, base2);
        }
    } else if (AST::attributeExpr::is(e1)) {
        if (AST::idT::is(e2)) { // use implementation for checking the symmetric case
            return isPathSameSuffix(e2, e1);
        } else if (AST::attributeExpr::is(e2)) { // same if same attribute
            return astText(AST::attributeExpr::id(e1)) == astText(AST::attributeExpr::id(e2));
        } else if (AST::subscriptExpr::is(e2)) { // attribute access and subscript are never same
            return false;
        }
    } else if (AST::subscriptExpr::is(e1)) {
        if (AST::idT::is(e2)) { // use implementation for checking the symmetric case
            return isPathSameSuffix(e2, e1);
        } else if (AST::attributeExpr::is(
                     e2)) { // use implementation for checking the symmetric case
            return isPathSameSuffix(e2, e1);
        } else if (AST::subscriptExpr::is(
                     e2)) { // always considered the same, ignore the actual subscript
            return true;
        }
    }
    return false;
}

bool Aliasing::isPathSuffix(AstNode& child, AstNode* parent)
{
    if (NULL == parent) {
        return false;
    }
    if (AST::attributeExpr::is(*parent)) {
        return &child == &AST::attributeExpr::base(*parent);
    }
    if (AST::subscriptExpr::is(*parent)) {
        return &child == &AST::subscriptExpr::expr(*parent);
    }
    return false;
}

AstNode& Aliasing::pathOrigin(AstNode& expr)
{
    if (AST::attributeExpr::is(expr)) {
        return pathOrigin(AST::attributeExpr::base(expr));
    }
    if (AST::subscriptExpr::is(expr)) {
        return pathOrigin(AST::subscriptExpr::expr(expr));
    }
    return expr;
}
}
