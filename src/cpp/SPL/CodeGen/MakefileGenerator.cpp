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

#include <SPL/CodeGen/CompositeDefinition.h>
#include <SPL/CodeGen/MakefileGenerator.h>
#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/CodeGen/SPLAnnotation.h>
#include <SPL/CodeGen/TypeGenerator.h>
#include <SPL/Core/Collector.h>
#include <SPL/Core/CompilationState.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/CompilerMessage.h>
#include <SPL/Core/EnumerationModelImpl.h>
#include <SPL/Core/OperatorGraph.h>
#include <SPL/Core/OperatorGraphNode.h>
#include <SPL/Core/OperatorModelImpl.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/Core/TupleModelImpl.h>
#include <SPL/Core/XMLModelImpl.h>
#include <SPL/Core/bundleInfoModel_v4000.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/UsedOperatorInvocations.h>
#include <SPL/FrontEnd/UsedSPLFunctions.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/ProcessLauncher.h>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>
#include <SPL/include/SPL/Runtime/Type/String.h>
#include <UTILS/Product.h>
#include <UTILS/RegEx.h>
#include <UTILS/SupportFunctions.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>

#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace SPL;
using namespace Enumeration_;
using namespace Tuple_;
using namespace std;
using namespace std::tr1;
using namespace Distillery;

using namespace xmlns::prod::streams::spl::bundleInfoModel;
namespace bf = boost::filesystem;

#define CPP_FLAG_FILE "cppOptions"
#define LD_FLAG_FILE "ldOptions"

MakefileGenerator::MakefileGenerator(std::string adlFileName)
  : _config(CompilerConfiguration::instance())
  , _types(*this)
  , _functions(*this)
  , _operators(*this)
  , _pes(*this, _config)
  , _adlFileName(adlFileName)
  , _bundleFile(bf::path(adlFileName).stem().string() + ".sab")
{
    // figure out the mode of the output directory to be used for mkdirs
    struct stat appStat;
    if (stat(CompilerConfiguration::instance()
               .getStringValue(CompilerConfiguration::OutputDir)
               .c_str(),
             &appStat) == 0) {
        stringstream s;
        s << "-m " << oct << (appStat.st_mode & 07777);
        _directoryPermissionMode = s.str();
    }
}

static string escapePath(const string& path)
{
    string result;
    // If a path being passed to the shell has a $ sign it needs to be escaped
    for (size_t i = 0, size = path.size(); i < size; ++i) {
        char c = path[i];
        if (c == '$') {
            result.push_back(c);
        }
        result.push_back(c);
    }
    return result;
}

static void removeExtraNewLine(string& text)
{
    string::size_type p = text.rfind("\\\n");
    if (p != string::npos) {
        text.erase(p);
    }
}

static void createDirectoryTargets(unordered_set<string> const& dirSet,
                                   const string& directoryPermissionMode,
                                   ofstream& mf)
{
    for (unordered_set<string>::const_iterator it = dirSet.begin(); it != dirSet.end(); ++it) {
        mf << *it << ":\n"
           << "\t-@mkdir " << directoryPermissionMode << " -p " << *it << "\n\n";
    }
}

static string operatorPath(const OperatorGraphNode& op)
{
    string path;
    const string className = op.getModel().getContext().getClass();

    // className has the form [::]*.name.  Each '::' delimited segment except
    // the last forms a directory
    size_t start = 0, end = className.find("::", 0);
    while (end != string::npos) {
        path += className.substr(start, end - start) + '/';
        start = end + 2;
        end = className.find("::", start);
    }
    path += className.substr(start);
    return path;
}

static bool uniqueName(unordered_set<string>& dups, const string& str)
{
    if (dups.find(str) == dups.end()) {
        dups.insert(str);
        return true;
    }
    return false;
}

static const string& typeNameFromType(const TypeGenerator& typeGenerator, const RootTyp& bt)
{
    // TODO: rework this to use a type interface to get the file name
    if (bt.is<TupleTyp>()) {
        const TupleTyp& tt = bt.as<TupleTyp>();
        const Tuple_::TupleModel& tm = typeGenerator.getTupleModel(tt);
        return tm.getClass();
    }

    if (bt.is<XMLTyp>()) {
        const XMLTyp& xt = bt.as<XMLTyp>();
        const XML_::XMLModel& xm = typeGenerator.getXMLModel(xt);
        return xm.getClass();
    }
    assert(bt.is<EnumTyp>());
    const EnumTyp& et = bt.as<EnumTyp>();
    const Enumeration_::EnumerationModel& em = typeGenerator.getEnumModel(et);
    return em.getClass();
}

string MakefileGenerator::shortenName(string const& name) const
{
    if (_config.getBooleanValue(CompilerConfiguration::Verbose)) {
        return name;
    }
    return Utility::shorten(name, 80);
}

MakefileGenerator::Types::Types(const MakefileGenerator& gen)
  : _typeGenerator(TypeGenerator::instance())
  , _gen(gen)
{}

void MakefileGenerator::Types::collect(const string& lineSeperator)
{
    string typeObjSpaces = lineSeperator + string(12, ' ');
    const FrontEndDriver::TypeMap& tmap = FrontEndDriver::instance().getUsedTypes();
    FrontEndDriver::TypeMap::const_iterator it;
    for (it = tmap.begin(); it != tmap.end(); it++) {
        const RootTyp& bt = *it->second;
        if (bt.is<TupleTyp>()) {
            const TupleTyp& tt = bt.as<TupleTyp>();
            const Tuple_::TupleModel& tm = _typeGenerator.getTupleModel(tt);
            _typeObjs += "build/type/" + tm.getClass() + ".o " + typeObjSpaces;
            _tuples.push_back(&tm);
        } else if (bt.is<XMLTyp>()) {
            const XMLTyp& xt = bt.as<XMLTyp>();
            const XML_::XMLModel& xm = _typeGenerator.getXMLModel(xt);
            _typeObjs += "build/type/" + xm.getClass() + ".o " + typeObjSpaces;
            _xmls.push_back(&xm);
        } else {
            assert(bt.is<EnumTyp>());
            const EnumTyp& et = bt.as<EnumTyp>();
            const Enumeration_::EnumerationModel& em = _typeGenerator.getEnumModel(et);
            _typeObjs += "build/type/" + em.getClass() + ".o " + typeObjSpaces;
            _enums.push_back(&em);
        }
    }
    removeExtraNewLine(_typeObjs);
}

void MakefileGenerator::Types::genTypeRule(ofstream& mf, const string& name, const string& niceName)
{
    mf << "build/type/" << name << ".o: \\\n\t    src/type/" << name << ".h \\\n\t    src/type/"
       << name
       << ".cpp \\\n\t    build/" CPP_FLAG_FILE " "
          "\\\n\t    | build/type\n";
    mf << "\t@echo ' [CXX-type] " << niceName << "'\n";
    mf << "\t@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/type/" << name << ".cpp\n\n";
}

