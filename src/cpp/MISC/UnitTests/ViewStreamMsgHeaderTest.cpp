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

#include <TRANS/ViewStreamMsgHeader.h>

#include "TestUtils.h"
#include <UTILS/DistilleryApplication.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/UTILSTypes.h>

#include <limits>

#define DBG_ASP "ViewStreamMsgHeaderTest"

UTILS_NAMESPACE_BEGIN

// Remove "unused variable" compilation warnings
#define LOGGER "ViewStreamMsgHeaderTest"

#define DEFAULT_EXTENDED_HEADER_SZ (8 + 1 + 4)
/**
 * @file ViewStreamMsgHeaderTest.cpp
 * Unit test for ViewStreamMsgHeader.
 */
class ViewStreamMsgHeaderTest : public Distillery::DistilleryApplication
{
  public:
    ViewStreamMsgHeaderTest() {}
    virtual ~ViewStreamMsgHeaderTest() {}

    void getArguments(option_vector_t& options) {}
    virtual int run(const std::vector<std::string>& /*remains*/);

    // test functions
    void test_serialize_one();
    void test_serialize_oneErr();
    void test_serialize_multiple();
    void test_serialize_multipleErr();
};
UTILS_NAMESPACE_END

/////////////////////////////////////////////////////////////////////////////
UTILS_NAMESPACE_USE

int ViewStreamMsgHeaderTest::run(const std::vector<std::string>&)
{
    std::cout << "[" << gettid() << "] Testing ViewStreamMsgHeader..." << std::endl;
    SPCDBG(L_DEBUG, "ViewStreamMsgHeaderTest::run", LOGGER);

    // test start
    uint64_t beginTime = TestUtils::getTimeMillis();

    test_serialize_one();
    test_serialize_oneErr();
    test_serialize_multiple();
    test_serialize_multipleErr();

    std::cout << "Success! Test ran for " << (TestUtils::getTimeMillis() - beginTime)
              << " millisecs" << std::endl;
    return 0;
}

/**
 * Tests successful code paths.
 */
void ViewStreamMsgHeaderTest::test_serialize_one()
{
    char buf[256];
    int ec = ViewStreamMsgHeader::E_SUCCESS;
    // value for an unassigned channel index
    int noChannelIndex = -1;
    {
        // serialize
        ec = ViewStreamMsgHeader::E_SUCCESS;
        ViewStreamMsgHeader* hdr = ViewStreamMsgHeader::serialize(buf, 256, -1, 1, ec);
        ASSERT_EQUALS((int)ViewStreamMsgHeader::E_SUCCESS, ec);
        hdr->setViewId(2ULL);
        hdr->setSequenceNumber(3);
        SPCDBG(L_DEBUG, "ViewStreamMsgHeader::size(1)=" << ViewStreamMsgHeader::size(1), LOGGER);
        ASSERT_EQUALS((size_t)24 + DEFAULT_EXTENDED_HEADER_SZ, ViewStreamMsgHeader::size(1));
        SPCDBG(L_DEBUG, "hdr->size()=" << hdr->size(), LOGGER);
        ASSERT_EQUALS((size_t)24 + DEFAULT_EXTENDED_HEADER_SZ, hdr->size());
    }
    {
        // deserialize and verify
        SPCDBG(L_DEBUG, "ViewStreamMsgHeader::deserialize((const char*)buf, size, ec)", LOGGER);
        ec = ViewStreamMsgHeader::E_SUCCESS;
        const ViewStreamMsgHeader* hdr =
          ViewStreamMsgHeader::deserialize((const char*)buf, 256, ec);
        ASSERT_EQUALS((int)ViewStreamMsgHeader::E_SUCCESS, ec);
        ASSERT_EQUALS((size_t)1, hdr->getNumViewIds());
        ASSERT_EQUALS(2ULL, hdr->getViewId());
        ASSERT_EQUALS(3ULL, hdr->getSequenceNumber());
        ASSERT_EQUALS(noChannelIndex, hdr->getChannelIndex()); // unassigned channel index
        ASSERT_EQUALS((size_t)24 + DEFAULT_EXTENDED_HEADER_SZ, ViewStreamMsgHeader::size(1));
        ASSERT_EQUALS((size_t)24 + DEFAULT_EXTENDED_HEADER_SZ, hdr->size());
        SPCDBG(L_INFO, "Header is " << *hdr, LOGGER);
    }
    {
        // deserialize
        SPCDBG(L_DEBUG, "ViewStreamMsgHeader::deserialize((const char*)buf)", LOGGER);
        const ViewStreamMsgHeader* hdr = ViewStreamMsgHeader::deserialize((const char*)buf);
        ASSERT_EQUALS((size_t)1, hdr->getNumViewIds());
        ASSERT_EQUALS(2ULL, hdr->getViewId());
        ASSERT_EQUALS(3ULL, hdr->getSequenceNumber());
        ASSERT_EQUALS(noChannelIndex, hdr->getChannelIndex()); // unassigned channel index
        ASSERT_EQUALS((size_t)24 + DEFAULT_EXTENDED_HEADER_SZ, ViewStreamMsgHeader::size(1));
        ASSERT_EQUALS((size_t)24 + DEFAULT_EXTENDED_HEADER_SZ, hdr->size());
    }
}

