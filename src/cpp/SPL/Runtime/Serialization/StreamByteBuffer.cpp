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

#include <SPL/Runtime/Serialization/StreamByteBuffer.h>

#include <SPL/Runtime/Type/String.h>
#include <SPL/Runtime/Type/TypeHelper.h>
#include <UTILS/SupportFunctions.h>

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <unistd.h>

#include <boost/scoped_array.hpp>

using namespace std;
using namespace SPL;
using namespace Distillery;
using namespace Streams;

void StreamByteBuffer::addChar(const char ch)
{
    _stream.write(&ch, sizeof(ch));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "add failed in addChar");
    }
}

char StreamByteBuffer::getChar()
{
    char ch;
    _stream.read(&ch, sizeof(ch));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "get failed in getChar");
    }
    return ch;
}

void StreamByteBuffer::addBool(const bool b)
{
    _stream.write((const char*)&b, sizeof(b));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "add failed in addBool");
    }
}

bool StreamByteBuffer::getBool()
{
    bool ch;
    _stream.read((char*)&ch, sizeof(ch));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "get failed in getBool");
    }
    return ch;
}

void StreamByteBuffer::addUInt8(const uint8_t i)
{
    _stream.write((const char*)&i, sizeof(i));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "add failed in addUInt8");
    }
}

uint8_t StreamByteBuffer::getUInt8()
{
    uint8_t i;
    _stream.read((char*)&i, sizeof(i));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "get failed in getUInt8");
    }
    return i;
}

void StreamByteBuffer::addInt8(const int8_t i)
{
    _stream.write((const char*)&i, sizeof(i));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "add failed in addInt8");
    }
}

int8_t StreamByteBuffer::getInt8()
{
    int8_t i;
    _stream.read((char*)&i, sizeof(i));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "get failed in getInt8");
    }
    return i;
}

void StreamByteBuffer::addUInt16(const uint16_t i)
{
    _stream.write(host_to_network<uint16_t>(i).rawData, sizeof(uint16_t));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "add failed in addUInt16");
    }
}

uint16_t StreamByteBuffer::getUInt16()
{
    uint16_t temp;
    _stream.read((char*)&temp, sizeof(temp));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "get failed in getUInt16");
    }
    return network_to_host<uint16_t>(temp).data;
}

void StreamByteBuffer::addInt16(const int16_t i)
{
    _stream.write(host_to_network<int16_t>(i).rawData, sizeof(int16_t));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "add failed in addInt16");
    }
}

int16_t StreamByteBuffer::getInt16()
{
    int16_t temp;
    _stream.read((char*)&temp, sizeof(temp));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "get failed in getInt16");
    }
    return network_to_host<int16_t>(temp).data;
}

void StreamByteBuffer::addUInt32(const uint32_t i)
{
    _stream.write(host_to_network<uint32_t>(i).rawData, sizeof(uint32_t));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "add failed in addUInt32");
    }
}

uint32_t StreamByteBuffer::getUInt32()
{
    uint32_t temp;
    _stream.read((char*)&temp, sizeof(temp));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "get failed in getUInt32");
    }
    return network_to_host<uint32_t>(temp).data;
}

void StreamByteBuffer::addInt32(const int32_t i)
{
    _stream.write(host_to_network<int32_t>(i).rawData, sizeof(int32_t));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "add failed in addInt32");
    }
}

int32_t StreamByteBuffer::getInt32()
{
    int32_t temp;
    _stream.read((char*)&temp, sizeof(temp));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "get failed in getInt32");
    }
    return network_to_host<int32_t>(temp).data;
}

void StreamByteBuffer::addUInt64(const uint64_t i)
{
    _stream.write(host_to_network<uint64_t>(i).rawData, sizeof(uint64_t));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "add failed in addUInt64");
    }
}

uint64_t StreamByteBuffer::getUInt64()
{
    uint64_t temp;
    _stream.read((char*)&temp, sizeof(temp));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "get failed in getUInt64");
    }
    return network_to_host<uint64_t>(temp).data;
}

void StreamByteBuffer::addInt64(const int64_t i)
{
    _stream.write(host_to_network<int64_t>(i).rawData, sizeof(int64_t));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "add failed in addInt64");
    }
}

