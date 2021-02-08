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

#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>
#include <SPL/Runtime/Type/Decimal.h>
#include <UTILS/HostToNetwork.h>

namespace SPL {
/*
 * The original version of network serialization/deserialization just did what the native
 * byte buffer version did, which was to dump the bytes out from memory.  This isn't good
 * for working with other implementations, in particular big endian ones.   We should have
 * used the documented format, but we didn't.
 *
 * In order to remain backward compatible, the network byte format is defined to be that of
 * the little endian x86 implementation.   That is a pain, but doesn't break anything, and the
 * documentation will be updated to reflect this.
 *
 * Handle the big-endian code appropriately.
 */

#if __BYTE_ORDER != __LITTLE_ENDIAN
static void addLittleEndianBytes(ByteBuffer<NetworkByteBuffer>& buf, uint32_t value)
{
    Streams::HostNetworkTmp<uint32_t> src, dst;
    src.data = value;
    for (uint32_t i = 0; i < sizeof(src); ++i)
        dst.rawData[i] = src.rawData[sizeof(src) - i - 1];
    buf.addCharSequence(dst.rawData, sizeof(uint32_t));
}

static uint32_t extractLittleEndianBytes(ByteBuffer<NetworkByteBuffer>& buf)
{
    Streams::HostNetworkTmp<uint32_t> src, dst;
    memcpy((void*)&src.rawData, buf.getFixedCharSequence(sizeof(uint32_t)), sizeof(uint32_t));
    for (uint32_t i = 0; i < sizeof(src); ++i)
        dst.rawData[i] = src.rawData[sizeof(src) - i - 1];
    return dst.data;
}
#endif

ByteBuffer<NetworkByteBuffer>& serialize(ByteBuffer<NetworkByteBuffer>& buf, const decimal32& x)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    buf.addCharSequence((char*)&x, sizeof(x));
#else // Big-endian
    uint32_t dAsU32;
    memcpy(&dAsU32, &x, sizeof x);
    addLittleEndianBytes(buf, dAsU32);
#endif
    return buf;
}

ByteBuffer<NetworkByteBuffer>& deserialize(ByteBuffer<NetworkByteBuffer>& buf, decimal32& x)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    memcpy((void*)&x, buf.getFixedCharSequence(sizeof(x)), sizeof(x));
#else // Big-endian
    uint32_t dAsU32 = extractLittleEndianBytes(buf);
    memcpy(&x, &dAsU32, sizeof x);
#endif
    return buf;
}

ByteBuffer<NetworkByteBuffer>& serialize(ByteBuffer<NetworkByteBuffer>& buf, const decimal64& x)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    buf.addCharSequence((char*)&x, sizeof(x));
#else // Big-endian
    uint32_t dAsU32[2];
    memcpy(&dAsU32, &x, sizeof x);
    addLittleEndianBytes(buf, dAsU32[0]);
    addLittleEndianBytes(buf, dAsU32[1]);
#endif
    return buf;
}

ByteBuffer<NetworkByteBuffer>& deserialize(ByteBuffer<NetworkByteBuffer>& buf, decimal64& x)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    memcpy((void*)&x, buf.getFixedCharSequence(sizeof(x)), sizeof(x));
#else // Big-endian
    uint32_t dAsU32[2];
    dAsU32[0] = extractLittleEndianBytes(buf);
    dAsU32[1] = extractLittleEndianBytes(buf);
    memcpy(&x, &dAsU32, sizeof x);
#endif
    return buf;
}

ByteBuffer<NetworkByteBuffer>& serialize(ByteBuffer<NetworkByteBuffer>& buf, const decimal128& x)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    buf.addCharSequence((char*)&x, sizeof(x));
#else // Big-endian
    uint32_t dAsU32[4];
    memcpy(&dAsU32, &x, sizeof x);
    addLittleEndianBytes(buf, dAsU32[0]);
    addLittleEndianBytes(buf, dAsU32[1]);
    addLittleEndianBytes(buf, dAsU32[2]);
    addLittleEndianBytes(buf, dAsU32[3]);
#endif
    return buf;
}

ByteBuffer<NetworkByteBuffer>& deserialize(ByteBuffer<NetworkByteBuffer>& buf, decimal128& x)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    memcpy((void*)&x, buf.getFixedCharSequence(sizeof(x)), sizeof(x));
#else // Big-endian
    uint32_t dAsU32[4];
    dAsU32[0] = extractLittleEndianBytes(buf);
    dAsU32[1] = extractLittleEndianBytes(buf);
    dAsU32[2] = extractLittleEndianBytes(buf);
    dAsU32[3] = extractLittleEndianBytes(buf);
    memcpy(&x, &dAsU32, sizeof x);
#endif
    return buf;
}

#if __BYTE_ORDER != __LITTLE_ENDIAN
static void addLittleEndianBytes(VirtualByteBuffer& buf, uint32_t value)
{
    Streams::HostNetworkTmp<uint32_t> src, dst;
    src.data = value;
    for (uint32_t i = 0; i < sizeof(src); ++i)
        dst.rawData[i] = src.rawData[sizeof(src) - i - 1];
    buf.addCharSequence(dst.rawData, sizeof(uint32_t));
}

static uint32_t extractLittleEndianBytes(VirtualByteBuffer& buf)
{
    Streams::HostNetworkTmp<uint32_t> src, dst;
    buf.getFixedCharSequence(src.rawData, sizeof(src));
    for (uint32_t i = 0; i < sizeof(src); ++i)
        dst.rawData[i] = src.rawData[sizeof(src) - i - 1];
    return dst.data;
}
#endif

