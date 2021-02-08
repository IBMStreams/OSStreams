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
#include <SPL/Core/PEModelImpl.h>
#include <UTILS/DistilleryApplication.h>

#include <fstream>
#include <string>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace SPL::PE;

class PEModelTest : public UTILS_NAMESPACE::DistilleryApplication
{
  public:
    PEModelTest() {}

    void getArguments(option_vector_t& options) { setKeepRemainingArguments(); }

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        PEModel model;
        populateModel(model);
        auto_ptr<peModelType> modelXml = model.toXsdInstance();
        xml_schema::namespace_infomap map;
        map[""].name = "http://www.ibm.com/xmlns/prod/streams/spl/pe";
        map["xsi"].name = "http://www.w3.org/2001/XMLSchema-instance";
        xmlns::prod::streams::spl::pe::peModel(cout, *modelXml, map);
        return EXIT_SUCCESS;
    }

    void populateModel(PEModel& model)
    {
        populateContext(model.getContext());

        model.getOperators().push_back(OperatorPtr(new Operator()));
        model.getOperators().push_back(OperatorPtr(new Operator()));
        populateOperator0(*model.getOperators()[0]);
        populateOperator1(*model.getOperators()[1]);
    }

    void populateContext(Context& context) { context.getClass() = "PE_Merhaba"; }

    void populateOperator0(Operator& oper) { oper.getClass() = "BIOP_Merhaba1"; }

    void populateOperator1(Operator& oper) { oper.getClass() = "BIOP_Merhaba2"; }

  private:
    string fileName_;
};

MAIN_APP(PEModelTest)