int64_t StreamByteBuffer::getInt64()
{
    int64_t temp;
    _stream.read((char*)&temp, sizeof(temp));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "get failed in getInt64");
    }
    return network_to_host<int64_t>(temp).data;
}

void StreamByteBuffer::addFloat(const float f)
{
    _stream.write(host_to_network<float>(f).rawData, sizeof(float));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "add failed in addFloat");
    }
}

float StreamByteBuffer::getFloat()
{
    float temp;
    _stream.read((char*)&temp, sizeof(temp));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "get failed in getFloat");
    }
    return network_to_host<float>(temp).data;
}

void StreamByteBuffer::addDouble(const double f)
{
    _stream.write(host_to_network<double>(f).rawData, sizeof(double));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "add failed in addDouble");
    }
}

double StreamByteBuffer::getDouble()
{
    double temp;
    _stream.read((char*)&temp, sizeof(temp));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "get failed in getDouble");
    }
    return network_to_host<double>(temp).data;
}

void StreamByteBuffer::addLongDouble(const long double f)
{
    _stream.write(host_to_network<long double>(f).rawData, sizeof(HostNetworkTmp<long double>));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "add failed in addLongDouble");
    }
}

long double StreamByteBuffer::getLongDouble()
{
    HostNetworkTmp<long double> temp;
    _stream.read((char*)&temp, sizeof(temp));
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "get failed in getLongDouble");
    }
    return network_to_host<long double>(temp.data).data;
}

void StreamByteBuffer::addNTStr(const char* str)
{
    uint32_t stlen = strlen(str) + 1;
    _stream.write(str, stlen);
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "add failed in addNTStr");
    }
}

char* StreamByteBuffer::getNTStr(char* buffer, uint32_t bsize)
{
    // Read a string of unknown length into a buffer
    bool malloced = false;
    if (!buffer || bsize == 0) {
        buffer = (char*)malloc(1024);
        bsize = 1024;
        malloced = true;
    }
    uint32_t size = 0;
    while (!_stream && !_stream.eof()) {
        // will the next character fit?
        if (size == bsize) {
            // grow it
            if (malloced) {
                buffer = (char*)realloc(buffer, bsize * 2);
                bsize *= 2;
            } else {
                bsize = bsize > 1024 ? (2 * bsize) : 2048;
                malloced = true;
                buffer = (char*)malloc(bsize);
            }
        }
        char ch;
        _stream >> ch;
        buffer[size++] = ch;
        if (ch == '\0') {
            break; // aha! end of string
        }
    }
    return buffer;
}

void StreamByteBuffer::addSTLString(const std::string& str)
{
    uint32_t stlen = str.size();
    serializeLength(*this, stlen);
    _stream.write(str.c_str(), stlen);
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "add failed in addSTLStr");
    }
}

std::string StreamByteBuffer::getSTLString()
{
    uint32_t stlen = deserializeLength(*this);
    char buffer[2 * 1048];
    if (stlen > sizeof(buffer)) {
        char* x = new char[stlen];
        std::auto_ptr<char> p(x);
        _stream.read(x, stlen);
        if (!_stream) {
            THROW_CHAR(SPLRuntimeSerialization, "get failed in getSTLStr");
        }
        return std::string(x, stlen);
    }
    _stream.read(buffer, stlen);
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "get failed in getSTLStr");
    }
    return std::string(buffer, stlen);
}

void StreamByteBuffer::getSTLString(std::string& str)
{
    uint32_t stlen = deserializeLength(*this);
    str.resize(stlen);
    _stream.read(&str[0], stlen);
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "get failed in getSTLStr");
    }
}

void StreamByteBuffer::addSPLString(const RSTRING_BB_TYPE& str)
{
    uint32_t stlen = str.size();
    serializeLength(*this, stlen);
    _stream.write(str.c_str(), stlen);
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "add failed in addSTLStr");
    }
}

RSTRING_BB_TYPE
StreamByteBuffer::getSPLString()
{
    uint32_t stlen = deserializeLength(*this);
    char buffer[2 * 1048];
    if (stlen > sizeof(buffer)) {
        char* x = new char[stlen];
        std::auto_ptr<char> p(x);
        _stream.read(x, stlen);
        if (!_stream) {
            THROW_CHAR(SPLRuntimeSerialization, "get failed in getSTLStr");
        }
        return RSTRING_BB_TYPE(x, stlen);
    }
    _stream.read(buffer, stlen);
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "get failed in getSTLStr");
    }
    return RSTRING_BB_TYPE(buffer, stlen);
}

