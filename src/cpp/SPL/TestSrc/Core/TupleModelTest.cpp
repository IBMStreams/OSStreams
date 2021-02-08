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
#include <SPL/Core/TupleModelImpl.h>
#include <UTILS/DistilleryApplication.h>

#include <fstream>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace SPL::Tuple_;

class TupleModelTest : public UTILS_NAMESPACE::DistilleryApplication
{
  public:
    TupleModelTest() {}

    void getArguments(option_vector_t& options) { setKeepRemainingArguments(); }

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        TupleModel model;
        populateModel(model);
        auto_ptr<tupleModelType> modelXml = model.toXsdInstance();
        xml_schema::namespace_infomap map;
        map[""].name = "http://www.ibm.com/xmlns/prod/streams/spl/tuple";
        map["xsi"].name = "http://www.w3.org/2001/XMLSchema-instance";
        xmlns::prod::streams::spl::tuple::tupleModel(cout, *modelXml, map);
        return EXIT_SUCCESS;
    }

    void populateModel(TupleModel& model)
    {
        // Set a class name
        model.getClass() = "myTupleClass";

        // it isn't a facade tuple
        model.getFacade() = true;

        // Add two headers
        model.getHeaders().push_back("hisTupleClass.h");
        model.getHeaders().push_back("herTupleClass.h");

        // Add two attributes
        model.getAttributes().push_back(AttributePtr(new Attribute()));
        model.getAttributes().push_back(AttributePtr(new Attribute()));

        // Populate the attributes
        populateAttribute0(*model.getAttributes()[0]);
        populateAttribute1(*model.getAttributes()[1]);
    }

    void populateAttribute0(Attribute& attribute)
    {
        attribute.getName() = "t1";
        attribute.getCPPType() = "hisTupleClass";
        attribute.setFixedSize(20);
    }

    void populateAttribute1(Attribute& attribute)
    {
        attribute.getName() = "t2";
        attribute.getCPPType() = "string";
    }
};

MAIN_APP(TupleModelTest)
