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

#ifndef SPL_RUNTIME_OPERATOR_PORT_SPLITTER_H
#define SPL_RUNTIME_OPERATOR_PORT_SPLITTER_H

#include <SPL/Runtime/Operator/Port/PEOutputPortDelegate.h>
#include <SPL/Runtime/Operator/Port/PortSignal.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Runtime/Type/Tuple.h>
#include <UTILS/Atomic.h>

#include <boost/shared_ptr.hpp>
#include <vector>

namespace SPL {
class Splitter;

class SplitterAlgorithm
{
  public:
    SplitterAlgorithm(uint32_t channels);
    virtual ~SplitterAlgorithm() {}
    virtual void setActiveChannels(uint32_t channels);
    virtual uint32_t operator()(const Tuple& t) = 0;

  protected:
    uint32_t activeChannels_;
};

class RoundRobin : public SplitterAlgorithm
{
  public:
    RoundRobin(uint32_t channels, int32_t startPos);
    virtual ~RoundRobin() {}

    virtual uint32_t operator()(const Tuple& tuple)
    {
        int32_t oldValue;
        int32_t newValue;

        do {
            oldValue = next_;
            newValue = (oldValue + 1) % activeChannels_;
        } while (UTILS_NAMESPACE::atomic_cs32(&next_, oldValue, newValue) != 0);

        return oldValue;
    }

  private:
    UTILS_NAMESPACE::atomic_int32_t
      next_; // I would prefer to use an unsigned int, but we don't have atomic ops for those yet
};

class AttributeHash : public SplitterAlgorithm
{
  public:
    AttributeHash(const std::vector<uint32_t>& attributes, uint32_t channels);
    virtual ~AttributeHash() {}

    virtual uint32_t operator()(const Tuple& tuple) { return hashTuple(tuple) % activeChannels_; }

  private:
    uint64_t hashTuple(const Tuple& tuple)
    {
        if (attributes_.size() == 1) {
            return tuple.getAttributeValue(attributes_[0]).hashCode();
        } else {
            // technique for combining hashes (must match the one in generated
            // tuple code what can be found in the codegen template tuple_h.cgt)
            uint64_t hash = 17;
            for (size_t i = 0; i < attributes_.size(); ++i) {
                hash = 37 * hash + tuple.getAttributeValue(attributes_[i]).hashCode();
            }
            return hash;
        }
    }

    const std::vector<uint32_t> attributes_;
};

class ParallelConnection
{
  public:
    virtual ~ParallelConnection() {}
    virtual uint32_t submit(Tuple& tuple, bool lastChannel) = 0;
    virtual uint32_t submit(Punctuation& punct) = 0;
};

class SubmitSignal; // forward reference; SubmitSignal.h includes Splitter.h

class OperatorParallelConnection : public ParallelConnection
{
  public:
    OperatorParallelConnection(SubmitSignal& subsig, PortSignal* op)
      : _subsig(subsig)
      , _op(op)
    {}

    uint32_t submit(Tuple& tuple, bool lastConnection);
    uint32_t submit(Punctuation& punct);

  private:
    SubmitSignal& _subsig;
    PortSignal* _op;
};

class PEOutputPortParallelConnection : public ParallelConnection
{
  public:
    PEOutputPortParallelConnection(PEOutputPortDelegates pePort)
      : _pePort(pePort)
    {}

    uint32_t submit(Tuple& tuple, bool lastChannel);
    uint32_t submit(Punctuation& punct);

  private:
    PEOutputPortDelegates _pePort;
};

class ParallelChannel
{
  public:
    ParallelChannel(uint32_t i)
      : _index(i)
    {}
    ~ParallelChannel();

    void connect(ParallelConnection* conn);

    uint32_t submit(Tuple& tuple, bool lastSplitter);

    uint32_t submit(Punctuation& punct);

    uint32_t index() const { return _index; }

    void addSplitter(Splitter&);

  private:
    uint32_t _index;
    std::vector<ParallelConnection*> _connections;
    std::vector<Splitter*> _splitterCalls;
    std::vector<char> _splitterDropWindow;
};

class Splitter
{
  public:
    Splitter() {}
    Splitter(SplitterAlgorithm* a, bool d, uint32_t i);
    ~Splitter();

    ParallelChannel& addParallelChannel();
    void connectParallelChannel(uint32_t channelIndex, ParallelConnection& conn);

    uint32_t split(Tuple& tuple, bool lastSplitter)
    {
        if (_alg) {
            return _channels[(*_alg)(tuple)]->submit(tuple, lastSplitter);
        }
        uint32_t receivers = 0;
        // for-each
        for (size_t i = 0; i < _channels.size(); ++i) {
            receivers += _channels[i]->submit(tuple, lastSplitter);
        }
        return receivers;
    }

    uint32_t submit(Punctuation& punct)
    {
        uint32_t receivers = 0;
        for (size_t i = 0; i < _channels.size(); ++i) {
            receivers += _channels[i]->submit(punct);
        }
        return receivers;
    }

    bool dropWindowPuncts() const { return _dropWindowPuncts; }

    uint32_t index() const { return _index; }

  private:
    boost::shared_ptr<SplitterAlgorithm> _alg;
    std::vector<ParallelChannel*> _channels;
    bool _dropWindowPuncts;
    uint32_t _index;
};

inline uint32_t ParallelChannel::submit(Tuple& tuple, bool lastSplitter)
{
    uint32_t receivers = 0;
    for (size_t i = 0; i < _connections.size(); ++i) {
        receivers += _connections[i]->submit(tuple, lastSplitter && (i == _connections.size() - 1));
    }
    for (size_t i = 0; i < _splitterCalls.size(); ++i) {
        receivers += _splitterCalls[i]->split(tuple, i == _splitterCalls.size() - 1);
    }
    return receivers;
}

inline uint32_t ParallelChannel::submit(Punctuation& punct)
{
    uint32_t receivers = 0;
    for (size_t i = 0; i < _connections.size(); ++i) {
        receivers += _connections[i]->submit(punct);
    }
    for (size_t i = 0; i < _splitterCalls.size(); ++i) {
        if (_splitterDropWindow[i] && punct == Punctuation::WindowMarker) {
            continue;
        }
        receivers += _splitterCalls[i]->submit(punct);
    }
    return receivers;
}

};

#endif /* SPL_RUNTIME_OPERATOR_PORT_SPLITTER_H */
