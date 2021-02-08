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
#include <TRC/DistilleryDebug.h>
#include <limits>

UTILS_NAMESPACE_BEGIN
using namespace std;
#define LOGGER "transport"

const ViewStreamMsgHeader* ViewStreamMsgHeader::deserialize(const void* buf,
                                                            const size_t bufSize,
                                                            int& ec)
{
    SPCDBG(L_TRACE,
           "(" << std::hex << std::showbase << static_cast<const void*>(buf) << ", " << std::dec
               << bufSize << ")",
           LOGGER);
    const ViewStreamMsgHeader* hdr = reinterpret_cast<const ViewStreamMsgHeader*>(buf);

    if (buf == NULL) {
        ec = E_NULL;
        return hdr;
    }
    // Verify there is room for the fixed header part
    if (bufSize < ViewStreamMsgHeader::size(0, 0)) {
        ec = E_SIZE;
        return hdr;
    }
    // Verify there is room for a number of view Ids as decoded from the buffer
    size_t num = hdr->getNumViewIds();
    size_t numAllChannels = hdr->getNumAllChannels();
    if (bufSize < ViewStreamMsgHeader::size(num, numAllChannels)) {
        ec = E_SIZE;
    }
    SPCDBG(L_TRACE, "Exit", LOGGER);
    return hdr;
}

ViewStreamMsgHeader* ViewStreamMsgHeader::serialize(void* buf,
                                                    size_t bufSize,
                                                    int channel,
                                                    size_t numIds,
                                                    int& ec,
                                                    size_t numAllChannels)
{
    SPCDBG(L_TRACE,
           "(" << std::hex << std::showbase << buf << std::dec << ", bufSize=" << bufSize
               << ", channel=" << channel << ", numIds=" << numIds << ")",
           LOGGER);
    ViewStreamMsgHeader* hdr = reinterpret_cast<ViewStreamMsgHeader*>(buf);
    if (buf == NULL) {
        ec = E_NULL;
        return hdr;
    }
    if (bufSize < ViewStreamMsgHeader::size(numIds, numAllChannels)) {
        ec = E_SIZE;
        return hdr;
    }
    hdr->setNumViewIds(numIds);
    hdr->setChannelIndex(channel);
    hdr->setNumAllChannels(numAllChannels);

    // Set default field values
    hdr->setSequenceNumber(0);

    SPCDBG(L_TRACE, "Exit", LOGGER);
    return hdr;
}

const char* ViewStreamMsgHeader::errCodeToString(int code)
{
    switch (code) {
#define M(v)                                                                                       \
    case v:                                                                                        \
        return #v
        M(E_SUCCESS);
        M(E_NULL);
        M(E_SIZE);
#undef M
        default:
            return "???";
    }
}

std::ostream& ViewStreamMsgHeader::print(std::ostream& o) const
{
    o << "ViewStreamMsgHeader{seq = " << getSequenceNumber();
    o << " channelIndex = " << getChannelIndex();

    size_t numViewIds = getNumViewIds();
    o << std::dec << ", viewIds[" << numViewIds << "] = {";
    for (size_t i = 0; i < numViewIds; i++) {
        if (i != 0) {
            o << ", ";
        }
        o << getViewId(i);
    }
    o << "}";

    o << ", eyecatcher = " << getEyeCatcher();
    o << ", version = " << static_cast<uint32_t>(getVersion());

    size_t numAllChannels = getNumAllChannels();
    o << std::dec << ", allChannels[" << numAllChannels << "] = {";
    for (size_t i = 0; i < numAllChannels; i++) {
        if (i != 0) {
            o << ", ";
        }
        o << getAllChannelsIndex(i);
    }
    o << "}";

    o << "}";
    return o;
}

UTILS_NAMESPACE_END
