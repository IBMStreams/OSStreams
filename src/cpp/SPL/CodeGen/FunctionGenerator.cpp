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

#include <SPL/CodeGen/FunctionGenerator.h>

#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Literal.h>
#include <SPL/CodeGen/Statement.h>
#include <SPL/CodeGen/TypeGenerator.h>
#include <SPL/Core/Collector.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/CompilerMessage.h>
#include <SPL/Core/EnumerationModelImpl.h>
#include <SPL/Core/ExpressionValue.h>
#include <SPL/Core/TupleModelImpl.h>
#include <SPL/Core/Visitor.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Optimizer/BackwardStoreMotion.h>
#include <SPL/Optimizer/ExpandAssignFrom.h>
#include <SPL/Optimizer/OptimizeMappedOperator.h>
#include <SPL/Runtime/Utility/Singleton.t>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/OptimizeControl.h>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>
#include <UTILS/SupportFunctions.h>

#include <algorithm>
#include <fstream>
#include <iostream>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>

#define HDR_GUARD "SPL_FUNCTION_"

using namespace std;
using namespace std::tr1;
using namespace SPL;

namespace bf = boost::filesystem;

template class Singleton<FunctionGenerator>;

// static bool FunctionHeaderSorter (const FunctionHeadSym *a, const FunctionHeadSym *b)
// {
//     return a->fullName() > b->fullName();
// }

// static bool BaseTypeSorter (const RootTyp *a, const RootTyp *b)
// {
//     return a->getName() > b->getName();
// }

static string genHeader(const vector<string>& ns, const FunctionHeadSym& fcn)
{
    string s;
    TypeGenerator& tg = TypeGenerator::instance();

    if (fcn.isStateful()) {
        s = "/* stateful */ ";
    }

    s += tg.getCppType(fcn.returnType());
    s += " ";
    s += fcn.name();
    s += " (";
    const FunctionHeadSym::Formals& formals = fcn.formals();
    for (uint32_t i = 0; i < formals.size(); i++) {
        if (i > 0) {
            s += ", ";
        }
        const FunctionFormalSym& f = *formals[i];
        if (!f.isMutable()) {
            s += "const ";
        }
        s += tg.getCppType(f.type());
        s += '&';
        if (f.hasName()) {
            s += " ";
            s += Utility::remapId(f.name());
        }
    }
    s += ")";
    return s;
}

struct LitReplacer
{
    LitReplacer(vector<LiteralExpression*>& lits)
      : _lits(lits)
    {}
    std::string getFcnName(LiteralExpression& l);
    Debug::DebugFlag debugFlag() const { return Debug::TraceHistoryVisitor; }

  private:
    vector<LiteralExpression*>& _lits;
};

std::string LitReplacer::getFcnName(LiteralExpression& l)
{
    // Have we seen this literal before?
    for (uint32_t i = 0, iu = _lits.size(); i < iu; i++) {
        LiteralExpression& lit = *_lits[i];
        if (lit.getValue() == l.getValue()) {
            return lit.getReplacementFunctionName();
        }
    }
    ostringstream s;
    s << "_litValue" << _lits.size();
    _lits.push_back(&l);
    return s.str();
}

// Modifier to update literals
namespace SPL {
template<>
bool DepthFirstModifier<LitReplacer>::visit(LitReplacer& context, LiteralExpression& expr)
{
    const Literal& lit = expr.getValue();
    if (!lit.compileTimeEvaluatable()) {
        return false;
    }

    // Ignore simple primitives
    if (lit.getKind() == Literal::Primitive) {
        switch (lit.primitive().type()) {
            case MetaType::USTRING:
            case MetaType::BSTRING:
                // We want to gen these
                break;
            default:
                return false; // keep inline
        }
    }

    // We have something - replace it
    expr.setReplacementFunctionName(context.getFcnName(expr));
    return false;
}
}

static void replaceLiteralsInFunctions(const vector<FunctionInformation::OneFcnInfo>& fv,
                                       vector<LiteralExpression*>& lits)
{
    LitReplacer litReplacerContext(lits);
    for (uint32_t i = 0; i < fv.size(); i++) {
        DepthFirstModifier<LitReplacer> modifier;
        modifier.visit(litReplacerContext, fv[i].body());
    }
}

static void indent(ostream& s, uint32_t depth)
{
    for (uint32_t i = 0; i < depth; i++) {
        s << "    ";
    }
}

static void generateLiteralFunctions(ostringstream& body, vector<LiteralExpression*>& lits)
{
    TypeGenerator& tg = TypeGenerator::instance();
    for (uint32_t i = 0, iu = lits.size(); i < iu; i++) {
        LiteralExpression& l = *lits[i];
        const Literal& lit = l.getValue();
        const std::string& cppType = tg.getCppType(lit.getType());

        body << "static inline " << cppType << "& " << l.getReplacementFunctionName() << "() {\n";
        body << "    static " << cppType << " dummy";
        // Tuples are a problem; g++ thinks this is a function declaration
        if (lit.getType().getMetaType() == MetaType::TUPLE ||
            lit.getType().getMetaType() == MetaType::OPTIONAL) {
            body << " = " << cppType;
        }
        lit.generate(body, true);
        body << ";\n    return dummy;\n}\n\n";
    }
}

