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
#include <SPL/Core/EnumerationModelImpl.h>
#include <UTILS/DistilleryApplication.h>

#include <fstream>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace SPL::Enumeration_;

class EnumerationModelTest : public UTILS_NAMESPACE::DistilleryApplication
{
  public:
    EnumerationModelTest() {}

    void getArguments(option_vector_t& options) { setKeepRemainingArguments(); }

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        EnumerationModel model;
        populateModel(model);
        auto_ptr<enumerationModelType> modelXml = model.toXsdInstance();
        xml_schema::namespace_infomap map;
        map[""].name = "http://www.ibm.com/xmlns/prod/streams/spl/enumeration";
        map["xsi"].name = "http://www.w3.org/2001/XMLSchema-instance";
        xmlns::prod::streams::spl::enumeration::enumerationModel(cout, *modelXml, map);
        return EXIT_SUCCESS;
    }

    void populateModel(EnumerationModel& model)
    {
        // Set a class name
        model.getClass() = "LogLevel";

        // Add four enumerators
        for (unsigned i = 0; i < 4; ++i) {
            model.getEnumerators().push_back(EnumeratorPtr(new Enumerator()));
        }

        // Populate the enumerators
        model.getEnumerators()[0]->getName() = "error";
        model.getEnumerators()[1]->getName() = "info";
        model.getEnumerators()[2]->getName() = "debug";
        model.getEnumerators()[3]->getName() = "trace";
    }
};

MAIN_APP(EnumerationModelTest)