void MakefileGenerator::Types::createRules(ofstream& mf)
{
    // Create the Appropriate make lines for types:
    mf << "types: $(SPL_TYPES)\n\n";

    // Now for each one, create the dependencies:
    string name;
    for (uint32_t i = 0; i < _enums.size(); i++) {
        const Enumeration_::EnumerationModel& em = *_enums[i];
        genTypeRule(mf, em.getClass(), _gen.shortenName(_typeGenerator.getEnumType(em).getName()));
    }
    for (uint32_t i = 0; i < _tuples.size(); i++) {
        const Tuple_::TupleModel& tm = *_tuples[i];
        genTypeRule(mf, tm.getClass(), _gen.shortenName(_typeGenerator.getTupleType(tm).getName()));
    }
    for (uint32_t i = 0; i < _xmls.size(); i++) {
        const XML_::XMLModel& xm = *_xmls[i];
        genTypeRule(mf, xm.getClass(), _gen.shortenName(_typeGenerator.getXMLType(xm).getName()));
    }

    // Add a target to make the type directory.
    mf << "build/type:\n\t-@mkdir " << _gen.directoryPermissionMode() << " build/type\n\n";
}

MakefileGenerator::Functions::Functions(const MakefileGenerator& gen)
  : _splFunctions(FrontEndDriver::instance().getUsedSPLFunctions())
  , _gen(gen)
{}

void MakefileGenerator::Functions::collect(const string& lineSeperator)
{
    string fcnObjSpaces = lineSeperator + string(16, ' ');
    const UsedSPLFunctions::FunctionInfo& fInfo = _splFunctions.getFunctionInformation();
    UsedSPLFunctions::FunctionInfo::const_iterator it;
    for (it = fInfo.begin(); it != fInfo.end(); it++) {
        const FunctionInformation& f = it->second;
        _fcnObjs += "build/function/" + f.getRootFileName() + ".o " + fcnObjSpaces;

        unordered_set<const RootTyp*> types;
        unordered_set<const FunctionHeadSym*> functions;
        const vector<FunctionInformation::OneFcnInfo>& fcns = f.getFunctions();

        // Loop over any overloaded functions to gather the types they use and functions they call
        for (uint32_t i = 0, ui = fcns.size(); i < ui; i++) {
            const FunctionInformation::OneFcnInfo& of = fcns[i];
            Collector::collectTypes(types, of.header());
            Collector::collectTypesAndFunctions(&types, &functions, of.body(), false);
            for (unordered_set<const FunctionHeadSym*>::const_iterator it1 = functions.begin();
                 it1 != functions.end(); ++it1) {
            }
        }

        // We have all the types used by all the functions
        _fcnTypes.insert(make_pair(fcns[0].header().fullName(), types));
        _fcnsCalled.insert(make_pair(fcns[0].header().fullName(), functions));
    }
    removeExtraNewLine(_fcnObjs);
}

void MakefileGenerator::Functions::createRules(ofstream& mf, std::string adlFileName)
{
    const CompilerApp::ProvisionMap& provMap = CompilerApp::getSPLCompiler().provMap();
    mf << "functions: $(SPL_FUNCTIONS)\n\n";

    unordered_set<string> dirSet;
    bf::path appDir =
      CompilerConfiguration::instance().getStringValue(CompilerConfiguration::OutputDir);
    bf::path depDir = bf::path("build") / "dep" / "function";

    // Now for each one, create the dependencies:
    const UsedSPLFunctions::FunctionInfo& fInfo = _splFunctions.getFunctionInformation();
    UsedSPLFunctions::FunctionInfo::const_iterator it;
    for (it = fInfo.begin(); it != fInfo.end(); it++) {
        const FunctionInformation& f = it->second;
        unordered_set<string>::const_iterator it2;

        bf::path fcnDepFile = depDir / (f.getRootFileName() + ".dep");
        Utility::createDirectory(appDir / fcnDepFile.branch_path());

        bf::path dir = "build/function/" + f.getRootFileName();
        string pth = dir.branch_path().string();
        dirSet.insert(pth);
        bf::path depFile = dir;
        depFile.replace_extension("d");
        mf << "-include " << depFile.string() << '\n';

        mf << "build/function/" << f.getRootFileName() << ".o: \\\n\t    src/function/"
           << f.getRootFileName() << ".h \\\n\t    src/function/" << f.getRootFileName()
           << ".cpp\\\n\t    " << fcnDepFile.string();
        for (it2 = f.typeIncludes().begin(); it2 != f.typeIncludes().end(); it2++) {
            mf << " \\\n\t    src/type/" << *it2;
        }
        for (it2 = f.functionIncludes().begin(); it2 != f.functionIncludes().end(); it2++) {
            mf << " \\\n\t    src/function/" << *it2;
        }
        unordered_set<ProvisionedLibrary>::const_iterator it3;
        string fcnDepInfo;
        for (it3 = f.getProvisionedLibs().begin(); it3 != f.getProvisionedLibs().end(); it3++) {
            string s = it3->_name + '/' + it3->_version;
            CompilerApp::ProvisionMap::const_iterator it4 = provMap.find(s);
            if (it4 == provMap.end()) {
                // Missing information
                SysOut::errorln(SPL_CORE_MISSING_PROVISIONED_LIBRARY(it3->_name, it3->_version,
                                                                     it3->_description));
            } else {
                mf << " \\\n\t    `$(PKGCONFIG) --cflags " << it4->second << '`';
                fcnDepInfo += it4->second + ' ';
            }
        }

        mf << " \\\n\t    build/" CPP_FLAG_FILE " \\\n\t    | " << pth << "\n";
        mf << "\t@echo ' [CXX-function] " << _gen.shortenName(it->first) << "'\n";
        mf << "\t@$(CXX) -o $@ -c ";
        // Generate dependencies
        mf << "-MD ";

        // Add any needed include paths
        for (it2 = f.functionIncludePaths().begin(); it2 != f.functionIncludePaths().end(); it2++) {
            mf << "-I " << *it2 << ' ';
            fcnDepInfo += *it2 + ' ';
        }

        mf << "-DSPLJAVAFUNCTION_ADL_FILENAME=\\\"" << adlFileName << "\\\" ";

        mf << "$(SPL_CXXFLAGS) src/function/" << f.getRootFileName() << ".cpp\n\n";
        Utility::writeIfNecessary(appDir / fcnDepFile, fcnDepInfo);
    }

    // Create a target for all the function directories
    createDirectoryTargets(dirSet, _gen.directoryPermissionMode(), mf);
}

void MakefileGenerator::Operators::collect(const string& lineSeperator)
{
    string operObjSpaces = lineSeperator + string(16, ' ');

    const UsedOperatorInvocations::Operators& op_map =
      UsedOperatorInvocations::instance().getOperators();
    if (!op_map.empty()) {
        const OperatorGraph& g = OperatorGraph::instance();
        uint32_t i, n = g.numNodes();

        for (i = 0; i < n; i++) {
            const OperatorGraphNode& op = g.getNode(i);
            if (!op.isMasterNode()) {
                continue;
            }
            _operatorObjs += "build/operator/" + operatorPath(op) + ".o ";
            _operatorObjs += operObjSpaces;
        }
        removeExtraNewLine(_operatorObjs);
    }
}

