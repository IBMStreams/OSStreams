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

#include <Runtime/Operator/EventTime/EventTimeContextImpl.h>
#include <Runtime/Operator/EventTime/WatermarkReceiver.h>
#include <Runtime/Operator/EventTime/WatermarkReceiverImpl.h>

#include <SPL/TestSrc/Utility/TestUtils.h>

using namespace std;
using namespace Distillery;

namespace SPL {

const static std::string WM_TEST = ":::WatermarkReceiverTest";

/////////////////////////////////////////////////////////////////////////////
// Extend classes under test to force initialization

class OneWmReceiver : public OneStreamWatermarkReceiver
{
  public:
    explicit OneWmReceiver(WatermarkReceiver::WmEntryType const& entry)
      : OneStreamWatermarkReceiver()
    {
        wmStorage_ = entry;
    }
    ~OneWmReceiver() {}
};

class TwoWmReceiver : public TwoStreamWatermarkReceiver
{
  public:
    TwoWmReceiver(WatermarkReceiver::WmEntryType const& e1,
                  WatermarkReceiver::WmEntryType const& e2)
      : TwoStreamWatermarkReceiver()
    {
        wmStorage_[0] = e1;
        wmStorage_[1] = e2;
    }
    ~TwoWmReceiver() {}
};

class MultiWmReceiver : public MultiStreamWatermarkReceiver
{
  public:
    MultiWmReceiver(WatermarkReceiver::WmEntryType const* entries, size_t entryCount)
      : MultiStreamWatermarkReceiver(entryCount)
    {
        for (size_t i = 0; i < entryCount; i++) {
            wmStorage_->operator[](i) = entries[i];
        }
        init();
    }
    ~MultiWmReceiver() {}
};

class ZeroWmReceiver : public ZeroStreamWatermarkReceiver
{
  public:
    explicit ZeroWmReceiver()
      : ZeroStreamWatermarkReceiver()
    {}
    ~ZeroWmReceiver() {}
};

/////////////////////////////////////////////////////////////////////////////

class WatermarkReceiverTest : public TestBase
{
  public:
    const int a;

    WatermarkReceiverTest()
      : TestBase()
      , a(0)
    {}

  private:
    static PayloadContainer* getPunctPayloadContainer(Punctuation& punct)
    {
        PayloadContainer* payloadC = punct.getPayloadContainer();
        if (payloadC == NULL) {
            payloadC = new PayloadContainer();
            punct.setPayloadContainer(payloadC);
        }
        return payloadC;
    }

    static WatermarkPunctPayload* getPayload(PayloadContainer& payloadC)
    {
        // Create payload only if punctuation does not already have it
        Payload* payload = payloadC.find(WatermarkPunctPayload::name);
        if (payload == NULL) {
            payload = &(PayloadContainer::createPayload(WatermarkPunctPayload::name));

            // PayloadContainer::add() moves 'payload' rather than making a copy
            payloadC.add(WatermarkPunctPayload::name, *payload);
        }
        return static_cast<WatermarkPunctPayload*>(payload);
    }

    static WatermarkPunctPayload* getPayload(Punctuation& punct)
    {
        PayloadContainer* payloadC = getPunctPayloadContainer(punct);
        return getPayload(*payloadC);
    }

    static Punctuation& setStreamId(Punctuation& punct, WatermarkReceiver::ConnectionId id)
    {
        WatermarkPunctPayload* payload = getPayload(punct);
        payload->setJobId(id.jobId);
        payload->setOperatorInstanceId(id.operatorIndex);
        payload->setOperatorOPortIdx(id.oportIndex);
        return punct;
    }

    static Punctuation& setWatermark(Punctuation& p, Watermark const& wm)
    {
        EventTimeContext::impl::setWatermark(p, wm);
        return p;
    }

    void runTests()
    {
        test_ZeroWmReceiver_0();
        test_OneWmReceiver_0();
        test_OneWmReceiver_1();
        test_OneWmReceiver_2();
        test_TwoWmReceiver_0();
        test_MultiWmReceiver_0();
    }

