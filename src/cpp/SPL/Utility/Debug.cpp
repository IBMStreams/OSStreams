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

#include <SPL/Utility/Debug.h> // Include this header first

#include <cassert>
#include <iostream>

#include <SPL/Runtime/Utility/Singleton.t>

using namespace SPL;
using namespace std;
using namespace std::tr1;

template class Singleton<Debug>;
// Explicitly instatiate the singleton code here

Debug debug;

Debug::Debug()
  : Singleton<Debug>(*this)
{
    const char* debugString = getenv("STREAMS_SPL_DUMP");
    if (debugString) {
        addDebugString(debugString);
    }
}

void Debug::addDebugString(string const& str)
{
    size_t start = 0, end = str.find(",", start);
    while (end != string::npos) {
        addString(str.substr(start, end - start));
        start = end + 1;
        end = str.find(",", start);
    }
    addString(str.substr(start, end));
}

#define DEBUGCASE(f)                                                                               \
    if (str == #f) {                                                                               \
        _flags[Debug::f] = true;                                                                   \
        return;                                                                                    \
    }

void Debug::addString(string const& str)
{
    DEBUGCASE(COLA)
    DEBUGCASE(Core)
    DEBUGCASE(DumpTransportStatData)
    DEBUGCASE(ExpressionTreeVisitor)
    DEBUGCASE(ForceTransportStatLoad)
    DEBUGCASE(IRGen)
    DEBUGCASE(IRValidation)
    DEBUGCASE(NoBackwardStoreMotion)
    DEBUGCASE(NoExpandAssignFrom)
    DEBUGCASE(NoOptimizeFunction)
    DEBUGCASE(NoOptimizeImmutableStreams)
    DEBUGCASE(NoOptimizeMappedOp)
    DEBUGCASE(NoOptimizeStatement)
    DEBUGCASE(NoSplitMappedOp)
    DEBUGCASE(PhsInfo)
    DEBUGCASE(ReadSampleTransportStat)
    DEBUGCASE(ShowAST)
    DEBUGCASE(ShowExpressionTypeFinderTree)
    DEBUGCASE(ShowFunctionPrototypeAst)
    DEBUGCASE(ShowIR)
    DEBUGCASE(ShowIRFinal)
    DEBUGCASE(ShowNameDefTree)
    DEBUGCASE(ShowOperatorGraph)
    DEBUGCASE(ShowParseTree)
    DEBUGCASE(ShowPEGraph)
    DEBUGCASE(ShowScopes)
    DEBUGCASE(ShowSig)
    DEBUGCASE(ShowStreams)
    DEBUGCASE(ShowSymbols)
    DEBUGCASE(ShowSynthesizedAssignments)
    DEBUGCASE(ShowTypeAnalyzerTree)
    DEBUGCASE(ShowTypePromoterTree)
    DEBUGCASE(TraceAstStageChanges)
    DEBUGCASE(TraceAttributeVisitor)
    DEBUGCASE(Traceback)
    DEBUGCASE(TraceBundling)
    DEBUGCASE(TraceCatchAnnotationChecker)
    DEBUGCASE(TraceCGDepCheck)
    DEBUGCASE(TraceCollection)
    DEBUGCASE(TraceCompositeExpansion)
    DEBUGCASE(TraceConsistentCut)
    DEBUGCASE(TraceCustomOutputFunction)
    DEBUGCASE(TraceDiagnostician)
    DEBUGCASE(TraceEmptyContainerTypeFinder)
    DEBUGCASE(TraceExpressionTypeFinder)
    DEBUGCASE(TraceFrontEnd)
    DEBUGCASE(TraceFunctionGenerator)
    DEBUGCASE(TraceFunctionCallResolution)
    DEBUGCASE(TraceHistoryVisitor)
    DEBUGCASE(TraceIRGeneration)
    DEBUGCASE(TraceMakefileGenerator)
    DEBUGCASE(TraceNameDefFinder)
    DEBUGCASE(TraceNameLookup)
    DEBUGCASE(TraceNameUseFinder)
    DEBUGCASE(TraceOperatorGenerator)
    DEBUGCASE(TraceOperatorVerify)
    DEBUGCASE(TraceOptimizer)
    DEBUGCASE(TraceOutputSideEffectsChecker)
    DEBUGCASE(TracePEGenerator)
    DEBUGCASE(TracePortScopeVisitor)
    DEBUGCASE(TracePromotionChecker)
    DEBUGCASE(TraceRelativeNameUseFinder)
    DEBUGCASE(TraceScopeExpander)
    DEBUGCASE(TraceScopeStack)
    DEBUGCASE(TraceStandaloneGenerator)
    DEBUGCASE(TraceSubmitLogicVisitor)
    DEBUGCASE(TraceSymbolCreation)
    DEBUGCASE(TraceTranslation)
    DEBUGCASE(TraceTransportStatRead)
    DEBUGCASE(TraceTupleMutationVisitor)
    DEBUGCASE(TraceTypeAnalyzer)
    DEBUGCASE(TraceTypeGenerator)
    DEBUGCASE(TraceTypePromoter)
    DEBUGCASE(TraceTypeUnification)
    DEBUGCASE(TraceVariableUseChecker)

    DEBUGCASE(None)

    // We we get here, we don't know about the flag - complain
    cout << "Warning - ignoring spl-dump flag '" << str << '\'' << endl;
}
