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

#define SPL_TMP_TUPLE
#include "SPL/Core/PathSearch.h"
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/ProbePointExpression.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/Runtime/Type/Meta/BaseType.h>
#include <SPL/Runtime/Type/SPLType.h>

#include <UTILS/DistilleryApplication.h>

#include <ostream>
using namespace std;
using namespace SPL;
using namespace Distillery;

namespace SPL {

class ProbePointExpressionTest : public CompilerApp
{
  public:
    ProbePointExpressionTest() {}

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        testSimple();

        return EXIT_SUCCESS;
    }

    MAKE_SPL_TUPLE_FIELD(name)
    MAKE_SPL_TUPLE_FIELD(age)

    void testSimple()
    {

        typedef tuple<rstring FIELD(name), int32 FIELD(age)> PersonTuple;

        PersonTuple i1;
        rstring& name1 = i1.getFIELD(name);
        int32& age1 = i1.getFIELD(age);
        name1 = "Roch";
        age1 = 50;

        const Meta::TupleType& personTupleType =
          dynamic_cast<const Meta::TupleType&>(Meta::BaseType::makeType(i1));

        BaseProbePointExpression::ContextDefinition defContext;
        ProbePointExpression probe1("age <= 50", personTupleType, defContext);

        FASSERT(probe1.valid());

        BaseProbePointExpression::ContextEvaluation context;
        FASSERT(probe1.evaluate(i1, context));

        age1 = 51;
        FASSERT(!probe1.evaluate(i1, context));
    }
};

};

MAIN_APP(SPL::ProbePointExpressionTest)
