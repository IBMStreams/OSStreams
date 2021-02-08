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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_PE_VISUALIZER_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_PE_VISUALIZER_H

#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/Type/Tuple.h>
#include <SPL/Runtime/Utility/Thread.h>
#include <TRANS/DataSender.h>
#include <UTILS/CV.h>
#include <UTILS/Mutex.h>
#include <list>
#include <vector>

namespace Distillery {
class ViewStreamSender;
};

namespace SPL {

class Tuple;

class PEVisualizer : public Thread
{
  public:
    /// Constructor
    PEVisualizer(PEImpl const& pe);

    /// Destructor
    ~PEVisualizer();

    /// Open the transport port
    /// @param viewLabel transport label
    void openPort(const std::string& viewlabel);

    /// Close the transport port
    void closePort();

    /// Write a tuple to the visualizer
    /// @param viewIds All the viewIDs associated with this particular operator output port
    /// @param channel The channel index associated with this particular operator
    /// @param allChannels parallel channel indexes for all nested parallel regions
    /// @param tupleSeq Tuple sequence number to show how many tuples have been dropped
    /// @param tuple Tuple to send
    void write(const std::vector<uint64_t>& viewIds,
               int32_t channel,
               std::vector<int32_t>& allChannels,
               uint64_t tupleSeq,
               const Tuple& tuple);

    /// Write a tuple as a buffer to the visualizer
    /// @param viewIds All the viewIDs associated with this particular operator output port
    /// @param channel The channel index associated with this particular operator
    /// @param allChannels parallel channel indexes for all nested parallel regions
    /// @param tupleSeq Tuple sequence number to show how many tuples have been dropped
    /// @param buffer Tuple (in the form of a buffer) to send
    void write(const std::vector<uint64_t>& viewIds,
               int32_t channel,
               std::vector<int32_t>& allChannels,
               uint64_t tupleSeq,
               const NativeByteBuffer& buffer);

    /// Shut down the visualizer
    void shutdown();

  private:
    // Dequeuing thread
    void* run(void* threadArgs);

    // Implement a cut-down version of the active Q, but not running continuously
    struct Item
    {
        Item()
          : _tuple(NULL)
        {}
        Item(const std::vector<uint64_t>& viewIds,
             int32_t channel,
             const std::vector<int32_t>& allChannels,
             uint64_t tupleSeq,
             const Tuple& tuple)
          : _viewIds(viewIds)
          , _channel(channel)
          , _allChannels(allChannels)
          , _tupleSeq(tupleSeq)
          , _tuple(tuple.clone())
        {}

        Item(const std::vector<uint64_t>& viewIds,
             int32_t channel,
             const std::vector<int32_t>& allChannels,
             uint64_t tupleSeq,
             const NativeByteBuffer& buffer)
          : _viewIds(viewIds)
          , _channel(channel)
          , _allChannels(allChannels)
          , _tupleSeq(tupleSeq)
          , _tuple(NULL)
        {
            _buffer = new NativeByteBuffer(buffer);
        }

        const Tuple& getTuple() const { return *_tuple; }
        const NativeByteBuffer& getBuffer() const { return *_buffer; }
        bool isTuple() { return _tuple != NULL; }

        const std::vector<uint64_t>& getViewIds() const { return _viewIds; }
        int32_t getChannel() const { return _channel; }
        const std::vector<int32_t>& getAllChannels() const { return _allChannels; }
        uint64_t getTupleSeq() const { return _tupleSeq; }

      private:
        std::vector<uint64_t> _viewIds;
        int32_t _channel;
        std::vector<int32_t> _allChannels;
        uint64_t _tupleSeq;
        const Tuple* _tuple;
        const NativeByteBuffer* _buffer;
    };

    std::list<Item> _queue;
    Distillery::CV _consCV;
    Distillery::Mutex _mutex;
    PEImpl const& _pe;
    uint32_t _currentSize;
    uint32_t _maxSize;
    bool _shutdown;
    std::auto_ptr<Distillery::ViewStreamSender> _visPort;
    std::auto_ptr<NativeByteBuffer> _buffer;
};
};
#endif // SPL_RUNTIME_PROCESSING_ELEMENT_PE_VISUALIZER_H
