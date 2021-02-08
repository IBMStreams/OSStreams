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

//
// Class Description:
//
// Implements a resizable buffer used for serialization purposes

#ifndef SBUFFER_H
#define SBUFFER_H

#include <UTILS/DistilleryException.h>
#include <UTILS/SerializationBuffer.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/UTILSTypes.h>
#include <exception>
#include <limits.h>

UTILS_NAMESPACE_BEGIN

/// This class implements a serialization buffer, where objects can be
/// serialiazed into the buffer or de-serialized from the buffer. In a typical
/// case, the underlying memory is managed (allocated/resized/deallocated)
/// by the SBuffer class. Such a behavior can be changed by handing a
/// buffer to the constructor that takes an externally allocated buffer.
/// In such case the underlying memory will have to be deallocated externally,
/// unless setAutoDealloc is invoked to re-assign the memory management tasks
/// to the class
class SBuffer : public SerializationBuffer
{
  public:
    /// Default Constructor
    SBuffer(void);

    /// Constructor
    /// @param myCxxMemAllocStyle is true if new/delete are to be used to
    /// obtain memory allocations/deallocations instead of malloc/free
    /// @param initialSize buffer initial size (the buffer is automatically
    /// resized when necessary if autoDealloc is true, i.e., the buffer
    /// itself has not been externally alloced)
    SBuffer(const unsigned initialSize, const bool myCxxMemAllocStyle = true);

    /// Constructor -- used for handing externally allocated buffers to
    /// SBuffer (the default behavior is for the memory management
    /// capabilities to reside with the external entity, i.e., by default
    /// memory will have to be deallocated externally)
    /// @param mybuf an externally allocated buffer
    /// @param mysize the buffer size (this buffer is allocated externally --
    /// resizings can only be done later if autoDealloc is set to be true)
    SBuffer(unsigned char* mybuf, const unsigned mysize);

    /// Constructor -- used for handing externally allocated buffers to
    /// SBuffer (the default behavior is for the memory management
    /// capabilities to reside with the external entity, i.e., by default
    /// memory will have to be deallocated externally)
    /// @param mybuf an externally allocated buffer
    /// @param mysize the buffer size (this buffer is allocated externally --
    /// resizings can only be done later if autoDealloc is set to be true)
    SBuffer(char* mybuf, const unsigned mysize);

    /// Copy constructor
    /// @param sb an existing SBuffer object
    SBuffer(const SBuffer& sb);

    /// Destructive copy Constructor
    /// @param sb an existing SBuffer object
    /// @param isDestructive true means that the original object will be
    /// destroyed (i.e., the internal buffer for the original buffer will be
    /// nullified)
    SBuffer(SBuffer& sb, const bool isDestructive);

    /// Assign a serialization buffer object to another
    /// @param sb an existing SBuffer object
    SBuffer& assign(const SBuffer& sb);

    /// Copy the serialized contents of another buffer object
    /// Do no change any properties of the buffer, just the data gets copied
    /// @param sb an existing SBuffer object
    SBuffer& copyData(const SBuffer& sb);

    /// Add/serialize a character to the serialization buffer
    /// @param c a character
    inline void addChar(const char ch)
    {
        if (icursor + sizeof(char) > size) {
            resize(sizeof(char) - (size - icursor));
        }
        buf[icursor] = ch;
        icursor++;
    }

    /// Add/serialize a character to the serialization buffer
    /// @param offset deserialization offset
    /// @param c a character
    inline void addChar(unsigned offset, const char ch)
    {
        if (offset + sizeof(char) > size) {
            THROW(Serialization, "attempt to write beyond buffer limits");
        }
        buf[offset] = ch;
    }

