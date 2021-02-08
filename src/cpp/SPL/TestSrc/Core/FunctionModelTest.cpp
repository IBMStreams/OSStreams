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
#include <SPL/Core/FunctionModelImpl.h>
#include <UTILS/DistilleryApplication.h>

#include <boost/filesystem/path.hpp>

#include <fstream>
#include <string>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace SPL::Function;

class FunctionModelTest : public UTILS_NAMESPACE::DistilleryApplication
{
  public:
    FunctionModelTest() {}

    void getArguments(option_vector_t& options)
    {
        setKeepRemainingArguments();
        option_t args[] = {
            { 'i', "input-file", ARG, "", ".xml input file",
              STR_OPT(FunctionModelTest::setInFilename), 0, 0 },
            { 'o', "output-file", ARG, "", ".xml output file",
              STR_OPT(FunctionModelTest::setOutFilename), 0, 0 },
        };
        APPEND_OPTIONS(options, args);
    }
    void setInFilename(const option_t* /*option*/, const char* value) { inFileName_ = value; }
    void setOutFilename(const option_t* /*option*/, const char* value) { outFileName_ = value; }
    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        auto_ptr<functionModelType> rawModel;
        ifstream istr(inFileName_.c_str(), fstream::in);
        if (!istr) {
            cerr << "ERROR: Cannot open file '" << inFileName_ << "'" << endl;
            exit(EXIT_FAILURE);
        }
        try {
            string buildDir(_argv_0);
            string::size_type pos = buildDir.rfind("/.libs");
            if (pos != string::npos) {
                buildDir.erase(pos);
            } else {
                buildDir.erase(buildDir.rfind("/"));
            }
            ::xml_schema::properties props;
            props.schema_location("http://www.ibm.com/xmlns/prod/streams/spl/function",
                                  buildDir + "/../schema/xsd/functionModel.xsd");
            rawModel = functionModel(istr, 0, props);
            // rawModel = functionModel(istr, xsd::cxx::tree::flags::dont_validate);
        } catch (::xsd::cxx::exception& e) {
            cerr << "ERROR: Cannot load xml model '" << inFileName_ << "' for reading" << endl;
            cerr << "ERROR: " << e.what() << endl;
            exit(EXIT_FAILURE);
        }
        auto_ptr<FunctionModel> model;
        try {
            char cwd[1024];
            getcwd(cwd, sizeof cwd);
            model.reset(new FunctionModel(*rawModel, cwd));
        } catch (SPLCompilerException& e) {
            cerr << "ERROR: " << e.getExplanation() << endl;
            exit(EXIT_FAILURE);
        }
        auto_ptr<functionModelType> loadedModel(model->toXsdInstance());
        xml_schema::namespace_infomap map;
        map[""].name = "http://www.ibm.com/xmlns/prod/streams/spl/function";
        map["cmn"].name = "http://www.ibm.com/xmlns/prod/streams/spl/common";
        map["xsi"].name = "http://www.w3.org/2001/XMLSchema-instance";
        ofstream ostr(outFileName_.c_str(), fstream::out);
        if (!ostr) {
            cerr << "ERROR: Cannot open file '" << outFileName_ << "' for writing" << endl;
            exit(EXIT_FAILURE);
        }
        try {
            xmlns::prod::streams::spl::function::functionModel(ostr, *loadedModel, map);
        } catch (::xsd::cxx::exception& e) {
            cerr << "ERROR: Cannot write xml model to '" << outFileName_ << "'" << endl;
            cerr << "ERROR: " << e.what() << endl;
            exit(EXIT_FAILURE);
        }
        return EXIT_SUCCESS;
    }

  private:
    string inFileName_;
    string outFileName_;
};

MAIN_APP(FunctionModelTest)