/**
 * Tests error code paths.
 */
void ViewStreamMsgHeaderTest::test_serialize_oneErr()
{
    char buf[256];
    int ec = ViewStreamMsgHeader::E_SUCCESS;
    {
        // serialize
        SPCDBG(L_DEBUG, "ViewStreamMsgHeader::serialize(buf, 256, 1, ec)", LOGGER);
        ec = ViewStreamMsgHeader::E_SUCCESS;
        ViewStreamMsgHeader* hdr = ViewStreamMsgHeader::serialize(buf, 256, -1, 1, ec);
        ASSERT_EQUALS((int)ViewStreamMsgHeader::E_SUCCESS, ec);
        hdr->setViewId(2ULL);
        hdr->setSequenceNumber(3);
        ASSERT_EQUALS((size_t)24 + DEFAULT_EXTENDED_HEADER_SZ, ViewStreamMsgHeader::size(1));
        ASSERT_EQUALS((size_t)24 + DEFAULT_EXTENDED_HEADER_SZ, hdr->size());
    }

    // error paths
    {
        // serialize to NULL
        ec = ViewStreamMsgHeader::E_SUCCESS;
        ViewStreamMsgHeader* hdr = ViewStreamMsgHeader::serialize(NULL, 256, -1, 1, ec);
        ASSERT_EQUALS((int)ViewStreamMsgHeader::E_NULL, ec);
        ASSERT_TRUE(NULL == hdr); // avoid an "unused variable" compiler warning
    }
    {
        // serialize to zero-length buffer
        ec = ViewStreamMsgHeader::E_SUCCESS;
        ViewStreamMsgHeader* hdr = ViewStreamMsgHeader::serialize(buf, 0, -1, 1, ec);
        ASSERT_EQUALS((int)ViewStreamMsgHeader::E_SIZE, ec);
        ASSERT_TRUE(NULL != hdr); // avoid an "unused variable" compiler warning
    }
    {
        // serialize to small buffer (no room for header with 1 views)
        size_t size = ViewStreamMsgHeader::size(1);
        ASSERT_EQUALS((size_t)24 + DEFAULT_EXTENDED_HEADER_SZ, size);

        ec = ViewStreamMsgHeader::E_SUCCESS;
        ViewStreamMsgHeader* hdr = ViewStreamMsgHeader::serialize(buf, size - 1, -1, 1, ec);
        ASSERT_EQUALS((int)ViewStreamMsgHeader::E_SIZE, ec);
        ASSERT_TRUE(NULL != hdr); // avoid an "unused variable" compiler warning
    }

    {
        // deserialize from NULL
        ec = ViewStreamMsgHeader::E_SUCCESS;
        const ViewStreamMsgHeader* hdr = ViewStreamMsgHeader::deserialize(NULL, 256, ec);
        ASSERT_EQUALS((int)ViewStreamMsgHeader::E_NULL, ec);
        ASSERT_TRUE(NULL == hdr); // avoid an "unused variable" compiler warning
    }
    {
        // deserialize from zero-length buffer
        ec = ViewStreamMsgHeader::E_SUCCESS;
        const ViewStreamMsgHeader* hdr = ViewStreamMsgHeader::deserialize((const char*)buf, 0, ec);
        ASSERT_EQUALS((int)ViewStreamMsgHeader::E_SIZE, ec);
        ASSERT_TRUE(NULL != hdr); // avoid an "unused variable" compiler warning
    }
    {
        // deserialize from small buffer (no room for header with 0 views)
        size_t size = ViewStreamMsgHeader::size(0);
        ASSERT_EQUALS((size_t)16 + DEFAULT_EXTENDED_HEADER_SZ, size);

        ec = ViewStreamMsgHeader::E_SUCCESS;
        const ViewStreamMsgHeader* hdr =
          ViewStreamMsgHeader::deserialize((const char*)buf, size - 1, ec);
        ASSERT_EQUALS((int)ViewStreamMsgHeader::E_SIZE, ec);
        ASSERT_TRUE(NULL != hdr); // avoid an "unused variable" compiler warning
    }
    {
        // deserialize from small buffer (no room for header with 1 views)
        size_t size = ViewStreamMsgHeader::size(1);
        ASSERT_EQUALS((size_t)24 + DEFAULT_EXTENDED_HEADER_SZ, size);

        ec = ViewStreamMsgHeader::E_SUCCESS;
        const ViewStreamMsgHeader* hdr =
          ViewStreamMsgHeader::deserialize((const char*)buf, size - 1, ec);
        ASSERT_EQUALS((int)ViewStreamMsgHeader::E_SIZE, ec);
        ASSERT_TRUE(NULL != hdr); // avoid an "unused variable" compiler warning
    }
}

