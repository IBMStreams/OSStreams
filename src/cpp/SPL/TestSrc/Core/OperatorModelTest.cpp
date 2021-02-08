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

#include "../Utility/UtlTestCommon.h"
#include <SPL/Core/CompilerException.h>
#include <SPL/Core/OperatorModelImpl.h>
#include <UTILS/DistilleryApplication.h>

#include <boost/filesystem/path.hpp>

#include <fstream>
#include <string>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace SPL::Operator;
using namespace Distillery;
using namespace opmodel;

static bool findLast(const string& path, const string& lookFor)
{
    size_t pos = path.rfind(lookFor);
    FASSERT(pos != string::npos);
    return pos == path.size() - lookFor.size();
}

class OperatorModelTest : public UTILS_NAMESPACE::DistilleryApplication
{
  public:
    OperatorModelTest() {}

    void getArguments(option_vector_t& options)
    {
        setKeepRemainingArguments();
        option_t args[] = {
            { 'i', "input-file", ARG, "", ".xml input file",
              STR_OPT(OperatorModelTest::setFilename), 0, 0 },
        };
        APPEND_OPTIONS(options, args);
    }
    void setFilename(const option_t* /*option*/, const char* value) { fileName_ = value; }
    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        auto_ptr<operatorModelType> rawModel;
        ifstream istr(fileName_.c_str(), fstream::in);
        if (!istr) {
            cerr << "ERROR: Cannot open file '" << fileName_ << "'" << endl;
            exit(EXIT_FAILURE);
        }
        try {
            string buildDir(_argv_0);
            string::size_type pos = buildDir.rfind("/.libs");
            if (pos != string::npos)
                buildDir.erase(pos);
            else
                buildDir.erase(buildDir.rfind("/"));
            ::xml_schema::properties props;
            props.schema_location("http://www.ibm.com/xmlns/prod/streams/spl/operator",
                                  buildDir + "/../schema/xsd/operatorModel.xsd");
            rawModel = operatorModel(istr, 0, props);
            // rawModel = operatorModel(istr, xsd::cxx::tree::flags::dont_validate);
        } catch (::xsd::cxx::exception& e) {
            cerr << "ERROR: Cannot load xml model '" << fileName_ << "'" << endl;
            cerr << "ERROR: " << e.what() << endl;
            exit(EXIT_FAILURE);
        }
        auto_ptr<OperatorModel> model;
        try {
            char cwd[1024];
            getcwd(cwd, sizeof cwd);
            model.reset(new OperatorModel(*rawModel, cwd));
        } catch (SPLCompilerException& e) {
            cerr << "ERROR: " << e.getExplanation() << endl;
            exit(EXIT_FAILURE);
        }
        printModel(*model);
        return 0;
    }
    void printModel(const OperatorModel& model)
    {
        printContext(model.getContext());
        printParameters(model.getParameters());
        printInputPorts(model.getInputPorts());
        printOutputPorts(model.getOutputPorts());
    }
    void printContext(const Context& context)
    {
        const unordered_map<string, EnumerationPtr>& ofs = context.getCustomOutputFunctions();
        FASSERT(ofs.size() == 2);
        cout << "Custom Output Functions: " << ofs.size() << "\n";
        for (unordered_map<string, EnumerationPtr>::const_iterator it = ofs.begin();
             it != ofs.end(); ++it) {
            cout << "  " << it->first << " = { ";
            const unordered_set<string>& vals = it->second->getValues();
            for (unordered_set<string>::const_iterator it = vals.begin(); it != vals.end(); ++it) {
                if (it != vals.begin())
                    cout << ", ";
                cout << *it;
            }
            cout << " }\n";
        }
        FASSERT(ofs.count("DSPAggregation") == 1);
        FASSERT(ofs.find("DSPAggregation")->second->getValues().count("Convolve") == 1);
        FASSERT(ofs.find("DSPAggregation")->second->getValues().count("FFT") == 1);
        FASSERT(ofs.count("RelationalAggregation") == 1);
        FASSERT(ofs.find("RelationalAggregation")->second->getValues().count("Sum") == 1);
        FASSERT(ofs.find("RelationalAggregation")->second->getValues().count("Max") == 1);
        FASSERT(ofs.find("RelationalAggregation")->second->getValues().count("Avg") == 1);
        FASSERT(ofs.find("RelationalAggregation")->second->getValues().count("Min") == 1);

        const unordered_map<string, EnumerationPtr>& lits = context.getCustomLiterals();
        cout << "Custom Literals: " << lits.size() << "\n";
        FASSERT(lits.size() == 2);
        for (unordered_map<string, EnumerationPtr>::const_iterator it = lits.begin();
             it != lits.end(); ++it) {
            cout << "  " << it->first << " = { ";
            const unordered_set<string>& vals = it->second->getValues();
            for (unordered_set<string>::const_iterator it = vals.begin(); it != vals.end(); ++it) {
                if (it != vals.begin())
                    cout << ", ";
                cout << *it;
            }
            cout << " }\n";
        }
        FASSERT(lits.count("FileFormat") == 1);
        FASSERT(lits.find("FileFormat")->second->getValues().count("compressed") == 1);
        FASSERT(lits.find("FileFormat")->second->getValues().count("csvFormat") == 1);
        FASSERT(lits.find("FileFormat")->second->getValues().count("xmlFormat") == 1);
        FASSERT(lits.count("DelayFormat") == 1);
        FASSERT(lits.find("DelayFormat")->second->getValues().count("meanDelay") == 1);
        FASSERT(lits.find("DelayFormat")->second->getValues().count("initDelay") == 1);
        FASSERT(lits.find("DelayFormat")->second->getValues().count("noDelay") == 1);

        const vector<LibraryPtr>& deps = context.getLibraryDependencies();
        cout << "Library Dependencies: " << deps.size() << "\n";
        FASSERT(deps.size() == 2);
        for (vector<LibraryPtr>::const_iterator it = deps.begin(); it != deps.end(); ++it) {
            cout << "  Description: " << (*it)->getDescription() << "\n";
            if ((*it)->isManaged()) {
                const Common::ManagedLibrary& m = (*it)->managed();
                const vector<std::string>& libs = m.getLibs();
                cout << "    libs: " << libs.size() << "\n";
                for (vector<string>::const_iterator iit = libs.begin(); iit != libs.end(); ++iit)
                    cout << "      " << *iit << "\n";
                const vector<std::string>& libPaths = m.getLibPaths();
                cout << "    libPaths: " << libPaths.size() << "\n";
                for (vector<string>::const_iterator iit = libPaths.begin(); iit != libPaths.end();
                     ++iit)
                    cout << "      " << *iit << "\n";
                const vector<std::string>& includePaths = m.getIncludePaths();
                cout << "    includePaths: " << includePaths.size() << "\n";
                for (vector<string>::const_iterator iit = includePaths.begin();
                     iit != includePaths.end(); ++iit)
                    cout << "      " << *iit << "\n";
            } else {
                const Common::ProvisionedLibrary& p = (*it)->provisioned();
                cout << "    name = " << p.getName() << '\n';
                cout << "    version = " << p.getVersion() << '\n';
            }
        }
        FASSERT(deps[0]->getDescription() == "Tecla Library");
        FASSERT(deps[0]->isManaged());
        const Common::ManagedLibrary* m = &deps[0]->managed();
        FASSERT(m->getLibs().size() == 3);
        FASSERT(m->getLibs()[0] == "tecla");
        FASSERT(m->getLibs()[1] == "hilib");
        FASSERT(m->getLibs()[2] == "ho");
        FASSERT(m->getLibPaths().size() == 3);
        FASSERT(findLast(m->getLibPaths()[0], "/tecla/libs"));
        FASSERT(findLast(m->getLibPaths()[1], "/hilibpath"));
        FASSERT(findLast(m->getLibPaths()[2], "/ho"));
        FASSERT(m->getIncludePaths().size() == 3);
        FASSERT(findLast(m->getIncludePaths()[0], "/tecla/include"));
        FASSERT(findLast(m->getIncludePaths()[1], "/hiincpath"));
        FASSERT(findLast(m->getIncludePaths()[2], "/ho"));
        FASSERT(deps[1]->getDescription() == "Curl Library");
        FASSERT(!deps[1]->isManaged());
        const Common::ProvisionedLibrary& p = deps[1]->provisioned();
        FASSERT(p.getName() == "curl");
        FASSERT(p.getVersion() == "2.3.0.abcA-_d");

        FASSERT(context.getProvidesSingleThreadedContext() == SingleThreadedMode::Never);
        FASSERT(context.allowCustomLogic() == false);
        cout << "  allowCustomLogic: " << ((context.allowCustomLogic()) ? "true" : "false") << '\n';
    }

    void printParameters(const Parameters& params)
    {
        cout << "  Arbitrary params allowed: " << params.allowArbitraryParameters() << "\n";
        FASSERT(params.allowArbitraryParameters() == false);
        const unordered_map<string, ParameterPtr>& prms = params.getNamedParameters();
        cout << "Parameters: " << prms.size() << "\n";
        FASSERT(prms.size() == 4);
        for (unordered_map<string, ParameterPtr>::const_iterator it = prms.begin();
             it != prms.end(); ++it) {
            const ParameterPtr& prm = it->second;
            cout << "  Name: " << it->first << "\n";
            cout << "    Optional: " << prm->isOptional() << "\n";
            cout << "    RewriteAllowed: " << prm->isRewriteAllowed() << "\n";
            cout << "    ExpressionMode: " << prm->getExpressionMode() << "\n";
            if (prm->hasType())
                cout << "    Type: " << prm->getType() << "\n";
            if (prm->hasCardinality())
                cout << "    Cardinality: " << prm->getCardinality() << "\n";
            if (prm->hasPortScope()) {
                const unordered_set<unsigned>& scope = prm->getPortScope();
                cout << "    Port Scope: { ";
                for (unordered_set<unsigned>::const_iterator it = scope.begin(); it != scope.end();
                     ++it) {
                    if (it != scope.begin())
                        cout << ", ";
                    cout << *it;
                }
                cout << " }\n";
            }
        }
        FASSERT(prms.count("matchRight") == 1);
        FASSERT(prms.find("matchRight")->second->isOptional() == true);
        FASSERT(prms.find("matchRight")->second->isRewriteAllowed() == true);
        FASSERT(toString(prms.find("matchRight")->second->getExpressionMode()) == "Expression");
        FASSERT(prms.find("matchRight")->second->hasType() == false);
        FASSERT(prms.find("matchRight")->second->hasCardinality() == false);
        FASSERT(prms.find("matchRight")->second->hasPortScope() == true);
        FASSERT(prms.find("matchRight")->second->getPortScope().size() == 1);
        FASSERT(prms.find("matchRight")->second->getPortScope().count(1) == 1);
        FASSERT(prms.count("matchLeft") == 1);
        FASSERT(prms.find("matchLeft")->second->isOptional() == true);
        FASSERT(prms.find("matchLeft")->second->isRewriteAllowed() == true);
        FASSERT(toString(prms.find("matchLeft")->second->getExpressionMode()) == "Expression");
        FASSERT(prms.find("matchLeft")->second->hasType() == false);
        FASSERT(prms.find("matchLeft")->second->hasCardinality() == false);
        FASSERT(prms.find("matchLeft")->second->hasPortScope() == true);
        FASSERT(prms.find("matchLeft")->second->getPortScope().size() == 1);
        FASSERT(prms.find("matchLeft")->second->getPortScope().count(0) == 1);
        FASSERT(prms.count("uri") == 1);
        FASSERT(prms.find("uri")->second->isOptional() == false);
        FASSERT(prms.find("uri")->second->isRewriteAllowed() == true);
        FASSERT(toString(prms.find("uri")->second->getExpressionMode()) == "Constant");
        FASSERT(prms.find("uri")->second->hasType() == true);
        FASSERT(prms.find("uri")->second->getType() == "rstring");
        FASSERT(prms.find("uri")->second->hasCardinality() == true);
        FASSERT(prms.find("uri")->second->getCardinality() == 1);
        FASSERT(prms.find("uri")->second->hasPortScope() == false);

        FASSERT(prms.count("format") == 1);
        FASSERT(prms.find("format")->second->isOptional() == true);
        FASSERT(prms.find("format")->second->isRewriteAllowed() == false);
        FASSERT(toString(prms.find("format")->second->getExpressionMode()) == "CustomLiteral");
        FASSERT(prms.find("format")->second->hasType() == true);
        FASSERT(prms.find("format")->second->getType() == "FileFormat");
        FASSERT(prms.find("format")->second->hasCardinality() == false);
        FASSERT(prms.find("format")->second->hasPortScope() == false);
    }
    void printInputPorts(const InputPorts& iports)
    {
        cout << "Input ports: "
             << "min=" << iports.getMinNumberOfPorts()
             << ", "
                "max="
             << iports.getMaxNumberOfPorts() << "\n";
        FASSERT(iports.getMinNumberOfPorts() == 2);
        FASSERT(iports.getMaxNumberOfPorts() == std::numeric_limits<unsigned>::max());
        const vector<InputPortSetPtr>& prts = iports.getPortSets();
        FASSERT(prts.size() == 2);
        unsigned i = 0;
        for (vector<InputPortSetPtr>::const_iterator it = prts.begin(); it != prts.end(); ++it) {
            cout << "  Port set " << i++ << "\n";
            printInputPort(**it);
            cout << "    Is optional: " << (*it)->isOptional() << "\n";
            cout << "    Cardinality: " << (*it)->getCardinality() << "\n";
        }
        FASSERT(prts[0]->isTupleMutationAllowed() == false);
        FASSERT(toString(prts[0]->getWindowingMode()) == "Windowed");
        FASSERT(toString(prts[0]->getWindowPunctuationInputMode()) == "WindowBound");
        FASSERT(prts[0]->isOptional() == false);
        FASSERT(prts[0]->getCardinality() == 2);
        FASSERT(prts[1]->isTupleMutationAllowed() == false);
        FASSERT(toString(prts[1]->getWindowingMode()) == "NonWindowed");
        FASSERT(toString(prts[1]->getWindowPunctuationInputMode()) == "Oblivious");
        FASSERT(prts[1]->isOptional() == true);
        FASSERT(prts[1]->getCardinality() == 1);

        if (iports.hasPortOpenSet()) {
            cout << "  Open port set\n";
            const InputPortOpenSet& prt = iports.getPortOpenSet();
            printInputPort(prt);
        }
        FASSERT(iports.hasPortOpenSet() == true);
        FASSERT(iports.getPortOpenSet().isTupleMutationAllowed() == true);
        FASSERT(toString(iports.getPortOpenSet().getWindowingMode()) == "OptionallyWindowed");
        FASSERT(toString(iports.getPortOpenSet().getWindowPunctuationInputMode()) == "Expecting");
    }
    void printInputPort(const InputPort& iport)
    {
        cout << "    Tuple mutation allowed: " << iport.isTupleMutationAllowed() << "\n";
        cout << "    Windowing mode: " << iport.getWindowingMode() << "\n";
        cout << "    Window punctuation input mode: " << iport.getWindowPunctuationInputMode()
             << "\n";
    }
    void printOutputPorts(const OutputPorts& oports)
    {
        cout << "Output ports: "
             << "min=" << oports.getMinNumberOfPorts()
             << ", "
                "max="
             << oports.getMaxNumberOfPorts() << "\n";
        FASSERT(oports.getMinNumberOfPorts() == 1);
        FASSERT(oports.getMaxNumberOfPorts() == std::numeric_limits<unsigned>::max());
        const vector<OutputPortSetPtr>& prts = oports.getPortSets();
        FASSERT(prts.size() == 2);
        unsigned i = 0;
        for (vector<OutputPortSetPtr>::const_iterator it = prts.begin(); it != prts.end(); ++it) {
            cout << "  Port set " << i++ << "\n";
            printOutputPort(**it);
            cout << "    Is optional: " << (*it)->isOptional() << "\n";
            cout << "    Cardinality: " << (*it)->getCardinality() << "\n";
        }

        FASSERT(toString(prts[0]->getExpressionMode()) == "Expression");
        FASSERT(prts[0]->isAutoAssignmentEnabled() == true);
        FASSERT(prts[0]->isCompleteAssignmentRequired() == true);
        FASSERT(prts[0]->isRewriteAllowed() == true);
        FASSERT(prts[0]->hasOutputFunctions() == false);
        FASSERT(toString(prts[0]->getWindowPunctuationOutputMode()) == "Generating");
        FASSERT(prts[0]->isTupleMutationAllowed() == true);
        FASSERT(prts[0]->isOptional() == false);
        FASSERT(prts[0]->getCardinality() == 1);
        FASSERT(prts[0]->getFinalPunctuationPortScope()[0] == 1);
        FASSERT(toString(prts[1]->getExpressionMode()) == "Expression");
        FASSERT(prts[1]->isAutoAssignmentEnabled() == true);
        FASSERT(prts[1]->isCompleteAssignmentRequired() == true);
        FASSERT(prts[1]->isRewriteAllowed() == true);
        FASSERT(prts[1]->hasOutputFunctions() == true);
        FASSERT(prts[1]->getOutputFunctions().getOutputFunctionType() == "DSPAggregation");
        FASSERT(prts[1]->getOutputFunctions().getDefaultOutputFunction() == "FFT");
        FASSERT(toString(prts[1]->getWindowPunctuationOutputMode()) == "Free");
        FASSERT(prts[1]->isTupleMutationAllowed() == false);
        FASSERT(prts[1]->isOptional() == true);
        FASSERT(prts[1]->getCardinality() == 1);

        if (oports.hasPortOpenSet()) {
            cout << "  Open port set\n";
            const OutputPortOpenSet& prt = oports.getPortOpenSet();
            printOutputPort(prt);
        }
        FASSERT(oports.hasPortOpenSet() == true);
        FASSERT(toString(oports.getPortOpenSet().getExpressionMode()) == "Attribute");
        FASSERT(oports.getPortOpenSet().isAutoAssignmentEnabled() == true);
        FASSERT(oports.getPortOpenSet().isCompleteAssignmentRequired() == false);
        FASSERT(oports.getPortOpenSet().isRewriteAllowed() == true);
        FASSERT(oports.getPortOpenSet().hasOutputFunctions() == true);
        FASSERT(oports.getPortOpenSet().getOutputFunctions().getOutputFunctionType() ==
                "RelationalAggregation");
        FASSERT(oports.getPortOpenSet().getOutputFunctions().getDefaultOutputFunction() == "Min");
        FASSERT(toString(oports.getPortOpenSet().getWindowPunctuationOutputMode()) == "Free");
        FASSERT(oports.getPortOpenSet().isTupleMutationAllowed() == false);
    }
    void printOutputPort(const OutputPort& oport)
    {
        cout << "    Expression mode: " << oport.getExpressionMode() << "\n";
        cout << "    Auto assignment: " << oport.isAutoAssignmentEnabled() << "\n";
        cout << "    Complete assignment: " << oport.isCompleteAssignmentRequired() << "\n";
        cout << "    Rewrite allowed: " << oport.isRewriteAllowed() << "\n";
        if (oport.hasOutputFunctions()) {
            const OutputFunctions& ofs = oport.getOutputFunctions();
            cout << "    Output Functions: "
                 << "type=" << ofs.getOutputFunctionType()
                 << ", "
                    "default="
                 << ofs.getDefaultOutputFunction() << "\n";
        }
        cout << "    Window punctuation output mode: " << oport.getWindowPunctuationOutputMode()
             << "\n";
        cout << "    Tuple mutation allowed: " << oport.isTupleMutationAllowed() << "\n";
        std::vector<unsigned> const& portScope = oport.getFinalPunctuationPortScope();
        cout << "      Requires input ports: ";
        if (portScope.size() != 0) {
            for (size_t i = 0; i < portScope.size(); ++i) {
                cout << portScope[i] << ((i < portScope.size() - 1) ? "," : "");
            }
            cout << "\n";
        } else {
            cout << "all\n";
        }
    }

  private:
    string fileName_;
};

MAIN_APP(OperatorModelTest)