void MakefileGenerator::Operators::collectOperatorUses(const OperatorGraphNode& node,
                                                       unordered_set<const RootTyp*>& types)
{
    assert(node.getOperatorIR());
    const PrimitiveOperatorInstance& op = *node.getOperatorIR();

    // Collect the information from the operator
    const unordered_set<const RootTyp*>& allTypes = node.collectedTypes();

    // For types, we only care about those NOT in a port Tuple;
    unordered_set<const RootTyp*> portTypes;
    uint32_t i = 0, n = op.numInputPorts();
    for (; i < n; i++) {
        op.getInputPort(i).collectTypeAndFunctions(&portTypes, NULL, false);
    }
    n = op.numOutputPorts();
    for (i = 0; i < n; i++) {
        op.getOutputPort(i).collectTypeAndFunctions(&portTypes, NULL, false);
    }

    // Now figure out the difference
    set_difference(allTypes.begin(), allTypes.end(), portTypes.begin(), portTypes.end(),
                   inserter(types, types.begin()));
}

static void addIncludes(unordered_set<string>& operIncludeSet,
                        string& operIncludes,
                        const Common::Library& l)
{
    assert(l.isManaged()); // Only kind right now
    if (l.isManaged()) {
        const Common::ManagedLibrary& m = l.managed();
        const vector<string>& includePaths = m.getIncludePaths();
        for (uint32_t j = 0; j < includePaths.size(); j++) {
            bf::path incPath = includePaths[j];
            if (!incPath.is_complete()) {
                if (Utility::findNumLevelsUpFromOutputDirToAppDir() != -1) {
                    Utility::makeAppDirRelativePathOutputDirRelative(incPath);
                } else {
                    incPath = bf::system_complete(incPath);
                }
            }
            const string i = incPath.string();
            if (operIncludeSet.insert(i).second) {
                operIncludes += "-I " + i + ' ';
            }
        }
    }
}

void MakefileGenerator::Operators::createRules(ofstream& mf, std::string adlFileName)
{
    mf << "operators: $(SPL_OPERATORS)\n\n";

    unordered_set<string> dirSet;
    bf::path appDir =
      CompilerConfiguration::instance().getStringValue(CompilerConfiguration::OutputDir);
    bf::path depDir = bf::path("build") / "dep" / "operator";

    const UsedOperatorInvocations::Operators& op_map =
      UsedOperatorInvocations::instance().getOperators();
    UsedOperatorInvocations::Operators::const_iterator it;
    const TypeGenerator& typeGenerator = TypeGenerator::instance();
    for (it = op_map.begin(); it != op_map.end(); it++) {
        const OperatorGraph& g = OperatorGraph::instance();
        const PrimitiveOperatorInstance& op = **it;
        if (op.getKind() == "spl.adapter::Import" || op.getKind() == "spl.adapter::Export") {
            continue;
        }

        string operIncludes;
        unordered_set<string> operIncludeSet;

        const OperatorGraphNode& n = g.getNode(op.getNames()[0]);
        if (!n.isMasterNode()) {
            continue; // code only generated for master nodes
        }
        const string opPath = operatorPath(n);

        // For each operator, we will depend on .h files for types NOT used
        // in ports (they are referenced in .h file), and on the functions
        // that we reference
        unordered_set<const RootTyp*> types;
        collectOperatorUses(n, types);
        const unordered_set<const FunctionHeadSym*>& fcns = n.collectedFunctions();

        bf::path operatorDepFile = depDir / (opPath + ".dep");
        Utility::createDirectory(appDir / operatorDepFile.branch_path());

        bf::path dir = "build/operator/" + opPath;
        string pth = dir.branch_path().string();
        dirSet.insert(pth);
        bf::path depFile = dir;
        depFile.replace_extension("d");
        mf << "-include " << depFile.string() << '\n';

        // Create the makefile rule
        mf << "build/operator/" << opPath << ".o: \\\n\t    src/operator/" << opPath
           << ".h \\\n\t    src/operator/" << opPath << ".cpp\\\n\t    " << operatorDepFile.string()
           << ' ';
        // Don't forget types and functions
        unordered_set<const RootTyp*>::const_iterator it1;
        for (it1 = types.begin(); it1 != types.end(); it1++) {
            const string& name = typeNameFromType(typeGenerator, **it1);
            mf << "\\\n\t    src/type/" << name << ".h ";
        }
        unordered_set<const FunctionHeadSym*>::const_iterator it2;
        for (it2 = fcns.begin(); it2 != fcns.end(); it2++) {
            const FunctionHeadSym* fh = *it2;
            if (fh->isNativeFunction()) {
                const vector<Function::LibraryPtr>* libs = fh->dependencies();
                if (libs) {
                    vector<Function::LibraryPtr>::const_iterator it3;
                    for (it3 = libs->begin(); it3 != libs->end(); it3++) {
                        addIncludes(operIncludeSet, operIncludes, **it3);
                    }
                }
            } else {
                // SPL function
                mf << "\\\n\t    src/function/";
                string n1 = fh->nameSpace();
                if (!fh->isDefaultNameSpace(n1)) {
                    vector<string> ns;
                    Distillery::tokenize(n1, ns, ".", false);
                    for (uint32_t i = 0; i < ns.size(); i++) {
                        mf << ns[i] << '/';
                    }
                }
                mf << fh->name() << ".h ";
            }
        }
        mf << " \\\n\t    build/" CPP_FLAG_FILE " \\\n\t    | " << pth << "\n";
        mf << "\t@echo ' [CXX-operator] " << _gen.shortenName(n.getName()) << "'\n";
        mf << "\t@$(CXX) -o $@ ";

        // Generate dependencies
        mf << "-MD ";

        // Add include for tookit resources
        string resourcePath = PathSearch::instance().findResourcePath(op.getKind());
        if (!resourcePath.empty()) {
            bf::path p(resourcePath);
            if (!p.is_complete()) {
                if (-1 == Utility::findNumLevelsUpFromOutputDirToAppDir()) {
                    // The output directory is not under the app dir
                    p = bf::system_complete(p);
                } else {
                    // The output directory is under the app dir
                    Utility::makeAppDirRelativePathOutputDirRelative(p);
                }
            }

            p = p / "impl" / "nl" / "include";
            mf << "-I " << p.string() << " ";
        }

        // Add includes from operator library dependencies
        string directory;
        const Operator::OperatorModel* om =
          PathSearch::instance().findOperatorModel(op.getKind(), &directory);
        assert(om);
        const Operator::Context& context = om->getContext();
        if (context.hasLibraryDependencies()) {
            const vector<Operator::LibraryPtr>& libs = context.getLibraryDependencies();
            bf::path dirPath = directory;
            for (uint32_t i = 0, n1 = libs.size(); i < n1; i++) {
                addIncludes(operIncludeSet, operIncludes, *libs[i]);
            }
        }

        mf << operIncludes;

        mf << "-DSPLJAVAFUNCTION_ADL_FILENAME=\\\"" << adlFileName << "\\\" ";

        mf << "-c $(SPL_CXXFLAGS) src/operator/" << opPath << ".cpp\n\n";
        Utility::writeIfNecessary(appDir / operatorDepFile, operIncludes);
    }

    // Create a target for all the function directories
    createDirectoryTargets(dirSet, _gen.directoryPermissionMode(), mf);
}

