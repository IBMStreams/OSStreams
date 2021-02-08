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

#include <SPL/Runtime/Operator/Port/Splitter.h>
#include <SPL/Runtime/Operator/Port/SubmitSignal.h>
#include <UTILS/SupportFunctions.h>

using namespace SPL;

SplitterAlgorithm::SplitterAlgorithm(uint32_t channels)
  : activeChannels_(channels)
{
    assert(activeChannels_ > 0);
}

void SplitterAlgorithm::setActiveChannels(uint32_t active)
{
    assert(active > 0);
    activeChannels_ = active;
}

RoundRobin::RoundRobin(uint32_t channels, int32_t startPos)
  : SplitterAlgorithm(channels)
{
    if (startPos >= 0) {
        next_ = startPos % activeChannels_;
    } else {
        next_ = 0;
    }
}

AttributeHash::AttributeHash(const std::vector<uint32_t>& attributes, uint32_t channels)
  : SplitterAlgorithm(channels)
  , attributes_(attributes)
{
    assert(attributes_.size() > 0);
}

uint32_t OperatorParallelConnection::submit(Tuple& tuple, bool lastConnection)
{
    _subsig.submitToOperator(tuple, _op, lastConnection);
    return 1;
}

uint32_t OperatorParallelConnection::submit(Punctuation& punct)
{
    _op->submit(punct);
    return 1;
}

uint32_t PEOutputPortParallelConnection::submit(Tuple& tuple, bool lastChannel)
{
    return _pePort(tuple);
}

uint32_t PEOutputPortParallelConnection::submit(Punctuation& punct)
{
    return _pePort(punct);
}

ParallelChannel::~ParallelChannel()
{
    for (size_t i = 0; i < _connections.size(); i += 1) {
        delete _connections[i];
    }
}

void ParallelChannel::connect(ParallelConnection* conn)
{
    _connections.push_back(conn);
}

void ParallelChannel::addSplitter(Splitter& splitter)
{
    _splitterCalls.push_back(&splitter);
    _splitterDropWindow.push_back(splitter.dropWindowPuncts());
}

Splitter::Splitter(SplitterAlgorithm* a, bool d, uint32_t i)
  : _alg(a)
  , _dropWindowPuncts(d)
  , _index(i)
{}

Splitter::~Splitter()
{
    for (size_t i = 0; i < _channels.size(); ++i) {
        delete _channels[i];
    }
}

ParallelChannel& Splitter::addParallelChannel()
{
    ParallelChannel* pc = new ParallelChannel(_channels.size());
    _channels.push_back(pc);
    return *pc;
}

void Splitter::connectParallelChannel(uint32_t channelIndex, ParallelConnection& conn)
{
    _channels[channelIndex]->connect(&conn);
}
