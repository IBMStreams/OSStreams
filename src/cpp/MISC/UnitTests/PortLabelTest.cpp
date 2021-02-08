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

#include <TRANS/PortLabelImpl.h>

#include "TestUtils.h"
#include <UTILS/DistilleryApplication.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/UTILSTypes.h>

#define DBG_ASP "PortLabelTest"

UTILS_NAMESPACE_BEGIN

/**
 * @file PortLabelTest.cpp
 * Multi-threaded test for PortLabel and PortLabelImpl.
 */
class PortLabelTest : public Distillery::DistilleryApplication
{
  public:
    PortLabelTest() {}

    virtual ~PortLabelTest() {}

    void getArguments(option_vector_t& options) {}
    virtual int run(const std::vector<std::string>& /*remains*/);

    // test functions
    void test_portLabel();
    void test_portLabelImpl();
};
UTILS_NAMESPACE_END

/////////////////////////////////////////////////////////////////////////////
UTILS_NAMESPACE_USE

int PortLabelTest::run(const std::vector<std::string>&)
{
    std::cout << "[" << gettid() << "] Testing PortLabel and PortLabelImpl" << std::endl;

    // test start
    uint64_t beginTime = TestUtils::getTimeMillis();

    test_portLabel();
    test_portLabelImpl();

    std::cout << "Success! Test ran for " << (TestUtils::getTimeMillis() - beginTime)
              << " millisecs" << std::endl;
    return 0;
}

void PortLabelTest::test_portLabel()
{
    // PE and Port ID port label
    PortLabel pePl(1L, 5L, 0L);
    ASSERT_EQUALS("1.5@0", pePl.str());
    ASSERT_EQUALS("1.5@0", PortLabel::toString(1L, 5L, 0L));
    ASSERT_EQUALS_MSG("PortLabel::getPEId() retrieves wrong PE id", 1ULL, pePl.getPEId());
    ASSERT_EQUALS_MSG("PortLabel::getPortId() retrieves wrong Port id", 5ULL, pePl.getPortId());
    ASSERT_EQUALS_MSG("PortLabel::getJobId() retrieves wrong Job id", 0ULL, pePl.getJobId());

    // copy constructor
    PortLabel pePl2;
    ASSERT_TRUE(pePl2.empty());
    ASSERT_EQUALS("", pePl2.str());

    // assignment
    PortLabel pePl3 = pePl2;
    ASSERT_TRUE(pePl3.empty());
    ASSERT_EQUALS("", pePl3.str());

    // empty port label
    PortLabel emptyPl;
    ASSERT_TRUE_MSG("emptyPl.empty() must be true", emptyPl.empty());
    ASSERT_EQUALS_MSG("empty label must have empty string representation", "", emptyPl.str());
    ASSERT_TRUE_MSG("PortLabel::isErrorPEId() must indicate error", emptyPl.isErrorPEId());
    ASSERT_TRUE_MSG("PortLabel::isErrorPortId() must indicate error", emptyPl.isErrorPortId());

    PortLabel emptyPl2(emptyPl);
    ASSERT_TRUE(emptyPl2.empty());
    ASSERT_EQUALS("", emptyPl2.str());

    PortLabel emptyPl3 = emptyPl;
    ASSERT_TRUE(emptyPl3.empty());
    ASSERT_EQUALS("", emptyPl3.str());

    // non-PE port label
    PortLabel nonPePl("s1");
    ASSERT_EQUALS("s1", nonPePl.str());

    PortLabel nonPePl2(nonPePl);
    ASSERT_TRUE(!nonPePl2.empty());
    ASSERT_EQUALS("s1", nonPePl2.str());

    PortLabel nonPePl3 = nonPePl;
    ASSERT_TRUE(!nonPePl3.empty());
    ASSERT_EQUALS("s1", nonPePl3.str());

    // assignments
    PortLabel pl4;
    ASSERT_TRUE(pl4.empty());

    pl4 = std::string("s1");
    ASSERT_EQUALS("s1", pl4.str());

    pl4 = "s2";
    ASSERT_EQUALS("s2", pl4.str());

    {
        // parse must fail
        PortLabel pl("1.5a");
        ASSERT_EQUALS(std::string("1.5a"), pl.str());
        ASSERT_TRUE_MSG("PortLabel::isErrorPEId() must indicate error", pl.isErrorPEId());
        ASSERT_TRUE_MSG("PortLabel::isErrorPortId() must indicate error", pl.isErrorPortId());
    }
}

void PortLabelTest::test_portLabelImpl()
{
    {
        // PE and Port ID port label
        PortLabelImpl pePl(1L, 5L, 0L);
        ASSERT_EQUALS("1.5@0", pePl.str());
        ASSERT_EQUALS_MSG("PortLabelImpl::getPEId() retrieves wrong PE id", 1ULL, pePl.getPEId());
        ASSERT_EQUALS_MSG("PortLabelImpl::getPortId() retrieves wrong Port id", 5ULL,
                          pePl.getPortId());
    }

    {
        // cast from PortLabel
        PortLabel pl("1.5@0");
        PortLabelImpl pePl(pl);
        ASSERT_EQUALS(std::string("1.5@0"), pePl.str());
        ASSERT_EQUALS_MSG("PortLabelImpl::getPEId() retrieves wrong PE id", 1ULL, pePl.getPEId());
        ASSERT_EQUALS_MSG("PortLabelImpl::getPortId() retrieves wrong Port id", 5ULL,
                          pePl.getPortId());
        ASSERT_EQUALS_MSG("PortLabelImpl::getJobId() retrieves wrong Job id", 0ULL,
                          pePl.getJobId());
    }

    {
        // assignments
        PortLabelImpl pl4;
        ASSERT_TRUE(pl4.empty());

        pl4 = std::string("s1");
        ASSERT_EQUALS("s1", pl4.str());

        pl4 = "s2";
        ASSERT_EQUALS("s2", pl4.str());
        ASSERT_TRUE_MSG("PortLabel::isErrorPEId() must indicate error", pl4.isErrorPEId());
        ASSERT_TRUE_MSG("PortLabel::isErrorPortId() must indicate error", pl4.isErrorPortId());

        pl4 = "1.5@0";
        ASSERT_EQUALS_MSG("PortLabelImpl::getPEId() retrieves wrong PE id", 1ULL, pl4.getPEId());
        ASSERT_EQUALS_MSG("PortLabelImpl::getPortId() retrieves wrong Port id", 5ULL,
                          pl4.getPortId());
    }
}

MAIN_APP(Distillery::PortLabelTest)
