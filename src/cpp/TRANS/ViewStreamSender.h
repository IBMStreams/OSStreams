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

#ifndef TRANS_VIEWSTREAM_TRANSCEIVER_H_
#define TRANS_VIEWSTREAM_TRANSCEIVER_H_

#include <TRANS/DataSender.h>
#include <UTILS/RuntimeMessages.h>
#include <UTILS/UTILSTypes.h>

#include <stdint.h>

UTILS_NAMESPACE_BEGIN

class ViewStreamMsgHeader;

/**
 * @file ViewStreamTransceiver contains a sender and a receiver of View Tuples.
 */

class ViewStreamSender
{
  public:
    /**
     * Factory method.
     *
     * @param label endpoint label of the receiver this sender will connect to
     * @return pointer to a new object instance
     */
    static ViewStreamSender* newInstance(const std::string& label, const std::string& sectype);

    ~ViewStreamSender()
    {
        close();
        sender_.reset();
    }

    void close() { sender_->shutdown(); }

    /**
     * Send data to the View Server.
     *
     * @param viewIds array of view Ids that the data belongs to
     * @param viewIdsLen count of view Ids in the array
     * @param channelIndex parallel channel index of the source operator
     * @param allChannels parallel channel indexes for all nested parallel regions
     * @param allChannelsLen number of parallel channel indexes
     * @param tupleSeq tuple sequence number
     * @param tuple pointer to the tuple buffer to send
     * @param size size of the tuple buffer
     */
    void write(const unsigned long long* viewIds,
               size_t viewIdsLen,
               int channelIndex,
               const int32_t* allChannels,
               size_t allChannelsLen,
               unsigned long long tupleSeq,
               const void* tuple,
               size_t size);

// Overload write on 64 bit platforms because uint64_t does not have the size
// of a unsigned long long int.
#if __WORDSIZE == 64
    /**
     * Send data to the View Server.
     *
     * @param viewIds array of view Ids that the data belongs to
     * @param viewIdsLen count of view Ids in the array
     * @param channelIndex parallel channel index of the source operator
     * @param allChannels parallel channel indexes for all nested parallel regions
     * @param allChannelsLen number of parallel channel indexes
     * @param tupleSeq tuple sequence number
     * @param tuple pointer to the tuple buffer to send
     * @param size size of the tuple buffer
     */
    void write(const uint64_t* viewIds,
               size_t viewIdsLen,
               int channelIndex,
               const int32_t* allChannels,
               size_t allChannelsLen,
               unsigned long long tupleSeq,
               const void* tuple,
               size_t size);
#endif

  private:
    ViewStreamSender(const std::string& label, bool useInfiniband, bool isSecure);
    void connect() { sender_->connect(); }

    typedef void (*copyArray)(ViewStreamMsgHeader*, const void*, size_t);
    static void copyULL(ViewStreamMsgHeader* hdr, const void* data, size_t len);
    static void copyUInt64(ViewStreamMsgHeader* hdr, const void* data, size_t len);

    // Erase viewIds argument type and let the copyIds function handle it.
    void write(const void* viewIds,
               size_t viewIdsLen,
               int channelIndex,
               const int32_t* allChannels,
               size_t allChannelsLen,
               unsigned long long tupleSeq,
               const void* tuple,
               size_t size,
               ViewStreamSender::copyArray copyIds);

    std::auto_ptr<DataSender> sender_;
    bool enableSSL_;
};

UTILS_NAMESPACE_END

///////////////////////////////////////////////////////////////////////////
// Implementation
#include <TRANS/ViewStreamMsgHeader.h>
#include <TRC/RuntimeTrcAspects.h>
UTILS_NAMESPACE_BEGIN

inline void ViewStreamSender::write(const unsigned long long* viewIds,
                                    size_t viewIdsLen,
                                    int channelIndex,
                                    const int32_t* allChannels,
                                    size_t allChannelsLen,
                                    unsigned long long tupleSeq,
                                    const void* tuple,
                                    size_t size)
{
    write(static_cast<const void*>(viewIds), viewIdsLen, channelIndex, allChannels, allChannelsLen,
          tupleSeq, tuple, size, &copyULL);
}

#if __WORDSIZE == 64
inline void ViewStreamSender::write(const uint64_t* viewIds,
                                    size_t viewIdsLen,
                                    int channelIndex,
                                    const int32_t* allChannels,
                                    size_t allChannelsLen,
                                    unsigned long long tupleSeq,
                                    const void* tuple,
                                    size_t size)
{
    write(static_cast<const void*>(viewIds), viewIdsLen, channelIndex, allChannels, allChannelsLen,
          tupleSeq, tuple, size, &copyUInt64);
}
#endif

UTILS_NAMESPACE_END

#endif // ! TRANS_VIEWSTREAM_TRANSCEIVER_H_