static void addSortedIncludes(ostringstream& ostr,
                              const unordered_set<string>& names,
                              const string& pre,
                              const string& post)
{

    // sort to reduce changes
    vector<string> incls;
    unordered_set<string>::const_iterator it;
    for (it = names.begin(); it != names.end(); it++) {
        incls.push_back(*it);
    }
    sort(incls.begin(), incls.end());
    for (vector<string>::const_iterator it2 = incls.begin(); it2 != incls.end(); it2++) {
        ostr << pre << *it2 << post;
    }
}

static void createPossiblyOverLoadedFunction(const bf::path& srcDir,
                                             const bf::path& objDir,
                                             FunctionInformation& f)
{
    // We have something to do!
    const vector<FunctionInformation::OneFcnInfo>& fv = f.getFunctions();
    assert(fv.size() != 0);
    if (indebug(Debug::ShowIR)) {
        ostringstream ss;
        for (uint32_t i = 0; i < fv.size(); i++) {
            fv[i].header().print(ss);
            static_cast<const Statement&>(fv[i].body()).print(ss, 0);
        }
        SysOut::debug(ss.str());
    }

    // We may need create the directory
    // Tokenize the namespace.  We will use it a lot
    string n = fv[0].header().nameSpace();
    vector<string> ns;
    if (!SymbolTableEntry::isDefaultNameSpace(n)) {
        Distillery::tokenize(n, ns, ".", false);
    }

    bf::path s_dir = srcDir;
    bf::path root_dir;
    uint32_t i;

    for (i = 0; i < ns.size(); i++) {
        s_dir /= ns[i];
        root_dir /= ns[i];
    }

    // Compute fixup string for includes
    string myDepth;
    for (i = 0; i < ns.size(); i++) {
        myDepth += "../";
    }

    Utility::createDirectory(s_dir);

    string fcnName = fv[0].header().name();
    f.setRootFileName(root_dir.empty() ? fcnName : root_dir.string() + '/' + fcnName);

    // Create the .h file constents into a temporary string.
    // Compare to any existing matching file
    ostringstream header;
    // Guard the contents
    header << "#ifndef " HDR_GUARD;
    for (i = 0; i < ns.size(); i++) {
        header << ns[i] << "_";
    }
    header << fcnName << "_h\n#define " HDR_GUARD;
    for (i = 0; i < ns.size(); i++) {
        header << ns[i] << "_";
    }
    header << fcnName << "_h\n\n";

    // Includes needed to define the types
    // We need type information for the declarations
    unordered_set<const RootTyp*> declTypes;
    for (uint32_t j = 0; j < fv.size(); j++) {
        Collector::collectTypes(declTypes, fv[j].header());
    }
    f.typeIncludes(declTypes);
    const unordered_set<string>& typeIncludes = f.typeIncludes();

    addSortedIncludes(header, typeIncludes, "#include \"" + myDepth + "../type/", "\"\n");

    // Now the contents - first put in the correct namespaces
    for (i = 0; i < ns.size(); i++) {
        indent(header, i);
        header << "namespace " << ns[i] << " {\n";
    }

    // Finally, the function headers themselves
    for (uint32_t k = 0; k < fv.size(); k++) {
        indent(header, ns.size());
        header << genHeader(ns, fv[k].header()) << ';' << endl;
    }

    // End the namespace scope
    for (i = ns.size(); i != 0; i--) {
        indent(header, i - 1);
        header << "}\n";
    }

    // End the guard
    header << "\n#endif // " HDR_GUARD;
    for (i = 0; i < ns.size(); i++) {
        header << ns[i] << "_";
    }
    header << fcnName << "_h\n";

    // Okay, we have the .h file generated.  Compare
    bf::path h = s_dir / (fcnName + ".h");
    Utility::writeIfNecessary(h, header.str());

    // Okay, now the bodies
    ostringstream body;
    body << "#include \"SPL/Runtime/Function/SPLFunctions.h\"\n";

    // Include the base definition
    body << "#include \"./" << fcnName << ".h\"\n";

    // Includes needed to define the called functions
    unordered_set<const RootTyp*> defnTypes;
    unordered_set<const FunctionHeadSym*> fcnInfo;
    for (uint32_t i2 = 0; i2 < fv.size(); i2++) {
        Collector::collectTypesAndFunctions(&defnTypes, &fcnInfo, fv[i2].body(), false);
    }

    // We can ignore types already used by the decls
    unordered_set<const RootTyp*> newTypes;
    set_difference(defnTypes.begin(), defnTypes.end(), declTypes.begin(), declTypes.end(),
                   inserter(newTypes, newTypes.begin()));

    unordered_set<string> headerTypeIncludes = typeIncludes;
    f.typeIncludes(newTypes);
    unordered_set<string> newTypeIncludes;
    set_difference(typeIncludes.begin(), typeIncludes.end(), headerTypeIncludes.begin(),
                   headerTypeIncludes.end(), inserter(newTypeIncludes, newTypeIncludes.begin()));

    f.functionIncludes(fcnInfo);
    addSortedIncludes(body, newTypeIncludes, "#include \"" + myDepth + "../type/", "\"\n");
    addSortedIncludes(body, f.functionIncludes(), "#include \"" + myDepth + "../function/", "\"\n");
    addSortedIncludes(body, f.nativeFunctionIncludes(), "#include <", ">\n");

    // Now the contents - first put in the correct namespaces
    for (i = 0; i < ns.size(); i++) {
        body << "namespace " << ns[i] << " { ";
    }
    body << '\n';

    // Replace literals in functions with call to static routines
    vector<LiteralExpression*> lits;
    replaceLiteralsInFunctions(fv, lits);

    // Generate the needed routines
    generateLiteralFunctions(body, lits);

    // Finally, the function bodies themselves
    for (uint32_t i3 = 0; i3 < fv.size(); i3++) {
        body << genHeader(ns, fv[i3].header()) << '\n';
        fv[i3].body().Statement::generate(body);
    }

    // End the namespace scope
    for (i = ns.size(); i != 0; i--) {
        body << "} ";
    }
    body << '\n';

    // Okay, we have the .cpp file generated.  Compare with existing
    bf::path cpp = s_dir / (fcnName + ".cpp");
    Utility::writeIfNecessary(cpp, body.str());
}

