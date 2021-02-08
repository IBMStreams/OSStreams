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

#include <SPL/Runtime/Utility/PayloadContainer.h>

#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>
#include <SPL/Runtime/Utility/DrainPunctPayload.h>
#include <SPL/Runtime/Utility/FinalPunctPayload.h>
#include <SPL/Runtime/Utility/LoggingPayload.h>
#include <SPL/Runtime/Utility/ResetPunctPayload.h>
#include <SPL/Runtime/Utility/ResumePunctPayload.h>
#include <SPL/Runtime/Utility/WatermarkPunctPayload.h>

using namespace SPL;

PayloadContainer::PayloadContainer(const PayloadContainer& rhs)
{
    for (uint32_t i = 0, n = rhs._payloads.size(); i < n; i++) {
        const std::pair<std::string, Payload*>& elem = rhs._payloads[i];
        _payloads.push_back(std::make_pair(elem.first, elem.second->clone()));
    }
}

PayloadContainer::~PayloadContainer()
{
    for (uint32_t i = 0, n = _payloads.size(); i < n; i++) {
        delete _payloads[i].second;
    }
}

bool PayloadContainer::add(const std::string& name, Payload& payload)
{
    if (_payloads.size() >= 255) {
        return false;
    }

    for (uint32_t i = 0, n = _payloads.size(); i < n; i++) {
        const std::pair<std::string, Payload*>& elem = _payloads[i];
        if (elem.first == name) {
            return false;
        }
    }
    // didn't match it.
    _payloads.push_back(std::make_pair(name, &payload));
    return true;
}

Payload* PayloadContainer::find(const std::string& name) const
{
    for (uint32_t i = 0, n = _payloads.size(); i < n; i++) {
        const std::pair<std::string, Payload*>& elem = _payloads[i];
        if (elem.first == name) {
            return elem.second;
        }
    }
    return NULL;
}

Payload* PayloadContainer::remove(const std::string& name)
{
    for (uint32_t i = 0, n = _payloads.size(); i < n; i++) {
        std::pair<std::string, Payload*>& elem = _payloads[i];
        if (elem.first == name) {
            Payload* p = elem.second;
            _payloads.erase(_payloads.begin() + i);
            return p;
        }
    }
    return NULL;
}

void PayloadContainer::remove(const Payload& payload)
{
    for (uint32_t i = 0, n = _payloads.size(); i < n; i++) {
        std::pair<std::string, Payload*>& elem = _payloads[i];
        if (elem.second == &payload) {
            _payloads.erase(_payloads.begin() + i);
            return;
        }
    }
}

void PayloadContainer::serialize(NetworkByteBuffer& buf) const
{
    uint8_t numPayloads = _payloads.size();
    buf.addUInt8(numPayloads);
    for (uint32_t i = 0; i < numPayloads; i++) {
        const std::pair<std::string, Payload*>& elem = _payloads[i];
        uint32_t stlen = elem.first.size();
        buf.addUInt32(stlen);
        if (stlen > 0) {
            const char* ptr = elem.first.c_str();
            buf.addCharSequence(ptr, stlen);
        }
        elem.second->serialize(buf);
    }
}

void PayloadContainer::deserialize(NetworkByteBuffer& buf)
{
    uint8_t n = buf.getUInt8();

    if (n == 0) {
        return;
    }

    for (uint32_t i = 0; i < n; i++) {
        uint32_t stlen = buf.getUInt32();
        std::string name;
        if (stlen != 0) {
            char* p = buf.getFixedCharSequence(stlen);
            name = std::string(p, stlen);
        }
        Payload& p = createPayload(name);
        p.deserialize(buf);
        _payloads.push_back(std::make_pair(name, &p));
    }
}

void PayloadContainer::serialize(NativeByteBuffer& buf) const
{
    uint8_t numPayloads = _payloads.size();
    buf.addUInt8(numPayloads);
    for (uint32_t i = 0; i < numPayloads; i++) {
        const std::pair<std::string, Payload*>& elem = _payloads[i];
        uint32_t stlen = elem.first.size();
        buf.addUInt32(stlen);
        if (stlen > 0) {
            const char* ptr = elem.first.c_str();
            buf.addCharSequence(ptr, stlen);
        }
        elem.second->serialize(buf);
    }
}

void PayloadContainer::deserialize(NativeByteBuffer& buf)
{
    uint8_t n = buf.getUInt8();

    if (n == 0) {
        return;
    }

    for (uint32_t i = 0; i < n; i++) {
        uint32_t stlen = buf.getUInt32();
        std::string name;
        if (stlen != 0) {
            char* p = buf.getFixedCharSequence(stlen);
            name = std::string(p, stlen);
        }
        // TODO Is this right?  We might create a payload with an empty
        // string as its name.
        Payload& p = createPayload(name);
        p.deserialize(buf);
        _payloads.push_back(std::make_pair(name, &p));
    }
}

Payload& PayloadContainer::createPayload(const std::string& name)
{
    if (name == LoggingPayload::name) {
        return *new LoggingPayload();
    } else if (name == FinalPunctPayload::name) {
        return *new FinalPunctPayload();
    } else if (name == DrainPunctPayload::name) {
        return *new DrainPunctPayload();
    } else if (name == ResetPunctPayload::name) {
        return *new ResetPunctPayload();
    } else if (name == ResumePunctPayload::name) {
        return *new ResumePunctPayload();
    } else if (name == WatermarkPunctPayload::name) {
        return *new WatermarkPunctPayload();
    }

    THROW_STRING(SPLRuntimePayload, SPL_RUNTIME_UNKNOWN_PAYLOAD(name));
}
