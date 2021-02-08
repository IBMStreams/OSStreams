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

#include <SPL/FrontEnd/CompositeFormalExpander.h>

#include <SPL/FrontEnd/AstVisitor.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/FrontEndStage.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SysOut.h>

#include <vector>

namespace SPL {

class InputStreamReferenceWalker : public AstVisitor
{
  public:
    InputStreamReferenceWalker(AstNode& ast);
    bool referencesInputStream() const { return _referencesInputStream; }

  protected:
    virtual void visitAttributeExpr(AstNode& ast);

  private:
    bool _referencesInputStream;
};

InputStreamReferenceWalker::InputStreamReferenceWalker(AstNode& ast)
  : AstVisitor(Debug::TraceCompositeExpansion)
  , _referencesInputStream(false)
{
    AstVisitor::visit(ast);
}

void InputStreamReferenceWalker::visitAttributeExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting ast " << (void*)&ast, debugFlag());
    AstNode& base = AST::attributeExpr::base(ast);
    SymbolTableEntry* sym = astSymbolPtr(base);
    if (NULL != sym && sym->is<CompositeInputPortSym>()) {
        SPLDBG(__PRETTY_FUNCTION__ << ": sym is " << (void*)sym, Debug::TraceCompositeExpansion);
        _referencesInputStream = true;
    }
}

static bool isDef(AstNode& parentAst, AstNode& childAst)
{
    assert(AST::idT::is(childAst));
    AstNode& p = parentAst;
    AstNode* c = &childAst;
    return ((AST::attributeDecl::is(p) && c == &AST::attributeDecl::id(p)) ||
            (AST::localDeclItem::is(p) && c == &AST::localDeclItem::id(p)) ||
            (AST::forInit::is(p) && c == &AST::forInit::id(p)) ||
            (AST::attributeAssign::is(p) && c == &AST::attributeAssign::id(p)));
}

// It may be the case that the cloned tree has nodes that need re-irgen, and a parent node
// is marked as DONE.  Walk the tree and ensure that any children needing an irgen visit are
// reachable
void resetAstStage(AstPool& pool, AstNode& ast)
{
    for (int i = 0, size = astSize(ast); i < size; ++i) {
        AstNode& child = astChild(ast, i);
        resetAstStage(pool, child);
    }
    if (astStage(ast) > FrontEndStage::COMPOSITE_FORMAL_EXPANDER) {
        setAstStage(pool, ast, FrontEndStage::COMPOSITE_FORMAL_EXPANDER);
        clearAstExpression(pool, ast);
        // The cloned expression may have associated symbols.  Some need to be kept because they
        // were created in a stage before ir generation
        SymbolTableEntry* symPtr = astSymbolPtr(ast);
        if (NULL != symPtr) {
            if (symPtr->is<ExpressionSym>() || symPtr->is<CallExpressionSym>()) {
                clearAstSymbol(pool, ast);
            }
        }
    }
}

