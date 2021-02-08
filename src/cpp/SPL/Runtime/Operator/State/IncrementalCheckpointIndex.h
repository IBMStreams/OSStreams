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
 * \file IncrementalCheckpointIndex.h \brief Definition of the SPL::IncrementalCheckpointIndex
 * class.
 */
#ifndef SPL_RUNTIME_OPERATOR_STATE_INCREMENTAL_CHECKPOINT_INDEX_H
#define SPL_RUNTIME_OPERATOR_STATE_INCREMENTAL_CHECKPOINT_INDEX_H

#include <SPL/Runtime/Utility/Visibility.h>
#include <stdint.h>

namespace SPL {
/// Foward declaration
class DataStoreByteBuffer;

/// \brief Class that represents an index to checkpointed data in either base or delta form
class DLL_PUBLIC IncrementalCheckpointIndex
{
  public:
    /// Types of checkpoint
    typedef uint8_t Type;

    /// Default constructor
    IncrementalCheckpointIndex();

    /// Destructor
    ~IncrementalCheckpointIndex();

    /// Serialize the index to a DataStoreByteBuffer
    /// @param buffer a DataStoreByteBuffer handle
    void serialize(DataStoreByteBuffer& buffer) const;

    /// Deserialize the index from a DataStoreByteBuffer
    /// @param buffer a DataStoreByteBuffer handle
    void deserialize(DataStoreByteBuffer& buffer);

    /// Get the type of index
    /// @return the type of index
    Type getType() const { return type_; }

    void setType(Type type) { type_ = type; }

    /// Test whether this index is for base checkpoint
    /// @return true if this index is for base checkpoint, return false otherwise
    bool isBase() const { return type_ == BASE; }

    /// Get the starting offset of the checkpoint data pointed to by this index
    /// @reurn the starting offset of the checkpoint data pointed to by this index
    uint64_t getOffset() const { return offset_; }

    /// Set the starting offset of the checkpoint data pointed to by this index
    /// @param offset the starting offset of the checkpoint data pointed to by this index
    void setOffset(const uint64_t offset) { offset_ = offset; }

    /// Get the previous delta checkpoint sequence ID
    /// @return the previous checkpoint sequence ID, return 0 if this index itself is base
    /// or the first delta checkpoint
    int64_t getPrevID() const { return prevID_; }

    /// Set the previous checkpoint sequence ID
    /// @param id the previous checkpoint sequence ID
    void setPrevID(const int64_t id) { prevID_ = id; }

    /// Get the index offset of previous checkpoint
    /// @return the index offset of the previous delta checkpoint, return 0 if this index
    /// itself is base or the first delta checkpoint
    uint64_t getPrevIndexOffset() const { return prevIndexOffset_; }

    /// Set the index offset of previous checkpoint
    /// @param offset the index offset of previous delta checkpoint
    void setPrevIndexOffset(const uint64_t offset) { prevIndexOffset_ = offset; }

    /// Constants
    static const Type BASE = 0x00;
    static const Type FIRST_DELTA = 0x01;
    static const Type DELTA = 0x02;

  private:
    /// Get the serialized length of 64-bit integer
    /// @return the serialized length of 64-bit integer (0x00 for 1, 0x01 ofr 2, 0x02 for 4, ora0x03
    /// for 8)
    static uint8_t getSerializedLength(const uint64_t i);

    /// Variable-length serialization of 64-bit unsigned integer
    /// @param buffer seriaization buffer
    /// @param i integer to serialize
    static void serializeUInt64(DataStoreByteBuffer& buffer, const uint64_t i);

    /// Variable-length deserialization of 64-bit unsigned integer
    /// @param buffer seriaization buffer
    /// @param number of bytes of serialized integer (0x00 for 1, 0x01 ofr 2, 0x02 for 4, ora0x03
    /// for 8)
    /// @return deserialized integer
    static uint64_t deserializeUInt64(DataStoreByteBuffer& buffer, const uint8_t numBytes);

    Type type_;       // type of checkpoint being indexed
    uint64_t offset_; // offset in delta buffer
    int64_t prevID_;  // sequence ID of previous delta checkpoint (0 if there is no previous one)
    uint64_t prevIndexOffset_; // offset in previous delta checkpoint's index buffer
};
} // namespace SPL

#endif // SPL_RUNTIME_OPERATOR_STATE_INCREMENTAL_CHECKPOINT_INDEX_H
