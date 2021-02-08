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

#ifndef SPL_DEBUG_H
#define SPL_DEBUG_H

#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Utility/SysOut.h>
#include <UTILS/HashMapHelpers.h>

#include <sstream>
#include <string>

#define SPLDBG(text, aspect)                                                                       \
    do {                                                                                           \
        if (indebug(aspect)) {                                                                     \
            std::stringstream ss;                                                                  \
            ss << text;                                                                            \
            SysOut::debug(ss.str());                                                               \
        }                                                                                          \
    } while (0)

#define SPLPRODDBG(text, aspect)                                                                   \
    do {                                                                                           \
        if (prodindebug(aspect)) {                                                                 \
            std::stringstream ss;                                                                  \
            ss << text;                                                                            \
            SysOut::debug(ss.str());                                                               \
        }                                                                                          \
    } while (0)

namespace SPL {

class Debug : public SPL::Singleton<Debug>
{
  public:
    Debug();

    /// Enum DebugFlag
    /// Specify some information to be dumped during the compile
    enum DebugFlag
    {
        COLA,                       /*!< Trace COLA */
        Core,                       /*!< Trace compiler core */
        DumpTransportStatData,      /*!< Dump transport stats data */
        ExpressionTreeVisitor,      /*!< Show the nodes visited by the expression tree visitor */
        ForceTransportStatLoad,     /*!< Always load transport stats file */
        IRGen,                      /*!< Dump info on IR generation */
        IRValidation,               /*!< Dump info on IR validations */
        NoBackwardStoreMotion,      /*!< don't move stores out of loops */
        NoExpandAssignFrom,         /*!< don't expand assignFrom function call */
        NoOptimizeFunction,         /*!< don't optimize SPL functions */
        NoOptimizeImmutableStreams, /*!< don't optimize immutable streams */
        NoOptimizeMappedOp,         /*!< don't optimize mapped operators */
        NoOptimizeStatement,        /*!< don't optimize SPL statements */
        NoSplitMappedOp, /*!< don't split mapped op expressions into multiple SPL statements */
        PhsInfo,         /*!< Show the elapsed and total time for phases */
        ReadSampleTransportStat,      /*!< Read file SPL/stat/transportStatModelSample.xml */
        ShowAST,                      /*!< Dump the AST nodes */
        ShowExpressionTypeFinderTree, /*!< Dump the AST after the ExpressionTypeFinder has run */
        ShowFunctionPrototypeAst,     /*!< Dump the AST nodes for function prototypes */
        ShowIR,                       /*!< Dump the IR on input to the validator */
        ShowIRFinal,                  /*!< Dump the IR on output from the validator */
        ShowNameDefTree,              /*!< Dump the AST after the NameDefFinder has run */
        ShowOperatorGraph,            /*!< Dump the operator graph before COLA */
        ShowParseTree,                /*!< Dump the AST from the ANTLR parser just after parsing */
        ShowPEGraph,                  /*!< Dump the PE graph after COLA */
        ShowScopes,                   /*!< Dump all scope operations */
        ShowSig,                      /*!< Dump the code gen signature as it is being generated */
        ShowStreams,                  /*!< Dump the StreamsUseDef table */
        ShowSymbols,                  /*!< Dump info from the symbol table */
        ShowSynthesizedAssignments,  /*!< List which output attribute assignments are synthesized */
        ShowTypeAnalyzerTree,        /*!< Dump the AST after the type analyzer has run */
        ShowTypePromoterTree,        /*!< Dump the AST after the type promoter has run */
        TraceAstStageChanges,        /*!< Trace all the changes to node ast stages */
        TraceAttributeVisitor,       /*!< Show the nodes visited by the attribute visitor */
        Traceback,                   /*!< dump traceback on fault signals */
        TraceBundling,               /*!< Trace aspects of building an app bundle */
        TraceCatchAnnotationChecker, /*!< Trace catch annotation checking */
        TraceCGDepCheck,             /*!< Trace checking of the code generator dependency files */
        TraceCollection,           /*!< Show the nodes visited by the type and function collector */
        TraceCompositeExpansion,   /*!< Trace composite expansion */
        TraceConsistentCut,        /*!< Trace consistent cut processing */
        TraceCustomOutputFunction, /*!< Trace custom output function visitor */
        TraceDiagnostician,        /*!< Show the nodes visited by the diagnostician */
        TraceEmptyContainerTypeFinder, /*!< Trace the EmptyContainerTypeFinder walker */
        TraceExpressionTypeFinder,     /*!< Trace the ExpressionTypeFinder walker */
        TraceFrontEnd,                 /*!< Trace aspects of the execution of the FE */
        TraceFunctionGenerator,        /*!< Show info from generating functions */
        TraceFunctionCallResolution,   /*!< Trace the resolving of function calls */
        TraceHistoryVisitor,           /*!< Show the nodes visited by the history visitor */
        TraceIRGeneration,             /*!< Trace the IRGenerator walker */
        TraceMakefileGenerator,        /*!< Show info from makefile generation */
        TraceNameDefFinder,            /*!< Trace the NameDefFinder walker */
        TraceNameLookup,               /*!< Trace name lookup in the FE */
        TraceNameUseFinder,            /*!< Trace the NameUseFinder walker */
        TraceOperatorGenerator,        /*!< Show info from operator generation */
        TraceOperatorVerify,           /*!< Show info from operator verification */
        TraceOptimizer,                /*!< trace progress of optimizer */
        TraceOutputSideEffectsChecker, /*!< Trace the OutputSideEffectsChecker walker */
        TracePEGenerator,              /*!< Show info from PE generation */
        TracePortScopeVisitor,         /*!< Show the nodes visited by the port scope visitor */
        TracePromotionChecker,
        TraceRelativeNameUseFinder, /*!< Trace the RelativeNameUseFinder walker */
        TraceScopeExpander,         /*!< Trace scope expander walker */
        TraceScopeStack,            /*!< Trace scope stack activity */
        TraceStandaloneGenerator,   /*!< Show info from Standalone generation */
        TraceSubmitLogicVisitor,    /*!< Show the nodes visited when looking for submit in a logic
                                       clause */
        TraceSymbolCreation,        /*!< Trace the creation of symbols */
        TraceTranslation,           /*!< Trace translation of SPADE to SPL */
        TraceTransportStatRead,     /*!< Trace reading of transport stats data */
        TraceTupleMutationVisitor,  /*!< Show the nodes visited when looking for tuple mutation */
        TraceTypeAnalyzer,          /*!< Trace the type analyzer */
        TraceTypeGenerator,         /*!< Show info from type generator */
        TraceTypePromoter,          /*!< Show info from type promoter */
        TraceTypeUnification,       /*!< Trace actions in the type unifier */
        TraceVariableUseChecker,    /*!< Trace the VariableUseChecker */

        None, /*!< A default that does no debugging */
        NumDebugFlags
        //*** NOTE:  IF YOU ADD AN ENUMERATOR TO THIS LIST YOU MUST ALSO UPDATE Debug.cpp *******
    };

    bool indebugCheck(DebugFlag flag) const { return (flag == None) ? false : _flags[flag]; }
    void addDebugString(std::string const& str);

  private:
    void addString(std::string const& str);
    bool _flags[NumDebugFlags];
};

/// Checks if the specified flag is enabled for output based on the setting of the SPL_DEBUG
/// environment variable.
#ifndef NDEBUG
inline bool indebug(Debug::DebugFlag flag)
{
    return Debug::instance().indebugCheck(flag);
}
#else
inline bool indebug(Debug::DebugFlag flag)
{
    return false;
}
#endif
inline bool prodindebug(Debug::DebugFlag flag)
{
    return Debug::instance().indebugCheck(flag);
}

} // namespace SPL

#endif // SPL_DEBUG_H