void MakefileGenerator::PEs::collect(const string& lineSeperator)
{
    string peObjSpaces = lineSeperator + string(14, ' ');
    string& peBinSpaces = peObjSpaces;
    string suffix = ".so";

    const UsedOperatorInvocations::Operators& op_map =
      UsedOperatorInvocations::instance().getOperators();
    if (!op_map.empty()) {
        const OperatorGraph& g = OperatorGraph::instance();
        uint32_t i, n = g.numNodes();
        for (i = 0; i < n; i++) {
            const OperatorGraphNode& op = g.getNode(i);
            if (!op.isMasterNode()) {
                continue;
            }
            _peBins += "bin/" + op.getName() + suffix + " " + peBinSpaces;
        }
    }

    removeExtraNewLine(_peBins);
}

void MakefileGenerator::PEs::grabLinkInformation(const Common::Library& l,
                                                 uint32_t toolkitIndex,
                                                 string& linkCmd,
                                                 unordered_set<string>& dups)
{

    // Add library paths in to link line
    uint32_t j;
    assert(l.isManaged()); // Only kind right now
    if (l.isManaged()) {
        const Common::ManagedLibrary& m = l.managed();
        const vector<string>& libPaths = m.getLibPaths();
        for (j = 0; j < libPaths.size(); j++) {
            bf::path libPath = libPaths[j];
            bf::path clibPath = Utility::normalize(bf::system_complete(libPath));
            const std::vector<const Toolkit*>& toolkits = PathSearch::instance().toolkits();
            const Toolkit& tk = *toolkits[toolkitIndex];
            bf::path tkPath = bf::system_complete(tk.path());
            tkPath = bf::system_complete(tkPath);
            tkPath = Utility::normalize(tkPath);
            int32_t nup; // See if we can create a path relative to this toolkit
            if (Utility::makeRelativePath(clibPath, tkPath, nup)) {
                bf::path tempPath = "\\$$ORIGIN/../toolkits";
                tempPath /= tk.name();
                tempPath /= clibPath;
                clibPath = tempPath;
            }

            if (!libPath.is_complete()) {
                if (Utility::findNumLevelsUpFromOutputDirToAppDir() != -1) {
                    Utility::makeAppDirRelativePathOutputDirRelative(libPath);
                } else {
                    libPath = bf::system_complete(libPath);
                }
            }
            string str =
              "-L " + libPath.string() + " -Wl,-rpath " + "-Wl," + clibPath.string() + ' ';
            if (uniqueName(dups, str)) {
                linkCmd += str;
            }
        }

        // Add libraries in to link line (always do this to handle operator ordering)
        const vector<string>& libs = m.getLibs();
        for (j = 0; j < libs.size(); j++) {
            linkCmd += "-l" + libs[j] + ' ';
        }
    }
}

void MakefileGenerator::PEs::grabLinkInformation(const vector<Operator::LibraryPtr>& libs,
                                                 uint32_t toolkitIndex,
                                                 string& linkCmd,
                                                 unordered_set<string>& dups)
{
    for (uint32_t i = 0, n = libs.size(); i < n; i++) {
        grabLinkInformation(*libs[i], toolkitIndex, linkCmd, dups);
    }
}

static bool fcnHeadSorter(const FunctionHeadSym* a, const FunctionHeadSym* b)
{
    return a->fullName() < b->fullName();
}

void MakefileGenerator::PEs::addCalledFunctions(
  const string& functionName,
  unordered_set<const FunctionHeadSym*>& duplicateFunctions,
  unordered_set<string>& duplicates,
  string& objectDeps,
  string& linkCmd)
{
    const MakefileGenerator::Functions::FunctionSet& calledFunctions =
      _gen.getFunctions().getCalledFunctions(functionName);
    for (MakefileGenerator::Functions::FunctionSet::const_iterator it = calledFunctions.begin(),
                                                                   itEnd = calledFunctions.end();
         it != itEnd; ++it) {
        const FunctionHeadSym& fh = **it;
        if (duplicateFunctions.find(&fh) != duplicateFunctions.end()) {
            continue;
        }
        duplicateFunctions.insert(&fh);
        if (fh.isNativeFunction()) {
            const string& includeFile = fh.includeFileName();
            if (!includeFile.empty() && fh.includeIsBuiltin()) {
                continue;
            }
            const vector<Operator::LibraryPtr>* libs = fh.dependencies();
            if (libs) {
                uint32_t toolkitIndex = fh.getToolkitIndex();
                grabLinkInformation(*libs, toolkitIndex, linkCmd, duplicates);
            }
        } else { // SPL function
            string depLine = "\\\n\t    build/function/";
            string n = fh.nameSpace();
            if (!fh.isDefaultNameSpace(n)) {
                vector<string> ns;
                Distillery::tokenize(n, ns, ".", false);
                for (uint32_t j = 0; j < ns.size(); j++) {
                    depLine += ns[j] + '/';
                }
            }
            depLine += fh.name() + ".o ";
            if (uniqueName(duplicates, depLine)) {
                objectDeps += depLine;
            }
            // Call recursively to see if we have any other functions to add
            addCalledFunctions(fh.fullName(), duplicateFunctions, duplicates, objectDeps, linkCmd);
        }
    }
}