    void test_ZeroWmReceiver_0()
    {
        SPCDBG_ENTER(WM_TEST);

        ZeroWmReceiver rec = ZeroWmReceiver();
        ASSERT_EQUALS(std::numeric_limits<Watermark>::max(), rec.getWatermark());

        // No input should modify the receiver's watermark
        Punctuation punct = Punctuation(Punctuation::WatermarkMarker);
        EventTimeContext::impl::setWatermark(punct, Watermark(1000000));
        ASSERT_EQUALS(Watermark(), rec.receive(punct, 0));
        ASSERT_EQUALS(std::numeric_limits<Watermark>::max(), rec.getWatermark());

        SPCDBG_EXIT(WM_TEST);
    }

    void test_OneWmReceiver_0()
    {
        SPCDBG_ENTER(WM_TEST);

        // Receive from <Op,Port>={0,0}
        Watermark wmInit;
        WatermarkReceiver::WmEntryType entry =
          std::make_pair(WatermarkReceiver::ConnectionId(0, 0, 0, 0), wmInit);
        OneWmReceiver rec = OneWmReceiver(entry);

        Punctuation punct = Punctuation(Punctuation::WatermarkMarker);
        EventTimeContext::impl::setWatermark(punct, Watermark());
        rec.receive(punct, 0);

        ASSERT_EQUALS(Watermark(), rec.getWatermark());

        SPCDBG_EXIT(WM_TEST);
    }

    void test_OneWmReceiver_1()
    {
        SPCDBG_ENTER(WM_TEST);

        // Receive from <Op,Port>={0,0}
        WatermarkReceiver::WmEntryType entry =
          std::make_pair(WatermarkReceiver::ConnectionId(0, 0, 0, 0), Watermark());
        OneWmReceiver rec = OneWmReceiver(entry);

        Punctuation punct = Punctuation(Punctuation::WatermarkMarker);
        setStreamId(punct, WatermarkReceiver::ConnectionId(0, 0, 0, 0));

        ASSERT_EQUALS(Watermark(0), rec.receive(setWatermark(punct, Watermark()), 0));
        ASSERT_EQUALS(Watermark(2), rec.receive(setWatermark(punct, Watermark(2)), 0));

        // WM back in time WM=1 after WM=2
        try {
            ASSERT_EQUALS(Watermark(0), rec.receive(setWatermark(punct, Watermark(1)), 0));
            SPCDBG(L_DEBUG, "Ignored back in time Watermark from {0,0}", WM_TEST);
        } catch (SPLRuntimeException const& e) {
            FAIL("Caught SPLRuntimeException for receiving back in time Watermark from {0,0}: "
                 << e.getExplanation());
        }

        ASSERT_EQUALS(Watermark(2), rec.getMinWatermark());

        ASSERT_EQUALS(Watermark(3), rec.receive(setWatermark(punct, Watermark(3)), 0));

        ASSERT_EQUALS(Watermark(3), rec.getMinWatermark());

        ASSERT_EQUALS(Watermark(3), rec.getWatermark());

        WatermarkPunctPayload* payload = getPayload(punct);
        try {
            // Receive WM from unknown operator
            payload->setOperatorInstanceId(1);
            payload->setOperatorOPortIdx(0);
            rec.receive(punct, 0);
            FAIL("Expected SPLRuntimeInvalidIndexException for receiving Punctuation from "
                 "unexpected operatorIdx=1");
        } catch (SPLRuntimeInvalidIndexException const& e1) {
            SPCDBG(L_DEBUG,
                   "Caught expected SPLRuntimeInvalidIndexException: " << e1.getExplanation(),
                   WM_TEST);

            try {
                // Receive WM from unknown port
                payload->setOperatorInstanceId(0);
                payload->setOperatorOPortIdx(1);
                rec.receive(punct, 0);
                FAIL("Expected SPLRuntimeInvalidIndexException for receiving Punctuation from "
                     "unexpected oportIdx=1");
            } catch (SPLRuntimeInvalidIndexException const& e2) {
                SPCDBG(L_DEBUG,
                       "Caught expected SPLRuntimeInvalidIndexException: " << e2.getExplanation(),
                       WM_TEST);
            }
        }
        SPCDBG_EXIT(WM_TEST);
    }