    /// Extract/de-serialize a character from the serialization buffer
    /// @return a character
    /// @throws a SerializationException if a character cannot be extracted
    /// from the buffer
    inline char getChar(void)
    {
        if (ocursor >= size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        char ch = buf[ocursor];
        ocursor++;
        return ch;
    }

    /// Extract/de-serialize a character from the serialization buffer
    /// @param offset deserialization offset
    /// @return a character
    /// @throws a SerializationException if a character cannot be extracted
    /// from the buffer
    inline char getChar(unsigned offset) const
    {
        if (offset >= size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        char ch = buf[offset];
        return ch;
    }

    /// Add/serialize a UChar to the serialization buffer
    /// @param c a UChar
    inline void addUChar(const UChar ch)
    {
        if (icursor + sizeof(UChar) > size) {
            resize(sizeof(UChar) - (size - icursor));
        }
        memcpy(buf + icursor, Streams::host_to_network<UChar>(ch).rawData, sizeof(UChar));
        icursor += sizeof(UChar);
    }

    /// Add/serialize a UChar to the serialization buffer
    /// @param offset deserialization offset
    /// @param c a UChar
    inline void addUChar(const unsigned offset, const UChar ch)
    {
        if (offset + sizeof(UChar) > size) {
            THROW(Serialization, "attempt to write beyond buffer limits");
        }
        memcpy(buf + offset, Streams::host_to_network<UChar>(ch).rawData, sizeof(UChar));
    }

    /// Extract/de-serialize a UChar from the serialization buffer
    /// @return a UChar
    /// @throws a SerializationException if a UChar cannot be extracted
    /// from the buffer
    inline UChar getUChar(void)
    {
        if (ocursor + sizeof(UChar) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        UChar temp = Streams::network_to_host<UChar>(*reinterpret_cast<UChar*>(buf + ocursor)).data;
        ocursor += sizeof(UChar);
        return temp;
    }

    /// Extract/de-serialize a UChar from the serialization buffer
    /// @param offset deserialization offset
    /// @return a UChar
    /// @throws a SerializationException if a UChar cannot be extracted
    /// from the buffer
    inline UChar getUChar(unsigned offset) const
    {
        if (offset + sizeof(UChar) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        return Streams::network_to_host<UChar>(*reinterpret_cast<UChar*>(buf + offset)).data;
    }

    /// Add/serialize a boolean to the serialization buffer
    /// @param b a boolean
    inline void addBool(const bool b)
    {
        if (icursor + sizeof(bool) > size) {
            resize(sizeof(bool) - (size - icursor));
        }
        buf[icursor] = b ? 'T' : 'F';
        icursor++;
    }

    /// Add/serialize a boolean to the serialization buffer
    /// @param offset deserialization offset
    /// @param b a boolean
    inline void addBool(const unsigned offset, const bool b)
    {
        if (offset + sizeof(bool) > size) {
            THROW(Serialization, "attempt to write beyond buffer limits");
        }
        buf[offset] = b ? 'T' : 'F';
    }

    /// Extract/de-serialize a boolean from the serialization buffer
    /// @return a boolean
    /// @throws a SerializationException if a boolean cannot be extracted
    /// from the buffer
    inline bool getBool(void)
    {
        if (ocursor >= size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        char c = buf[ocursor];
        if (c != 'T' && c != 'F') {
            THROW(Serialization, "boolean was incorrectly encoded");
        }
        bool b = (c == 'T') ? true : false;
        ocursor++;
        return b;
    }

    /// Extract/de-serialize a boolean from the serialization buffer
    /// @param offset deserialization offset
    /// @return a boolean
    /// @throws a SerializationException if a boolean cannot be extracted
    /// from the buffer
    inline bool getBool(const unsigned offset) const
    {
        if (offset >= size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        char c = buf[offset];
        if (c != 'T' && c != 'F') {
            THROW(Serialization, "boolean was incorrectly encoded");
        }
        bool b = (c == 'T') ? true : false;
        return b;
    }

    // HANDLING UINT8 -------------------------------------------------------------

    /// Add/serialize an uint8_t to the serialization buffer
    /// @param i an uint8_t
    inline void addUInt8(const uint8_t i)
    {
        if (icursor + sizeof(uint8_t) > size) {
            resize(sizeof(uint8_t) - (size - icursor));
        }
        buf[icursor] = i;
        icursor++;
    }

    /// Add/serialize an uint8_t to the serialization buffer
    /// @param offset deserialization offset
    /// @param i an uint8_t
    inline void addUInt8(const unsigned offset, const uint8_t i)
    {
        if (offset + sizeof(uint8_t) > size) {
            THROW(Serialization, "attempt to write beyond buffer limits");
        }
        buf[offset] = i;
    }

    /// Extract/de-serialize an uint8_t from the serialization buffer
    /// @return an uint8_t
    /// @throws a SerializationException if an uint8_t cannot be extracted
    /// from the buffer
    inline uint8_t getUInt8(void)
    {
        if (ocursor + sizeof(uint8_t) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        char i = buf[ocursor];
        ocursor++;
        return i;
    }

    /// Extract/de-serialize an uint8_t from the serialization buffer
    /// @param offset deserialization offset
    /// @return an uint8_t
    /// @throws a SerializationException if an uint8_t cannot be extracted
    /// from the buffer
    inline uint8_t getUInt8(const unsigned offset) const
    {
        if (offset + sizeof(uint8_t) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        char i = buf[offset];
        return i;
    }

    // HANDLING INT8 --------------------------------------------------------------

    /// Add/serialize an int8_t to the serialization buffer
    /// @param i an int8_t
    inline void addInt8(const int8_t i)
    {
        if (icursor + sizeof(int8_t) > size) {
            resize(sizeof(int8_t) - (size - icursor));
        }
        buf[icursor] = i;
        icursor++;
    }

    /// Add/serialize an int8_t to the serialization buffer
    /// @param offset deserialization offset
    /// @param i an int8_t
    inline void addInt8(const unsigned offset, const int8_t i)
    {
        if (offset + sizeof(int8_t) > size) {
            THROW(Serialization, "attempt to write beyond buffer limits");
        }
        buf[offset] = i;
    }

    /// Extract/de-serialize an int8_t from the serialization buffer
    /// @return an int8_t
    /// @throws a SerializationException if an int8_t cannot be extracted
    /// from the buffer
    inline int8_t getInt8(void)
    {
        if (ocursor + sizeof(int8_t) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        char i = buf[ocursor];
        ocursor++;
        return i;
    }

    /// Extract/de-serialize an int8_t from the serialization buffer
    /// @param offset deserialization offset
    /// @return an int8_t
    /// @throws a SerializationException if an int8_t cannot be extracted
    /// from the buffer
    inline int8_t getInt8(const unsigned offset) const
    {
        if (offset + sizeof(int8_t) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        char i = buf[offset];
        return i;
    }

    // HANDLING UINT16 ------------------------------------------------------------

    /// Add/serialize an uint16_t to the serialization buffer
    /// @param i an uint16_t
    inline void addUInt16(const uint16_t i)
    {
        if (icursor + sizeof(uint16_t) > size) {
            resize(sizeof(uint16_t) - (size - icursor));
        }
        memcpy(buf + icursor, Streams::host_to_network<uint16_t>(i).rawData, sizeof(uint16_t));
        icursor += sizeof(uint16_t);
    }

    /// Add/serialize an uint16_t to the serialization buffer
    /// @param offset deserialization offset
    /// @param i an uint16_t
    inline void addUInt16(const unsigned offset, const uint16_t i)
    {
        if (offset + sizeof(uint16_t) > size) {
            THROW(Serialization, "attempt to write beyond buffer limits");
        }
        memcpy(buf + offset, Streams::host_to_network<uint16_t>(i).rawData, sizeof(uint16_t));
    }

    /// Extract/de-serialize a uint16_t from the serialization buffer
    /// @return a uint16_t
    /// @throws a SerializationException if a uint16_t cannot be extracted
    /// from the buffer
    inline uint16_t getUInt16(void)
    {
        if (ocursor + sizeof(uint16_t) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        uint16_t temp =
          Streams::network_to_host<uint16_t>(*reinterpret_cast<uint16_t*>(buf + ocursor)).data;
        ocursor += sizeof(uint16_t);
        return temp;
    }

    /// Extract/de-serialize a uint16_t from the serialization buffer
    /// @param offset deserialization offset
    /// @return a uint16_t
    /// @throws a SerializationException if a uint16_t cannot be extracted
    /// from the buffer
    inline uint16_t getUInt16(const unsigned offset) const
    {
        if (offset + sizeof(uint16_t) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        return Streams::network_to_host<uint16_t>(*reinterpret_cast<uint16_t*>(buf + offset)).data;
    }

    // HANDLING INT16 -------------------------------------------------------------

    /// Add/serialize an int16_t to the serialization buffer
    /// @param i an int16_t
    inline void addInt16(const int16_t i)
    {
        if (icursor + sizeof(int16_t) > size) {
            resize(sizeof(int16_t) - (size - icursor));
        }
        memcpy(buf + icursor, Streams::host_to_network<int16_t>(i).rawData, sizeof(int16_t));
        icursor += sizeof(int16_t);
    }

    /// Add/serialize an int16_t to the serialization buffer
    /// @param offset deserialization offset
    /// @param i an int16_t
    inline void addInt16(const unsigned offset, const int16_t i)
    {
        if (offset + sizeof(int16_t) > size) {
            THROW(Serialization, "attempt to write beyond buffer limits");
        }
        memcpy(buf + offset, Streams::host_to_network<int16_t>(i).rawData, sizeof(int16_t));
    }

    /// Extract/de-serialize an int16_t from the serialization buffer
    /// @return an int16_t
    /// @throws a SerializationException if an int16_t cannot be extracted
    /// from the buffer
    inline int16_t getInt16(void)
    {
        if (ocursor + sizeof(int16_t) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        int16_t temp =
          Streams::network_to_host<int16_t>(*reinterpret_cast<int16_t*>(buf + ocursor)).data;
        ocursor += sizeof(int16_t);
        return temp;
    }

    /// Extract/de-serialize an int16_t from the serialization buffer
    /// @param offset deserialization offset
    /// @return an int16_t
    /// @throws a SerializationException if an int16_t cannot be extracted
    /// from the buffer
    inline int16_t getInt16(const unsigned offset) const
    {
        if (offset + sizeof(int16_t) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        return Streams::network_to_host<int16_t>(*reinterpret_cast<int16_t*>(buf + offset)).data;
    }

    // HANDLING UINT32 ------------------------------------------------------------

    /// Add/serialize an uint32_t to the serialization buffer
    /// @param i an uint32_t
    inline void addUInt32(const uint32_t i)
    {
        if (icursor + sizeof(uint32_t) > size) {
            resize(sizeof(uint32_t) - (size - icursor));
        }
        memcpy(buf + icursor, Streams::host_to_network<uint32_t>(i).rawData, sizeof(uint32_t));
        icursor += sizeof(uint32_t);
    }

    /// Add/serialize an uint32_t to the serialization buffer
    /// @param offset deserialization offset
    /// @param i an uint32_t
    inline void addUInt32(const unsigned offset, const uint32_t i)
    {
        if (offset + sizeof(uint32_t) > size) {
            THROW(Serialization, "attempt to write beyond buffer limits");
        }
        memcpy(buf + offset, Streams::host_to_network<uint32_t>(i).rawData, sizeof(uint32_t));
    }

    /// Extract/de-serialize a uint32_t from the serialization buffer
    /// @return a uint32_t
    /// @throws a SerializationException if a uint32_t cannot be extracted
    /// from the buffer
    inline uint32_t getUInt32(void)
    {
        if (ocursor + sizeof(uint32_t) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        uint32_t temp =
          Streams::network_to_host<uint32_t>(*reinterpret_cast<uint32_t*>(buf + ocursor)).data;
        ocursor += sizeof(uint32_t);
        return temp;
    }

    /// Extract/de-serialize a uint32_t from the serialization buffer
    /// @param offset deserialization offset
    /// @return a uint32_t
    /// @throws a SerializationException if a uint32_t cannot be extracted
    /// from the buffer
    inline uint32_t getUInt32(const unsigned offset) const
    {
        if (offset + sizeof(uint32_t) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        return Streams::network_to_host<uint32_t>(*reinterpret_cast<uint32_t*>(buf + offset)).data;
    }

    // HANDLING INT32 -------------------------------------------------------------

    /// Add/serialize an int32_t to the serialization buffer
    /// @param i an int32_t
    inline void addInt32(const int32_t i)
    {
        if (icursor + sizeof(int32_t) > size) {
            resize(sizeof(int32_t) - (size - icursor));
        }
        memcpy(buf + icursor, Streams::host_to_network<int32_t>(i).rawData, sizeof(int32_t));
        icursor += sizeof(int32_t);
    }

    /// Add/serialize an int32_t to the serialization buffer
    /// @param offset deserialization offset
    /// @param i an int32_t
    inline void addInt32(const unsigned offset, const int32_t i)
    {
        if (offset + sizeof(int32_t) > size) {
            THROW(Serialization, "attempt to write beyond buffer limits");
        }
        memcpy(buf + offset, Streams::host_to_network<int32_t>(i).rawData, sizeof(int32_t));
    }

    /// Extract/de-serialize an int32_t from the serialization buffer
    /// @return an int32_t
    /// @throws a SerializationException if an int32_t cannot be extracted
    /// from the buffer
    inline int32_t getInt32(void)
    {
        if (ocursor + sizeof(int32_t) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        uint32_t temp =
          Streams::network_to_host<uint32_t>(*reinterpret_cast<uint32_t*>(buf + ocursor)).data;
        ocursor += sizeof(int32_t);
        return temp;
    }

    /// Extract/de-serialize an int32_t from the serialization buffer
    /// @param offset deserialization offset
    /// @return an int32_t
    /// @throws a SerializationException if an int32_t cannot be extracted
    /// from the buffer
    inline int32_t getInt32(const unsigned offset) const
    {
        if (offset + sizeof(int32_t) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        return Streams::network_to_host<uint32_t>(*reinterpret_cast<uint32_t*>(buf + offset)).data;
    }

    // HANDLING UINT64 ------------------------------------------------------------

    /// Add/serialize a uint64_t to the serialization buffer
    /// @param i a uint64_t
    inline void addUInt64(const uint64_t i)
    {
        if (icursor + sizeof(uint64_t) > size) {
            resize(sizeof(uint64_t) - (size - icursor));
        }
        memcpy(buf + icursor, Streams::host_to_network<uint64_t>(i).rawData, sizeof(uint64_t));
        icursor += sizeof(uint64_t);
    }

    /// Add/serialize a uint64_t to the serialization buffer
    /// @param offset deserialization offset
    /// @param i a uint64_t
    inline void addUInt64(const unsigned offset, const uint64_t i)
    {
        if (offset + sizeof(uint64_t) > size) {
            THROW(Serialization, "attempt to write beyond buffer limits");
        }
        memcpy(buf + offset, Streams::host_to_network<uint64_t>(i).rawData, sizeof(uint64_t));
    }

    /// Extract/de-serialize a uint64_t from the serialization buffer
    /// @return a uint64_t
    /// @throws a SerializationException if a uint64_t cannot be extracted
    /// from the buffer
    inline uint64_t getUInt64(void)
    {
        if (ocursor + sizeof(uint64_t) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        uint64_t temp =
          Streams::network_to_host<uint64_t>(*reinterpret_cast<uint64_t*>(buf + ocursor)).data;
        ocursor += sizeof(uint64_t);
        return temp;
    }

    /// Extract/de-serialize a uint64_t from the serialization buffer
    /// @param offset deserialization offset
    /// @return a uint64_t
    /// @throws a SerializationException if a uint64_t cannot be extracted
    /// from the buffer
    inline uint64_t getUInt64(const unsigned offset) const
    {
        if (offset + sizeof(uint64_t) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        return Streams::network_to_host<uint64_t>(*reinterpret_cast<uint64_t*>(buf + offset)).data;
    }

    // HANDLING INT64 -------------------------------------------------------------

    /// Add/serialize an int64_t to the serialization buffer
    /// @param i an int64_t
    void addInt64(const int64_t i)
    {
        if (icursor + sizeof(int64_t) > size) {
            resize(sizeof(int64_t) - (size - icursor));
        }
        memcpy(buf + icursor, Streams::host_to_network<int64_t>(i).rawData, sizeof(int64_t));
        icursor += sizeof(int64_t);
    }

    /// Add/serialize an int64_t to the serialization buffer
    /// @param offset deserialization offset
    /// @param i an int64_t
    void addInt64(const unsigned offset, const int64_t i)
    {
        if (offset + sizeof(int64_t) > size) {
            THROW(Serialization, "attempt to write beyond buffer limits");
        }
        memcpy(buf + offset, Streams::host_to_network<int64_t>(i).rawData, sizeof(int64_t));
    }

    /// Extract/de-serialize an int64_t from the serialization buffer
    /// @return an int64_t
    /// @throws a SerializationException if an int64_t cannot be extracted
    /// from the buffer
    inline int64_t getInt64(void)
    {
        if (ocursor + sizeof(int64_t) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        int64_t temp =
          Streams::network_to_host<int64_t>(*reinterpret_cast<int64_t*>(buf + ocursor)).data;
        ocursor += sizeof(int64_t);
        return temp;
    }

    /// Extract/de-serialize an int64_t from the serialization buffer
    /// @param offset deserialization offset
    /// @return an int64_t
    /// @throws a SerializationException if an int64_t cannot be extracted
    /// from the buffer
    inline int64_t getInt64(const unsigned offset) const
    {
        if (offset + sizeof(int64_t) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        return Streams::network_to_host<int64_t>(*reinterpret_cast<int64_t*>(buf + offset)).data;
    }

    // HANDLING FLOAT -------------------------------------------------------------

    /// Add/serialize a float to the serialization buffer
    /// @param f a single-precision floating point number
    inline void addFloat(const float f)
    {
        if (icursor + sizeof(float) > size) {
            resize(sizeof(float) - (size - icursor));
        }
        memcpy(buf + icursor, Streams::host_to_network<float>(f).rawData, sizeof(float));
        icursor += sizeof(float);
    }

    /// Add/serialize a float to the serialization buffer
    /// @param offset deserialization offset
    /// @param f a single-precision floating point number
    inline void addFloat(const unsigned offset, const float f)
    {
        if (offset + sizeof(float) > size) {
            THROW(Serialization, "attempt to write beyond buffer limits");
        }
        memcpy(buf + offset, Streams::host_to_network<float>(f).rawData, sizeof(float));
    }

    /// Extract/de-serialize a float from the serialization buffer
    /// @return a float
    /// @throws a SerializationException if a float cannot be extracted
    /// from the buffer
    inline float getFloat(void)
    {
        if (ocursor + sizeof(float) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        float temp = Streams::network_to_host<float>(*reinterpret_cast<float*>(buf + ocursor)).data;
        ocursor += sizeof(float);
        return temp;
    }

    /// Extract/de-serialize a float from the serialization buffer
    /// @param offset deserialization offset
    /// @return a float
    /// @throws a SerializationException if a float cannot be extracted
    /// from the buffer
    inline float getFloat(const unsigned offset) const
    {
        if (offset + sizeof(float) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        return Streams::network_to_host<float>(*reinterpret_cast<float*>(buf + offset)).data;
    }

    // HANDLING DOUBLE -------------------------------------------------------------

    /// Add/serialize a double to the serialization buffer
    /// @param d a double precision floating point number
    inline void addDouble(const double d)
    {
        if (icursor + sizeof(double) > size) {
            resize(sizeof(double) - (size - icursor));
        }
        memcpy(buf + icursor, Streams::host_to_network<double>(d).rawData, sizeof(double));
        icursor += sizeof(double);
    }

    /// Add/serialize a double to the serialization buffer
    /// @param offset deserialization offset
    /// @param d a double precision floating point number
    inline void addDouble(const unsigned offset, const double d)
    {
        if (offset + sizeof(double) > size) {
            THROW(Serialization, "attempt to write beyond buffer limits");
        }
        memcpy(buf + offset, Streams::host_to_network<double>(d).rawData, sizeof(double));
    }

    /// Extract/de-serialize a double from the serialization buffer
    /// @return a double
    /// @throws a SerializationException if a double cannot be extracted
    /// from the buffer
    inline double getDouble(void)
    {
        if (ocursor + sizeof(double) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        double temp =
          Streams::network_to_host<double>(*reinterpret_cast<double*>(buf + ocursor)).data;
        ocursor += sizeof(double);
        return temp;
    }

    /// Extract/de-serialize a double from the serialization buffer
    /// @param offset deserialization offset
    /// @return a double
    /// @throws a SerializationException if a double cannot be extracted
    /// from the buffer
    inline double getDouble(const unsigned offset) const
    {
        if (offset + sizeof(double) > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        return Streams::network_to_host<double>(*reinterpret_cast<double*>(buf + offset)).data;
    }

    // ----------------------------------------------------------------------------

    /// Add/serialize a NULL-terminated string to the serialization buffer
    /// @param str a NULL terminated string
    inline void addNTStr(const char* str)
    {
        // adds a NULL Terminated String
        unsigned stlen = strlen(str) + 1;
        if (icursor + stlen > size) {
            resize(stlen - (size - icursor));
        }
        memcpy(buf + icursor, str, stlen);
        icursor += stlen;
    }

    /// Add/serialize a NULL-terminated string to the serialization buffer
    /// @param offset deserialization offset
    /// @param str a NULL terminated string
    inline void addNTStr(const unsigned offset, const char* str)
    {
        // adds a NULL Terminated String
        unsigned stlen = strlen(str) + 1;
        if (offset + stlen > size) {
            THROW(Serialization, "attempt to write beyond buffer limits");
        }
        memcpy(buf + offset, str, stlen);
    }

    /// Extract/de-serialize a NULL-terminated string from the
    /// serialization buffer
    /// @return a pointer to a NULL-terminated string
    /// @throws a SerializationException if a NULL-terminated string cannot
    /// be extracted from the buffer
    inline char* getNTStr(void)
    {
        unsigned stlen = strlen((char*)buf + ocursor) + 1;
        if (ocursor + stlen > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        char* ptr = reinterpret_cast<char*>(buf) + ocursor;
        ocursor += stlen;
        return ptr;
    }

    /// Extract/de-serialize a NULL-terminated string from the
    /// serialization buffer
    /// @param offset deserialization offset
    /// @return a pointer to a NULL-terminated string
    /// @throws a SerializationException if a NULL-terminated string cannot
    /// be extracted from the buffer
    inline char* getNTStr(const unsigned offset) const
    {
        unsigned stlen = strlen((char*)buf + offset) + 1;
        if (offset + stlen > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        char* ptr = reinterpret_cast<char*>(buf) + offset;
        return ptr;
    }

    /// Add/serialize an STL string to the serialization buffer
    /// @param str an STL string
    inline void addSTLString(const std::string& str)
    {
        unsigned stlen = str.size();
        if (stlen >= 0xffffffff) {
            THROW(Serialization, "maximum string size is " << 0xffffffff);
        }
        addUInt32(stlen);
        if (stlen > 0) {
            if (icursor + stlen > size) {
                resize(stlen - (size - icursor));
            }
            memcpy(buf + icursor, str.c_str(), stlen);
            icursor += stlen;
        }
    }

    /// Add/serialize an STL string to the serialization buffer
    /// @param offset deserialization offset
    /// @param str an STL string
    inline void addSTLString(const unsigned offset, const std::string& str)
    {
        unsigned stlen = str.size();
        if (stlen >= 0xffffffff) {
            THROW(Serialization, "maximum string size is " << 0xffffffff);
        }
        addUInt32(stlen);
        if (stlen > 0) {
            if (offset + stlen + 2 > size) {
                THROW(Serialization, "attempt to write beyond buffer limits");
            }
            memcpy(buf + offset + 2, str.c_str(), stlen);
        }
    }

    /// Extract/de-serialize an STL string from the serialization buffer
    /// @return an STL string
    /// @throws a SerializationException if an STL string cannot
    /// be extracted from the buffer
    inline std::string getSTLString(void)
    {
        unsigned stlen = getUInt32();
        if (stlen > 0) {
            if (ocursor + stlen > size) {
                THROW(Serialization, "attempt to read beyond buffer limits");
            }
            char* ptr = reinterpret_cast<char*>(buf) + ocursor;
            ocursor += stlen;
            return std::string(ptr, stlen);
        }
        return std::string("");
    }

    /// Extract/de-serialize an STL string from the serialization buffer
    /// @param offset deserialization offset
    /// @return an STL string
    /// @throws a SerializationException if an STL string cannot
    /// be extracted from the buffer
    inline std::string getSTLString(const unsigned offset) const
    {
        unsigned stlen = getUInt32(offset);
        if (stlen > 0) {
            if (offset + stlen + 2 > size) {
                THROW(Serialization, "attempt to read beyond buffer limits");
            }
            char* ptr = reinterpret_cast<char*>(buf) + offset + 2;
            return std::string(ptr, stlen);
        }
        return std::string("");
    }

    /// Add/serialize an ICU Unicode string to the serialization buffer
    /// @param ustr an ICU Unicode string
    inline void addUnicodeString(const icu::UnicodeString& ustr)
    {
        uint32_t stlen = ustr.length();
        addUInt32(stlen);
        const UChar* ptr = ustr.getBuffer();
        for (unsigned i = 0; i < stlen; ++i) {
            addUChar(ptr[i]);
        }
    }

    /// Add/serialize an ICU Unicode string to the serialization buffer
    /// @param offset deserialization offset
    /// @param ustr an ICU Unicode string
    inline void addUnicodeString(const unsigned offset, const icu::UnicodeString& ustr)
    {
        uint32_t stlen = ustr.length();
        addUInt32(offset, stlen);
        const UChar* ptr = ustr.getBuffer();
        for (unsigned i = 0; i < stlen; ++i) {
            addUChar(offset + i + 4, ptr[i]);
        }
    }

    /// Extract/de-serialize an ICU Unicode string from the
    /// serialization buffer
    /// @return an ICU Unicode string
    inline icu::UnicodeString getUnicodeString(void)
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

    /// Extract/de-serialize an ICU Unicode string from the
    /// @param offset deserialization offset
    /// serialization buffer
    /// @return an ICU Unicode string
    inline icu::UnicodeString getUnicodeString(const unsigned offset)
    {
        uint32_t stlen = getUInt32(offset);
        UChar* uch = new UChar[stlen];
        unsigned i;
        for (i = 0; i < stlen; ++i) {
            uch[i] = getUChar(offset + 4 + i);
        }
        // this is really ugly, but that is how it is (to avoid leaks)!
        icu::UnicodeString temp(uch, stlen, stlen);
        icu::UnicodeString temp2(temp);
        delete[] uch;
        return icu::UnicodeString(temp2);
    }

    /// Add/serialize an opaque blob to the serialization buffer
    /// @param blob a pointer to an opaque blob
    /// @param blobsize the opaque blob size
    inline void addBlob(const void* blob, const uint32_t bsize)
    {
        addUInt32(bsize);
        if (icursor + bsize > size) {
            resize(bsize - (size - icursor));
        }
        memcpy(buf + icursor, blob, bsize);
        icursor += bsize;
    }
    /// Add/serialize an opaque blob to the serialization buffer
    /// @param offset deserialization offset
    /// @param blob a pointer to an opaque blob
    /// @param blobsize the opaque blob size
    inline void addBlob(const unsigned offset, const void* blob, const uint32_t bsize)
    {
        addUInt32(offset, bsize);
        if (offset + bsize + 4 > size) {
            THROW(Serialization, "attempt to write beyond buffer limits");
        }
        memcpy(buf + offset + 4, blob, bsize);
    }

    /// Extract/de-serialize an opaque blob from the serialization buffer
    /// @param size the size of the returned opaque blob
    /// @return a pointer to an opaque blob
    /// @throws a SerializationException if an opaque blob cannot
    /// be extracted from the buffer
    inline unsigned char* getBlob(uint32_t& mysize)
    {
        mysize = getUInt32();
        if (ocursor + mysize > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        unsigned char* ptr = buf + ocursor;
        ocursor += mysize;
        return ptr;
    }

    /// Extract/de-serialize an opaque blob from the serialization buffer
    /// @param offset deserialization offset
    /// @param size the size of the returned opaque blob
    /// @return a pointer to an opaque blob
    /// @throws a SerializationException if an opaque blob cannot
    /// be extracted from the buffer
    inline unsigned char* getBlob(const unsigned offset, uint32_t& mysize) const
    {
        mysize = getUInt32(offset);
        if (offset + mysize + 4 > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        unsigned char* ptr = buf + offset + 4;
        return ptr;
    }

    /// Add/serialize a sequence of characters to the serialization buffer
    /// NOTE: the size of the sequence is not serialized (for that use
    /// addBlob)
    /// @param chsbuf a pointer to a sequence of characters
    /// @param chssize the size for the sequence of characters
    inline void addCharSequence(const char* chsbuf, const uint32_t chssize)
    {
        if (icursor + chssize > size) {
            resize(chssize - (size - icursor));
        }
        memcpy(buf + icursor, chsbuf, chssize);
        icursor += chssize;
    }

    /// Add/serialize a sequence of characters to the serialization buffer
    /// NOTE: the size of the sequence is not serialized (for that use
    /// addBlob)
    /// @param chsbuf a pointer to a sequence of characters
    /// @param chssize the size for the sequence of characters
    inline void addCharSequence(const unsigned offset, const char* chsbuf, const uint32_t chssize)
    {
        if (offset + chssize > size) {
            THROW(Serialization, "attempt to write beyond buffer limits");
        }
        memcpy(buf + offset, chsbuf, chssize);
    }

    /// Extract/de-serialize a sequence of characters from the serialization
    /// buffer -- the sequence begins at the output cursor position and
    /// ends at the end of the serialization buffer
    /// @param sizeTillEOB the size of the sequence until the end of the
    /// serialization buffer
    /// @return a pointer to sequence of characters
    inline char* getCharSequence(uint32_t& sizeTillEOB)
    {
        sizeTillEOB = icursor - ocursor;
        unsigned char* ptr = buf + ocursor;
        ocursor = size;
        return reinterpret_cast<char*>(ptr);
    }

    /// Extract/de-serialize a sequence of characters from the serialization
    /// buffer -- the sequence begins at the output cursor position and
    /// ends at the end of the serialization buffer
    /// @param offset deserialization offset
    /// @param sizeTillEOB the size of the sequence until the end of the
    /// serialization buffer
    /// @return a pointer to sequence of characters
    inline char* getCharSequence(const unsigned offset, uint32_t& sizeTillEOB) const
    {
        sizeTillEOB = icursor - offset;
        unsigned char* ptr = buf + offset;
        return reinterpret_cast<char*>(ptr);
    }

    /// Extract/de-serialize a fixed-size sequence of characters from
    /// the serialization buffer -- the sequence begins at the output cursor
    /// position
    /// @param size the size of the sequence
    /// serialization buffer
    /// @return a pointer to sequence of characters
    inline char* getFixedCharSequence(const uint32_t mysize)
    {
        if (ocursor + mysize > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        unsigned char* ptr = buf + ocursor;
        ocursor += mysize;
        return reinterpret_cast<char*>(ptr);
    }

    /// Extract/de-serialize a fixed-size sequence of characters from
    /// the serialization buffer -- the sequence begins at the output cursor
    /// position
    /// @param offset deserialization offset
    /// @param size the size of the sequence
    /// serialization buffer
    /// @return a pointer to sequence of characters
    inline char* getFixedCharSequence(const unsigned offset, const uint32_t mysize) const
    {
        if (offset + mysize > size) {
            THROW(Serialization, "attempt to read beyond buffer limits");
        }
        unsigned char* ptr = buf + offset;
        return reinterpret_cast<char*>(ptr);
    }

    /// Add/serialize a pointer to the serialization buffer
    /// @param ptr a void* pointer
    inline void addPointer(const void* ptr)
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

    /// Add/serialize a pointer to the serialization buffer
    /// @param offset deserialization offset
    /// @param ptr a void* pointer
    inline void addPointer(const unsigned offset, const void* ptr)
    {
#ifdef __LP64__
        assert(sizeof(void*) == sizeof(int64_t));
        addChar(offset, PTR_64BIT);
        int64_t addr = reinterpret_cast<int64_t>(ptr);
        addInt64(offset + 1, addr);
#else
        assert(sizeof(void*) == sizeof(int32_t));
        addChar(offset, PTR_32BIT);
        int32_t addr = reinterpret_cast<int32_t>(ptr);
        addInt32(offset + 1, addr);
#endif
    }

    /// Extract/de-serialize a pointer from the serialization buffer
    /// @return an void* pointer
    /// @throws a SerializationException if a void* pointer cannot
    /// be extracted from the buffer
    inline void* getPointer(void)
    {
        char c = getChar();
#ifdef __LP64__
        assert(sizeof(void*) == sizeof(int64_t));
        if (c != PTR_64BIT) {
            THROW(Serialization,
                  "SBuffer cannot convert pointer to 64-bit pointer (you probably do not want that "
                  "anyway)");
        }
        int64_t ptr = getInt64();
        return reinterpret_cast<void*>(ptr);
#else
        assert(sizeof(void*) == sizeof(int32_t));
        if (c != PTR_32BIT) {
            THROW(Serialization,
                  "SBuffer cannot convert pointer to 32-bit pointer (you probably do not want that "
                  "anyway)");
        }
        int32_t ptr = getInt32();
        return reinterpret_cast<void*>(ptr);
#endif
    }

    /// Extract/de-serialize a pointer from the serialization buffer
    /// @param offset deserialization offset
    /// @return an void* pointer
    /// @throws a SerializationException if a void* pointer cannot
    /// be extracted from the buffer
    inline void* getPointer(const unsigned offset) const
    {
        char c = getChar(offset);
#ifdef __LP64__
        assert(sizeof(void*) == sizeof(int64_t));
        if (c != PTR_64BIT) {
            THROW(Serialization,
                  "SBufferNBF cannot convert pointer to 64-bit pointer (you probably do not want "
                  "that anyway)");
        }
        int64_t ptr = getInt64(offset + 1);
        return reinterpret_cast<void*>(ptr);
#else
        assert(sizeof(void*) == sizeof(int32_t));
        if (c != PTR_32BIT) {
            THROW(Serialization,
                  "SBufferNBF cannot convert pointer to 32-bit pointer (you probably do not want "
                  "that anyway)");
        }
        int32_t ptr = getInt32(offset + 1);
        return reinterpret_cast<void*>(ptr);
#endif
    }

    /// Set the cursor for the next serialization operation
    /// @param off the new offset
    virtual void setICursor(const unsigned off)
    {
        if (off > size) {
            resize(off - size);
        }
        icursor = off;
    };

    /// Get the internal buffer input cursor
    /// @return current input cursor buffer cursor
    inline unsigned getICursor(void) const { return icursor; };

    /// Obtain the size of the content currently in the buffer
    /// @return the size of the content currently in the buffer
    virtual unsigned getContentSize(void) const { return icursor; };

    /// Obtain the size of the buffer
    /// @return the size of the buffer
    virtual unsigned getSize(void) const { return size; };

    /// Set the cursor for the next de-serialization operation
    /// @param off the new offset
    virtual void setOCursor(const unsigned off)
    {
        if (off >= size) {
            resize(off - size + 1);
        }
        ocursor = off;
    };

    /// Get the internal buffer output cursor
    /// @return current output cursor buffer cursor
    virtual inline unsigned getOCursor(void) const { return ocursor; };

    /// Obtain number of serialized bytes remaining from the location
    /// pointed by the output cursor till the end of the buffer
    virtual inline unsigned getNRemainingBytes(void) const { return icursor - ocursor; }

    /// Obtain the pointer for the internal buffer
    /// @return an unsigned char* pointer to the internal buffer
    inline unsigned char* getPtr(void) { return buf; };

    /// Obtain the pointer for the internal buffer
    /// @return an unsigned char* pointer to the internal buffer
    inline unsigned char* getUCharPtr(void) { return buf; };

    /// Obtain the pointer for the internal buffer
    /// @return a char* pointer to the internal buffer
    virtual inline char* getCharPtr(void) { return reinterpret_cast<char*>(buf); };

    /// Obtain the (const) pointer for the internal buffer
    /// @return a char* pointer to the internal buffer
    virtual inline const char* getCharPtr(void) const
    {
        return reinterpret_cast<const char*>(buf);
    };

    /// Obtain the actual size of the serialized data (as opposed to the
    /// size of the serialization buffer)
    /// @return the size of the serialized data
    inline unsigned getSerializedDataSize(void) const { return icursor; };

    /// Set/reset the auto deallocation flag (true means that the buffer
    /// is self-managed and false means that the buffer is externally
    /// managed/owned)
    /// @param myAutoDealloc the new value for the auto deallocation flag
    /// @param myCxxMemAllocStyle is true if new/delete are too be used to
    /// obtain memory allocations/deallocations instead of malloc/free
    inline void setAutoDealloc(const bool myAutoDealloc, const bool myCxxMemAllocStyle = true)
    {
        autoDealloc = myAutoDealloc;
        cxxMemAllocStyle = myCxxMemAllocStyle;
    };

    /// Write the buffer to a (file/socket/pipe) descriptor
    /// @param descriptor the (file/socket/pipe) descriptor
    /// @throws a SerializationException if the write operation fails
    virtual void write(const int descriptor) const;

    /// Read the buffer from a (file/socket/pipe) descriptor
    /// @param descriptor the (file/socket/pipe) descriptor
    /// @throws a SerializationException if the read operation fails
    virtual void read(const int descriptor);

    /// Equality operator, checks if the buffer size and binary contents are
    /// the same
    /// @param b the sbuffer with which we are checking equality with
    virtual bool operator==(const SBuffer& b) const;

    /// Inequality operator, checks if the buffer size and binary contents
    /// are the same
    /// @param b the sbuffer with which we are checking equality with
    virtual bool operator!=(const SBuffer& b) const { return !(this->operator==(b)); }

    /// Compress the contents of an SBuffer into another one
    /// @param out the output SBuffer with the compressed contents
    void compress(SBuffer& out);

    /// Decompress the contents of an SBuffer into another one
    /// @param out the output SBuffer with the decompressed contents
    void decompress(SBuffer& out);

    /// Destructor
    ///
    virtual ~SBuffer(void);

  protected:
    /// Resize buffer
    /// @param the additional number of bytes necessary in the resizing
    /// @throws a SerializationException if the original buffer to be
    /// resized was externally allocated
    inline void resize(const uint64_t extra)
    {
#ifdef DEBUG
        cout << "Resizing....." << endl;
#endif
        if (!autoDealloc) {
            THROW(Serialization, "attempt to resize external buffer -- current size "
                                   << size << ". There is a need for " << extra << " bytes");
        }

        uint64_t maxGrowth = UINT_MAX - size;
        uint64_t growth = extra;
        if (extra > maxGrowth) {
            THROW(Serialization,
                  "attempt to resize external buffer will lead to a size greater than the maximum "
                  "allowed size -- current size "
                    << size << ". There is a need for " << extra
                    << " bytes, but the maximum allowed growth is " << maxGrowth << " bytes");
        } else {
            // testing whether we have to more than double the current buffer
            // size
            if (extra > size) {
                // grow by the required amount
                growth = extra;
            } else {
                // we know that we can grow by the necessary amount, but
                // let's see if we can double the size
                if (size > maxGrowth) {
                    // we can't double the size, so let's grow as much as we
                    // can
                    growth = maxGrowth;
                } else {
                    // we can double the size, so let's do it
                    growth = size;
                }
            }
        }

        uint64_t newsize = size + growth;
        unsigned char* newbuf;

        newbuf = alloc(newsize);

        if (size > 0) {
            memcpy(newbuf, buf, size);
        }

        dealloc(buf);
        size = newsize;
        buf = newbuf;
    }

    /// Allocate internal buffer
    /// @param size amount of memory to be allocated
    inline unsigned char* alloc(const unsigned mysize)
    {
        if (cxxMemAllocStyle) {
            return new unsigned char[mysize];
        } else {
            return static_cast<unsigned char*>(malloc(mysize));
        }
    }

    /// Deallocated memory
    void dealloc(unsigned char* mybuf)
    {
        if (cxxMemAllocStyle) {
            delete[] mybuf;
        } else {
            free(mybuf);
        }
    }

    /// should memory be allocated/deallocated C++-style (new/delete)
    bool cxxMemAllocStyle;

    /// is the buffer self-allocated/self-managed
    bool autoDealloc;

    /// buffer
    unsigned char* buf;

    /// buffer size
    uint64_t size;

    /// the cursor for data that is being serialized (i.e., added to
    /// the buffer)
    uint64_t icursor;

    /// ocursor: is the cursor for data that is being de-serialized (i.e.,
    /// being read and de-serialized from the buffer)
    uint64_t ocursor;
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
