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

#include <NAM/NAM_NameService.h>
#include <TRANS/TCPInstance.h>
#include <TRANS/TCPSender.h>
#include <TRANS/ViewStreamMsgHeader.h>
#include <TRANS/ViewStreamSender.h>
#include <UTILS/Security.h>

#include <iostream>

#define TRANSPORT_DBG "transport"
#define VIEW_SMALL_BUFFER 256

using namespace std;
NAM_NAMESPACE_USE
UTILS_NAMESPACE_USE

ViewStreamSender* ViewStreamSender::newInstance(const std::string& label,
                                                const std::string& sectype)
{
    bool isInfiniband = false;
    bool isSecure = TCPInstance::instance()->isSecure();
    // Find out if receiver is using Infiniband
    SPCDBG(L_TRACE, "Looking up NAM for: " << label, TRANSPORT_DBG);

    // Our transport security type does not match with the TCPInstance information. This likely
    // happens because this PE has no transport ports that would have initialized this value. We
    // will initialize it here in case of a mismatch.

    if (Security::parseType(sectype) != Security::None && !isSecure) {
        SPCDBG(L_TRACE, "Transport security information was not set, now setting to " << sectype,
               TRANSPORT_DBG);
        TCPInstance::instance()->setSecureMethod(sectype);
        isSecure = TCPInstance::instance()->isSecure();
    }

    std::auto_ptr<ViewStreamSender> sender(new ViewStreamSender(label, isInfiniband, isSecure));
    sender->connect();
    return sender.release();
}
ViewStreamSender::ViewStreamSender(const std::string& label, bool useInfiniband, bool isSecure)
  : enableSSL_(isSecure)
{
    DataSender::PortDescription pd(label, "");
    if (!isSecure) {
        std::vector<DataSender::PortDescription> pds;
        pds.push_back(pd);
        sender_.reset(TCPInstance::instance()->newSender(0, pds, TCPInstance::Static));
    } else {
        std::vector<DataSender::PortDescription> pds;
        pds.push_back(pd);
        sender_.reset(TCPInstance::instance()->newSender(0, pds, TCPInstance::Static));
        SPCDBG(L_ERROR, "VIEW_SSL_ENABLED: Using secure TCPInstance for View.", TRANSPORT_DBG);
    }
}

void ViewStreamSender::copyULL(ViewStreamMsgHeader* hdr, const void* data, size_t len)
{
    const unsigned long long* ull = reinterpret_cast<const unsigned long long*>(data);
    for (size_t i = 0; i < len; i++) {
        hdr->setViewId(*ull++, i);
    }
}

void ViewStreamSender::copyUInt64(ViewStreamMsgHeader* hdr, const void* data, size_t len)
{
    const uint64_t* uint64 = reinterpret_cast<const uint64_t*>(data);
    for (size_t i = 0; i < len; i++) {
        hdr->setViewId(*uint64++, i);
    }
}

void ViewStreamSender::write(const void* viewIds,
                             size_t viewIdsLen,
                             int channelIndex,
                             const int32_t* allChannels,
                             size_t allChannelsLen,
                             unsigned long long tupleSeq,
                             const void* tuple,
                             size_t size,
                             ViewStreamSender::copyArray copyIds)
{
    assert(sender_.get());

    SPCDBG(L_TRACE,
           "Writing tuple with sequence number "
             << tupleSeq << " channel " << channelIndex << " allChannels {"
             << debug::joinArray(allChannels, allChannelsLen) << "} to " << viewIdsLen
             << " views, tuple size " << std::dec << size << " data=" << std::hex << std::showbase
             << tuple,
           TRANSPORT_DBG);

    char hdrOnStack[256];
    std::vector<char> hdrOnHeap;
    size_t hdrBufSize = ViewStreamMsgHeader::size(viewIdsLen, allChannelsLen);

    // allocate header buffer
    char* hdrBuf;
    if (hdrBufSize > 256) {
        hdrOnHeap.resize(hdrBufSize);
        hdrBuf = hdrOnHeap.data();
    } else {
        hdrBuf = hdrOnStack;
    }

    memset(hdrBuf, 0, hdrBufSize);
    // serialize message header
    ViewStreamMsgHeader* hdr =
      ViewStreamMsgHeader::serialize(hdrBuf, tupleSeq, channelIndex, viewIdsLen);
    // serialize viewIds
    (*copyIds)(hdr, viewIds, viewIdsLen);
    // serialize extended header for allChannels
    hdr->serializeExtendedHeader(allChannels, allChannelsLen);

    SPCDBG(L_TRACE, "Writing header " << *hdr, TRANSPORT_DBG);

    assert(hdrBufSize <= static_cast<size_t>(std::numeric_limits<int>::max()));
    assert(size <= static_cast<size_t>(std::numeric_limits<int>::max()));

    // set message fragments
    const void* frags[2] = { hdr, tuple };
    const int fragLens[2] = { (int)hdrBufSize, (int)size };
    if (enableSSL_) {
        // Copy fragments into contiguous buffer
        uint32_t messageLen = fragLens[0] + fragLens[1];
        SPCDBG(L_TRACE, "Creating final data of length " << messageLen, TRANSPORT_DBG);
        std::auto_ptr<char> new_buffer(new char[messageLen]);
        char* data = new_buffer.get();
        memcpy(data, frags[0], fragLens[0]);
        memcpy(data + fragLens[0], frags[1], fragLens[1]);
        sender_->write(data, messageLen);
    } else {
        // Copy fragments into contiguous buffer, then write using the DataSender.
        // Use stack if total fragments smaller than VIEW_SMALL_BUFFER
        unsigned char small_buffer[VIEW_SMALL_BUFFER];

        // Copy fragments into contiguous buffer
        uint32_t messageLen = fragLens[0] + fragLens[1];
        std::auto_ptr<unsigned char> new_buffer;
        if (messageLen > VIEW_SMALL_BUFFER) {
            new_buffer.reset(new unsigned char[messageLen]);
        }
        unsigned char* data = new_buffer.get() ?: small_buffer;
        memcpy(data, frags[0], fragLens[0]);
        memcpy(data + fragLens[0], frags[1], fragLens[1]);
        sender_->write(data, messageLen);
    }
    SPCDBG(L_TRACE, "End", TRANSPORT_DBG);
}
