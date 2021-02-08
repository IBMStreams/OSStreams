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
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Type/Blob.h>
#include <SPL/Runtime/Type/List.h>
#include <SPL/Runtime/Type/PrimitiveType.h>

#include <cstring>
#include <iomanip>
#include <sstream>

#define REAL_SIZE(sz) ((sz) & ~DONT_FREE)

using namespace std;
using namespace std::tr1;
using namespace SPL;

blob::blob(const blob& ot)
{
    copyData(ot.data_, REAL_SIZE(ot.size_));
}

blob::blob(unsigned char const* data, uint64_t size)
{
    copyData(data, size);
}

blob::blob(const SPL::List& bytes)
{
    if (bytes.getElementMetaType() != Meta::Type::UINT8) {
        SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, SPL_RUNTIME_INVALID_LIST_CAST_TO_BLOB,
                            SPL_FUNC_DBG);
    }
    size_ = bytes.getSize();
    if (size_ == 0) {
        data_ = NULL;
        return;
    }

    data_ = new unsigned char[size_];
    // we have data from 'bytes'
    ConstValueHandle cvh = bytes.getElement(0);
    const SPL::uint8& data = cvh;
    memcpy(data_, (const unsigned char*)&data, size_);
}

blob::~blob()
{
    deleteData();
}

void blob::deleteData()
{
    if (data_) {
        if ((size_ & DONT_FREE) == 0) {
            delete[] data_;
        }
        data_ = NULL;
    }
}

void blob::copyData(unsigned char const* data, uint64_t size)
{
    size_ = size;
    if (data) {
        data_ = new unsigned char[size];
        memcpy(data_, data, size);
    } else {
        data_ = NULL;
        size_ = 0;
    }
}

blob& blob::operator=(const blob& ot)
{
    if (&ot == this) {
        return *this;
    }
    deleteData();
    copyData(ot.data_, REAL_SIZE(ot.size_));
    return *this;
}

// Append data from ot to the end of my data
void blob::append(unsigned char const* data, uint64_t size)
{
    if (!data || size == 0) {
        return;
    }
    uint64_t mySize = REAL_SIZE(size_);
    if (!data_ || mySize == 0) {
        // just grab a copy of the data
        deleteData(); // in case we had some with size 0
        copyData(data, size);
        return;
    }
    // we have data from this and new data.  Concatenate it
    unsigned char* newData = new unsigned char[mySize + size];
    memcpy(newData, data_, mySize);
    memcpy(newData + mySize, data, size);

    // transfer it in
    adoptData(newData, mySize + size);
}

blob& blob::operator+=(const blob& ot)
{
    append(ot.data_, REAL_SIZE(ot.size_));
    return *this;
}

blob& blob::operator+=(SPL::uint8 byte)
{
    append((unsigned char*)(&byte), 1);
    return *this;
}

blob& blob::operator+=(const SPL::List& bytes)
{
    if (bytes.getElementMetaType() != Meta::Type::UINT8) {
        SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, SPL_RUNTIME_INVALID_LIST_CAST_TO_BLOB,
                            SPL_FUNC_DBG);
    }
    // we have data from 'bytes'
    ConstValueHandle cvh = bytes.getElement(0);
    const SPL::uint8& data = cvh;
    append((const unsigned char*)&data, bytes.getSize());
    return *this;
}

// Return a new blob with the concatenation of this one and ot
blob blob::operator+(const blob& ot) const
{
    uint64_t newSize;

    // Figure out the combined size
    uint64_t mySize = REAL_SIZE(size_);
    if (data_ && mySize != 0) {
        newSize = mySize;
    } else {
        newSize = 0;
    }

    uint64_t otSize = REAL_SIZE(ot.size_);
    if (ot.data_ && otSize != 0) {
        newSize += otSize;
    }

    // Somewhere to put the data
    unsigned char* newData = new unsigned char[newSize];
    unsigned char* next = newData;

    // Now create the concatenated blob
    if (data_ && mySize != 0) {
        memcpy(newData, data_, mySize);
        next = newData + mySize;
    }
    if (ot.data_ && otSize != 0) {
        memcpy(next, ot.data_, otSize);
    }

    blob result;
    result.adoptData(newData, newSize);
    return result;
}

bool blob::operator==(const blob& ot) const
{
    if (&ot == this) {
        return true;
    }

    uint64_t mySize = REAL_SIZE(size_);
    uint64_t otSize = REAL_SIZE(ot.size_);
    // Both 0 size?
    if ((!data_ || mySize == 0) && (!ot.data_ || otSize == 0)) {
        return true;
    }

    // Now that we have looked at the both empty case, we can compare the sizes
    if (otSize != mySize) {
        return false;
    }

    // Point to same data? They are now the same size
    if (data_ == ot.data_) {
        return true;
    }

    // only one is empty?
    if (!data_ || mySize == 0 || !ot.data_ || otSize == 0) {
        return false;
    }

    // both have data of same size - just compare it
    return (0 == memcmp(data_, ot.data_, mySize));
}