void ViewStreamMsgHeaderTest::test_serialize_multiple()
{
    char buf[256];
    int ec = ViewStreamMsgHeader::E_SUCCESS;
    {
        // serialize multiple ids
        ec = ViewStreamMsgHeader::E_SUCCESS;
        ViewStreamMsgHeader* hdr = ViewStreamMsgHeader::serialize(buf, 256, 4, 3, ec, 3);
        ASSERT_EQUALS((int)ViewStreamMsgHeader::E_SUCCESS, ec);
        hdr->setViewId(2ULL);
        hdr->setViewId(3ULL, 1);
        hdr->setViewId(5ULL, 2);
        hdr->setSequenceNumber(3);

        std::vector<int32_t> allChannels;
        allChannels.push_back(10);
        allChannels.push_back(11);
        allChannels.push_back(12);
        hdr->serializeExtendedHeader(allChannels.data(), allChannels.size());
        ASSERT_EQUALS((size_t)40 + DEFAULT_EXTENDED_HEADER_SZ + (3 * 4),
                      ViewStreamMsgHeader::size(3, 3));
        ASSERT_EQUALS((size_t)40 + DEFAULT_EXTENDED_HEADER_SZ + (3 * 4), hdr->size());
        // std::cout << "test_serialize_multiple serialize1 hdr: " << *hdr << std::endl;
    }
    {
        // deserialize and verify
        ec = ViewStreamMsgHeader::E_SUCCESS;
        const ViewStreamMsgHeader* hdr =
          ViewStreamMsgHeader::deserialize((const char*)buf, 256, ec);
        ASSERT_EQUALS((int)ViewStreamMsgHeader::E_SUCCESS, ec);
        ASSERT_EQUALS((size_t)3, hdr->getNumViewIds());
        ASSERT_EQUALS(2ULL, hdr->getViewId());
        ASSERT_EQUALS(3ULL, hdr->getViewId(1));
        ASSERT_EQUALS(5ULL, hdr->getViewId(2));
        ASSERT_EQUALS(3ULL, hdr->getSequenceNumber());
        ASSERT_EQUALS(4, hdr->getChannelIndex());
        ASSERT_EQUALS((size_t)40 + DEFAULT_EXTENDED_HEADER_SZ + (3 * 4),
                      ViewStreamMsgHeader::size(3, 3));
        ASSERT_EQUALS((size_t)40 + DEFAULT_EXTENDED_HEADER_SZ + (3 * 4), hdr->size());
        // std::cout << "test_serialize_multiple deserialize1 hdr: " << *hdr << std::endl;
    }
    {
        // deserialize don't verify
        const ViewStreamMsgHeader* hdr = ViewStreamMsgHeader::deserialize((const char*)buf);
        ASSERT_EQUALS((size_t)3, hdr->getNumViewIds());
        ASSERT_EQUALS(2ULL, hdr->getViewId());
        ASSERT_EQUALS(3ULL, hdr->getViewId(1));
        ASSERT_EQUALS(5ULL, hdr->getViewId(2));
        ASSERT_EQUALS(3ULL, hdr->getSequenceNumber());
        ASSERT_EQUALS(4, hdr->getChannelIndex());
        ASSERT_EQUALS((size_t)40 + DEFAULT_EXTENDED_HEADER_SZ + (3 * 4),
                      ViewStreamMsgHeader::size(3, 3));
        ASSERT_EQUALS((size_t)40 + DEFAULT_EXTENDED_HEADER_SZ + (3 * 4), hdr->size());
        // std::cout << "test_serialize_multiple deserialize2 hdr: " << *hdr << std::endl;
    }
}

