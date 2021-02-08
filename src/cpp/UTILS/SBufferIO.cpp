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

#include <UTILS/SBufferIO.h>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <unistd.h>

using namespace std;
UTILS_NAMESPACE_USE
using namespace Streams;

void SBufferIO::addChar(const char ch)
{
    _stream.write(&ch, sizeof(ch));
    if (!_stream) {
        THROW(Serialization, "add failed in addChar");
    }
}

char SBufferIO::getChar()
{
    char ch;
    _stream.read(&ch, sizeof(ch));
    if (!_stream) {
        THROW(Serialization, "get failed in getChar");
    }
    return ch;
}

void SBufferIO::addUChar(const UChar ch)
{
    _stream.write(host_to_network<UChar>(ch).rawData, sizeof(ch));
    if (!_stream) {
        THROW(Serialization, "add failed in addUChar");
    }
}

UChar SBufferIO::getUChar()
{
    UChar temp;
    _stream.read((char*)&temp, sizeof(temp));
    if (!_stream) {
        THROW(Serialization, "get failed in getUChar");
    }
    return network_to_host<UChar>(temp).data;
}

void SBufferIO::addBool(const bool b)
{
    _stream.write((const char*)&b, sizeof(b));
    if (!_stream) {
        THROW(Serialization, "add failed in addBool");
    }
}

bool SBufferIO::getBool()
{
    bool ch;
    _stream.read((char*)&ch, sizeof(ch));
    if (!_stream) {
        THROW(Serialization, "get failed in getBool");
    }
    if (ch != 'T' && ch != 'F') {
        THROW(Serialization, "boolean was incorrectly encoded");
    }
    return (ch == 'T');
}

void SBufferIO::addUInt8(const uint8_t i)
{
    _stream.write((const char*)&i, sizeof(i));
    if (!_stream) {
        THROW(Serialization, "add failed in addUInt8");
    }
}

uint8_t SBufferIO::getUInt8()
{
    uint8_t i;
    _stream.read((char*)&i, sizeof(i));
    if (!_stream) {
        THROW(Serialization, "get failed in getUInt8");
    }
    return i;
}

void SBufferIO::addInt8(const int8_t i)
{
    _stream.write((const char*)&i, sizeof(i));
    if (!_stream) {
        THROW(Serialization, "add failed in addInt8");
    }
}

int8_t SBufferIO::getInt8()
{
    int8_t i;
    _stream.read((char*)&i, sizeof(i));
    if (!_stream) {
        THROW(Serialization, "get failed in getInt8");
    }
    return i;
}

void SBufferIO::addUInt16(const uint16_t i)
{
    _stream.write(host_to_network<uint16_t>(i).rawData, sizeof(uint16_t));
    if (!_stream) {
        THROW(Serialization, "add failed in addUInt16");
    }
}

uint16_t SBufferIO::getUInt16()
{
    uint16_t temp;
    _stream.read((char*)&temp, sizeof(temp));
    if (!_stream) {
        THROW(Serialization, "get failed in getUInt16");
    }
    return network_to_host<uint16_t>(temp).data;
}

void SBufferIO::addInt16(const int16_t i)
{
    _stream.write(host_to_network<int16_t>(i).rawData, sizeof(int16_t));
    if (!_stream) {
        THROW(Serialization, "add failed in addInt16");
    }
}

int16_t SBufferIO::getInt16()
{
    int16_t temp;
    _stream.read((char*)&temp, sizeof(temp));
    if (!_stream) {
        THROW(Serialization, "get failed in getInt16");
    }
    return network_to_host<int16_t>(temp).data;
}

void SBufferIO::addUInt32(const uint32_t i)
{
    _stream.write(host_to_network<uint32_t>(i).rawData, sizeof(uint32_t));
    if (!_stream) {
        THROW(Serialization, "add failed in addUInt32");
    }
}

uint32_t SBufferIO::getUInt32()
{
    uint32_t temp;
    _stream.read((char*)&temp, sizeof(temp));
    if (!_stream) {
        THROW(Serialization, "get failed in getUInt32");
    }
    return network_to_host<uint32_t>(temp).data;
}

void SBufferIO::addInt32(const int32_t i)
{
    _stream.write(host_to_network<int32_t>(i).rawData, sizeof(int32_t));
    if (!_stream) {
        THROW(Serialization, "add failed in addInt32");
    }
}

int32_t SBufferIO::getInt32()
{
    int32_t temp;
    _stream.read((char*)&temp, sizeof(temp));
    if (!_stream) {
        THROW(Serialization, "get failed in getInt32");
    }
    return network_to_host<int32_t>(temp).data;
}

void SBufferIO::addUInt64(const uint64_t i)
{
    _stream.write(host_to_network<uint64_t>(i).rawData, sizeof(uint64_t));
    if (!_stream) {
        THROW(Serialization, "add failed in addUInt64");
    }
}

uint64_t SBufferIO::getUInt64()
{
    uint64_t temp;
    _stream.read((char*)&temp, sizeof(temp));
    if (!_stream) {
        THROW(Serialization, "get failed in getUInt64");
    }
    return network_to_host<uint64_t>(temp).data;
}

void SBufferIO::addInt64(const int64_t i)
{
    _stream.write(host_to_network<int64_t>(i).rawData, sizeof(int64_t));
    if (!_stream) {
        THROW(Serialization, "add failed in addInt64");
    }
}

int64_t SBufferIO::getInt64()
{
    int64_t temp;
    _stream.read((char*)&temp, sizeof(temp));
    if (!_stream) {
        THROW(Serialization, "get failed in getInt64");
    }
    return network_to_host<int64_t>(temp).data;
}