void MakefileGenerator::PEs::createRules(ofstream& mf)
{
    mf << "sos: $(SPL_SO_BINS)\n\n";

    string suffix = ".so";
    const TypeGenerator& typeGenerator = TypeGenerator::instance();
    bf::path appDir = _config.getStringValue(CompilerConfiguration::OutputDir);
    bf::path depDir = appDir / "build" / "dep";
    Utility::createDirectory(depDir);
    const UsedOperatorInvocations::Operators& op_map =
      UsedOperatorInvocations::instance().getOperators();
    if (!op_map.empty()) {
        const OperatorGraph& g = OperatorGraph::instance();
        uint32_t i, n = g.numNodes();
        for (i = 0; i < n; i++) {
            const OperatorGraphNode& op = g.getNode(i);

            // Work with the master node
            const OperatorGraphNode& node = op.getMasterNode();
            const string& peName = op.getName();

            string linkDeps;   // make dependency line
            string objectDeps; // object files depended on
            string linkCmd;    // command to link PE

            string peLdOptionFile = peName + suffix;

            unordered_set<string> duplicates;
            unordered_set<const FunctionHeadSym*> duplicateFunctions;
            linkDeps = "bin/" + peName + suffix + ": \\\n\t$(SO" + peName + ") \\\n\tbuild/dep/" +
                       peLdOptionFile + " \\\n\tbuild/" LD_FLAG_FILE;
            linkCmd = "\t@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/" + peName + suffix + ' ';
            linkCmd += "-fPIC -shared ";
            linkCmd += "$(SO" + peName + ") ";

            const string opPath = operatorPath(node);

            objectDeps += "\\\n\t    build/operator/" + opPath + ".o ";

            // We also have to handle libs and libPaths
            // We want this to be ordered for the link dependencies
            unordered_set<const FunctionHeadSym*> fcns = node.collectedFunctions();
            vector<const FunctionHeadSym*> orderedFcns;
            for (unordered_set<const FunctionHeadSym*>::const_iterator it2 = fcns.begin();
                 it2 != fcns.end(); ++it2) {
                orderedFcns.push_back(*it2);
            }
            sort(orderedFcns.begin(), orderedFcns.end(), fcnHeadSorter);
            for (vector<const FunctionHeadSym*>::const_iterator it2 = orderedFcns.begin();
                 it2 != orderedFcns.end(); ++it2) {
                const FunctionHeadSym& fh = **it2;
                // Have we already seen this?
                if (duplicateFunctions.find(&fh) != duplicateFunctions.end()) {
                    continue;
                }
                duplicateFunctions.insert(&fh);
                if (fh.isNativeFunction()) {
                    const string& includeFile = fh.includeFileName();
                    if (!includeFile.empty() && fh.includeIsBuiltin()) {
                        continue;
                    }
                    const vector<Operator::LibraryPtr>* libs = fh.dependencies();
                    if (libs) {
                        uint32_t toolkitIndex = fh.getToolkitIndex();
                        grabLinkInformation(*libs, toolkitIndex, linkCmd, duplicates);
                    }
                } else {
                    // SPL function
                    string depLine = "\\\n\t    build/function/";
                    string nspace = fh.nameSpace();
                    if (!fh.isDefaultNameSpace(nspace)) {
                        vector<string> ns;
                        Distillery::tokenize(nspace, ns, ".", false);
                        for (uint32_t j = 0; j < ns.size(); j++) {
                            depLine += ns[j] + '/';
                        }
                    }
                    depLine += fh.name() + ".o ";
                    if (uniqueName(duplicates, depLine)) {
                        objectDeps += depLine;
                    }

                    // We need to include any functions directly or indirectly called by this
                    // function (including via all overloaded versions)
                    addCalledFunctions(fh.fullName(), duplicateFunctions, duplicates, objectDeps,
                                       linkCmd);

                    // We also need to include all the types used by the function (including all
                    // overloaded versions)
                    const unordered_set<const RootTyp*>& types =
                      _gen.getFunctions().getTypes(fh.fullName());
                    unordered_set<const RootTyp*>::const_iterator typeIt;
                    for (typeIt = types.begin(); typeIt != types.end(); typeIt++) {
                        const string& name1 = typeNameFromType(typeGenerator, **typeIt);
                        if (uniqueName(duplicates, name1)) {
                            objectDeps += "\\\n\t    build/type/" + name1 + ".o ";
                        }
                    }
                }
            }
            // We also need to have all the types around for linking
            unordered_set<const RootTyp*> const& types = node.collectedTypes();
            unordered_set<const RootTyp*>::const_iterator it1;
            for (it1 = types.begin(); it1 != types.end(); it1++) {
                const string& name1 = typeNameFromType(typeGenerator, **it1);
                if (uniqueName(duplicates, name1)) {
                    objectDeps += "\\\n\t    build/type/" + name1 + ".o ";
                }
            }

            bool isInCCRegion = false;

            // Add libs from operator library dependencies
            const PrimitiveOperatorInstance& ir = *node.getOperatorIR();
            string directory;
            const Operator::OperatorModel* om =
              PathSearch::instance().findOperatorModel(ir.getKind(), &directory);
            assert(om);
            uint32_t toolkitIndex = ir.getToolkitIndex();

            const Operator::Context& context = om->getContext();

            if (context.hasLibraryDependencies()) {
                const vector<Operator::LibraryPtr>& libs = context.getLibraryDependencies();
                for (uint32_t i1 = 0, size = libs.size(); i1 < size; i1++) {
                    const Common::Library& l = *libs[i1];
                    grabLinkInformation(l, toolkitIndex, linkCmd, duplicates);
                }
            }
            if (!isInCCRegion) {
                const vector<const SPLAnnotation*>& annotations = ir.annotations();
                for (vector<const SPLAnnotation*>::const_iterator it2 = annotations.begin();
                     it2 != annotations.end(); ++it2) {
                    const SPLAnnotation& annot = **it2;
                    if (annot.tag() == "consistentRegionEntry") {
                        linkCmd += "-lstreams-stdtk-javaop -lpthread -ldl -lstreams-stdtk-runtime ";
                        break;
                    }
                }
            }

            // Finish off the link line
            linkCmd += "$(SPL_LDFLAGS)";

            // Now the link dependency and cmd
            mf << "SO" << peName << " = " << objectDeps << "\n\n";
            mf << linkDeps << '\n';
            mf << "\t@echo ' [LD-so] so " << peName << "'\n";
            mf << linkCmd << "\n\n";
            Utility::writeIfNecessary(depDir / peLdOptionFile, linkCmd + objectDeps);
        }
    }
}

void MakefileGenerator::collect()
{
    string lineSeperator = "\\\n";
    _types.collect(lineSeperator);
    _functions.collect(lineSeperator);
    _operators.collect(lineSeperator);
    _pes.collect(lineSeperator);
}

void MakefileGenerator::createStandaloneRules(ofstream& mf)
{
    CompositeDefinition const* main = FrontEndDriver::instance().getMainComposite();
    string fname = Utility::replaceDoubleColonsWithDot(main->getKind());
    const OperatorGraph& g = OperatorGraph::instance();
    if (g.numNodes() == 0) {
        mf << "standalone: \n";
        mf << "\t@rm -f bin/standalone.exe\n"
           << "\t@rm -f bin/" << fname << "\n"
           << "\t@rm -f bin/sab\n"
           << "\t@rm -f bin/standalone\n\n";
        return;
    }

    mf << "standalone: bin/standalone.exe\n\n";

    mf << "build/standalone/standalone.o: \\\n"
       << "\t    src/standalone/standalone.h \\\n"
       << "\t    src/standalone/standalone.cpp ";

    mf << " \\\n\t    build/" CPP_FLAG_FILE " \\\n\t    | build/standalone\n";
    mf << "\t@echo ' [CXX-standalone] standalone'\n";
    mf << "\t@$(CXX) -o $@ -c $(SPL_CXXFLAGS) src/standalone/standalone.cpp\n\n";

    // Now the link dependency and cmd
    mf << "bin/standalone.exe: build/standalone/standalone.o build/" LD_FLAG_FILE "\n";
    mf << "\t@echo ' [LD-standalone] standalone'\n";
    mf << "\t@$(CXX_LN) $(SPL_CXXFLAGS) -o bin/standalone.exe build/standalone/standalone.o "
          "$(SO_STAND_LIB)\n";

    // Only create a symlink if the main composte name is not standalone
    // because there is already a script named standalone in that directory
    if (main && main->getKind() != "standalone") {
        mf << "\t@echo ' [LN-standalone] standalone '\n";
        mf << "\t@ln -sf standalone bin/" << fname << "\n";
    }

    // Add a target to make the pe directory
    mf << "build/standalone:\n\t-@mkdir " << _directoryPermissionMode << " build/standalone\n\n";
}

