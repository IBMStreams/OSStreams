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

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>

#include <UTILS/SBuffer.h>
#include <UTILS/SupportFunctions.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <new>
#include <sstream>
#include <unistd.h>

using namespace std;
using namespace boost::iostreams;
UTILS_NAMESPACE_USE

SBuffer::SBuffer(void)
  : cxxMemAllocStyle(true)
  , autoDealloc(true)
  , buf(NULL)
  , size(DEFAULT_SIZE)
  , icursor(0)
  , ocursor(0)
{

    if ((sizeof(uint16_t) != sizeof(UChar))) {
        THROW(Serialization, "SBuffer implementation requires sizeof(uint16_t)==sizeof(UChar)");
    }

    if (size == 0) {
        buf = NULL;
    } else {
        buf = alloc(size);
    }
}

SBuffer::SBuffer(const unsigned initialSize, const bool myCxxMemAllocStyle)
  : cxxMemAllocStyle(myCxxMemAllocStyle)
  , autoDealloc(true)
  , buf(NULL)
  , size(initialSize)
  , icursor(0)
  , ocursor(0)
{

    if ((sizeof(uint16_t) != sizeof(UChar))) {
        THROW(Serialization, "SBuffer implementation requires sizeof(uint16_t)==sizeof(UChar)");
    }

    if (size == 0) {
        buf = NULL;
    } else {
        buf = alloc(size);
    }
}

SBuffer::SBuffer(unsigned char* mybuf, const unsigned mysize)
  : cxxMemAllocStyle(true)
  , autoDealloc(false)
  ,
  // autoDealloc is false because we're just using a buffer allocated elsewhere
  // and thus someone else should take care of deallocating it for us
  buf(mybuf)
  , size(mysize)
  , icursor(mysize)
  , ocursor(0)
{
    if ((sizeof(uint16_t) != sizeof(UChar))) {
        THROW(Serialization, "SBuffer implementation requires sizeof(uint16_t)==sizeof(UChar)");
    }
}

SBuffer::SBuffer(char* mybuf, const unsigned mysize)
  : cxxMemAllocStyle(true)
  , autoDealloc(false)
  ,
  // autoDealloc is false because we're just using a buffer allocated elsewhere
  // and thus someone else should take care of deallocating it for us
  buf((unsigned char*)mybuf)
  , size(mysize)
  , icursor(mysize)
  , ocursor(0)
{
    if ((sizeof(uint16_t) != sizeof(UChar))) {
        THROW(Serialization, "SBuffer implementation requires sizeof(uint16_t)==sizeof(UChar)");
    }
}

SBuffer::SBuffer(const SBuffer& r)
  : cxxMemAllocStyle(true)
  , autoDealloc(true)
  , buf(NULL)
  , size(r.size)
  , icursor(r.icursor)
  , ocursor(r.ocursor)
{
    if ((sizeof(uint16_t) != sizeof(UChar))) {
        THROW(Serialization, "SBuffer implementation requires sizeof(uint16_t)==sizeof(UChar)");
    }
    buf = alloc(size);
    memcpy(buf, r.buf, size);
}

SBuffer::SBuffer(SBuffer& r, const bool isDestructive)
  : cxxMemAllocStyle(true)
  , autoDealloc(true)
  , buf(NULL)
  , size(r.size)
  , icursor(r.icursor)
  , ocursor(r.ocursor)
{
    if ((sizeof(uint16_t) != sizeof(UChar))) {
        THROW(Serialization, "SBuffer implementation requires sizeof(uint16_t)==sizeof(UChar)");
    }
    // a destructive copy will destroy the original object
    if (isDestructive) {
        buf = r.buf;
        r.cxxMemAllocStyle = true;
        r.autoDealloc = true;
        r.buf = NULL;
        r.size = 0;
        r.icursor = 0;
        r.ocursor = 0;
    }
    // a non-destructive copy will duplicate the original object
    else {
        buf = alloc(size);
        memcpy(buf, r.buf, size);
    }
}

SBuffer& SBuffer::assign(const SBuffer& sb)
{
    if (this != &sb) {
        cxxMemAllocStyle = true;
        autoDealloc = true;
        size = sb.size;
        icursor = sb.icursor;
        ocursor = sb.ocursor;
        dealloc(buf);
        buf = alloc(size);
        memcpy(buf, sb.buf, size);
    }
    return *this;
}

SBuffer& SBuffer::copyData(const SBuffer& sb)
{
    if (this != &sb) {
        if (size < sb.icursor) {
            resize(sb.icursor - size);
        }
        memcpy(buf, sb.buf, sb.icursor);
        icursor = sb.icursor;
    }
    ocursor = 0;
    return *this;
}

void SBuffer::write(const int descriptor) const
{
    // write the size
    int ret = ::write(descriptor, &size, sizeof(size));
    if (ret != sizeof(size)) {
        THROW(Serialization, "write failed");
    }
    // write the buffer
    ret = ::write(descriptor, buf, size);
    if (ret != static_cast<int>(size)) {
        THROW(Serialization, "write failed");
    }
}

void SBuffer::read(const int descriptor)
{
    // read the size
    int ret = ::read(descriptor, &size, sizeof(size));
    if (ret != sizeof(size)) {
        THROW(Serialization, "read failed");
    }
    if (buf && autoDealloc) {
        dealloc(buf);
    }
    buf = alloc(size);
    // read the buffer
    ret = ::read(descriptor, buf, size);
    if (ret != static_cast<int>(size)) {
        THROW(Serialization, "read failed");
    }
    autoDealloc = true;
}

bool SBuffer::operator==(const SBuffer& sb) const
{
    // first check size
    if (getSerializedDataSize() != sb.getSerializedDataSize()) {
        return false;
    }
    // check bytes consistency
    if (memcmp(buf, sb.buf, getSerializedDataSize()) != 0) {
        return false;
    }
    // no difference
    return true;
}

void SBuffer::compress(SBuffer& out)
{
    stringstream tcompressed;
    stringstream tuncompressed;
    std::stringbuf* pbuf = tuncompressed.rdbuf();
    pbuf->sputn(reinterpret_cast<char*>(buf), icursor);
    boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    in.push(gzip_compressor());
    in.push(tuncompressed);
    boost::iostreams::copy(in, tcompressed);
    /*
    cout << "compressed: ";
    for(unsigned i=0;i<tcompressed.str().size();++i) {
      cout << static_cast<int>(tcompressed.str().c_str()[i]) << " ";
    }
    cout << endl;
    */
    out.addBlob(tcompressed.str().c_str(), tcompressed.str().size());
}

void SBuffer::decompress(SBuffer& out)
{
    stringstream tcompressed;
    stringbuf* pbuf = tcompressed.rdbuf();
    pbuf->sputn(reinterpret_cast<char*>(buf), icursor);
    /*
    cout << "to be decompressed: ";
    for(unsigned i=0;i<tcompressed.str().size();++i) {
      cout << static_cast<int>(tcompressed.str().c_str()[i]) << " ";
    }
    cout << endl;
    */
    stringstream tuncompressed;
    filtering_streambuf<input> in;
    in.push(gzip_decompressor());
    in.push(tcompressed);
    boost::iostreams::copy(in, tuncompressed);
    out.addBlob(tuncompressed.str().c_str(), tuncompressed.str().size());
}

SBuffer::~SBuffer(void)
{
    if (autoDealloc) {
        dealloc(buf);
        buf = NULL;
    }
}