VirtualByteBuffer& operator<<(VirtualByteBuffer& buf, const decimal32& x)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    buf.addCharSequence((char*)&x, sizeof(x));
#else // Big-endian
    uint32_t dAsU32;
    memcpy(&dAsU32, &x, sizeof x);
    addLittleEndianBytes(buf, dAsU32);
#endif
    return buf;
}

VirtualByteBuffer& operator>>(VirtualByteBuffer& buf, decimal32& x)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    buf.getFixedCharSequence((char*)&x, sizeof(decimal32));
#else // Big-endian
    uint32_t dAsU32 = extractLittleEndianBytes(buf);
    memcpy(&x, &dAsU32, sizeof x);
#endif
    return buf;
}

VirtualByteBuffer& operator<<(VirtualByteBuffer& buf, const decimal64& x)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    buf.addCharSequence((char*)&x, sizeof(x));
#else // Big-endian
    uint32_t dAsU32[2];
    memcpy(&dAsU32, &x, sizeof x);
    addLittleEndianBytes(buf, dAsU32[0]);
    addLittleEndianBytes(buf, dAsU32[1]);
#endif
    return buf;
}

VirtualByteBuffer& operator>>(VirtualByteBuffer& buf, decimal64& x)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    buf.getFixedCharSequence((char*)&x, sizeof(decimal64));
#else // Big-endian
    uint32_t dAsU32[2];
    dAsU32[0] = extractLittleEndianBytes(buf);
    dAsU32[1] = extractLittleEndianBytes(buf);
    memcpy(&x, &dAsU32, sizeof x);
#endif
    return buf;
}

VirtualByteBuffer& operator<<(VirtualByteBuffer& buf, const decimal128& x)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    buf.addCharSequence((char*)&x, sizeof(x));
#else // Big-endian
    uint32_t dAsU32[4];
    memcpy(&dAsU32, &x, sizeof x);
    addLittleEndianBytes(buf, dAsU32[0]);
    addLittleEndianBytes(buf, dAsU32[1]);
    addLittleEndianBytes(buf, dAsU32[2]);
    addLittleEndianBytes(buf, dAsU32[3]);
#endif
    return buf;
}

VirtualByteBuffer& operator>>(VirtualByteBuffer& buf, decimal128& x)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    buf.getFixedCharSequence((char*)&x, sizeof(decimal128));
#else // Big-endian
    uint32_t dAsU32[4];
    dAsU32[0] = extractLittleEndianBytes(buf);
    dAsU32[1] = extractLittleEndianBytes(buf);
    dAsU32[2] = extractLittleEndianBytes(buf);
    dAsU32[3] = extractLittleEndianBytes(buf);
    memcpy(&x, &dAsU32, sizeof x);
#endif
    return buf;
}

NetworkByteBuffer& serialize(NetworkByteBuffer& buf, const decimal32& x)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    buf.addCharSequence((char*)&x, sizeof(x));
#else // Big-endian
    uint32_t dAsU32;
    memcpy(&dAsU32, &x, sizeof x);
    addLittleEndianBytes(buf, dAsU32);
#endif
    return buf;
}

NetworkByteBuffer& deserialize(NetworkByteBuffer& buf, decimal32& x)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    memcpy((void*)&x, buf.getFixedCharSequence(sizeof(x)), sizeof(x));
#else // Big-endian
    uint32_t dAsU32 = extractLittleEndianBytes(buf);
    memcpy(&x, &dAsU32, sizeof x);
#endif
    return buf;
}

NetworkByteBuffer& serialize(NetworkByteBuffer& buf, const decimal64& x)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    buf.addCharSequence((char*)&x, sizeof(x));
#else // Big-endian
    uint32_t dAsU32[2];
    memcpy(&dAsU32, &x, sizeof x);
    addLittleEndianBytes(buf, dAsU32[0]);
    addLittleEndianBytes(buf, dAsU32[1]);
#endif
    return buf;
}

NetworkByteBuffer& deserialize(NetworkByteBuffer& buf, decimal64& x)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    memcpy((void*)&x, buf.getFixedCharSequence(sizeof(x)), sizeof(x));
#else // Big-endian
    uint32_t dAsU32[2];
    dAsU32[0] = extractLittleEndianBytes(buf);
    dAsU32[1] = extractLittleEndianBytes(buf);
    memcpy(&x, &dAsU32, sizeof x);
#endif
    return buf;
}

NetworkByteBuffer& serialize(NetworkByteBuffer& buf, const decimal128& x)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    buf.addCharSequence((char*)&x, sizeof(x));
#else // Big-endian
    uint32_t dAsU32[4];
    memcpy(&dAsU32, &x, sizeof x);
    addLittleEndianBytes(buf, dAsU32[0]);
    addLittleEndianBytes(buf, dAsU32[1]);
    addLittleEndianBytes(buf, dAsU32[2]);
    addLittleEndianBytes(buf, dAsU32[3]);
#endif
    return buf;
}

NetworkByteBuffer& deserialize(NetworkByteBuffer& buf, decimal128& x)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    memcpy((void*)&x, buf.getFixedCharSequence(sizeof(x)), sizeof(x));
#else // Big-endian
    uint32_t dAsU32[4];
    dAsU32[1] = extractLittleEndianBytes(buf);
    dAsU32[0] = extractLittleEndianBytes(buf);
    dAsU32[3] = extractLittleEndianBytes(buf);
    dAsU32[2] = extractLittleEndianBytes(buf);
    memcpy(&x, &dAsU32, sizeof x);
#endif
    return buf;
}
}