    void test_OneWmReceiver_2()
    {
        SPCDBG_ENTER(WM_TEST);

        // Receive from <Op,Port>={0,0}
        Watermark wmInit;
        WatermarkReceiver::WmEntryType entry =
          std::make_pair(WatermarkReceiver::ConnectionId(0, 0, 0, 0), wmInit);
        OneWmReceiver rec = OneWmReceiver(entry);

        Punctuation punct = Punctuation(Punctuation::WatermarkMarker);
        EventTimeContext::impl::setWatermark(punct, std::numeric_limits<Watermark>::max());
        rec.receive(punct, 0);

        SPCDBG_EXIT(WM_TEST);
    }

    /* Test time line:
     *
     * stream   {0,0}: 0     3   4   6
     * stream   {1,0}:   1 4   3!  4   5 6
     * Expected minWm: 0 0 0 3 3 4 4 4 5 6
     * --------------------------------------------> time
     */
    void test_TwoWmReceiver_0()
    {
        SPCDBG_ENTER(WM_TEST);

        // Receive from <Op,Port> = {{0,0}, {1,0})
        TwoWmReceiver rec =
          TwoWmReceiver(std::make_pair(WatermarkReceiver::ConnectionId(0, 0, 0, 0), Watermark()),
                        std::make_pair(WatermarkReceiver::ConnectionId(0, 1, 0, 0), Watermark()));

        Punctuation p0 = Punctuation(Punctuation::WatermarkMarker);
        setStreamId(p0, WatermarkReceiver::ConnectionId(0, 0, 0, 0));

        Punctuation p1 = Punctuation(Punctuation::WatermarkMarker);
        setStreamId(p1, WatermarkReceiver::ConnectionId(0, 1, 0, 0));

        ASSERT_EQUALS(Watermark(0), rec.receive(setWatermark(p0, Watermark()), 0));
        ASSERT_EQUALS(Watermark(0), rec.receive(setWatermark(p1, Watermark(1)), 0));
        ASSERT_EQUALS(Watermark(0), rec.receive(setWatermark(p1, Watermark(4)), 0));
        ASSERT_EQUALS(Watermark(3), rec.receive(setWatermark(p0, Watermark(3)), 0));

        try {
            // Receive backward watermark WM=3 after WM=4 from operator 1
            rec.receive(setWatermark(p1, Watermark(3)), 0);
            SPCDBG(L_DEBUG, "Ignored back in time Watermark", WM_TEST);
        } catch (SPLRuntimeException const& e) {
            FAIL("Caught SPLRuntimeException for receiving back in time Watermark: "
                 << e.getExplanation());
        }

        EventTimeContext::impl::setWatermark(p0, Watermark(4));
        ASSERT_EQUALS(Watermark(4), rec.receive(setWatermark(p0, Watermark(4)), 0));

        // OK to receive two equal watermarks on same stream?
        ASSERT_EQUALS(Watermark(0), rec.receive(setWatermark(p1, Watermark(4)), 0));

        ASSERT_EQUALS(Watermark(0), rec.receive(setWatermark(p0, Watermark(6)), 0));

        ASSERT_EQUALS(Watermark(5), rec.receive(setWatermark(p1, Watermark(5)), 0));

        ASSERT_EQUALS(Watermark(5), rec.getMinWatermark());

        ASSERT_EQUALS(Watermark(6), rec.receive(setWatermark(p1, Watermark(6)), 0));

        ASSERT_EQUALS(Watermark(6), rec.getWatermark());

        SPCDBG_EXIT(WM_TEST);
    }

