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

#include <Runtime/Operator/EventTime/WatermarkReceiver.h>
#include <Runtime/Operator/EventTime/WatermarkReceiverImpl.h>
#include <SAM/applicationModel.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>

using namespace SPL;

/////////////////////////////////////////////////////////////////////////////
WatermarkReceiver::~WatermarkReceiver() {}

WatermarkReceiver* WatermarkReceiver::newInstance(OperatorContextImpl const& ctx,
                                                  EventTimeContextSetup const& setup)
{
    using namespace xmlns::prod::streams::application;

    typedef operInstanceInputPortType opIPort_type;

    // get the list of input connections enabled for event-time context
    std::list<EventTimeContextSetup::InputConnection> const& enabledInputConnections =
      setup.inputConnections;

    OPModel const& opmod = ctx.getOperatorModel();

    const operInstanceInputPortsType::inputPort_sequence& iports = opmod.inputPorts().inputPort();
    size_t numPorts = iports.size();

    size_t count = 0;
    for (uint32_t i = 0; i < numPorts; i++) {
        operInstanceInputPortType const& port = iports[i];

        EventTimeContextSetup::InputConnection thisConnection;
        thisConnection.inputPortIndex = port.index();

        const opIPort_type::intraPeConnection_sequence& conns2 = port.intraPeConnection();
        opIPort_type::intraPeConnection_const_iterator it2;
        for (it2 = conns2.begin(); it2 != conns2.end(); it2++) {

            thisConnection.connectedOperatorIndex = it2->operInstanceIndex();
            thisConnection.connectedPortIndex = it2->oportIndex();

            if (std::find(enabledInputConnections.begin(), enabledInputConnections.end(),
                          thisConnection) != enabledInputConnections.end()) {
                ++count;
            }
        }

        const opIPort_type::interPeConnection_sequence& conns3 = port.interPeConnection();
        opIPort_type::interPeConnection_const_iterator it3;
        for (it3 = conns3.begin(); it3 != conns3.end(); it3++) {

            thisConnection.connectedOperatorIndex = it3->operInstanceIndex();
            thisConnection.connectedPortIndex = it3->oportIndex();
            if (std::find(enabledInputConnections.begin(), enabledInputConnections.end(),
                          thisConnection) != enabledInputConnections.end()) {
                ++count;
            }
        }
    }

    /* Use APPTRC. SPLAPPTRC tries to read the current operator name using
     * PEImpl::getOperatorName(), which fails until after the operator has
     * been initialized and the PE has added its name to an internal list.
     */
    APPTRC(L_DEBUG,
           "Operator " << ctx.getName() << " receives " << count
                       << " event-time enabled stream connections",
           SPL_EVENTTIME_DBG);
    if (count == 0) {
        return new ZeroStreamWatermarkReceiver(ctx.getPE().getJobId());
    }
    if (count == 1) {
        return new OneStreamWatermarkReceiver(ctx, setup);
    } else if (count == 2) {
        return new TwoStreamWatermarkReceiver(ctx, setup);
    } else {
        return new MultiStreamWatermarkReceiver(ctx, setup, count);
    }
}

/////////////////////////////////////////////////////////////////////////////
ZeroStreamWatermarkReceiver::ZeroStreamWatermarkReceiver(uint64_t jobId)
  : WatermarkReceiverBase<ZeroStreamWatermarkReceiver>(
      jobId,
      std::numeric_limits<Watermark>::max()) // watermark never advances
{}

Watermark ZeroStreamWatermarkReceiver::receive(Punctuation const& punct, uint32_t inputPort)
{
    SPLAPPTRC(L_WARN, "Unexpected input watermark punctuation: " << punct,
              splAppTrcAspect(SPL_EVENTTIME_DBG));
    return Watermark(); // input watermark did not advance
}

OneStreamWatermarkReceiver::OneStreamWatermarkReceiver(OperatorContextImpl const& ctx,
                                                       EventTimeContextSetup const& setup)
  : WatermarkReceiverBase<OneStreamWatermarkReceiver>(ctx.getPE().getJobId())
{
    initStorage(ctx, setup, 1);
}

TwoStreamWatermarkReceiver::TwoStreamWatermarkReceiver(OperatorContextImpl const& ctx,
                                                       EventTimeContextSetup const& setup)
  : WatermarkReceiverBase<TwoStreamWatermarkReceiver>(ctx.getPE().getJobId())
{
    initStorage(ctx, setup, 2);
}

MultiStreamWatermarkReceiver::MultiStreamWatermarkReceiver(OperatorContextImpl const& ctx,
                                                           EventTimeContextSetup const& setup,
                                                           size_t streamCount)
  : WatermarkReceiverBase<MultiStreamWatermarkReceiver>(ctx.getPE().getJobId())
  , wmStorage_(new std::vector<WmEntryType>(streamCount))
  , wmMap_(streamCount)
  , wmHeap_(streamCount)
{
    assert(streamCount > 2);
    initStorage(ctx, setup, streamCount);
    init();
}

OneStreamWatermarkReceiver::OneStreamWatermarkReceiver()
  : WatermarkReceiverBase<OneStreamWatermarkReceiver>(0)
{}

TwoStreamWatermarkReceiver::TwoStreamWatermarkReceiver()
  : WatermarkReceiverBase<TwoStreamWatermarkReceiver>(0)
{}

MultiStreamWatermarkReceiver::MultiStreamWatermarkReceiver(size_t streamCount)
  : WatermarkReceiverBase<MultiStreamWatermarkReceiver>(0)
  , wmStorage_(new std::vector<WmEntryType>(streamCount))
  , wmMap_(streamCount)
  , wmHeap_(streamCount)
{}

void MultiStreamWatermarkReceiver::init()
{
    // Initialize input WM map
    // For each inter PE and intra PE input connection add a watermark map entry
    WmEntryType* pEnd = wmStorage_->data() + wmStorage_->size();
    for (WmEntryType* p = wmStorage_->data(); p < pEnd; ++p) {
        wmMap_.insert(std::make_pair(p->first, p));
    }

#if 0
  for (WatermarkMapType::iterator it = wmMap_.begin(); it != wmMap_.end(); ++it) {
    SPLAPPTRC(L_TRACE, "*** wmMap_ = " << it->first << " -> " << *(it->second), SPL_EVENTTIME_DBG);
  }
#endif

    // Initialize input WM max heap
    WatermarkHeapType::iterator it2 = wmHeap_.begin();
    for (WmEntryType* p = wmStorage_->data(); p < pEnd; ++p, ++it2) {
        *it2 = p;
    }

#if 0
  for (WatermarkHeapType::iterator it = wmHeap_.begin(); it != wmHeap_.end(); ++it) {
    SPLAPPTRC(L_TRACE, "*** wmHeap_ = " << **it, SPL_EVENTTIME_DBG);
  }
#endif
}
