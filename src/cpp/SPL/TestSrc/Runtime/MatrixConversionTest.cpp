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

#include <SPL/Runtime/Function/BoostMatrixHelper.h>
#include <SPL/Runtime/Function/SPLFunctions.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <UTILS/DistilleryApplication.h>

#include <boost/numeric/ublas/io.hpp>

#include <algorithm>

using namespace std;
using namespace SPL;
using namespace Distillery;

namespace SPL {

class MatrixConversionTest : public DistilleryApplication
{
  public:
    MatrixConversionTest() {}

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        list<list<double> > lm;
        lm.push_back(list<double>());
        lm.push_back(list<double>());
        lm[0].push_back(11);
        lm[0].push_back(12);
        lm[0].push_back(13);
        lm[1].push_back(21);
        lm[1].push_back(22);
        lm[1].push_back(23);

        ostringstream ostr;

        using namespace boost::numeric::ublas;

        // small matrix, from large list
        matrix<double> m(0, 0);
        SPL::Functions::Math::Compat::convertFromNestedListToUblasMatrix(m, lm);
        ostr.str("");
        ostr << m;
        FASSERT(ostr.str() == "[2,3]((11,12,13),(21,22,23))");

        // large matrix, from small list
        list<list<double> > lm1(lm);
        lm1.pop_back();
        SPL::Functions::Math::Compat::convertFromNestedListToUblasMatrix(m, lm1);
        ostr.str("");
        ostr << m;
        FASSERT(ostr.str() == "[1,3]((11,12,13))");
        lm1[0].pop_back();
        SPL::Functions::Math::Compat::convertFromNestedListToUblasMatrix(m, lm1);
        ostr.str("");
        ostr << m;
        FASSERT(ostr.str() == "[1,2]((11,12))");

        // back to original
        SPL::Functions::Math::Compat::convertFromNestedListToUblasMatrix(m, lm);

        // large matrix, to small list
        list<list<double> > lm2;
        SPL::Functions::Math::Compat::convertToNestedListFromUblasMatrix(lm2, m);
        ostr.str("");
        ostr << lm2;
        FASSERT(ostr.str() == "[[11,12,13],[21,22,23]]");

        // small matrix, to large list
        m.resize(2, 2);
        SPL::Functions::Math::Compat::convertToNestedListFromUblasMatrix(lm2, m);
        ostr.str("");
        ostr << lm2;
        FASSERT(ostr.str() == "[[11,12],[21,22]]");

        return EXIT_SUCCESS;
    }
};

};

MAIN_APP(SPL::MatrixConversionTest)