    /* Test time line:
     *
     * stream   {0,0}: 0         4           6
     * stream   {1,0}:   1         4   2!  6
     * stream   {2,0}:     0   3         4     7
     * stream   {2,1}:       1       4           7
     * Expected minWm: 0 0 0 0 0 1 1 3 3 4 4 4 4 6
     * ------------------------------------------------------> time
     */
    void test_MultiWmReceiver_0()
    {
        SPCDBG_ENTER(WM_TEST);

        // Receive watermarks from <Op,Port> = {{0,0}, {1,0}, {2,0}, {2,1})
        WatermarkReceiver::WmEntryType storage[4] = {
            std::make_pair(WatermarkReceiver::ConnectionId(0, 0, 0, 0), Watermark()),
            std::make_pair(WatermarkReceiver::ConnectionId(0, 1, 0, 0), Watermark()),
            std::make_pair(WatermarkReceiver::ConnectionId(0, 2, 0, 0), Watermark()),
            std::make_pair(WatermarkReceiver::ConnectionId(0, 2, 1, 0), Watermark())
        };
        MultiWmReceiver rec(storage, 4);

        Punctuation p0 = Punctuation(Punctuation::WatermarkMarker);
        setStreamId(p0, WatermarkReceiver::ConnectionId(0, 0, 0, 0));

        Punctuation p1 = Punctuation(Punctuation::WatermarkMarker);
        setStreamId(p1, WatermarkReceiver::ConnectionId(0, 1, 0, 0));

        Punctuation p2 = Punctuation(Punctuation::WatermarkMarker);
        setStreamId(p2, WatermarkReceiver::ConnectionId(0, 2, 0, 0));

        Punctuation p3 = Punctuation(Punctuation::WatermarkMarker);
        setStreamId(p3, WatermarkReceiver::ConnectionId(0, 2, 1, 0));

        ASSERT_EQUALS(Watermark(0), rec.receive(setWatermark(p0, Watermark(0)), 0));
        ASSERT_EQUALS(Watermark(0), rec.receive(setWatermark(p1, Watermark(1)), 0));
        ASSERT_EQUALS(Watermark(0), rec.receive(setWatermark(p2, Watermark(0)), 0));
        ASSERT_EQUALS(Watermark(0), rec.receive(setWatermark(p3, Watermark(1)), 0));
        ASSERT_EQUALS(Watermark(0), rec.receive(setWatermark(p2, Watermark(3)), 0));

        ASSERT_EQUALS(Watermark(1), rec.receive(setWatermark(p0, Watermark(4)), 0));
        ASSERT_EQUALS(Watermark(0), rec.receive(setWatermark(p1, Watermark(4)), 0));

        ASSERT_EQUALS(Watermark(3), rec.receive(setWatermark(p3, Watermark(4)), 0));

        try {
            // Receive backward watermark WM=2 after WM=4 on stream 1
            rec.receive(setWatermark(p1, Watermark(2)), 0);
            SPCDBG(L_DEBUG, "Ignored back in time Watermark", WM_TEST);
            ASSERT_EQUALS(Watermark(3), rec.getMinWatermark());
        } catch (SPLRuntimeException const& e) {
            FAIL("Caught SPLRuntimeException for receiving back in time Watermark: "
                 << e.getExplanation());
        }

        ASSERT_EQUALS(Watermark(4), rec.receive(setWatermark(p2, Watermark(4)), 0));
        ASSERT_EQUALS(Watermark(0), rec.receive(setWatermark(p1, Watermark(6)), 0));
        ASSERT_EQUALS(Watermark(0), rec.receive(setWatermark(p0, Watermark(6)), 0));
        ASSERT_EQUALS(Watermark(0), rec.receive(setWatermark(p2, Watermark(7)), 0));

        ASSERT_EQUALS(Watermark(6), rec.receive(setWatermark(p3, Watermark(7)), 0));

        ASSERT_EQUALS(Watermark(6), rec.getMinWatermark());
        ASSERT_EQUALS(Watermark(6), rec.getWatermark());

        SPCDBG_EXIT(WM_TEST);
    }
};
} // end namespace SPL

MAIN_APP(SPL::WatermarkReceiverTest)