bool blob::operator<(const blob& ot) const
{
    // Check some trivial cases first
    if (&ot == this) {
        return false;
    }
    uint64_t mySize = REAL_SIZE(size_);
    uint64_t otSize = REAL_SIZE(ot.size_);
    if (data_ == ot.data_) {
        return mySize < otSize;
    }

    if (data_ == NULL || otSize == 0) {
        // any empty blob must be less than any non-empty one
        return otSize > 0;
    }

    // We have data...  Does the RHS?
    if (ot.data_ == NULL || otSize == 0) {
        return false; // an empty RHS must be less
    }

    // Only case left is that both have data
    if (mySize == otSize) { // same size - just compare the data
        return memcmp(data_, ot.data_, mySize) < 0;
    }

    // Different sizes;  compare the common part first
    uint64_t cmpSize = (mySize < otSize) ? mySize : otSize;
    int cmp = memcmp(data_, ot.data_, cmpSize);

    if (cmp < 0) { // common part is less
        return true;
    }
    if (cmp > 0) { // common part is greater
        return false;
    }

    // The common part is identical.  The smaller one is less
    return cmpSize == mySize; // True if LHS is smaller one
}

bool blob::operator>(const blob& ot) const
{
    // Check some trivial cases first
    if (&ot == this) {
        return false;
    }

    uint64_t mySize = REAL_SIZE(size_);
    uint64_t otSize = REAL_SIZE(ot.size_);
    if (data_ == ot.data_) {
        return mySize < otSize;
    }

    if (data_ == NULL || mySize == 0) {
        // any empty blob can't be greater than anything
        return false;
    }

    // We have data...  Does the RHS?
    if (ot.data_ == NULL || otSize == 0) {
        return true; // an empty RHS must be less
    }

    // Only case left is that both have data
    if (mySize == otSize) { // same size - just compare the data
        return memcmp(data_, ot.data_, mySize) > 0;
    }

    // Different sizes;  compare the common part first
    uint64_t cmpSize = (mySize < otSize) ? mySize : otSize;
    int cmp = memcmp(data_, ot.data_, cmpSize);

    if (cmp > 0) { // common part is greater
        return true;
    }
    if (cmp < 0) { // common part is less
        return false;
    }

    // The common part is identical.  The smaller one is less
    return cmpSize == otSize; // True if RHS is smaller one
}

unsigned char const& blob::operator[](uint64_t idx) const
{
    if (idx >= REAL_SIZE(size_)) {
        indexOutOfBound(idx, REAL_SIZE(size_));
    }
    return data_[idx];
}

unsigned char& blob::operator[](uint64_t idx)
{
    if (idx >= REAL_SIZE(size_)) {
        indexOutOfBound(idx, REAL_SIZE(size_));
    }
    return data_[idx];
}

void blob::setData(unsigned char const* data, uint64_t size)
{
    deleteData();
    copyData(data, size);
}

void blob::adoptData(unsigned char* data, uint64_t size)
{
    deleteData();
    data_ = data;
    if (data) {
        size_ = size;
    } else {
        size_ = 0;
    }
}

unsigned char* blob::releaseData(uint64_t& size)
{
    if (size_ & DONT_FREE) {
        SPLTRACEMSGANDTHROW(SPLRuntimeBlobNotOwned, L_ERROR, SPL_RUNTIME_BLOB_MODIFY_UNOWNED,
                            SPL_FUNC_DBG);
    }
    size = size_;
    unsigned char* tdata = data_;
    size_ = 0;
    data_ = NULL;
    return tdata;
}

void blob::swap(blob& ot)
{
    // We can't use releaseData, in case we don't own some of the memory
    uint64_t msize = size_;
    unsigned char* mdata = data_;
    size_ = ot.size_;
    data_ = ot.data_;
    ot.size_ = msize;
    ot.data_ = mdata;
}

// we do not care about performance here
void blob::serialize(ostream& ostr) const
{
    using namespace std;
    ostringstream sstr;
    sstr.imbue(locale::classic());
    sstr << hex;
    uint64_t mySize = REAL_SIZE(size_);
    for (uint64_t i = 0; i < mySize; ++i) {
        sstr << setw(2) << setfill('0') << static_cast<uint32_t>(data_[i]);
    }
    ostr << sstr.str();
}

// we do not care about performance here
void blob::deserialize(istream& istr)
{
    using namespace std;
    deleteData();
    size_ = 0;

    // We can't just read the whole blob as a string, as we may read too much.
    // Read one character at a time.
    unsigned char byte;
    bool highByte = true;
    std::vector<unsigned char> temp;
    int c;
    while ((c = istr.peek()) >= 0) {
        // is this byte still part of the blob?
        int v;
        if (c >= '0' && c <= '9') {
            v = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            v = 10 + c - 'a';
        } else if (c >= 'A' && c <= 'F') {
            v = 10 + c - 'A';
        } else {
            break;
        }

        // okay, we have a nybble.   What to do?
        if (highByte) {
            // top half
            byte = v << 4;
            highByte = false;
        } else {
            // low byte
            byte |= v;
            temp.push_back(byte);
            highByte = true;
        }

        // Now read and discard the character
        (void)istr.get();
    }

    // Do we have an even number of nybbles?
    if (!highByte) {
        // We are in the middle of a byte
        istr.setstate(ios_base::failbit);
        return;
    }
    size_ = temp.size();
    data_ = new unsigned char[size_];
    memcpy(data_, &temp[0], size_);
}

size_t blob::hashCode() const
{
    size_t r = 17;
    std::tr1::hash<size_t> hs;
    for (uint64_t i = 0, iu = this->getSize(); i < iu; ++i) {
        r = 37 * r + hs(this->operator[](i));
    }
    return r;
}
