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

#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/ProcessingElement/PEVisualizer.h>
#include <SPL/Runtime/Type/Tuple.h>
#include <TRANS/ViewStreamSender.h>

using namespace SPL;
using namespace std;

PEVisualizer::PEVisualizer(PEImpl const& pe)
  : _pe(pe)
  , _currentSize(0)
  , _maxSize(100)
  , _shutdown(false)
{}

PEVisualizer::~PEVisualizer() {}

void PEVisualizer::openPort(const std::string& viewlabel)
{
    Distillery::AutoMutex am(_mutex);
    APPTRC(L_TRACE, "PEVisualizer::openPort: enter", "VIEW");
    std::string sectype = _pe.getPlatform().getTransportSecurityType();
    _visPort.reset(Distillery::ViewStreamSender::newInstance(viewlabel, sectype));
    _buffer.reset(new NativeByteBuffer);
}

void PEVisualizer::closePort()
{
    Distillery::AutoMutex am(_mutex);
    APPTRC(L_TRACE, "PEVisualizer::closePort: enter", "VIEW");
    _visPort.reset(NULL);
    _buffer.reset(NULL);
}

void PEVisualizer::write(const std::vector<uint64_t>& viewIds,
                         int32_t channel,
                         std::vector<int32_t>& allChannels,
                         uint64_t tupleSeq,
                         const Tuple& tuple)
{
    Distillery::AutoMutex am(_mutex);
    APPTRC(L_TRACE, "PEVisualizer::write: enter", "VIEW");
    if (_currentSize == _maxSize) {
        // Need to drop the oldest one
        APPTRC(L_TRACE, "PEVisualizer::write: drop oldest tuple", "VIEW");
        std::list<Item>::iterator it = _queue.begin();
        delete &(it->getTuple());
        _queue.erase(it);
        _currentSize--;
    }
    // Put the new one at the back
    _queue.push_back(Item(viewIds, channel, allChannels, tupleSeq, tuple));
    _currentSize++;
    if (_currentSize == 1) {
        APPTRC(L_TRACE, "PEVisualizer::write: signal thread", "VIEW");
        _consCV.signal();
    }
}

void PEVisualizer::write(const std::vector<uint64_t>& viewIds,
                         int32_t channel,
                         std::vector<int32_t>& allChannels,
                         uint64_t tupleSeq,
                         const NativeByteBuffer& buffer)
{
    Distillery::AutoMutex am(_mutex);
    APPTRC(L_TRACE, "PEVisualizer::write: enter", "VIEW");
    if (_currentSize == _maxSize) {
        // Need to drop the oldest one
        APPTRC(L_TRACE, "PEVisualizer::write: drop oldest tuple", "VIEW");
        std::list<Item>::iterator it = _queue.begin();
        delete &(it->getBuffer());
        _queue.erase(it);
        _currentSize--;
    }
    // Put the new one at the back
    _queue.push_back(Item(viewIds, channel, allChannels, tupleSeq, buffer));
    _currentSize++;
    if (_currentSize == 1) {
        APPTRC(L_TRACE, "PEVisualizer::write: signal thread", "VIEW");
        _consCV.signal();
    }
}

void* PEVisualizer::run(void* args)
{
    APPTRC(L_TRACE, "PEVisualizer::run: enter", "VIEW");
    registerThread("Visualizer");

    while (!_shutdown) {
        Item item;
        Distillery::AutoMutex am(_mutex);
        while (_currentSize == 0) {
            APPTRC(L_TRACE, "PEVisualizer::run: wait for data", "VIEW");
            _consCV.wait(_mutex);
            if (_shutdown) {
                APPTRC(L_TRACE, "PEVisualizer::run: shutdown exit", "VIEW");
                return NULL;
            }
        }
        item = *_queue.begin();
        _currentSize--;
        _queue.pop_front();

        const vector<uint64_t> viewIds = item.getViewIds();
        uint64_t tupleSeq = item.getTupleSeq();
        int channel = item.getChannel();
        const vector<int32_t> allChannels = item.getAllChannels();

        APPTRC(L_TRACE, "PEVisualizer::run: visualize tuple", "VIEW");
        if (item.isTuple()) {
            const Tuple& tuple = item.getTuple();
            if (_visPort.get()) {
                if (tuple.isFacade()) {
                    // Write facade tuple
                    _visPort->write(&viewIds[0], viewIds.size(), channel, &allChannels[0],
                                    allChannels.size(), tupleSeq, tuple.getSerializedDataPtr() - 1,
                                    tuple.getSerializedSize() + 1);
                } else {
                    // Write non-facade tuple
                    _buffer->setICursor(0);
                    _buffer->addUInt8(0);
                    tuple.serialize(*_buffer);
                    _visPort->write(&viewIds[0], viewIds.size(), channel, &allChannels[0],
                                    allChannels.size(), tupleSeq, _buffer->getPtr(),
                                    _buffer->getSerializedDataSize());
                }
            }
            delete &tuple;
        } else {
            // Write native buffer
            const NativeByteBuffer& buffer = item.getBuffer();
            if (_visPort.get()) {
                _buffer->setICursor(0);
                _buffer->addUInt8(0);
                _buffer->copyData(buffer, sizeof(uint8_t));
                _visPort->write(&viewIds[0], viewIds.size(), channel, &allChannels[0],
                                allChannels.size(), tupleSeq, _buffer->getPtr(),
                                _buffer->getSerializedDataSize());
            }
            delete &buffer;
        }
    }
    APPTRC(L_TRACE, "PEVisualizer::run: exit", "VIEW");
    return NULL;
}

void PEVisualizer::shutdown()
{
    // Just toss the whole thing
    _shutdown = true;
    {
        Distillery::AutoMutex am(_mutex);
        for (std::list<Item>::iterator it = _queue.begin(); it != _queue.end(); it++) {
            delete &(it->getTuple());
            delete &(it->getBuffer());
        }
        _consCV.broadcast();
    }

    // Reap the thread
    if (hasStarted) {
        void* dummy;
        this->join((void*)&dummy);
        hasStarted = false;
    }
}
