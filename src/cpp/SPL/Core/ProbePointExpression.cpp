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

#include <SPL/CodeGen/Expression.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/ExpressionEvaluator.h>
#include <SPL/Core/ExpressionValue.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/Core/ProbePointExpression.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/Scope.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/TreeHelper.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Runtime/Common/RuntimeDebugAspect.h>
#include <SPL/Runtime/Type/Meta/BaseType.h>
#include <SPL/Runtime/Type/Meta/Type.h>
#include <SPL/Runtime/Type/Tuple.h>
#include <SPL/Utility/ProcessLauncher.h>
#include <SPL/Utility/SysOut.h>
#include <TRC/DistilleryDebug.h>
#include <UTILS/SupportFunctions.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <ostream>

using namespace SPL;
using namespace std;
namespace bf = boost::filesystem;

// Non-mangled version to allow dynamic library loading
extern "C" BaseProbePointExpression* createProbePoint(
  std::string const& exprAsString,
  Meta::TupleType const& tupleType,
  BaseProbePointExpression::ContextDefinition const& context)
{
    static std::auto_ptr<CompilerApp> compApp(new CompilerApp);
    return new ProbePointExpression(exprAsString, tupleType, context);
}

class BuiltinLoader
{
  public:
    BuiltinLoader();
};

BuiltinLoader::BuiltinLoader()
{
    // Grab the default toolkit.
    string toolKitDir;
    string xsdDir;
    try {
        std::string instDir = Distillery::getInstallationDirectory();
        toolKitDir = instDir + "/toolkits/spl";
        xsdDir = instDir + "/etc/xsd/SPL";
    } catch (const Distillery::DistilleryException& e) {
        SysOut::errorln(SPL_CORE_STREAMS_ERROR(e.getExplanation()));
    }
    vector<std::string> paths;
    paths.push_back(toolKitDir);
    CompilerConfiguration& cf = CompilerConfiguration::instance();
    cf.setStringValue(CompilerConfiguration::ToolkitDir, toolKitDir);
    cf.setStringValue(CompilerConfiguration::XsdDir, xsdDir);
    PathSearch& ps = PathSearch::instance();
    ps.setPaths(paths);
    ps.loadToolkits();
}

ProbePointExpression::ProbePointExpression(string const& exprAsString,
                                           Meta::TupleType const& tupleType,
                                           ContextDefinition const& context)
  : isEvaluatable_(true) // assume that we will be able to evaluate the expression
{
    // We have to ensure our builtins are loaded
    static BuiltinLoader builtinScope;

    APPTRC(L_TRACE, "Before call to createProbeExpression....", SPL_SDB_DBG);

    // the following calls the parser to create the spl expression
    AutoErrorTrap aet(false);
    probeExpn_.reset(createProbeExpression(exprAsString, tupleType, context));

    // check if the above resulted in spl expression
    if (probeExpn_.get() == NULL || aet.getNumErrorsInCurrentTrap() > 0)
        isEvaluatable_ = false;
    else if (probeExpn_->getType().getMetaType() != Meta::Type::BOOLEAN) {
        isEvaluatable_ = false;
        SysOut::errorln(
          SPL_CORE_PROBE_EXPRESSION_NOT_BOOLEAN(exprAsString, probeExpn_->getType().getName()));
    } else if (!ExpressionEvaluator::isEvaluatable(*probeExpn_)) {
        isEvaluatable_ = false;
        SysOut::errorln(SPL_CORE_PROBE_EXPRESSION_NOT_EVALUATABLE(exprAsString));
    } else if (probeExpn_->hasSideEffects()) {
        isEvaluatable_ = false;
        SysOut::errorln(SPL_CORE_PROBE_EXPRESSION_HAS_SIDE_EFFECT(exprAsString));
    }

    APPTRC(L_TRACE, "End of constructor, ProbePointExpression ", SPL_SDB_DBG);

    if (probeExpn_.get())
        APPTRC(L_TRACE, "ProbePointExpression is: " << *probeExpn_.get(), SPL_SDB_DBG);
}

ProbePointExpression::~ProbePointExpression() {}

Expression* ProbePointExpression::createProbeExpression(std::string const& exprAsString,
                                                        Meta::TupleType const& tupleType,
                                                        ContextDefinition const& context)
{
    TupleTyp const& tupleTypeFE = RootTyp::fromRuntimeRepresentation(tupleType).as<TupleTyp>();
    SymbolTable& st = SymbolTable::instance();
    Scope* ctxScope = new Scope(&st.topLevelScope(), NULL);
    addEnumConstantsToScope(*ctxScope, tupleTypeFE);
    for (ContextDefinition::const_iterator i = context.begin(); i != context.end(); i++) {
        ctxScope->def(*new OtherSym(SourceLocation::invalidSourceLocation, i->first, ctxScope, NULL,
                                    &RootTyp::fromRuntimeRepresentation(*i->second)));
    }
    Scope* tupleScope = new Scope(SourceLocation::invalidSourceLocation, tupleTypeFE, ctxScope);
    Scope& oldScope = st.excursionBegin(*tupleScope);
    SPLAst& splAst = SPLAst::expressionFromString("debugger command line", exprAsString);
    analyzeExpressionAst(splAst);
    Expression& result = astExpression(splAst.ast());
    delete &splAst;
    st.excursionEnd(oldScope);
    return &result;
}

bool ProbePointExpression::evaluate(Tuple const& tupleValue, ContextEvaluation const& context)
{
    // we need a probe expression to evaluate
    if (!isEvaluatable_)
        return false;

    APPTRC(L_TRACE, "ProbePointExpression on entry to evaluate: " << *probeExpn_.get(),
           SPL_SDB_DBG);

    ValueTableType valueTable;

    // walk through the tupleValue's attributes and insert them and the
    // value into the valueTable
    ConstTupleIterator itb = tupleValue.getBeginIterator();
    ConstTupleIterator ite = tupleValue.getEndIterator();
    for (ConstTupleIterator it = itb; it != ite; ++it) {
        ConstTupleAttribute attrb = *it;
        std::string const& name = attrb.getName();
        ConstValueHandle value = attrb.getValue();
        // Skip ones that we can't handle and that we don't reference

        Meta::BaseType const& ctype = Meta::BaseType::makeType(value);
        if (ExpressionEvaluator::isEvaluatableType(RootTyp::fromRuntimeRepresentation(ctype)))
            valueTable.insert(make_pair(name, new ExpressionValue(value)));
    }
    // And the context variables
    for (ContextEvaluation::const_iterator i = context.begin(); i != context.end(); i++)
        valueTable.insert(make_pair(i->first, new ExpressionValue(i->second)));

    APPTRC(L_TRACE, "Value table from tuple values: " << valueTable, SPL_SDB_DBG);

    ExpressionEvaluator eval(CompilerConfiguration::instance(), true);
    enum ExpressionEvaluator::EvaluateFailureCode rc;

    bool probeResult = false; // assume failure

    ExpressionValue* evr = eval.evaluate(*probeExpn_.get(), valueTable, rc);
    if (rc == ExpressionEvaluator::Success && *evr == ExpressionValue(boolean(true)))
        probeResult = true;

    APPTRC(L_TRACE, "Result code from evaluate:  " << rc, SPL_SDB_DBG);
    APPTRC(L_TRACE, "Result of probe: " << probeResult, SPL_SDB_DBG);

    delete evr; // delete any storage allocated by evaluate

    return probeResult;
}