void StreamByteBuffer::getSPLString(RSTRING_BB_TYPE& str)
{
    uint32_t stlen = deserializeLength(*this);
    str.resize(stlen);
    _stream.read(&str[0], stlen);
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "get failed in getSTLStr");
    }
}

void StreamByteBuffer::addUnicodeString(const USTRING_BB_TYPE& ustr)
{
    uint32_t stlen = ustr.length();
    serializeLength(*this, stlen);
    const UChar* ptr = ustr.getBuffer();
    for (uint32_t i = 0; i < stlen; ++i) {
        addUInt16(ptr[i]);
    }
}

USTRING_BB_TYPE
StreamByteBuffer::getUnicodeString()
{
    uint32_t stlen = deserializeLength(*this);
    boost::scoped_array<UChar> uch(new UChar[stlen]);
    for (uint32_t i = 0; i < stlen; ++i) {
        uch[i] = getUInt16();
    }
    USTRING_BB_TYPE temp(uch.get(), stlen);
    return temp;
}

void StreamByteBuffer::addBlob(const void* blob, const uint32_t bsize)
{
    _stream.write((const char*)&bsize, sizeof(bsize));
    _stream.write((const char*)blob, bsize);
}

unsigned char* StreamByteBuffer::getBlob(unsigned char* buffer, uint32_t bsize)
{
    uint32_t blobSize;
    _stream.read((char*)&blobSize, sizeof(blobSize));
    if (!buffer || blobSize > bsize) {
        buffer = new unsigned char[blobSize];
    }
    _stream.read((char*)buffer, blobSize);
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "get failed in getBlob");
    }
    return buffer;
}

void StreamByteBuffer::addCharSequence(const char* chsbuf, const uint32_t chssize)
{
    _stream.write((const char*)chsbuf, chssize);
}

char* StreamByteBuffer::getFixedCharSequence(char* buffer, uint32_t bsize)
{
    if (!buffer) {
        buffer = new char[bsize];
    }
    _stream.read(buffer, bsize);
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "get failed in getFixedCharSequence");
    }
    return buffer;
}

char* StreamByteBuffer::getFixedCharSequence(char* buffer, uint32_t bsize, uint32_t& resultSize)
{
    if (!buffer) {
        buffer = new char[bsize];
    }
    if (!_stream) {
        THROW_CHAR(SPLRuntimeSerialization, "get failed in getFixedCharSequence");
    }
    _stream.read(buffer, bsize);
    resultSize = _stream.gcount();
    if (resultSize != bsize) {
        _stream.clear(_stream.rdstate() & ~std::ios_base::failbit); // don't fail here....
    }
    return buffer;
}

void StreamByteBuffer::addPointer(const void* ptr)
{
#ifdef __LP64__
    assert(sizeof(void*) == sizeof(int64_t));
    addChar(PTR_64BIT);
    int64_t addr = reinterpret_cast<int64_t>(ptr);
    addInt64(addr);
#else
    assert(sizeof(void*) == sizeof(int32_t));
    addChar(PTR_32BIT);
    int32_t addr = reinterpret_cast<int32_t>(ptr);
    addInt32(addr);
#endif
}

void* StreamByteBuffer::getPointer()
{
    char c = getChar();
#ifdef __LP64__
    assert(sizeof(void*) == sizeof(int64_t));
    if (c != PTR_64BIT) {
        THROW_CHAR(SPLRuntimeSerialization,
                   "StreamByteBuffer cannot convert pointer to 64-bit pointer (you probably do not "
                   "want that anyway)");
    }
    int64_t ptr = getInt64();
    return reinterpret_cast<void*>(ptr);
#else
    assert(sizeof(void*) == sizeof(int32_t));
    if (c != PTR_32BIT)
        THROW_CHAR(SPLRuntimeSerialization,
                   "StreamByteBuffer cannot convert pointer to 32-bit pointer (you probably do not "
                   "want that anyway)");
    int32_t ptr = getInt32();
    return reinterpret_cast<void*>(ptr);
#endif
}