void ViewStreamMsgHeaderTest::test_serialize_multipleErr()
{
    char buf[256];
    int ec = ViewStreamMsgHeader::E_SUCCESS;
    {
        // serialize multiple ids
        ec = ViewStreamMsgHeader::E_SUCCESS;
        ViewStreamMsgHeader* hdr = ViewStreamMsgHeader::serialize(buf, 256, -1, 3, ec);
        ASSERT_EQUALS((int)ViewStreamMsgHeader::E_SUCCESS, ec);
        hdr->setViewId(2ULL);
        hdr->setViewId(3ULL, 1);
        hdr->setViewId(5ULL, 2);
        hdr->setSequenceNumber(3);
        ASSERT_EQUALS((size_t)40 + DEFAULT_EXTENDED_HEADER_SZ, ViewStreamMsgHeader::size(3));
        ASSERT_EQUALS((size_t)40 + DEFAULT_EXTENDED_HEADER_SZ, hdr->size());
    }

    // error paths
    {
        // deserialize from small buffer (no room for header with 3 views)
        size_t size = ViewStreamMsgHeader::size(3);
        ASSERT_EQUALS((size_t)40 + DEFAULT_EXTENDED_HEADER_SZ, size);

        ec = ViewStreamMsgHeader::E_SUCCESS;
        ViewStreamMsgHeader* hdr = ViewStreamMsgHeader::serialize(buf, size - 1, -1, 3, ec);
        ASSERT_EQUALS((int)ViewStreamMsgHeader::E_SIZE, ec);
        ASSERT_TRUE(NULL != hdr); // avoid an "unused variable" compiler warning
    }
    {
        // deserialize from small buffer (no room for header with 3 views)
        size_t size = ViewStreamMsgHeader::size(3);

        ec = ViewStreamMsgHeader::E_SUCCESS;
        const ViewStreamMsgHeader* hdr =
          ViewStreamMsgHeader::deserialize((const char*)buf, size - 1, ec);
        ASSERT_EQUALS((int)ViewStreamMsgHeader::E_SIZE, ec);
        ASSERT_TRUE(NULL != hdr); // avoid an "unused variable" compiler warning
    }
}

MAIN_APP(Distillery::ViewStreamMsgHeaderTest)