bool expandCompositeFormals(ParserContext& pCtx, AstNode& parentAst)
{
    MODEL("Visit(expandCompositeFormals, enter, " << (void*)&parentAst << ")\n");
    AstPool& pool = AstPool::instance();
    bool changedChild = false;
    if (AST::compilationUnit::is(parentAst)) {
        if (astStage(parentAst) < FrontEndStage::COMPOSITE_FORMAL_EXPANDER) {
            setAstStage(pool, parentAst, FrontEndStage::COMPOSITE_FORMAL_EXPANDER);
        }
        goto exit;
    }
    if (astStage(parentAst) >= FrontEndStage::COMPOSITE_FORMAL_EXPANDER) {
        goto exit;
    }
    setAstStage(pool, parentAst, FrontEndStage::COMPOSITE_FORMAL_EXPANDER);
    for (int iChild = 0, nChildren = astSize(parentAst); iChild < nChildren; iChild++) {
        AstNode& oldChildAst = astChild(parentAst, iChild);
        if (AST::idT::is(oldChildAst)) {
            std::string const formalName = astText(oldChildAst);
            if ('$' == formalName.at(0)) {
                if (isDef(parentAst, oldChildAst)) {
                    SysOut::errorln(SPL_FRONT_END_COMPOSITE_PARAMETER_DEFINING(formalName),
                                    SourceLocationFactory::instance().create(pCtx, oldChildAst));
                } else {
                    CompositeInstanceSym const& instanceSym =
                      SymbolTable::instance().currentCompositeInstance();
                    if (instanceSym.hasActual(formalName)) {

                        AstNode& opActualAst = instanceSym.actual(formalName, oldChildAst);
                        SPLDBG("expandCompositeFormals: astStage of opActualAst is "
                                 << (void*)&opActualAst << astStage(opActualAst).toString(),
                               Debug::TraceCompositeExpansion);
                        AstNode* valueAst = NULL;
                        if (AST::opActualExprs::is(opActualAst)) {
                            if (1 <= AST::opActualExprs::exprCount(
                                       opActualAst)) { // any count != 1 is an error, but that gets
                                                       // reported elsewhere
                                valueAst = &AST::opActualExprs::expr(opActualAst, 0);
                            }
                        } else if (AST::opActualName::is(opActualAst)) {
                            valueAst = &AST::opActualName::name(opActualAst);
                        } else if (AST::opActualType::is(opActualAst)) {
                            valueAst = &AST::opActualType::type(opActualAst);
                        } else {
                            assert(!"should never reach this statement");
                        }
                        assert(NULL != valueAst);
                        if (NULL != valueAst) {
                            AstNode& newChildAst = *valueAst;

                            SPLDBG("expandCompositeFormals: astStage of newChildAst is "
                                     << (void*)&newChildAst << astStage(opActualAst).toString(),
                                   Debug::TraceCompositeExpansion);

                            InputStreamReferenceWalker walker(newChildAst);
                            if (walker.referencesInputStream()) {
                                resetAstStage(pool, newChildAst);
                            }
                            // If the child expression and the parent expression are both
                            // attributeExpr's we need to patch things up
                            if (AST::attributeExpr::is(newChildAst) &&
                                AST::attributeExpr::is(parentAst)) {
                                assert(1 == iChild);
                                AST::attributeExpr::setBase(parentAst,
                                                            AST::attributeExpr::base(newChildAst));
                                AST::attributeExpr::setId(parentAst,
                                                          AST::attributeExpr::id(newChildAst));
                                // resetAstChild(pool, parentAst, 0,
                                // AST::attributeExpr::base(newChildAst)); resetAstChild(pool,
                                // parentAst, 1, AST::attributeExpr::id(newChildAst));
                                resetAstSymbol(pool, parentAst, astSymbol(newChildAst));
                            } else if (AST::attributeExpr::is(parentAst) &&
                                       !AST::idT::is(newChildAst)) {
                                // Mark this subtree in error so no further phases are run
                                setAstStage(pool, parentAst, FrontEndStage::ERROR);
                                SysOut::errorln(
                                  SPL_FRONT_END_OPERATOR_NOT_ATTR_TYPE(formalName),
                                  SourceLocationFactory::instance().create(pCtx, oldChildAst));
                            } else {
                                resetAstChild(pool, parentAst, iChild, newChildAst);
                                // See defect 13694:  When running the relativeNameUseFinder it does
                                // name lookup when it sees the opActualName node.  Here we have
                                // replaced the ID child of the opActualName and we can't do name
                                // lookup because we don't have the scope available.  The ID has
                                // already been through all the phases so mark the opActualName the
                                // same.
                                if (AST::opActualName::is(parentAst) && AST::idT::is(newChildAst)) {
                                    setAstStage(pool, parentAst, astStage(newChildAst));
                                }
                            }
                            changedChild = true;
                        }
                    }
                }
            }
        } else {
            bool changedGrandChild = expandCompositeFormals(pCtx, oldChildAst);
            if (astStage(oldChildAst) == FrontEndStage::ERROR) {
                // If there was an error, propagate it back
                setAstStage(pool, parentAst, FrontEndStage::ERROR);
                return false;
            }
            if (changedGrandChild && AST::opActualName::is(oldChildAst)) {
                AstNode& grandChildAst = AST::opActualName::name(oldChildAst);
                if (!AST::idT::is(grandChildAst)) {
                    AstNode* newChildAst = NULL;
                    if (AST::typeName::is(grandChildAst) || AST::primitiveType::is(grandChildAst) ||
                        AST::rstringType::is(grandChildAst) || AST::listType::is(grandChildAst) ||
                        AST::mapType::is(grandChildAst) || AST::setType::is(grandChildAst) ||
                        AST::tupleType::is(grandChildAst)) {
                        newChildAst = &AST::opActualType::create(grandChildAst);
                    } else {
                        std::vector<AstNode*> v;
                        v.push_back(&grandChildAst);
                        newChildAst = &AST::opActualExprs::create(v);
                    }
                    resetAstChild(pool, parentAst, iChild, *newChildAst);
                    changedChild = true;
                }
            }
            if (changedGrandChild && AST::typeName::is(oldChildAst)) {
                AstNode& grandChildAst = AST::typeName::contents(oldChildAst);
                if (!AST::idT::is(grandChildAst) && !AST::attributeExpr::is(grandChildAst)) {
                    resetAstChild(pool, parentAst, iChild, grandChildAst);
                }
            }
            if (changedGrandChild && AST::opInvokeOutput::is(parentAst)) {
                AstNode& grandChildAst = AST::infixExpr::lhs(oldChildAst);
                if (AST::attributeExpr::is(grandChildAst)) {
                    resetAstChild(pool, oldChildAst, 0, AST::attributeExpr::id(grandChildAst));
                }
            }
        }
    }
    if (AST::typeName::is(parentAst) && changedChild) {
        copyAstSymbol(pool, parentAst, AST::typeName::contents(parentAst));
    }

exit:
    MODEL("Visit(expandCompositeFormals, exit, " << (void*)&parentAst << ")\n");
    return changedChild;
}
}; /*namespace SPL*/