void SBufferIO::addFloat(const float f)
{
    _stream.write(host_to_network<float>(f).rawData, sizeof(float));
    if (!_stream) {
        THROW(Serialization, "add failed in addFloat");
    }
}

float SBufferIO::getFloat()
{
    float temp;
    _stream.read((char*)&temp, sizeof(temp));
    if (!_stream) {
        THROW(Serialization, "get failed in getFloat");
    }
    return network_to_host<float>(temp).data;
}

void SBufferIO::addDouble(const double f)
{
    _stream.write(host_to_network<double>(f).rawData, sizeof(double));
    if (!_stream) {
        THROW(Serialization, "add failed in addDouble");
    }
}

double SBufferIO::getDouble()
{
    double temp;
    _stream.read((char*)&temp, sizeof(temp));
    if (!_stream) {
        THROW(Serialization, "get failed in getDouble");
    }
    return network_to_host<double>(temp).data;
}

void SBufferIO::addNTStr(const char* str)
{
    unsigned stlen = strlen(str) + 1;
    _stream.write(str, stlen);
    if (!_stream) {
        THROW(Serialization, "add failed in addNTStr");
    }
}

char* SBufferIO::getNTStr(char* buffer, unsigned bsize)
{
    // Read a string of unknown length into a buffer
    bool malloced = false;
    if (!buffer || bsize == 0) {
        buffer = (char*)malloc(1024);
        bsize = 1024;
        malloced = true;
    }
    unsigned size = 0;
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

void SBufferIO::addSTLString(const std::string& str)
{
    unsigned stlen = str.size();
    if (stlen >= 0xffffffff) {
        THROW(Serialization, "maximum string size is " << 0xffffffff);
    }
    addUInt32(stlen);
    _stream.write(str.c_str(), stlen);
    if (!_stream) {
        THROW(Serialization, "add failed in addSTLStr");
    }
}

std::string SBufferIO::getSTLString()
{
    unsigned stlen = getUInt32();
    char buffer[2 * 1048];
    if (stlen > sizeof(buffer)) {
        char* x = new char[stlen];
        std::auto_ptr<char> p(x);
        _stream.read(x, stlen);
        if (!_stream) {
            THROW(Serialization, "get failed in getSTLStr");
        }
        return std::string(x, stlen);
    }
    _stream.read(buffer, stlen);
    if (!_stream) {
        THROW(Serialization, "get failed in getSTLStr");
    }
    return std::string(buffer, stlen);
}

void SBufferIO::addUnicodeString(const icu::UnicodeString& ustr)
{
    uint32_t stlen = ustr.length();
    addUInt32(stlen);
    const UChar* ptr = ustr.getBuffer();
    for (unsigned i = 0; i < stlen; ++i) {
        addUChar(ptr[i]);
    }
}

icu::UnicodeString SBufferIO::getUnicodeString()
{
    uint32_t stlen = getUInt32();
    UChar* uch = new UChar[stlen];
    unsigned i;
    for (i = 0; i < stlen; ++i) {
        uch[i] = getUChar();
    }
    // this is really ugly, but that is how it is (to avoid leaks)!
    icu::UnicodeString temp(uch, stlen, stlen);
    icu::UnicodeString temp2(temp);
    delete[] uch;
    return icu::UnicodeString(temp2);
}

void SBufferIO::addBlob(const void* blob, const uint32_t bsize)
{
    _stream.write((const char*)&bsize, sizeof(bsize));
    _stream.write((const char*)blob, bsize);
}

unsigned char* SBufferIO::getBlob(unsigned char* buffer, unsigned bsize)
{
    uint32_t blobSize;
    _stream.read((char*)&blobSize, sizeof(blobSize));
    if (!buffer || blobSize > bsize) {
        buffer = new unsigned char[blobSize];
    }
    _stream.read((char*)buffer, blobSize);
    if (!_stream) {
        THROW(Serialization, "get failed in getBlob");
    }
    return buffer;
}

void SBufferIO::addCharSequence(const char* chsbuf, const uint32_t chssize)
{
    _stream.write((const char*)chsbuf, chssize);
}

char* SBufferIO::getFixedCharSequence(char* buffer, unsigned bsize)
{
    if (!buffer) {
        buffer = new char[bsize];
    }
    _stream.read(buffer, bsize);
    if (!_stream) {
        THROW(Serialization, "get failed in getFixedCharSequence");
    }
    return buffer;
}

char* SBufferIO::getFixedCharSequence(char* buffer, unsigned bsize, unsigned& resultSize)
{
    if (!buffer) {
        buffer = new char[bsize];
    }
    if (!_stream) {
        THROW(Serialization, "get failed in getFixedCharSequence");
    }
    _stream.read(buffer, bsize);
    resultSize = _stream.gcount();
    if (resultSize != bsize) {
        _stream.clear(_stream.rdstate() & ~std::ios_base::failbit); // don't fail here....
    }
    return buffer;
}

void SBufferIO::addPointer(const void* ptr)
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

void* SBufferIO::getPointer()
{
    char c = getChar();
#ifdef __LP64__
    assert(sizeof(void*) == sizeof(int64_t));
    if (c != PTR_64BIT) {
        THROW(Serialization,
              "SBufferIO cannot convert pointer to 64-bit pointer (you probably do not want that "
              "anyway)");
    }
    int64_t ptr = getInt64();
    return reinterpret_cast<void*>(ptr);
#else
    assert(sizeof(void*) == sizeof(int32_t));
    if (c != PTR_32BIT)
        THROW(Serialization,
              "SBufferIO cannot convert pointer to 32-bit pointer (you probably do not want that "
              "anyway)");
    int32_t ptr = getInt32();
    return reinterpret_cast<void*>(ptr);
#endif
}