void MakefileGenerator::generate()
{
    if (_config.getBooleanValue(CompilerConfiguration::JustTheAppModel)) {
        return;
    }
    SPLDBG("Generating Build Makefile", Debug::TraceMakefileGenerator);
    bf::path base(_config.getStringValue(CompilerConfiguration::OutputDir));

    // Need somewhere to put the Makefile
    Utility::createDirectory(base / "build");

    // Need somewhere to put the compiled types

    // Create the Makefile
    bf::path makefile = base / "build" / "Makefile";
    std::auto_ptr<ofstream> mf(new ofstream(makefile.string().c_str()));
    if (!*mf) {
        Utility::removeFile(makefile, false);
        mf.reset(new ofstream(makefile.string().c_str()));
        if (!*mf) {
            SysOut::die(
              SPL_CORE_CANNOT_CREATE_FILE(makefile.string(), RuntimeUtility::getErrorNoStr()));
        }
    }

    // Gather all the information
    collect();

    // Boilerplate start
    createHeader(*mf);

    // Types
    _types.createRules(*mf);

    // Operators
    _operators.createRules(*mf, _adlFileName);

    // Functions
    _functions.createRules(*mf, _adlFileName);

    // SOs
    _pes.createRules(*mf);

    // Standalone app
    createStandaloneRules(*mf);

    // The bundle
    createBundleRules(*mf);

    SPLDBG("Completed Building Makefile", Debug::TraceMakefileGenerator);
}

bool isIncluded(const bf::path& path, const vector<string>& includePatterns)
{
    bool isDirectory = is_directory(path);
    for (vector<string>::const_iterator it = includePatterns.begin(), itEnd = includePatterns.end();
         it != itEnd; ++it) {
        const string& pattern = *it;
        SPLDBG("isIncluded: Checking inclusion of path " << path.string() << " against pattern "
                                                         << pattern,
               Debug::TraceBundling);
        Distillery::RegEx expr(pattern);
        if (expr.match(path.string())) {
            SPLDBG("Including " << path.string(), Debug::TraceBundling);
            return true;
        } else if (isDirectory) {
            SPLDBG("Including directory " << path.string(), Debug::TraceBundling);
            return true;
        }
    }
    return false;
}

bool isExcluded(const bf::path& path, const vector<string>& excludePatterns)
{
    SPLDBG("isExcluded: Checking exclusion of path " << path.string(), Debug::TraceBundling);
    for (vector<string>::const_iterator it = excludePatterns.begin(), itEnd = excludePatterns.end();
         it != itEnd; ++it) {
        const string& pattern = *it;
        SPLDBG("isExcluded: Checking exclusion of path " << path.string() << " against pattern "
                                                         << pattern,
               Debug::TraceBundling);
        Distillery::RegEx expr(pattern);
        if (expr.match(path.string())) {
            SPLDBG("Excluding path " << path.string(), Debug::TraceBundling);
            return true;
        }
    }
    return false;
}

void MakefileGenerator::recursivelyAddDirectory(const bf::path& srcDir,
                                                const bf::path& tgtDir,
                                                const vector<string>& includeRegExs,
                                                const vector<string>& excludeRegExs,
                                                vector<pair<bf::path, bf::path> >& files) const
{
    bf::path newSrcDir = bf::canonical(srcDir);
    SPLDBG("recursivelyAddDirectory: adding directory " << newSrcDir.string(),
           Debug::TraceBundling);
    string outDir = _config.getStringValue(CompilerConfiguration::OutputDir);
    if (newSrcDir.string() == outDir) {
        SPLDBG("recursivelyAddDirectory: skipping directory " << newSrcDir.string(),
               Debug::TraceBundling);
        return;
    }
    for (bf::directory_iterator di(newSrcDir), diEnd; di != diEnd; ++di) {
        const bf::directory_entry& de = *di;
        const bf::path& p = de.path();
        if (isIncluded(p, includeRegExs) && !isExcluded(p, excludeRegExs)) {
            if (is_directory(p)) {
                recursivelyAddDirectory(p, tgtDir / de.path().filename(), includeRegExs,
                                        excludeRegExs, files);
            } else {
                files.push_back(make_pair(p, tgtDir));
            }
        }
    }
}

string escapeToolkitPath(const string& path)
{
    string result;
    for (size_t i = 0, size = path.size(); i < size; ++i) {
        char c = path[i];

        if (c == '$' || c == '<' || c == '>') {
            result.push_back('\\');
        }
        result.push_back(c);
    }
    return result;
}

void MakefileGenerator::copyToolkit(const bf::path& toolkitRoot,
                                    const Toolkit& toolkit,
                                    bundleInfoModelType& biModel,
                                    string& bundleDepInfo,
                                    vector<string>& applicationRegExs,
                                    vector<string>& outputDirRegExs) const
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Building file list", Debug::TraceBundling);

    bool isAppToolkit = toolkit.isCurrentToolkit();

    bf::path toolkitTgtPath = toolkitRoot / toolkit.name();
    Utility::createDirectory(toolkitTgtPath);
    bf::path toolkitSrcPath = bf::canonical(toolkit.path());
    vector<pair<bf::path, bf::path> > files;

    vector<string> includeRegExs;
    vector<string>& toolkitRegExs = (isAppToolkit) ? applicationRegExs : includeRegExs;
    vector<string> excludeRegExs;

    // Escape any path chars that are not to be part of the pattern
    bf::path escapedToolkitPath = escapeToolkitPath(toolkitSrcPath.string());
    bf::path escapedApplicationPath = escapeToolkitPath(
      bf::canonical(_config.getStringValue(CompilerConfiguration::ApplicationDir)).string());

    {
        const vector<BundleInclude>& bundleIncludes = toolkit.bundleIncludes();
        for (vector<BundleInclude>::const_iterator it = bundleIncludes.begin(),
                                                   itEnd = bundleIncludes.end();
             it != itEnd; ++it) {
            const BundleInclude& bi = *it;
            const string regEx = bi.regEx();
            if (bi.root() == BundleInclude::ToolkitDir) {
                toolkitRegExs.push_back((escapedToolkitPath / regEx).string());
            } else if (bi.root() == BundleInclude::ApplicationDir) {
                applicationRegExs.push_back((escapedApplicationPath / regEx).string());
            } else {
                outputDirRegExs.push_back(regEx);
            }
        }
    }
    {
        const vector<string>& bundleExcludes = toolkit.bundleExcludes();
        for (vector<string>::const_iterator it = bundleExcludes.begin(),
                                            itEnd = bundleExcludes.end();
             it != itEnd; ++it) {
            const string& regEx = *it;
            excludeRegExs.push_back((escapedToolkitPath / regEx).string());
        }
    }

    recursivelyAddDirectory(toolkitSrcPath, toolkitTgtPath, toolkitRegExs, excludeRegExs, files);

    if (!files.empty()) {
        bf::path toolkitPath = toolkit.path();
        if (toolkitPath.string() == ".") {
            toolkitPath = _config.getStringValue(CompilerConfiguration::ApplicationDir);
        }
        toolkitPath = bf::canonical(bf::system_complete(toolkitPath));
        size_t len = toolkitPath.string().size() + 1;
        auto_ptr<toolkitType> tkType(
          new toolkitType(toolkit.name(), toolkit.version(), toolkitPath.string()));

        for (vector<pair<bf::path, bf::path> >::const_iterator it2 = files.begin(),
                                                               it2End = files.end();
             it2 != it2End; ++it2) {
            const bf::path& file = it2->first;
            auto_ptr<fileType> ft(new fileType(file.string().substr(len)));
            tkType->file().push_back(ft);
            // The toolkit.xml file is always modified when a compile is run. We don't want the
            // bundle rebuild to be dependent on that.
            if (isAppToolkit && file.filename().string() == "toolkit.xml") {
                continue;
            }
            bundleDepInfo += " \\\n  " + escapePath(file.string());
        }
        biModel.toolkit().push_back(tkType);
    }
}

