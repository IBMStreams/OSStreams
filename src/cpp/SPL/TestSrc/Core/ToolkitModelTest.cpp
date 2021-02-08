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

#include <SPL/Core/CompilerException.h>
#include <SPL/Core/ToolkitModelImpl.h>
#include <UTILS/DistilleryApplication.h>

#include <fstream>
#include <boost/filesystem/path.hpp>
#include <string>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace SPL::Toolkit;

class ToolkitModelTest : public UTILS_NAMESPACE::DistilleryApplication
{
  public:
    ToolkitModelTest() {}

    void getArguments(option_vector_t& options)
    {
        setKeepRemainingArguments();
        option_t args[] = {
            { 'i', "input-file", ARG, "", ".xml input file", STR_OPT(ToolkitModelTest::setFilename),
              0, 0 },
        };
        APPEND_OPTIONS(options, args);
    }
    void setFilename(const option_t* /*option*/, const char* value) { fileName_ = value; }
    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        auto_ptr<toolkitModelType> rawModel;
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
            props.schema_location("http://www.ibm.com/xmlns/prod/streams/spl/toolkit",
                                  buildDir + "/../schema/xsd/toolkitModel.xsd");
            rawModel = toolkitModel(istr, 0, props);
            // rawModel = operatorModel(istr, xsd::cxx::tree::flags::dont_validate);
        } catch (::xsd::cxx::exception& e) {
            cerr << "ERROR: Cannot load xml model '" << fileName_ << "'" << endl;
            cerr << "ERROR: " << e.what() << endl;
            exit(EXIT_FAILURE);
        }
        auto_ptr<ToolkitModel> model;
        try {
            model.reset(new ToolkitModel(*rawModel));
        } catch (SPLCompilerException& e) {
            cerr << "ERROR: " << e.getExplanation() << endl;
            exit(EXIT_FAILURE);
        }
        printModel(*model);
        return 0;
    }
    void printModel(const ToolkitModel& model)
    {
        cout << "Description: " << model.getDescription() << "\n";
        cout << "Version: " << model.getVersion() << "\n";
        cout << "RequiredCompilerVersion: " << model.getRequiredCompilerVersion() << "\n";
        const unordered_map<string, NamespacePtr>& namespaces = model.getNamespaces();
        cout << "Namespaces: " << namespaces.size() << "\n";
        for (unordered_map<string, NamespacePtr>::const_iterator it = namespaces.begin();
             it != namespaces.end(); ++it) {
            printNamespace(*it->second);
        }
    }
    void printNamespace(const Namespace& nmspace)
    {
        cout << "  Namespace: " << nmspace.getName() << "\n";
        const unordered_map<string, PrimitiveOperatorPtr>& nopers = nmspace.getPrimitiveOperators();
        cout << "    Primitive operators: " << nopers.size() << "\n";
        for (unordered_map<string, PrimitiveOperatorPtr>::const_iterator it = nopers.begin();
             it != nopers.end(); ++it) {
            printPrimitiveOperator(*it->second);
        }
        const unordered_map<string, CompositeOperatorPtr>& copers = nmspace.getCompositeOperators();
        cout << "    Composite operators: " << copers.size() << "\n";
        for (unordered_map<string, CompositeOperatorPtr>::const_iterator it = copers.begin();
             it != copers.end(); ++it) {
            printCompositeOperator(*it->second);
        }
        const unordered_set<string>& sfuncs = nmspace.getSPLFunctionModelFiles();
        cout << "    SPL function model files: " << sfuncs.size() << "\n";
        for (unordered_set<string>::const_iterator it = sfuncs.begin(); it != sfuncs.end(); ++it) {
            cout << "      File: " << *it << "\n";
        }
        const unordered_set<string>& nfuncs = nmspace.getNativeFunctionModelFiles();
        cout << "    Native function model files: " << nfuncs.size() << "\n";
        for (unordered_set<string>::const_iterator it = nfuncs.begin(); it != nfuncs.end(); ++it) {
            cout << "      File: " << *it << "\n";
        }
    }
    void printOperator(const Operator& oper)
    {
        cout << "      Operator: " << oper.getName() << "\n";
        cout << "        Path: " << oper.getPath() << "\n";
        cout << "        Namespace (parent): " << oper.getNamespace().getName() << "\n";
    }
    void printPrimitiveOperator(const PrimitiveOperator& noper) { printOperator(noper); }
    void printCompositeOperator(const CompositeOperator& coper) { printOperator(coper); }

  private:
    string fileName_;
};

MAIN_APP(ToolkitModelTest)
