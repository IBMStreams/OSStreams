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

/*
 * Implementation of SPL::IncrementalCheckpointIndex class.
 */
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/State/DataStoreByteBuffer.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Operator/State/IncrementalCheckpointIndex.h>
#include <exception>

using namespace std;
using namespace SPL;

IncrementalCheckpointIndex::IncrementalCheckpointIndex()
  : type_(BASE)
  , offset_(0)
  , prevID_(0)
  , prevIndexOffset_(0)
{}

IncrementalCheckpointIndex::~IncrementalCheckpointIndex() {}

void IncrementalCheckpointIndex::serialize(DataStoreByteBuffer& buffer) const
{
    // the frist 2 bits tell whether it's base, first delta, or other delta
    uint8_t headCode = type_;
    // the 3-4 bits tell whether the offset is 1, 2, 4, or 8 bytes
    headCode |= getSerializedLength(offset_) << 2;
    if (type_ != BASE) {
        // the 5-6 bits tell whether the previous ID is 1, 2, 4, or 8 bytes
        headCode |= getSerializedLength(prevID_) << 4;
        // the 7-8 bites tell whether the previous index offset is 1, 2, 4, or 8 bytes
        headCode |= getSerializedLength(prevIndexOffset_) << 6;
    }
    buffer << headCode;
    serializeUInt64(buffer, offset_);
    if (type_ != BASE) {
        serializeUInt64(buffer, uint64_t(prevID_));
        serializeUInt64(buffer, prevIndexOffset_);
    }
}

void IncrementalCheckpointIndex::deserialize(DataStoreByteBuffer& buffer)
{
    uint8_t headCode;
    buffer >> headCode;
    type_ = headCode & 0x03;
    offset_ = deserializeUInt64(buffer, (headCode & 0x0C) >> 2);
    if (type_ != BASE) {
        prevID_ = int64_t(deserializeUInt64(buffer, (headCode & 0x30) >> 4));
        prevIndexOffset_ = deserializeUInt64(buffer, (headCode & 0xC0) >> 6);
    } else {
        prevID_ = 0;
        prevIndexOffset_ = 0;
    }
}

uint8_t IncrementalCheckpointIndex::getSerializedLength(const uint64_t i)
{
    if (i <= 0xFF) {
        return 0x00;
    } else if (i <= 0xFFFF) {
        return 0x01;
    } else if (i <= 0xFFFFFFFF) {
        return 0x02;
    } else {
        return 0x03;
    }
}

void IncrementalCheckpointIndex::serializeUInt64(DataStoreByteBuffer& buffer, const uint64_t i)
{
    // A 64-bit integer is serialized in either 1, 2, 4, or 8 bytes
    // For example, ox0000000001000000 becomes 0x01000000 (4 bytes)
    if (i <= 0xFF) {
        buffer.addUInt8(uint8_t(i));
    } else if (i <= 0xFFFF) {
        buffer.addUInt16(uint16_t(i));
    } else if (i <= 0xFFFFFFFF) {
        buffer.addUInt32(uint32_t(i));
    } else {
        buffer.addUInt64(i);
    }
}

uint64_t IncrementalCheckpointIndex::deserializeUInt64(DataStoreByteBuffer& buffer,
                                                       const uint8_t numBytes)
{
    if (numBytes == 0x00) {
        return uint64_t(buffer.getUInt8());
    } else if (numBytes == 0x01) {
        return uint64_t(buffer.getUInt16());
    } else if (numBytes == 0x02) {
        return uint64_t(buffer.getUInt32());
    } else {
        return buffer.getUInt64();
    }
}