// NOTE: The "bundle version" is a non-dotted form of "VR00" from the "V.R.M.F" product version
// string. It is unfortunately not "V.R.0.0"; tooling expects "VR00". This is a convention that we
// follow, but it has not been documented anywhere. If we ever change the bundle version to instead
// be the "V.R.0.0" format, we must also change the code in com.ibm.streams.splappinfo.core that
// reads it.
static string getBundleVersion()
{
    vector<string> vrmf;
    boost::split(vrmf, Product::getVersion(), boost::is_any_of("."));
    assert(vrmf.size() >= 2);

    return vrmf[0] + vrmf[1] + "00";
}

void MakefileGenerator::createBundleRules(ofstream& mf)
{
    const OperatorGraph& g = OperatorGraph::instance();
    string bundleDepInfo;
    bundleDepInfo += _bundleFile + ':';
    // Build a bundle.xml file containing info on what is in the bundle
    {
        bf::path bundleDesc = bf::path(_config.getStringValue(CompilerConfiguration::OutputDir)) /
                              "build" / "bundleInfo.xml";
        Utility::removeFile(bundleDesc, false);

        auto_ptr<ofstream> ostr(new ofstream(bundleDesc.string().c_str()));
        if (NULL != ostr.get()) {
            ofstream& biStream = *ostr;
            try {
                string appDirName =
                  bf::path(_config.getStringValue(CompilerConfiguration::ApplicationDir))
                    .filename()
                    .string();
                string outputDirName =
                  bf::path(_config.getStringValue(CompilerConfiguration::OutputDir))
                    .filename()
                    .string();
                auto_ptr<bundleInfoModelType> biModel(
                  new bundleInfoModelType(getBundleVersion(), "", "", appDirName, outputDirName,
                                          _adlFileName, "distributed"));

                bf::path toolkitRoot =
                  bf::path(_config.getStringValue(CompilerConfiguration::OutputDir)) / "toolkits";
                // Ensure this is removed prior to the copy otherwise readonly files will cause
                // errors
                Utility::removeDirectory(toolkitRoot, true, true);

                const vector<const Toolkit*>& toolkits = PathSearch::instance().toolkits();

                // We need to visit the application toolkit last in case any of the other toolkits
                // are trying to force files from the application dir into the bundle
                const Toolkit* appToolkit = NULL;
                vector<string> applicationRegExs;
                vector<string> outputDirRegExs;
                for (vector<const Toolkit*>::const_iterator it = toolkits.begin(),
                                                            itEnd = toolkits.end();
                     it != itEnd; ++it) {
                    const Toolkit& tk = **it;
                    if (tk.isCurrentToolkit()) {
                        appToolkit = *it;
                        continue;
                    }
                    // Ignore this toolkit if not used
                    if (!CompilationState::instance().isToolkitUsed(tk)) {
                        continue;
                    }
                    copyToolkit(toolkitRoot, tk, *biModel, bundleDepInfo, applicationRegExs,
                                outputDirRegExs);
                }
                if (NULL != appToolkit) {
                    // Copy the app toolkit if it exists
                    copyToolkit(toolkitRoot, *appToolkit, *biModel, bundleDepInfo,
                                applicationRegExs, outputDirRegExs);
                }

                // If there are any includes specified for the output directory, write those to the
                // bundleInfo file
                if (!outputDirRegExs.empty()) {
                    auto_ptr<outputIncludesType> outputIncludes(new outputIncludesType);
                    for (vector<string>::const_iterator it = outputDirRegExs.begin(),
                                                        itEnd = outputDirRegExs.end();
                         it != itEnd; ++it) {
                        auto_ptr<outputIncludeType> outputInclude(new outputIncludeType(*it));
                        outputIncludes->outputInclude().push_back(outputInclude);
                    }
                    biModel->outputIncludes(outputIncludes);
                }

                xml_schema::namespace_infomap map;
                map[""].name = "http://www.ibm.com/xmlns/prod/streams/spl/bundleInfoModel";
                map["xsi"].name = "http://www.w3.org/2001/XMLSchema-instance";
                bundleInfoModel(biStream, *biModel, map);
                biStream.flush();
                biStream.close();
                if (!biStream) {
                    SysOut::die(SPL_CORE_CANNOT_CREATE_FILE(bundleDesc.string(),
                                                            RuntimeUtility::getErrorNoStr()));
                }

            } catch (::xsd::cxx::exception& e) {
                SysOut::die(SPL_CORE_XML_CREATION_FAILED(bundleDesc.string(), e.what()));
            }
        } else {
            SysOut::die(
              SPL_CORE_CANNOT_CREATE_FILE(bundleDesc.string(), RuntimeUtility::getErrorNoStr()));
        }
        bundleDepInfo += '\n';
    }

    mf << "sablink:\n";
    mf << "\n\t@ln -sf ../" << _bundleFile << " bin/sab\n\n";

    mf << "bundle: " << _bundleFile << "\n\n";

    bf::path outDir =
      CompilerConfiguration::instance().getStringValue(CompilerConfiguration::OutputDir);
    bf::path depDir = bf::path("build") / "dep";
    bf::path depFile = depDir / (_bundleFile + ".d");

    mf << "-include " << depFile.string() << '\n';
    mf << _bundleFile << ": $(SPL_SO_BINS) $(SPL_ADL_FILE) " << depFile.string();
    if (g.numNodes() > 0) {
        mf << " bin/standalone.exe";
    }
    mf << "\n";
    mf << "\t@echo ' [Bundle] " << _bundleFile << "'\n"
       << "\t@$(SPL_BUNDLE_BUILDER) $(SPL_OUTPUT_DIR_ROOT) $(SPL_OUTPUT_DIR) $(SPL_ADL_FILE) "
       << _bundleFile;
    mf << "\n\n";
    Utility::writeIfNecessary(outDir / depFile, bundleDepInfo);
}

static bool anyPEHasWrapperSDB()
{
    const OperatorGraph& g = OperatorGraph::instance();
    uint32_t i, n = g.numNodes();
    for (i = 0; i < n; i++) {
        const OperatorGraphNode& op = g.getNode(i);
        const Config* wrapper = op.getOperatorIR()->findConfig(Config::Wrapper);
        if (wrapper && wrapper->getWrapper() == "sdb") {
            return true;
        }
    }

    return false;
}

