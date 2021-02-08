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

#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/OutputSideEffectsChecker.h> // Include this header first
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/TreeHelper.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SysOut.h>

#include <iostream>
using namespace std;

namespace SPL {

void OutputSideEffectsChecker::run(ParserContext& pCtx, AstNode& ast)
{
    // We're not going to visit every node.  Make sure the root node is marked so that the
    // next iteration runs the correct phase
    setAstStage(AstPool::instance(), ast, FrontEndStage::OUTPUT_SIDE_EFFECTS_CHECKER);

    CompilerConfiguration const& config = CompilerConfiguration::instance();
    bool emitError = false;
    if (config.isSet(CompilerConfiguration::OutputSideEffectsBehavior)) {
        const string& value =
          config.getStringValue(CompilerConfiguration::OutputSideEffectsBehavior);
        if (value == "error") {
            emitError = true;
        } else if (value == "ignore") {
            return; // No output, no need to run anything
        }
    }
    OutputSideEffectsChecker checker(pCtx, emitError);
    checker.allChildrenVisit(ast);
}

void OutputSideEffectsChecker::useModError(const std::string& v1,
                                           const SourceLocation& loc1,
                                           const std::string& v2,
                                           const SourceLocation& loc2)
{
    if (_emitError) {
        SysOut::errorln(SPL_FRONT_END_SIDE_EFFECT_IN_OUTPUT_CLAUSE_ERROR(v1), loc1);
        SysOut::detailsErrorln(SPL_FRONT_END_USE_MOD_CONFLICT2(v2), loc2);
        return;
    }
    SysOut::warnln(SPL_FRONT_END_SIDE_EFFECT_IN_OUTPUT_CLAUSE_WARN(v1), loc1);
    SysOut::detailsWarnln(SPL_FRONT_END_USE_MOD_CONFLICT2(v2), loc2);
}

void OutputSideEffectsChecker::multipleStatefulCall(const std::string& fcn1,
                                                    const std::string& fcn2,
                                                    const SourceLocation& loc1)
{
    if (_emitError) {
        SysOut::errorln(SPL_FRONT_END_TWO_STATEFUL_CALLS_IN_OUTPUT_CLAUSE(fcn1, fcn2), loc1);
        return;
    }
    SysOut::warnln(SPL_FRONT_END_WARN_TWO_STATEFUL_CALLS_IN_OUTPUT_CLAUSE(fcn1, fcn2), loc1);
}

void OutputSideEffectsChecker::visitOpInvokeOutput(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceOutputSideEffectsChecker);
    // We only have to check if there are more than 1 output assignments, otherwise this
    // will be caught by regular expression checks
    if (AST::opInvokeOutput::exprCount(ast) > 1) {
        findSideEffects(_pCtx, ast);
    }
}

void OutputSideEffectsChecker::visitOpInvoke(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceOutputSideEffectsChecker);
    SymbolTable& symTab = SymbolTable::instance();
    symTab.pushOld(ast);
    allChildrenVisit(ast);
    symTab.pop(ast);
}

} // namespace SPL
