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

#include <SPL/Runtime/Utility/LoggingPayload.h>

#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>

using namespace SPL;

const std::string LoggingPayload::name("log");

LoggingPayload* LoggingPayload::clone() const
{
    return new LoggingPayload(_jobID, _peID, _opIndex, _portIndex, _seqNum);
}

void LoggingPayload::serialize(NativeByteBuffer& buf) const
{
    buf.addUInt32(_jobID);
    buf.addUInt32(_peID);
    buf.addUInt32(_opIndex);
    buf.addUInt32(_portIndex);
    buf.addUInt64(_seqNum);
}

void LoggingPayload::deserialize(NativeByteBuffer& buf)
{
    _jobID = buf.getUInt32();
    _peID = buf.getUInt32();
    _opIndex = buf.getUInt32();
    _portIndex = buf.getUInt32();
    _seqNum = buf.getUInt64();
}

void LoggingPayload::serialize(NetworkByteBuffer& buf) const
{
    buf.addUInt32(_jobID);
    buf.addUInt32(_peID);
    buf.addUInt32(_opIndex);
    buf.addUInt32(_portIndex);
    buf.addUInt64(_seqNum);
}

void LoggingPayload::deserialize(NetworkByteBuffer& buf)
{
    _jobID = buf.getUInt32();
    _peID = buf.getUInt32();
    _opIndex = buf.getUInt32();
    _portIndex = buf.getUInt32();
    _seqNum = buf.getUInt64();
}