void MakefileGenerator::createHeader(ofstream& mf)
{
    mf << // Note about how to run this makefile manually
      "# NOTE: This makefile is designed to be run from one level up in the directory\n"
      "# hierarchy compared to where it resides. For instance, if this makefile is\n"
      "# located at 'output/build/Makefile', then it can be launched as:\n"
      "# 'make -f build/Makefile' from the 'output' directory.\n\n";
    // Set the compiler
    // nb. at least on powerpc CXX is set to g++ by make if not set by the user
    mf << "SHELL=/bin/bash\n";
    mf << "ifeq ($(CXX),)\n  CXX=g++\nendif\n";
    // ATT had a compile-time issue due to an extreme number of attributes. They needed the ability
    // to compile generated types with the compat compiler supplied with RHEL7. To support this we
    // define two compiler macros, one for compiling objects and one for linking PEs.
    mf << "ifeq ($(CXX_LN),)\n  CXX_LN=g++\nendif\n";

    // The packages
    bf::path installDir(_config.getStringValue(CompilerConfiguration::InstallDir));
    bf::path pkg = installDir / "bin/dst-pe-pkg-config-alternative.sh";
    mf << "PKGCONFIG := " << pkg.string() << endl;

    bf::path bundleBuilder = installDir / "system" / "impl" / "bin" / "spl-bundle-builder";
    mf << "SPL_BUNDLE_BUILDER := " << bundleBuilder.string() << endl;

    mf << "SO_INCLUDE := $(shell $(PKGCONFIG) --cflags dst-spl-pe-install)\n";
    mf << "SO_LIB := $(shell $(PKGCONFIG) --libs dst-spl-pe-install)\n";
    mf << "SO_STAND_LIB := $(shell $(PKGCONFIG) --libs dst-spl-pe-install-standalone)\n";

    string cppFlags;
    if (_config.getBooleanValue(CompilerConfiguration::Optimized)) {
        cppFlags = "-O3 -DNDEBUG ";
    }

    if (_config.getCxxLangLevel() == CompilerConfiguration::LangLevelCxx98) {
        cppFlags += " -std=gnu++98 ";
    } else if (_config.getCxxLangLevel() == CompilerConfiguration::LangLevelCxx11) {
        cppFlags += " -std=gnu++11 -Wno-deprecated ";
    }

    if (_config.isSet(CompilerConfiguration::CxxFlags)) {
        cppFlags += _config.getStringValue(CompilerConfiguration::CxxFlags);
    }

    // We need some debugging information for Debugger (port numbers)
    if (_config.getBooleanValue(CompilerConfiguration::SetWrapperSDB) || anyPEHasWrapperSDB()) {
        cppFlags += " -g";
    }
    cppFlags += '\n';

    // Compiler options
    mf << "SPL_CXXFLAGS = -Wall -Wno-unused -Wno-sign-compare\n";
    mf << "SPL_CXXFLAGS += -D_REENTRANT -fPIC -finline-functions -fmessage-length=0"
#if defined(__powerpc64__)
          " -m64"
#endif
          "\n";

    mf << "SPL_CXXFLAGS += $(SO_INCLUDE) " << cppFlags;

    // Loader flags
    string ldFlags;
    if (_config.isSet(CompilerConfiguration::LdFlags)) {
        ldFlags = " " + _config.getStringValue(CompilerConfiguration::LdFlags);
    }
    ldFlags += '\n';
    mf << "SPL_LDFLAGS = $(SO_LIB)" << ldFlags << '\n';

    // Save the flags to build/{CppOptions,ldOptions} to force recompiles/relinks
    // if changed
    bf::path outDir = _config.getStringValue(CompilerConfiguration::OutputDir);
    bf::path buildDir = outDir / "build";
    Utility::writeIfNecessary(buildDir / CPP_FLAG_FILE, cppFlags);
    Utility::writeIfNecessary(buildDir / LD_FLAG_FILE, ldFlags);

    // All the types
    mf << "SPL_TYPES = " << _types.objectFiles() << '\n';

    // All the functions
    mf << "SPL_FUNCTIONS = " << _functions.objectFiles() << '\n';

    // All the operators - only if generating code
    if (!_config.getBooleanValue(CompilerConfiguration::GenerateCode)) {
        mf << "# ";
    }
    mf << "SPL_OPERATORS = " << _operators.objectFiles() << '\n';

    // All the SO binaries
    mf << "SPL_SO_BINS = " << _pes.executableFiles() << "\n\n";

    // The bundle name
    mf << "SPL_ADL_FILE = " << _adlFileName << "\n\n";
    mf << "SPL_OUTPUT_DIR_ROOT = $(shell printf '%q' '" << escapePath(outDir.parent_path().string())
       << "')\n";
    mf << "SPL_OUTPUT_DIR = $(shell printf '%q' '" << escapePath(outDir.filename().string())
       << "')\n\n";

    // things to build
    mf << ".PHONY: distclean clean all types functions operators sos bundle toolkits standalone "
          "sablink\n\n";
    mf << "all: bin types functions operators sos standalone bundle sablink";
    mf << "\n\n";
    mf << "distclean clean:\n";
    mf << "\t@rm -fr bin/* build/type build/function build/operator build/bundle build/standalone "
       << _bundleFile << "\n\n";
    mf << "bin:\n";
    mf << "\t@mkdir " << _directoryPermissionMode << " bin\n\n";
}

void MakefileGenerator::compile()
{
    if (_config.getBooleanValue(CompilerConfiguration::JustTheAppModel)) {
        return;
    }

    SysOut::writeln(SPL_CORE_BUILDING_BINARIES, cout);

    vector<string> args;
    if (_config.getBooleanValue(CompilerConfiguration::Verbose)) {
        args.push_back("-v");
    }

    int distcc = 0;
    if (_config.isSet(CompilerConfiguration::DistccRemoteHostCount)) {
        distcc = _config.getIntValue(CompilerConfiguration::DistccRemoteHostCount);
    }
    int thread = 0;
    if (_config.isSet(CompilerConfiguration::MakeThreadCount)) {
        thread = _config.getIntValue(CompilerConfiguration::MakeThreadCount);
    }
    // first check if running in container, if it is cpu is set to the resource
    // constraint of the container, otherwise the return value is -1. We will
    // then check /proc/cpuinfo
    static int cpu = SystemStatics::getContainerNCPUs();
    if (cpu == -1) {
        cpu = SystemStatics::getNCPUs();
    }

    if (thread == 0) {
        thread = max(1, cpu / 2);
    }
    if (distcc > 0) { // distcc is enabled
        thread = max(thread, cpu * (1 + distcc));
    }
    args.push_back("-j");
    args.push_back(toString(thread));
    args.push_back("-r");
    args.push_back(toString(distcc));

    bf::path appDir = _config.getStringValue(CompilerConfiguration::OutputDir);
    bf::path makeDir = appDir / "build";
    args.push_back("-m");
    args.push_back(makeDir.string());

    bf::path sdir = _config.getStringValue(CompilerConfiguration::InternalScriptDir);
    string script = (sdir / "spl-code-builder").string();
    ostringstream errStrm;
    try {
        int rc = ProcessLauncher::runProcess(script, args);
        if (rc == SPL_USER_ERROR) {
            SysOut::die(SPL_CORE_MAKE_FAILURE);
        } else if (rc != 0) {
            ostringstream ss;
            ss << "rc == " << rc;
            SysOut::die(SPL_CORE_CODE_BUILD_FAILURE(script, ss.str()));
        }
    } catch (SPLCompilerException& e) {
        SysOut::die(SPL_CORE_CODE_BUILD_FAILURE(script, e.getExplanation()));
    }
}
