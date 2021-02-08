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

#include <SPL/Runtime/Utility/DrainPunctPayload.h>

#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>

using namespace SPL;

const std::string DrainPunctPayload::name("dr");

DrainPunctPayload* DrainPunctPayload::clone() const
{
    return new DrainPunctPayload(_seqID);
}

void DrainPunctPayload::serialize(NativeByteBuffer& buf) const
{
    buf.addInt64(_seqID);
}

void DrainPunctPayload::deserialize(NativeByteBuffer& buf)
{
    _seqID = buf.getInt64();
}

void DrainPunctPayload::serialize(NetworkByteBuffer& buf) const
{
    buf.addInt64(_seqID);
}

void DrainPunctPayload::deserialize(NetworkByteBuffer& buf)
{
    _seqID = buf.getInt64();
}