void FunctionGenerator::generateCPPCode()
{

    SysOut::writeln(SPL_CORE_CREATING_FUNCTIONS, cout);

    UsedSPLFunctions& fcns = FrontEndDriver::instance().getUsedSPLFunctions();
    UsedSPLFunctions::FunctionInfo& map = fcns.getFunctionInformation();
    UsedSPLFunctions::FunctionInfo::iterator it = map.begin();
    if (it == map.end()) {
        return; // Nothing to do
    }

    // Create the function directory to hold the functions
    CompilerConfiguration& config = CompilerConfiguration::instance();
    bf::path cdir = config.getStringValue(CompilerConfiguration::OutputDir);
    bf::path srcDir = cdir / "src" / "function";
    Utility::createDirectory(srcDir);
    bf::path objDir = cdir / "build" / "function";
    Utility::createDirectory(objDir);
    for (; it != map.end(); ++it) {
        createPossiblyOverLoadedFunction(srcDir, objDir, it->second);
    }
}

static void optimizeFunction(FunctionInformation& f)
{

    const vector<FunctionInformation::OneFcnInfo>& fv = f.getFunctions();
    assert(fv.size() != 0);
    if (indebug(Debug::ShowIR)) {
        cout << "IR Before Optimization:" << endl;
        ostringstream ss;
        for (uint32_t i = 0; i < fv.size(); i++) {
            fv[i].header().print(ss);
            fv[i].body().print(ss, 0);
        }
        SysOut::debug(ss.str());
    }

    if (!optimizeControlCheck(OptimizeControl::NoOptimizeMappedOp)) {
        // walk each statement looking for mapped operators
        for (uint32_t i3 = 0; i3 < fv.size(); i3++) {
            Statement* body = &fv[i3].body();
            // NOTE: we won't replace the BlockStatement... only stmts inside it
            OptimizeMappedOperator::instance().optimizeMappedOperatorInStatement(body);
        }
    }

    if (!optimizeControlCheck(OptimizeControl::NoExpandAssignFrom)) {
        // walk each statement looking for assignFrom call
        for (uint32_t i3 = 0; i3 < fv.size(); i3++) {
            Statement* body = &fv[i3].body();
            // NOTE: we won't replace the BlockStatement... only stmts inside it
            ExpandAssignFrom::instance().expandAssignFromInStatement(body);
        }
    }

    if (!optimizeControlCheck(OptimizeControl::NoBackwardStoreMotion)) {
        // walk each statement looking for assignFrom call
        for (uint32_t i3 = 0; i3 < fv.size(); i3++) {
            BlockStatement& body = fv[i3].body();
            // NOTE: we won't replace the BlockStatement... only stmts inside it
            BackwardStoreMotion::instance().backwardStoreMotionInFunction(body);
        }
    }
}

void FunctionGenerator::optimize()
{

    UsedSPLFunctions& fcns = FrontEndDriver::instance().getUsedSPLFunctions();
    UsedSPLFunctions::FunctionInfo& map = fcns.getFunctionInformation();
    UsedSPLFunctions::FunctionInfo::iterator it = map.begin();
    if (it == map.end()) {
        return; // Nothing to do
    }

    // traverse used function and optimize one at a time
    for (; it != map.end(); ++it) {
        optimizeFunction(it->second);
    }
}
